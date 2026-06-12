#pragma once
#include <string>

class ConsoleUtil {
public:
    static void        enableVirtualTerminal();
    static void        printGreen(const std::string& msg);
    static void        printYellow(const std::string& msg);
    static void        printRed(const std::string& msg);
    static int         readInt();
    static std::string readLine();
    static std::string getCurrentDateTime();
    static std::string getCurrentDate();
};
