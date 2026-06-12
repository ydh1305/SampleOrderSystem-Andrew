#include "view/ProductionView.h"

ProductionView::ProductionView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void ProductionView::showNoRunningJob() {
    out_ << "현재 진행 중인 생산 작업이 없습니다.\n";
}

void ProductionView::showRunningJob(const ProductionJob& job, const Sample& s) {
    out_ << "=== 진행 중인 작업 ===\n"
         << "작업ID: " << job.jobId << "\n"
         << "시료: " << s.name << "\n"
         << "실 생산량: " << job.actualProd << "\n"
         << "총 생산시간: " << job.totalTime << " min\n"
         << "시작: " << job.startedAt << "\n";
}

void ProductionView::showWaitingQueue(const std::vector<ProductionJob>& queue) {
    out_ << "=== 대기 큐 ===\n";
    for (size_t i = 0; i < queue.size(); ++i) {
        out_ << "[" << (i + 1) << "] " << queue[i].jobId
             << " | 주문: " << queue[i].orderId << "\n";
    }
}

void ProductionView::showCompleteMenu(bool hasRunningJob) {
    if (hasRunningJob) {
        out_ << "[C] 생산 완료 처리\n";
    }
    out_ << "[0] 뒤로\n";
}

void ProductionView::showCompleteSuccess(const ProductionJob& job) {
    out_ << "생산 완료: " << job.jobId << "\n";
}

void ProductionView::showError(const std::string& msg) {
    out_ << "오류: " << msg << "\n";
}

char ProductionView::readCommand() {
    char c = 0;
    in_ >> c;
    in_.ignore();
    return c;
}
