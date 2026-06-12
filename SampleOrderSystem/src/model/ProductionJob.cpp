#include "ProductionJob.h"

JsonValue ProductionJob::toJson() const {
    std::map<std::string, JsonValue> obj;
    obj["jobId"]       = JsonValue(jobId);
    obj["orderId"]     = JsonValue(orderId);
    obj["sampleId"]    = JsonValue(sampleId);
    obj["shortage"]    = JsonValue(shortage);
    obj["actualProd"]  = JsonValue(actualProd);
    obj["totalTime"]   = JsonValue(totalTime);
    obj["status"]      = JsonValue(toString(status));
    obj["enqueuedAt"]  = JsonValue(enqueuedAt);
    obj["startedAt"]   = JsonValue(startedAt);
    obj["completedAt"] = JsonValue(completedAt);
    return JsonValue(std::move(obj));
}

ProductionJob ProductionJob::fromJson(const JsonValue& json) {
    ProductionJob job;
    job.jobId       = json["jobId"].asString();
    job.orderId     = json["orderId"].asString();
    job.sampleId    = json["sampleId"].asString();
    job.shortage    = json["shortage"].asInt();
    job.actualProd  = json["actualProd"].asInt();
    job.totalTime   = json["totalTime"].asDouble();
    job.status      = jobStatusFromString(json["status"].asString());
    job.enqueuedAt  = json["enqueuedAt"].asString();
    job.startedAt   = json["startedAt"].asString();
    job.completedAt = json["completedAt"].asString();
    return job;
}
