#include "ProductionCalculator.h"
#include <cmath>

int ProductionCalculator::calculateActualProduction(int shortage, double yieldRate) {
    if (shortage <= 0) return 0;
    return static_cast<int>(std::ceil(shortage / (yieldRate * 0.9)));
}

double ProductionCalculator::calculateTotalTime(double avgProductionTime, int actualProd) {
    return avgProductionTime * actualProd;
}

int ProductionCalculator::calculateGoodYield(int actualProd, double yieldRate) {
    return static_cast<int>(std::floor(actualProd * yieldRate));
}
