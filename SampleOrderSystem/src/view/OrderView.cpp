#include "view/OrderView.h"
#include "model/OrderStatus.h"

OrderView::OrderView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void OrderView::showReservedOrders(const std::vector<Order>& orders) {
    out_ << "=== 대기 주문 목록 ===\n";
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];
        out_ << "[" << (i + 1) << "] " << o.orderId
             << " | " << o.customerName
             << " | 수량: " << o.quantity << "\n";
    }
}

void OrderView::showNoReservedOrders() {
    out_ << "대기 중인 주문이 없습니다.\n";
}

void OrderView::showOrderList(const std::vector<Order>& orders) {
    out_ << "=== 주문 목록 ===\n";
    for (const auto& o : orders) {
        out_ << o.orderId << " | " << o.customerName
             << " | " << toString(o.status) << "\n";
    }
}

void OrderView::showOrderDetail(const Order& o) {
    out_ << "주문번호: " << o.orderId << "\n"
         << "시료: " << o.sampleId << "\n"
         << "고객: " << o.customerName << "\n"
         << "수량: " << o.quantity << "\n"
         << "상태: " << toString(o.status) << "\n";
}

void OrderView::showOrderCreated(const Order& o) {
    out_ << "주문 접수 완료: " << o.orderId << "\n";
}

void OrderView::showApprovalResult(const Order& o) {
    out_ << "승인 완료: " << o.orderId
         << " [" << toString(o.status) << "]\n";
}

void OrderView::showRejectResult(const Order& o) {
    out_ << "거절 완료: " << o.orderId << "\n";
}

void OrderView::showError(const std::string& msg) {
    out_ << "오류: " << msg << "\n";
}

int OrderView::readOrderIndex() {
    int idx = 0;
    in_ >> idx;
    in_.ignore();
    return idx;
}

std::string OrderView::readSampleId() {
    std::string id;
    std::getline(in_, id);
    return id;
}

std::string OrderView::readCustomerName() {
    std::string name;
    std::getline(in_, name);
    return name;
}

int OrderView::readQuantity() {
    int qty = 0;
    in_ >> qty;
    in_.ignore();
    return qty;
}

char OrderView::readYesNo() {
    char c = 0;
    in_ >> c;
    in_.ignore();
    return c;
}
