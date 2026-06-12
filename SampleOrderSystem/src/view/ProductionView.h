#pragma once
#include <iostream>
#include <vector>
#include "model/ProductionJob.h"
#include "model/Sample.h"

class ProductionView {
public:
    explicit ProductionView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showNoRunningJob();
    void showRunningJob(const ProductionJob& job, const Sample& s);
    void showWaitingQueue(const std::vector<ProductionJob>& queue);
    void showCompleteMenu(bool hasRunningJob);
    void showCompleteSuccess(const ProductionJob& job);
    void showError(const std::string& msg);

    char readCommand();

private:
    std::ostream& out_;
    std::istream& in_;
};
