#pragma once
#include <string>
#include "storage/JsonValue.h"
#include "model/JobStatus.h"

struct ProductionJob {
    std::string jobId;
    std::string orderId;
    std::string sampleId;
    int         shortage            = 0;
    int         actualProd          = 0;
    double      totalTime           = 0.0;
    JobStatus   status              = JobStatus::WAITING;
    std::string enqueuedAt;
    std::string startedAt;
    std::string completedAt;

    JsonValue           toJson() const;
    static ProductionJob fromJson(const JsonValue& json);
};
