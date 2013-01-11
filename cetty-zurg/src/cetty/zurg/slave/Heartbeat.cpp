#include <cetty/zurg/slave/Heartbeat.h>
#include <cetty/zurg/slave/ZurgSlaveConfig.h>
#include <cetty/zurg/master/master.pb.h>
#include <cetty/protobuf/service/service.pb.h>
#include <cetty/channel/EventLoopPtr.h>
#include <cetty/logging/LoggerHelper.h>
#include <cetty/zurg/Util.h>
#include <cetty/config/ConfigCenter.h>
#include <cetty/util/SmallFile.h>
#include <cetty/util/Process.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <sys/utsname.h>
#include <stdlib.h>

namespace cetty {
namespace zurg {
namespace slave {

using namespace cetty::util;
using namespace cetty::zurg;
using namespace cetty::protobuf::service;
using namespace cetty::zurg::master;

void ignoreCallback(const Empty*) {

}

// unique_ptr is better
typedef boost::shared_ptr<SmallFile> SmallFilePtr;

class ProcFs : boost::noncopyable {
public:
    typedef std::map<StringPiece, SmallFilePtr> FileMap;

    // char* to save construction of string.
    int readTo(const char* filename, std::string* out) {
        FileMap::iterator it = files_.find(filename);

        if (it == files_.end()) {
            SmallFilePtr ptr(new SmallFile(filename));
            it = files_.insert(std::make_pair(filename, ptr)).first;
        }

        assert(it != files_.end());
        int size = 0;
        int err = it->second->readToBuffer(&size);
        LOG_TRACE << filename << " " << err << " " << size;

        if (size > 0) {
            out->assign(it->second->buffer(), size);
        }

        return err;
    }

private:
    FileMap files_;
};

void strip_cpuinfo(std::string* cpuinfo) {
    // FIXME:
}

void fill_uname(SlaveHeartbeat* hb) {
    struct utsname buf;

    if (::uname(&buf) == 0) {
        SlaveHeartbeat::Uname* p = hb->mutable_uname();
        p->set_sys_name(buf.sysname);
        p->set_node_name(buf.nodename);
        p->set_release(buf.release);
        p->set_version(buf.version);
        p->set_machine(buf.machine);
        p->set_domain_name(buf.domainname);
    }
    else {
        LOG_ERROR << "uname";
    }
}

void strip_diskstats(std::string* diskstats) {
    std::string result;
    result.reserve(diskstats->size());
    StringPiece zeros(" 0 0 0 0 0 0 0 0 0 0 0\n");

    const char* p = diskstats->c_str();
    const char* nl = NULL;

    while ((nl = ::strchr(p, '\n')) != NULL) {
        int pos = static_cast<int>(nl - p + 1);
        StringPiece line(p, pos);

        if (line.size() > zeros.size()) {
            StringPiece end(line.data() + line.size()-zeros.size(), zeros.size());

            if (end != zeros) {
                result.append(line.data(), line.size());
            }
        }

        p += pos;
    }

    assert(p == &*diskstats->end());
    diskstats->swap(result);
}

struct AreBothSpaces {
    bool operator()(char x, char y) const {
        return x == ' ' && y == ' ';
    }
};

void strip_meminfo(std::string* meminfo) {
    std::string::iterator it =
        std::unique(meminfo->begin(), meminfo->end(), AreBothSpaces());
    meminfo->erase(it, meminfo->end());
}

void strip_stat(std::string* proc_stat) {
    const char* intr = ::strstr(proc_stat->c_str(), "\nintr ");

    if (intr != NULL) {
        const char* nl = ::strchr(intr + 1, '\n');
        assert(nl != NULL);

        StringPiece line(intr+1, static_cast<int>(nl-intr-1));
        const char* p = nl;

        while (p[-1] == '0' && p[-2] == ' ') {
            p -= 2;
        }

        ++p;
        proc_stat->erase(p - proc_stat->c_str(), nl-p);
    }
}

Heartbeat::Heartbeat(const EventLoopPtr& loop,
                     MasterService_Stub* stub)
    : loop_(loop),
      name_(std::string()),
      port_(0),
      stub_(stub),
      procFs_(new ProcFs),
      beating_(false) {
    //ConfigCenter::instance().configure(&config_);
    init();
}

Heartbeat::~Heartbeat() {

}

void Heartbeat::init() {
    // todo init data about config if some data is not inited
    //name_ = config_.name;

    //if (config_.listenPort <= 0) {
    //    LOG_ERROR << "Slave listen port not greater than 0.";
    //    exit(0);
    //}

    //port_ = config_.listenPort;

    //if (config_.heartbeatInterval <= 0) { config_.heartbeatInterval = 3000; }
}

void Heartbeat::start() {
    beating_ = true;
    loop_->runEvery(heartbeatInterval_, boost::bind(&Heartbeat::onTimer, this));
}

void Heartbeat::stop() {
    beating_ = false;
}

void Heartbeat::onTimer() {
    if (beating_) {
        beat(false);
    }
}

#define FILL_HB(PROC_FILE, FIELD)               \
    if (procFs_->readTo(PROC_FILE, hb.mutable_##FIELD()) != 0)     \
        hb.clear_##FIELD();

void Heartbeat::beat(bool showStatic) {
    LOG_DEBUG << (showStatic ? "full" : "");
    SlaveHeartbeat hb;
    hb.set_slave_name(name_);

    if (showStatic) {
        hb.set_host_name(hostname().c_str());

        if (port_ > 0) {
            hb.set_listen_port(port_);
        }

        hb.set_slave_pid(cetty::util::Process::id());
        hb.set_start_time_us(getMicroSecs(g_startTime));
        // hb.set_slave_version(slave_version);
        FILL_HB("/proc/cpuinfo", cpuinfo);
        FILL_HB("/proc/version", version);
        FILL_HB("/etc/mtab", etc_mtab);
        strip_cpuinfo(hb.mutable_cpuinfo());
        // sysctl
        fill_uname(&hb);
    }

    hb.set_send_time_us(now());

    FILL_HB("/proc/meminfo", meminfo);
    FILL_HB("/proc/stat", proc_stat);
    FILL_HB("/proc/loadavg", loadavg);
    FILL_HB("/proc/diskstats", diskstats);
    FILL_HB("/proc/net/dev", net_dev);
    FILL_HB("/proc/net/tcp", net_tcp);
    strip_diskstats(hb.mutable_diskstats());
    strip_meminfo(hb.mutable_meminfo());
    strip_stat(hb.mutable_proc_stat());

    // todo fix error
    slaveHeartbeatServiceFuturePtr hbsf = new slaveHeartbeatServiceFuture();
    stub_->slaveHeartbeat(&hb, hbsf);
}

}
}
}
