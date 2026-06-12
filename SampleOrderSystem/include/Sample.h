#pragma once
#include <string>
#include "IModel.h"

struct Sample : public IModel {
    std::string sampleId;
    std::string name;
    double      avgProductionTime = 0.0;
    double      yieldRate         = 0.0;
    int         stock             = 0;

    JsonValue toJson() const override;
    void fromJson(const JsonValue& json) override;
};
