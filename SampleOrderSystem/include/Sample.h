#pragma once
#include <string>
#include "storage/JsonValue.h"

struct Sample {
    std::string sampleId;
    std::string name;
    double      avgProductionTime = 0.0;
    double      yieldRate         = 0.0;
    int         stock             = 0;

    JsonValue     toJson() const;
    static Sample fromJson(const JsonValue& json);
};
