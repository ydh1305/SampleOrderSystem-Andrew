#include <gmock/gmock.h>
#include "model/ProductionCalculator.h"

// T1: basic case from spec (shortage=170, yieldRate=0.92 -> 206)
TEST(ProductionCalculatorTest, CalculateActualProductionSpecExample) {
    EXPECT_EQ(ProductionCalculator::calculateActualProduction(170, 0.92), 206);
}

// T2: ceiling is applied when division is not exact
TEST(ProductionCalculatorTest, CalculateActualProductionCeiling) {
    // shortage=1, yieldRate=1.0 -> ceil(1 / 0.9) = ceil(1.111) = 2
    EXPECT_EQ(ProductionCalculator::calculateActualProduction(1, 1.0), 2);
}

// T3: shortage=0 -> actualProd=0
TEST(ProductionCalculatorTest, CalculateActualProductionZeroShortage) {
    EXPECT_EQ(ProductionCalculator::calculateActualProduction(0, 0.92), 0);
}

// T4: exact division -> no ceiling needed
TEST(ProductionCalculatorTest, CalculateActualProductionExactDivision) {
    // shortage=90, yieldRate=1.0 -> ceil(90 / 0.9) = ceil(100.0) = 100
    EXPECT_EQ(ProductionCalculator::calculateActualProduction(90, 1.0), 100);
}

// T5: calculateTotalTime = avgProductionTime * actualProd
TEST(ProductionCalculatorTest, CalculateTotalTimeBasic) {
    EXPECT_DOUBLE_EQ(ProductionCalculator::calculateTotalTime(0.8, 206), 164.8);
}

// T6: calculateTotalTime with zero
TEST(ProductionCalculatorTest, CalculateTotalTimeZero) {
    EXPECT_DOUBLE_EQ(ProductionCalculator::calculateTotalTime(0.8, 0), 0.0);
}

// T7: calculateGoodYield = floor(actualProd * yieldRate)
TEST(ProductionCalculatorTest, CalculateGoodYieldBasic) {
    // floor(206 * 0.92) = floor(189.52) = 189
    EXPECT_EQ(ProductionCalculator::calculateGoodYield(206, 0.92), 189);
}

// T8: full pipeline - shortage -> actualProd -> totalTime -> goodYield
TEST(ProductionCalculatorTest, FullPipelineMatchesSpec) {
    int actualProd = ProductionCalculator::calculateActualProduction(170, 0.92);
    EXPECT_EQ(actualProd, 206);
    EXPECT_DOUBLE_EQ(ProductionCalculator::calculateTotalTime(0.8, actualProd), 164.8);
    EXPECT_EQ(ProductionCalculator::calculateGoodYield(actualProd, 0.92), 189);
}
