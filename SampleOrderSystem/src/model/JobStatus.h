#pragma once
#include <string>

enum class JobStatus {
    WAITING,
    RUNNING,
    DONE
};

inline std::string toString(JobStatus s) {
    switch (s) {
        case JobStatus::WAITING: return "WAITING";
        case JobStatus::RUNNING: return "RUNNING";
        case JobStatus::DONE:    return "DONE";
    }
    return "";
}

inline JobStatus jobStatusFromString(const std::string& s) {
    if (s == "RUNNING") return JobStatus::RUNNING;
    if (s == "DONE")    return JobStatus::DONE;
    return JobStatus::WAITING;
}
