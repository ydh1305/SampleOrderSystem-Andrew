#include "MainController.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/MonitoringController.h"
#include "controller/ReleaseController.h"
#include "controller/ProductionController.h"

MainController::MainController(const std::string& samplePath,
                               const std::string& orderPath,
                               const std::string& jobPath,
                               std::ostream& out,
                               std::istream& in)
    : sampleRepo_(samplePath), orderRepo_(orderPath), jobRepo_(jobPath),
      out_(out), in_(in) {}

void MainController::showMainMenu() {
    auto samples = sampleRepo_.findAll();
    auto orders  = orderRepo_.findAll();
    auto jobs    = jobRepo_.findAll();

    int reserved  = 0, producing = 0, confirmed = 0;
    for (const auto& o : orders) {
        if (o.status == OrderStatus::RESERVED)  reserved++;
        if (o.status == OrderStatus::PRODUCING) producing++;
        if (o.status == OrderStatus::CONFIRMED) confirmed++;
    }

    out_ << "\n========== S-Semi 시료 생산주문관리 ==========\n";
    out_ << "시료: " << samples.size()
         << "  |  대기: " << reserved
         << "  생산중: " << producing
         << "  확정: " << confirmed << "\n";
    out_ << "----------------------------------------------\n";
    out_ << "[1] 시료 관리        [2] 시료 주문\n";
    out_ << "[3] 주문 승인/거절   [4] 모니터링\n";
    out_ << "[5] 생산라인 조회    [6] 출고 처리\n";
    out_ << "[0] 종료\n";
    out_ << "선택: ";
}

void MainController::run() {
    while (true) {
        showMainMenu();

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) {
            out_ << "시스템을 종료합니다.\n";
            break;
        }

        SampleController     sampleCtrl(sampleRepo_, out_, in_);
        OrderController      orderCtrl(sampleRepo_, orderRepo_, jobRepo_, out_, in_);
        MonitoringController monCtrl(sampleRepo_, orderRepo_, out_, in_);
        ReleaseController    releaseCtrl(orderRepo_, out_, in_);
        ProductionController prodCtrl(sampleRepo_, orderRepo_, jobRepo_, out_, in_);

        switch (choice) {
        case 1: sampleCtrl.run();                  break;
        case 2: orderCtrl.placeOrder();             break;
        case 3: orderCtrl.processApproval();        break;
        case 4: monCtrl.run();                     break;
        case 5: prodCtrl.showProductionLine();     break;
        case 6: releaseCtrl.processRelease();      break;
        default: out_ << "잘못된 선택입니다.\n";  break;
        }
    }
}
