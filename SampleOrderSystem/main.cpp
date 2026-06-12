#include "controller/MainController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"
#include "util/DummyDataLoader.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--seed") {
        SampleRepository        sampleRepo("data/samples.json");
        OrderRepository         orderRepo("data/orders.json");
        ProductionJobRepository jobRepo("data/production_jobs.json");
        DummyDataLoader::load(sampleRepo, orderRepo, jobRepo);
        std::cout << "Demo data loaded.\n";
        return 0;
    }
    MainController().run();
    return 0;
}
