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
    job.jobId       = json.contains("jobId")       ? json["jobId"].asString()                       : "";
    job.orderId     = json.contains("orderId")     ? json["orderId"].asString()                     : "";
    job.sampleId    = json.contains("sampleId")    ? json["sampleId"].asString()                    : "";
    job.shortage    = json.contains("shortage")    ? json["shortage"].asInt()                       : 0;
    job.actualProd  = json.contains("actualProd")  ? json["actualProd"].asInt()                     : 0;
    job.totalTime   = json.contains("totalTime")   ? json["totalTime"].asDouble()                   : 0.0;
    job.status      = json.contains("status")      ? jobStatusFromString(json["status"].asString()) : JobStatus::WAITING;
    job.enqueuedAt  = json.contains("enqueuedAt")  ? json["enqueuedAt"].asString()                  : "";
    job.startedAt   = json.contains("startedAt")   ? json["startedAt"].asString()                   : "";
    job.completedAt = json.contains("completedAt") ? json["completedAt"].asString()                 : "";
    return job;
}
