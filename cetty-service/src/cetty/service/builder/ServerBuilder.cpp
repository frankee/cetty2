/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/service/builder/ServerBuilder.h>

#include <cetty/channel/socket/asio/AsioServicePool.h>
#include <cetty/channel/socket/asio/AsioServerSocketChannelFactory.h>
#include <cetty/bootstrap/ServerBootstrap.h>
#include <cetty/logging/Log4cplusLoggerFactory.h>
#include <cetty/config/ConfigCenter.h>

#if (defined(linux) || defined(__linux) || defined(__linux__))

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#include <ucontext.h>
#endif /* HAVE_BACKTRACE */

#endif


namespace cetty {
namespace service {
namespace builder {

using namespace cetty::channel;
using namespace cetty::channel::socket::asio;
using namespace cetty::bootstrap;
using namespace cetty::logging;
using namespace cetty::service;
using namespace cetty::config;

#if (defined(linux) || defined(__linux) || defined(__linux__))

/* Anti-warning macro... */
#define CETTY_NOTUSED(V) ((void) V)

static void daemonize() {
    int fd;

    if (fork() != 0) { exit(0); } /* parent exits */

    pid_t sid = setsid(); /* create a new session for the child process */

    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Every output goes to /dev/null. */
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        if (fd > STDERR_FILENO) {
            close(fd);
        }
    }
    else { /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}

void createPidFile(const char* pidfile) {
    if (NULL == pidfile) { return; }

    /* Try to write the pid file in a best-effort way. */
    FILE* fp = fopen(pidfile, "w");

    if (fp) {
        fprintf(fp,"%d\n",(int)getpid());
        fclose(fp);
    }
}

#ifdef HAVE_BACKTRACE
static void* getMcontextEip(ucontext_t* uc) {
#if defined(__FreeBSD__)
    return (void*) uc->uc_mcontext.mc_eip;
#elif defined(__dietlibc__)
    return (void*) uc->uc_mcontext.eip;
#elif defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
#if __x86_64__
    return (void*) uc->uc_mcontext->__ss.__rip;
#elif __i386__
    return (void*) uc->uc_mcontext->__ss.__eip;
#else
    return (void*) uc->uc_mcontext->__ss.__srr0;
#endif
#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
#if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
    return (void*) uc->uc_mcontext->__ss.__rip;
#else
    return (void*) uc->uc_mcontext->__ss.__eip;
#endif
#elif defined(__i386__)
    return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
    return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
    return (void*) uc->uc_mcontext.sc_ip;
#else
    return NULL;
#endif
}

void bugReportStart(void) {
    if (server.bug_report_start == 0) {
        redisLog(REDIS_WARNING,
                 "=== REDIS BUG REPORT START: Cut & paste starting from here ===");
        server.bug_report_start = 1;
    }
}

static void sigsegvHandler(int sig, siginfo_t* info, void* secret) {
    void* trace[100];
    char** messages = NULL;
    int i, trace_size = 0;
    ucontext_t* uc = (ucontext_t*) secret;
    sds infostring, clients;
    struct sigaction act;
    REDIS_NOTUSED(info);

    bugReportStart();
    redisLog(REDIS_WARNING,
             "    Redis %s crashed by signal: %d", REDIS_VERSION, sig);
    redisLog(REDIS_WARNING,
             "    Failed assertion: %s (%s:%d)", server.assert_failed,
             server.assert_file, server.assert_line);

    /* Generate the stack trace */
    trace_size = backtrace(trace, 100);

    /* overwrite sigaction with caller's address */
    if (getMcontextEip(uc) != NULL) {
        trace[1] = getMcontextEip(uc);
    }

    messages = backtrace_symbols(trace, trace_size);
    redisLog(REDIS_WARNING, "--- STACK TRACE");

    for (i=1; i<trace_size; ++i) {
        redisLog(REDIS_WARNING,"%s", messages[i]);
    }

    /* Log INFO and CLIENT LIST */
    redisLog(REDIS_WARNING, "--- INFO OUTPUT");
    infostring = genRedisInfoString();
    redisLog(REDIS_WARNING, infostring);
    redisLog(REDIS_WARNING, "--- CLIENT LIST OUTPUT");
    clients = getAllClientsInfoString();
    redisLog(REDIS_WARNING, clients);
    /* Don't sdsfree() strings to avoid a crash. Memory may be corrupted. */

    /* Log CURRENT CLIENT info */
    if (server.current_client) {
        redisClient* cc = server.current_client;
        sds client;
        int j;

        redisLog(REDIS_WARNING, "--- CURRENT CLIENT INFO");
        client = getClientInfoString(cc);
        redisLog(REDIS_WARNING,"client: %s", client);

        /* Missing sdsfree(client) to avoid crash if memory is corrupted. */
        for (j = 0; j < cc->argc; j++) {
            robj* decoded;

            decoded = getDecodedObject(cc->argv[j]);
            redisLog(REDIS_WARNING,"argv[%d]: '%s'", j, (char*)decoded->ptr);
            decrRefCount(decoded);
        }

        /* Check if the first argument, usually a key, is found inside the
         * selected DB, and if so print info about the associated object. */
        if (cc->argc >= 1) {
            robj* val, *key;
            dictEntry* de;

            key = getDecodedObject(cc->argv[1]);
            de = dictFind(cc->db->dict, key->ptr);

            if (de) {
                val = dictGetEntryVal(de);
                redisLog(REDIS_WARNING,"key '%s' found in DB containing the following object:", key->ptr);
                redisLogObjectDebugInfo(val);
            }

            decrRefCount(key);
        }
    }

    redisLog(REDIS_WARNING,
             "=== REDIS BUG REPORT END. Make sure to include from START to END. ===\n\n"
             "       Please report the crash opening an issue on github:\n\n"
             "           http://github.com/antirez/redis/issues\n\n"
             "  Suspect RAM error? Use redis-server --test-memory to veryfy it.\n\n"
            );

    /* free(messages); Don't call free() with possibly corrupted memory. */
    if (server.daemonize) { unlink(server.pidfile); }

    /* Make sure we exit with the right signal at the end. So for instance
     * the core will be dumped if enabled. */
    sigemptyset(&act.sa_mask);
    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
     * is used. Otherwise, sa_handler is used */
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = SIG_DFL;
    sigaction(sig, &act, NULL);
    kill(getpid(),sig);
}
#endif /* HAVE_BACKTRACE */

static void sigtermHandler(int sig) {
    CETTY_NOTUSED(sig);

    //LOGGER(LOGGER_WARN,"Received SIGTERM, scheduling shutdown...");
    //server.shutdown_asap = 1;
}

#undef CETTY_NOTUSED

void setupSignalHandlers() {
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);

#ifdef HAVE_BACKTRACE
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
    act.sa_sigaction = sigsegvHandler;
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS, &act, NULL);
    sigaction(SIGFPE, &act, NULL);
    sigaction(SIGILL, &act, NULL);
#endif

