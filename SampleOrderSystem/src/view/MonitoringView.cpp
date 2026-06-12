#include "view/MonitoringView.h"

MonitoringView::MonitoringView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void MonitoringView::showSubMenu() {
    out_ << "[1] 상태별 주문량\n"
         << "[2] 시료별 재고 상태\n"
         << "[0] 뒤로\n";
}

void MonitoringView::showOrderSummary(const std::map<OrderStatus, int>& counts) {
    out_ << "=== 상태별 주문량 ===\n";
    for (const auto& kv : counts) {
        out_ << toString(kv.first) << ": " << kv.second << "\n";
    }
}

void MonitoringView::showStockStatus(const std::vector<SampleStockInfo>& info) {
    out_ << "=== 시료별 재고 상태 ===\n";
    for (const auto& item : info) {
        out_ << item.sample.sampleId << " | " << item.sample.name
             << " | 재고: " << item.sample.stock
             << " | 대기수량: " << item.pendingQty
             << " | " << item.stockStatus << "\n";
    }
}

int MonitoringView::readMenuChoice() {
    int choice = 0;
    in_ >> choice;
    in_.ignore();
    return choice;
}
