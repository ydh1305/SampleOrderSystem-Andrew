#pragma once
#include <string>
#include "IModel.h"
#include "JobStatus.h"

struct ProductionJob : public IModel {
    std::string jobId;
    std::string orderId;
    std::string sampleId;
    int         shortage            = 0;
    int         actualProduction    = 0;
    double      totalProductionTime = 0.0;
    JobStatus   status              = JobStatus::WAITING;
    std::string enqueuedAt;

    JsonValue toJson() const override;
    void fromJson(const JsonValue& json) override;
};