    return;
}

#else

static void daemonize() {}
static void createPidFile(const char* pidfile) {}

#endif

ServerBuilder::ServerBuilder() {
    init();
}

ServerBuilder::ServerBuilder(int parentThreadCnt, int childThreadCnt) {
    //config.threadCount = threadCnt;
    init();
}

ServerBuilder::ServerBuilder(const ServerBuilderConfig& config)
    : config(config) {
    init();
}

ServerBuilder::~ServerBuilder() {
    deinit();
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const ChannelPipelinePtr& pipeline,
                                int port) {
    return build(name, pipeline, std::string(), port);
}

ChannelPtr ServerBuilder::build(const std::string& name,
                                const ChannelPipelinePtr& pipeline,
                                const std::string& host,
                                int port) {
    if (name.empty()) {
        printf("parameter error, empty name or invalid address.");
        return ChannelPtr();
    }

    if (!servicePool) {
        servicePool = new AsioServicePool(config.threadCount);
    }

    ServerBootstrap* bootstrap = new ServerBootstrap(
        new AsioServerSocketChannelFactory(servicePool));
    bootstraps.insert(std::make_pair(name, bootstrap));

    bootstrap->setOption(ChannelOption::CO_SO_LINGER, 0);
    bootstrap->setOption(ChannelOption::CO_SO_REUSEADDR, true);
    bootstrap->setChildOption(ChannelOption::CO_TCP_NODELAY, true);
    bootstrap->setPipeline(pipeline);

    ChannelFuturePtr future;
    if (host.empty()) {
        future = bootstrap->bind(port);
    }
    else {
        future = bootstrap->bind(host, port);
    }

    if (future->await()->isSuccess()) {
        return future->getChannel();
    }
    else {
        return ChannelPtr();
    }
}

ChannelPtr ServerBuilder::build(const std::string& name, int port) {
    ChannelPipelinePtr pipeline = getPipeline(name);

    if (pipeline) {
        return build(name, pipeline, port);
    }

    return ChannelPtr();
}

int ServerBuilder::init() {
    if (config.deamonize) {
        daemonize();
    }

    if (config.logger == "log4cplus") {
        //if (confCenter) {
            //InternalLoggerFactory::setDefaultFactory(
            //    new Log4cplusLoggerFactory(*confCenter));
        //}
    }

    return 0;
}

void ServerBuilder::deinit() {

}

void ServerBuilder::stop() {
    std::map<std::string, ServerBootstrap*>::iterator itr;
    for (itr = bootstraps.begin(); itr != bootstraps.end(); ++itr) {

    }
}

void ServerBuilder::waitingForExit() {
    if (config.deamonize) {
        createPidFile(config.pidfile.c_str());
    }
    else {
        //ChannelPtr c = bootstraps.begin()->second->getFactory()->;
        //if (c && c->isBound()) {
            printf("Server is running...\n");
            printf("To quit server, press 'q'.\n");

            char input;

            do {
                input = getchar();

                if (input == 'q') {
                    stop();
                }
            }
            while (true);
        //}
    }
}

void ServerBuilder::buildAll() {
    std::size_t j = config.servers.size();

    for (std::size_t i = 0; i < j; ++i) {
        const ServerBuilderConfig::Server& server = *config.servers[i];

        if (!server.pipeline.empty()) {
            ChannelPipelinePtr pipeline = getPipeline(server.pipeline);

            if (server.host.empty()) {
                build(server.pipeline, pipeline, server.port);
            }
            else {
                build(server.pipeline, pipeline, server.host, server.port);
            }
        }
        else {
            printf("");
        }
    }
}

void ServerBuilder::getBuiltServers(std::map<std::string, ChannelPtr>* names) {

}

void ServerBuilder::registerPipeline(const std::string& name,
                                     const ChannelPipelinePtr& pipeline) {
    if (!name.empty()) {
        pipelines[name] = pipeline;
    }
}

void ServerBuilder::unregisterPipeline(const std::string& name) {
    std::map<std::string, ChannelPipelinePtr>::iterator itr =
        pipelines.find(name);

    if (itr != pipelines.end()) {
        pipelines.erase(itr);
    }
}

cetty::channel::ChannelPipelinePtr ServerBuilder::getPipeline(const std::string& name) {
    std::map<std::string, ChannelPipelinePtr>::iterator itr = pipelines.find(name);
    if (itr != pipelines.end()) {
        return itr->second;
    }
    return ChannelPipelinePtr();
}

}
}
}
