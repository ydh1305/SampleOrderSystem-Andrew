#include "OrderController.h"
#include "service/OrderApprovalService.h"
#include "view/ConsoleUtil.h"
#include <iomanip>
#include <sstream>

OrderController::OrderController(ISampleRepository& sampleRepo,
                                 IOrderRepository& orderRepo,
                                 IProductionJobRepository& jobRepo,
                                 std::ostream& out,
                                 std::istream& in)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), jobRepo_(jobRepo),
      out_(out), in_(in) {}

void OrderController::placeOrder() {
    out_ << "시료 ID: ";
    std::string sampleId;
    std::getline(in_, sampleId);

    auto sampleOpt = sampleRepo_.findById(sampleId);
    if (!sampleOpt.has_value()) {
        out_ << "존재하지 않는 시료입니다.\n";
        return;
    }

    out_ << "고객명: ";
    std::string customerName;
    std::getline(in_, customerName);

    out_ << "주문 수량: ";
    int quantity;
    in_ >> quantity;
    in_.ignore();

    out_ << "주문하시겠습니까? (Y/N): ";
    std::string confirm;
    std::getline(in_, confirm);

    if (confirm != "Y" && confirm != "y") {
        out_ << "취소되었습니다.\n";
        return;
    }

    std::string date = ConsoleUtil::getCurrentDate();
    int seq = orderRepo_.getNextSequence(date);

    std::ostringstream oss;
    oss << "ORD-" << date << "-" << std::setfill('0') << std::setw(4) << seq;
    std::string orderId = oss.str();

    Order order;
    order.orderId      = orderId;
    order.sampleId     = sampleId;
    order.customerName = customerName;
    order.quantity     = quantity;
    order.status       = OrderStatus::RESERVED;
    order.createdAt    = ConsoleUtil::getCurrentDateTime();
    order.updatedAt    = "";

    orderRepo_.save(order);
    out_ << "주문이 접수되었습니다: " << orderId << "\n";
}

void OrderController::processApproval() {
    auto reserved = orderRepo_.findByStatus(OrderStatus::RESERVED);
    if (reserved.empty()) {
        out_ << "승인 대기 중인 주문이 없습니다.\n";
        return;
    }

    out_ << "\n[승인 대기 주문 목록]\n";
    for (size_t i = 0; i < reserved.size(); ++i) {
        const auto& o = reserved[i];
        out_ << (i + 1) << ". " << o.orderId
             << "  " << o.sampleId
             << "  " << o.customerName
             << "  수량: " << o.quantity << "\n";
    }

    out_ << "번호 선택: ";
    int idx;
    in_ >> idx;
    in_.ignore();

    if (idx < 1 || idx > static_cast<int>(reserved.size())) {
        out_ << "잘못된 번호입니다.\n";
        return;
    }

    const std::string& orderId = reserved[idx - 1].orderId;

    out_ << "승인(A) / 거절(R): ";
    std::string action;
    std::getline(in_, action);

    OrderApprovalService svc(sampleRepo_, orderRepo_, jobRepo_);
    if (action == "A" || action == "a") {
        svc.approve(orderId);
        out_ << "승인 완료: " << orderId << "\n";
    } else if (action == "R" || action == "r") {
        svc.reject(orderId);
        out_ << "거절 완료: " << orderId << "\n";
    } else {
        out_ << "잘못된 입력입니다.\n";
    }
}

void OrderController::run() {
    while (true) {
        out_ << "\n[주문 관리]\n";
        out_ << "1. 주문 접수\n";
        out_ << "2. 주문 승인/거절\n";
        out_ << "0. 뒤로\n";
        out_ << "선택: ";

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) break;
        else if (choice == 1) placeOrder();
        else if (choice == 2) processApproval();
    }
}
