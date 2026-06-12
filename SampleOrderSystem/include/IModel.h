#pragma once
#include "storage/JsonValue.h"

class IModel {
public:
    virtual ~IModel() = default;
    virtual JsonValue toJson() const = 0;
};
