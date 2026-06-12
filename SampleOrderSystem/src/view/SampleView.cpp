#include "view/SampleView.h"

SampleView::SampleView(std::ostream& out, std::istream& in)
    : out_(out), in_(in) {}

void SampleView::showSubMenu() {
    out_ << "[1] 시료 등록\n"
         << "[2] 목록 조회\n"
         << "[3] 검색\n"
         << "[0] 뒤로\n";
}

void SampleView::showSampleList(const std::vector<Sample>& samples) {
    out_ << "=== 시료 목록 ===\n";
    for (const auto& s : samples) {
        out_ << s.sampleId << " | " << s.name
             << " | 재고: " << s.stock << "\n";
    }
}

void SampleView::showNoSamples() {
    out_ << "등록된 시료가 없습니다.\n";
}

void SampleView::showSampleDetail(const Sample& s) {
    out_ << "ID: " << s.sampleId << "\n"
         << "이름: " << s.name << "\n"
         << "평균 생산시간: " << s.avgProductionTime << " min/ea\n"
         << "수율: " << s.yieldRate << "\n"
         << "재고: " << s.stock << "\n";
}

void SampleView::showSearchResults(const std::vector<Sample>& samples) {
    showSampleList(samples);
}

void SampleView::showNoResults() {
    out_ << "검색 결과가 없습니다.\n";
}

void SampleView::showSaveSuccess(const std::string& id) {
    out_ << "등록 완료: " << id << "\n";
}

void SampleView::showError(const std::string& msg) {
    out_ << "오류: " << msg << "\n";
}

int SampleView::readMenuChoice() {
    int choice = 0;
    in_ >> choice;
    in_.ignore();
    return choice;
}

std::string SampleView::readName() {
    std::string name;
    std::getline(in_, name);
    return name;
}

std::string SampleView::readSampleId() {
    std::string id;
    std::getline(in_, id);
    return id;
}

double SampleView::readAvgProductionTime() {
    double val = 0.0;
    in_ >> val;
    in_.ignore();
    return val;
}

double SampleView::readYieldRate() {
    double val = 0.0;
    in_ >> val;
    in_.ignore();
    return val;
}

int SampleView::readStock() {
    int val = 0;
    in_ >> val;
    in_.ignore();
    return val;
}

char SampleView::readYesNo() {
    char c = 0;
    in_ >> c;
    in_.ignore();
    return c;
}
