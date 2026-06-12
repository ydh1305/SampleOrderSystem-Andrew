#pragma once
#include <gmock/gmock.h>
#include "ISampleRepository.h"
#include "IOrderRepository.h"
#include "IProductionJobRepository.h"

class MockSampleRepository : public ISampleRepository {
public:
    MOCK_METHOD(void, save, (const Sample& sample), (override));
    MOCK_METHOD(std::optional<Sample>, findById, (const std::string& id), (const, override));
    MOCK_METHOD(std::vector<Sample>, findAll, (), (const, override));
    MOCK_METHOD(void, update, (const Sample& sample), (override));
    MOCK_METHOD(std::string, generateNextId, (), (const, override));
};

class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(void, save, (const Order& order), (override));
    MOCK_METHOD(std::optional<Order>, findById, (const std::string& id), (const, override));
    MOCK_METHOD(std::vector<Order>, findAll, (), (const, override));
    MOCK_METHOD(std::vector<Order>, findByStatus, (OrderStatus status), (const, override));
    MOCK_METHOD(void, update, (const Order& order), (override));
    MOCK_METHOD(int, getNextSequence, (const std::string& date), (const, override));
};

class MockProductionJobRepository : public IProductionJobRepository {
public:
    MOCK_METHOD(void, save, (const ProductionJob& job), (override));
    MOCK_METHOD(std::optional<ProductionJob>, findById, (const std::string& id), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, findAll, (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, findByStatus, (JobStatus status), (const, override));
    MOCK_METHOD(void, update, (const ProductionJob& job), (override));
    MOCK_METHOD(std::string, generateNextId, (), (const, override));
    MOCK_METHOD(std::optional<ProductionJob>, findRunning, (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, findWaiting, (), (const, override));
};
