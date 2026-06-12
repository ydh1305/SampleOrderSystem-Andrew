#include "ConsoleUtil.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <iostream>

namespace {
std::string formatNow(const char* fmt) {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
    localtime_s(&tm, &now);
    std::ostringstream ss;
    ss << std::put_time(&tm, fmt);
    return ss.str();
}
}

void ConsoleUtil::enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

void ConsoleUtil::printGreen(const std::string& msg)  { std::cout << "\033[32m" << msg << "\033[0m\n"; }
void ConsoleUtil::printYellow(const std::string& msg) { std::cout << "\033[33m" << msg << "\033[0m\n"; }
void ConsoleUtil::printRed(const std::string& msg)    { std::cout << "\033[31m" << msg << "\033[0m\n"; }

int ConsoleUtil::readInt() {
    int v; std::cin >> v; std::cin.ignore();
    return v;
}

std::string ConsoleUtil::readLine() {
    std::string line; std::getline(std::cin, line);
    return line;
}

std::string ConsoleUtil::getCurrentDateTime() { return formatNow("%Y-%m-%d %H:%M:%S"); }
std::string ConsoleUtil::getCurrentDate()     { return formatNow("%Y%m%d"); }
