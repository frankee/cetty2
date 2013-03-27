
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

#include <cetty/Platform.h>

#if (CETTY_OS_FAMILY_UNIX)

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>

#endif

#include <string>
#include <cetty/Common.h>
#include <cetty/Platform.h>
#include <cetty/bootstrap/ServerUtil.h>
#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace bootstrap {

#if (CETTY_OS_FAMILY_UNIX)

void ServerUtil::daemonize() {
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

void ServerUtil::createPidFile(const std::string& pidfile) {
    if (pidfile.empty()) {
        LOG_WARN << "pidfile is empty, do not create pid file";
        return;
    }

    LOG_INFO << "create the pid file " << pidfile;

    /* Try to write the pid file in a best-effort way. */
    FILE* fp = fopen(pidfile.c_str(), "w");

    if (fp) {
        fprintf(fp,"%d\n",(int)getpid());
        fclose(fp);
    }
}

static void sigtermHandler(int sig) {
    //CETTY_REFERENCED(sig);
    LOG_INFO << "Received signal " << sig;

    /*
    std::vector<SignalHandler>::const_iterator itr = handlers_.begin();
    for (; itr != handlers_.end(); ++itr) {
        if (*itr) {
            (*itr)(sig);
        }
    }

    if (sig == SIGTERM || sig == SIGINT) {
        LOG_INFO << "Received SIGTERM, scheduling shutdown...";
    }*/
}

void ServerUtil::setupSignalHandlers() {
    //signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act;

    /* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
     * Otherwise, sa_handler is used. */
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
    act.sa_handler = sigtermHandler;
    sigaction(SIGTERM, &act, NULL);

    return;
}

#else

void ServerUtil::daemonize() {}
void ServerUtil::createPidFile(const std::string& pidfile) {}
void ServerUtil::setupSignalHandlers() {}

#endif

void ServerUtil::registerSignalHandler(const SignalHandler& handler) {
    if (handler) {
        handlers_.push_back(handler);
    }
}

void ServerUtil::setupSignalHandlers(const SignalHandler& handler) {
    registerSignalHandler(handler);
    setupSignalHandlers();
}

std::vector<ServerUtil::SignalHandler> ServerUtil::handlers_;

}
}
