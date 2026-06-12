#include "MonitoringController.h"
#include <map>
#include <string>

MonitoringController::MonitoringController(ISampleRepository& sampleRepo,
                                           IOrderRepository& orderRepo,
                                           std::ostream& out,
                                           std::istream& in)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), out_(out), in_(in) {}

void MonitoringController::showOrderCounts() {
    auto orders = orderRepo_.findAll();

    std::map<std::string, int> counts;
    counts["RESERVED"]  = 0;
    counts["PRODUCING"] = 0;
    counts["CONFIRMED"] = 0;
    counts["RELEASED"]  = 0;

    for (const auto& o : orders) {
        switch (o.status) {
        case OrderStatus::RESERVED:  counts["RESERVED"]++;  break;
        case OrderStatus::PRODUCING: counts["PRODUCING"]++; break;
        case OrderStatus::CONFIRMED: counts["CONFIRMED"]++; break;
        case OrderStatus::RELEASED:  counts["RELEASED"]++;  break;
        case OrderStatus::REJECTED:  /* REJECTED 제외 */     break;
        }
    }

    out_ << "\n[상태별 주문 현황]\n";
    out_ << "RESERVED : " << counts["RESERVED"]  << "\n";
    out_ << "PRODUCING: " << counts["PRODUCING"] << "\n";
    out_ << "CONFIRMED: " << counts["CONFIRMED"] << "\n";
    out_ << "RELEASED : " << counts["RELEASED"]  << "\n";
}

void MonitoringController::showStockStatus() {
    auto samples = sampleRepo_.findAll();
    auto orders  = orderRepo_.findAll();

    out_ << "\n[시료별 재고 현황]\n";
    for (const auto& s : samples) {
        int reserved  = 0;
        int confirmed = 0;
        for (const auto& o : orders) {
            if (o.sampleId != s.sampleId) continue;
            if (o.status == OrderStatus::RESERVED)  reserved  += o.quantity;
            if (o.status == OrderStatus::CONFIRMED) confirmed += o.quantity;
        }
        out_ << s.sampleId << "  " << s.name
             << "  재고: " << s.stock
             << "  RESERVED: " << reserved
             << "  CONFIRMED: " << confirmed
             << "\n";
    }
    if (samples.empty()) {
        out_ << "등록된 시료가 없습니다.\n";
    }
}

void MonitoringController::run() {
    while (true) {
        out_ << "\n[모니터링]\n";
        out_ << "1. 상태별 주문 현황\n";
        out_ << "2. 시료별 재고 현황\n";
        out_ << "0. 뒤로\n";
        out_ << "선택: ";

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) break;
        else if (choice == 1) showOrderCounts();
        else if (choice == 2) showStockStatus();
    }
}
