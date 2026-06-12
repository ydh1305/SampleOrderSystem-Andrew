#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "model/Sample.h"

class SampleView {
public:
    explicit SampleView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showSubMenu();
    void showSampleList(const std::vector<Sample>& samples);
    void showNoSamples();
    void showSampleDetail(const Sample& s);
    void showSearchResults(const std::vector<Sample>& samples);
    void showNoResults();
    void showSaveSuccess(const std::string& id);
    void showError(const std::string& msg);

    int         readMenuChoice();
    std::string readName();
    std::string readSampleId();
    double      readAvgProductionTime();
    double      readYieldRate();
    int         readStock();
    char        readYesNo();

private:
    std::ostream& out_;
    std::istream& in_;
};
