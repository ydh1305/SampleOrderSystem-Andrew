#pragma once

class ProductionCalculator {
public:
    static int    calculateActualProduction(int shortage, double yieldRate);
    static double calculateTotalTime(double avgProductionTime, int actualProd);
    static int    calculateGoodYield(int actualProd, double yieldRate);
};
