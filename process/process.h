#ifndef STDX_PROCESS_H
#define STDX_PROCESS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>
#include <cassert>
#include <cstring>
#include <string>
#include <sstream>

namespace stdx {

struct ProcessState {
    int Status;
    rusage Usage;

    ProcessState(): Status(0) { ::memset(&Usage, 0, sizeof(Usage)); }

    bool Exited() const { return WIFEXITED(Status); }
    bool Signalled() const { return WIFSIGNALED(Status); }
    bool Success() const { return Exitcode() == EXIT_SUCCESS; }
    long SystemTime() const { return ms(Usage.ru_stime); }
    long UserTime() const { return ms(Usage.ru_utime); }
    long MaxRSS() const { return Usage.ru_maxrss; }

    int Exitcode() const {
        if(Exited()) {
            return WEXITSTATUS(Status);
        } else if(Signalled()) {
            return WTERMSIG(Status);
        } else {
            assert(0 && "invalid status");
            return -1;
        }
    }

    std::string String() const {
        std::stringstream ss;
        ss << "{Exitcode: " << Exitcode() << ", "
            << "UserTime: " << UserTime() << "ms, "
            << "SysTime: " << SystemTime() << "ms, "
            << "MaxRSS: " << MaxRSS() / 1024 << "K}";
        return ss.str();
    }

protected:
    long ms(::timeval t) const { return t.tv_sec * 1000 + t.tv_usec / 1000; }
};

struct Process {
    pid_t Pid;
    Process(): Pid(-1) {}
    virtual ~Process() {}

    bool Start() {
        if(! prepare()) {
            return false;
        }

        Pid = ::fork();
        if(Pid < 0) {
            return false;
        } else if(Pid > 0) {
            return parent();
        } else {
            if(child()) {
                ::exit(EXIT_SUCCESS);
            } else {
                ::exit(EXIT_FAILURE);
            }
        }
    }

    bool Kill(int sig) {
        assert(Pid != -1);
        return ::kill(Pid, sig) >= 0;
    }

    bool Wait(ProcessState& s) {
        assert(Pid != -1);
        pid_t pid = -1;
        do {
            pid = ::wait4(Pid, &s.Status, 0, &s.Usage);
        } while (pid < 0 && errno == EINTR);
        return pid == Pid;
    }

protected:
    virtual bool prepare() = 0;
    virtual bool parent() = 0;
    virtual bool child() = 0;
};


}
#endif
