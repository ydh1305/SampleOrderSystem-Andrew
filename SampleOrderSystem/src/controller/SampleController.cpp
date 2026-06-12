#include "SampleController.h"
#include <algorithm>
#include <iomanip>

SampleController::SampleController(ISampleRepository& repo,
                                   std::ostream& out,
                                   std::istream& in)
    : repo_(repo), out_(out), in_(in) {}

void SampleController::listSamples() {
    auto samples = repo_.findAll();
    if (samples.empty()) {
        out_ << "등록된 시료가 없습니다.\n";
        return;
    }
    out_ << std::left
         << std::setw(8)  << "ID"
         << std::setw(16) << "이름"
         << std::setw(12) << "생산시간(min)"
         << std::setw(10) << "수율"
         << std::setw(8)  << "재고"
         << "\n";
    out_ << std::string(54, '-') << "\n";
    for (const auto& s : samples) {
        out_ << std::left
             << std::setw(8)  << s.sampleId
             << std::setw(16) << s.name
             << std::setw(12) << s.avgProductionTime
             << std::setw(10) << s.yieldRate
             << std::setw(8)  << s.stock
             << "\n";
    }
}

void SampleController::addSample() {
    out_ << "시료 이름: ";
    std::string name;
    std::getline(in_, name);

    out_ << "평균 생산시간(min/ea): ";
    double avgTime;
    in_ >> avgTime;
    in_.ignore();

    out_ << "수율(0~1): ";
    double yieldRate;
    in_ >> yieldRate;
    in_.ignore();

    out_ << "초기 재고: ";
    int stock;
    in_ >> stock;
    in_.ignore();

    out_ << "저장하시겠습니까? (Y/N): ";
    std::string confirm;
    std::getline(in_, confirm);

    if (confirm != "Y" && confirm != "y") {
        out_ << "취소되었습니다.\n";
        return;
    }

    std::string sampleId = repo_.generateNextId();
    repo_.save({sampleId, name, avgTime, yieldRate, stock});
    out_ << "시료가 등록되었습니다: " << sampleId << "\n";
}

void SampleController::searchSamples() {
    out_ << "검색어: ";
    std::string keyword;
    std::getline(in_, keyword);

    std::string lower = keyword;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    auto all = repo_.findAll();
    bool found = false;
    for (const auto& s : all) {
        std::string nameLower = s.name;
        std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
        if (nameLower.find(lower) != std::string::npos) {
            out_ << s.sampleId << "  " << s.name << "  재고: " << s.stock << "\n";
            found = true;
        }
    }
    if (!found) {
        out_ << "검색 결과가 없습니다.\n";
    }
}

void SampleController::run() {
    while (true) {
        out_ << "\n[시료 관리]\n";
        out_ << "1. 시료 등록\n";
        out_ << "2. 시료 목록\n";
        out_ << "3. 시료 검색\n";
        out_ << "0. 뒤로\n";
        out_ << "선택: ";

        int choice;
        if (!(in_ >> choice)) break;
        in_.ignore();

        if (choice == 0) break;
        else if (choice == 1) addSample();
        else if (choice == 2) listSamples();
        else if (choice == 3) searchSamples();
    }
}
