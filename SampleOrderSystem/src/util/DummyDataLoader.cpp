#include "util/DummyDataLoader.h"

void DummyDataLoader::load(ISampleRepository&        sampleRepo,
                            IOrderRepository&         orderRepo,
                            IProductionJobRepository& jobRepo) {
    // {sampleId, name, avgProductionTime, yieldRate, stock}
    sampleRepo.save({"S-001", "Si-Wafer-8inch",    0.8, 0.92, 480});
    sampleRepo.save({"S-002", "GaN-Epitaxial",     0.3, 0.88, 220});
    sampleRepo.save({"S-003", "SiC-PowerSubstrate",0.8, 0.92, 30});
    sampleRepo.save({"S-004", "Oxide-Wafer",       0.6, 0.95, 0});

    orderRepo.save({"ORD-20260612-0001", "S-001", "Alice",  50, OrderStatus::RESERVED,  "2026-06-12", ""});
    orderRepo.save({"ORD-20260612-0002", "S-001", "Bob",    30, OrderStatus::RESERVED,  "2026-06-12", ""});
    orderRepo.save({"ORD-20260612-0003", "S-002", "Carol",  10, OrderStatus::RESERVED,  "2026-06-12", ""});
    orderRepo.save({"ORD-20260612-0004", "S-001", "Dave",  100, OrderStatus::CONFIRMED, "2026-06-12", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0005", "S-002", "Eve",    80, OrderStatus::CONFIRMED, "2026-06-12", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0006", "S-003", "Frank", 200, OrderStatus::PRODUCING, "2026-06-12", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0007", "S-004", "Grace",  20, OrderStatus::PRODUCING, "2026-06-12", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0008", "S-001", "Heidi", 300, OrderStatus::RELEASED,  "2026-06-10", "2026-06-11"});
    orderRepo.save({"ORD-20260612-0009", "S-002", "Ivan",   50, OrderStatus::RELEASED,  "2026-06-10", "2026-06-11"});
    orderRepo.save({"ORD-20260612-0010", "S-003", "Judy",   25, OrderStatus::CONFIRMED, "2026-06-11", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0011", "S-004", "Karl",   10, OrderStatus::PRODUCING, "2026-06-12", "2026-06-12"});
    orderRepo.save({"ORD-20260612-0012", "S-001", "Laura",  75, OrderStatus::RELEASED,  "2026-06-09", "2026-06-10"});

    jobRepo.save({"JOB-001", "ORD-20260612-0006", "S-003", 170, 206, 164.8,
                  JobStatus::RUNNING, "2026-06-12 09:00:00", "2026-06-12 09:05:00", ""});
    jobRepo.save({"JOB-002", "ORD-20260612-0007", "S-004",  20,  34,  27.2,
                  JobStatus::WAITING, "2026-06-12 09:10:00", "", ""});
    jobRepo.save({"JOB-003", "ORD-20260612-0011", "S-004",  10,  17,  13.6,
                  JobStatus::WAITING, "2026-06-12 09:20:00", "", ""});
}
