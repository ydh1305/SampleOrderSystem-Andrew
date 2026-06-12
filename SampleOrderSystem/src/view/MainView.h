#pragma once
#include <iostream>

class MainView {
public:
    explicit MainView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showMainMenu();
    void showSummary(int sampleCount, int totalStock, int pendingOrders, int runningJobs);
    void showError(const std::string& msg);

    int readMenuChoice();

private:
    std::ostream& out_;
    std::istream& in_;
};
