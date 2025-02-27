#include <cstdint>
#include <ctime>
#include <spdlog/spdlog.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>

void UniformRandomNumber() {
    uint32_t numRand = 3;
    vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
    // randomSequence->SetSeed(8775070);
    randomSequence->SetSeed(time(NULL));

    for (uint32_t i = 0; i < numRand; ++i) {
        auto a = randomSequence->GetRangeValue(0.0, 2.0);
        randomSequence->Next();
        SPDLOG_INFO("{}", a);
    }
    std::cout << std::endl;
}