#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Sample.h"

class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;

    virtual void               save(const Sample& sample)              = 0;
    virtual std::optional<Sample> findById(const std::string& id) const = 0;
    virtual std::vector<Sample>   findAll() const                      = 0;
    virtual void               update(const Sample& sample)            = 0;
    virtual void               remove(const std::string& id)           = 0;
};
