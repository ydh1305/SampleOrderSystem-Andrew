#pragma once
#include <string>
#include <vector>
#include <optional>
#include "model/Sample.h"
#include "storage/JsonStorage.h"
#include "ISampleRepository.h"

class SampleRepository : public ISampleRepository {
public:
    explicit SampleRepository(const std::string& filePath = "data/samples.json");

    std::vector<Sample>   findAll() const;
    std::optional<Sample> findById(const std::string& sampleId) const;
    void                  save(const Sample& sample);
    void                  update(const Sample& sample);
    std::string           generateNextId() const;

private:
    JsonStorage storage_;
};
