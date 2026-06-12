#include <gmock/gmock.h>
#include <regex>
#include "view/ConsoleUtil.h"

// T16: getCurrentDateTime returns YYYY-MM-DD HH:MM:SS format
TEST(ConsoleUtilTest, GetCurrentDateTimeFormat) {
    std::string dt = ConsoleUtil::getCurrentDateTime();
    std::regex pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
    EXPECT_TRUE(std::regex_match(dt, pattern)) << "got: " << dt;
}

// T17: getCurrentDate returns YYYYMMDD format
TEST(ConsoleUtilTest, GetCurrentDateFormat) {
    std::string d = ConsoleUtil::getCurrentDate();
    std::regex pattern(R"(\d{8})");
    EXPECT_TRUE(std::regex_match(d, pattern)) << "got: " << d;
}

// T18: getCurrentDate prefix matches getCurrentDateTime date part
TEST(ConsoleUtilTest, GetCurrentDateMatchesDateTimeDatePart) {
    std::string dt = ConsoleUtil::getCurrentDateTime();
    std::string d  = ConsoleUtil::getCurrentDate();
    // dt = "YYYY-MM-DD ..." -> remove dashes to get YYYYMMDD
    std::string datePart = dt.substr(0, 10);
    datePart.erase(std::remove(datePart.begin(), datePart.end(), '-'), datePart.end());
    EXPECT_EQ(datePart, d);
}
