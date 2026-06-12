#pragma once
#include <iostream>
#include <string>
#include "ISampleRepository.h"

class SampleController {
public:
    SampleController(ISampleRepository& repo,
                     std::ostream& out = std::cout,
                     std::istream& in  = std::cin);

    void listSamples();
    void addSample();
    void searchSamples();
    void run();

private:
    ISampleRepository& repo_;
    std::ostream& out_;
    std::istream& in_;
};
