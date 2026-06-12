#include "controller/MainController.h"
#include "view/ConsoleUtil.h"
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    ConsoleUtil::enableVirtualTerminal();
    MainController().run();
    return 0;
}
