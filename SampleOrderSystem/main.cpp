#include "controller/MainController.h"
#include "view/ConsoleUtil.h"

int main() {
    ConsoleUtil::enableVirtualTerminal();
    MainController().run();
    return 0;
}
