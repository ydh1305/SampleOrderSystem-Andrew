#include "ProductionController.h"
#include "service/ProductionService.h"

ProductionController::ProductionController(ISampleRepository& sampleRepo,
                                           IOrderRepository& orderRepo,
                                           IProductionJobRepository& jobRepo,
                                           std::ostream& out,
                                           std::istream& in)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), jobRepo_(jobRepo),
      out_(out), in_(in) {}

void ProductionController::showProductionLine() {
    auto runningList = jobRepo_.findByStatus(JobStatus::RUNNING);
    auto waitingList = jobRepo_.findByStatus(JobStatus::WAITING);

    out_ << "\n[생산라인 현황]\n";
    out_ << "-- 생산 중 --\n";
    if (runningList.empty()) {
        out_ << "  (없음)\n";
    } else {
        const auto& j = runningList[0];
        out_ << "  " << j.jobId << "  주문: " << j.orderId
             << "  시료: " << j.sampleId
             << "  실생산량: " << j.actualProd
             << "  총시간: " << j.totalTime << "min\n";
    }

    out_ << "-- 대기 중 (" << waitingList.size() << "건) --\n";
    for (const auto& j : waitingList) {
        out_ << "  " << j.jobId << "  주문: " << j.orderId
             << "  시료: " << j.sampleId << "\n";
    }
}

void ProductionController::completeCurrentJob() {
    ProductionService svc(sampleRepo_, orderRepo_, jobRepo_);
    svc.completeCurrentJob();
    out_ << "현재 작업 완료 처리되었습니다.\n";
}

void ProductionController::run() {
    while (true) {
        out_ << "\n[생산라인]\n";
        out_ << "1. 생산라인 현황\n";
        out_ << "2. 현재 작업 완료\n";
        out_ << "0. 뒤로\n";
        out_ << "선택: ";

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) break;
        else if (choice == 1) showProductionLine();
        else if (choice == 2) completeCurrentJob();
    }
}
