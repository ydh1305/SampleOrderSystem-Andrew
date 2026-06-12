#include "ReleaseController.h"
#include "view/ConsoleUtil.h"

ReleaseController::ReleaseController(IOrderRepository& orderRepo,
                                     std::ostream& out,
                                     std::istream& in)
    : orderRepo_(orderRepo), out_(out), in_(in) {}

void ReleaseController::processRelease() {
    auto confirmed = orderRepo_.findByStatus(OrderStatus::CONFIRMED);
    if (confirmed.empty()) {
        out_ << "출고 가능한 주문이 없습니다.\n";
        return;
    }

    out_ << "\n[출고 가능 주문 목록]\n";
    for (size_t i = 0; i < confirmed.size(); ++i) {
        const auto& o = confirmed[i];
        out_ << (i + 1) << ". " << o.orderId
             << "  " << o.sampleId
             << "  " << o.customerName
             << "  수량: " << o.quantity << "\n";
    }

    out_ << "번호 선택: ";
    int idx;
    in_ >> idx;
    in_.ignore();

    if (idx < 1 || idx > static_cast<int>(confirmed.size())) {
        out_ << "잘못된 번호입니다.\n";
        return;
    }

    out_ << "출고 처리하시겠습니까? (Y/N): ";
    std::string confirm;
    std::getline(in_, confirm);

    if (confirm != "Y" && confirm != "y") {
        out_ << "취소되었습니다.\n";
        return;
    }

    Order order = confirmed[idx - 1];
    order.status    = OrderStatus::RELEASED;
    order.updatedAt = ConsoleUtil::getCurrentDateTime();
    orderRepo_.update(order);
    out_ << "출고 처리 완료: " << order.orderId << "\n";
}

void ReleaseController::run() {
    while (true) {
        out_ << "\n[출고 처리]\n";
        out_ << "1. 출고 처리\n";
        out_ << "0. 뒤로\n";
        out_ << "선택: ";

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) break;
        else if (choice == 1) processRelease();
    }
}
