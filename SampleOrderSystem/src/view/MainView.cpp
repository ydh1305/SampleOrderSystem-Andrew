#include "view/MainView.h"
#include <string>

MainView::MainView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void MainView::showMainMenu() {
    out_ << "=== S-Semi 시료 생산주문관리 시스템 ===\n"
         << "[1] 시료 관리        [2] 시료 주문\n"
         << "[3] 주문 승인/거절   [4] 모니터링\n"
         << "[5] 생산라인 조회    [6] 출고 처리\n"
         << "[0] 종료\n";
}

void MainView::showSummary(int sampleCount, int totalStock, int pendingOrders, int runningJobs) {
    out_ << "=== 시스템 현황 ===\n"
         << "시료 종류: " << sampleCount << "\n"
         << "총 재고: " << totalStock << "\n"
         << "대기 주문: " << pendingOrders << "\n"
         << "진행 작업: " << runningJobs << "\n";
}

void MainView::showError(const std::string& msg) {
    out_ << "오류: " << msg << "\n";
}

int MainView::readMenuChoice() {
    int choice = 0;
    in_ >> choice;
    in_.ignore();
    return choice;
}
