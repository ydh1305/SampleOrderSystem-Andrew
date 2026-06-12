#include "view/ReleaseView.h"

ReleaseView::ReleaseView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void ReleaseView::showConfirmedOrders(const std::vector<Order>& orders) {
    out_ << "=== 출고 대기 주문 ===\n";
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& o = orders[i];
        out_ << "[" << (i + 1) << "] " << o.orderId
             << " | " << o.customerName
             << " | 수량: " << o.quantity << "\n";
    }
}

void ReleaseView::showNoConfirmedOrders() {
    out_ << "출고 대기 중인 주문이 없습니다.\n";
}

void ReleaseView::showReleaseSuccess(const Order& o) {
    out_ << "출고 완료: " << o.orderId << "\n";
}

void ReleaseView::showError(const std::string& msg) {
    out_ << "오류: " << msg << "\n";
}

int ReleaseView::readOrderIndex() {
    int idx = 0;
    in_ >> idx;
    in_.ignore();
    return idx;
}

char ReleaseView::readYesNo() {
    char c = 0;
    in_ >> c;
    in_.ignore();
    return c;
}
