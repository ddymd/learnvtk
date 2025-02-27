#include <cstdint>
#include <spdlog/spdlog.h>
#include <vtkNew.h>
#include <vtkBoxMuellerRandomSequence.h>
// This example generates three random numbers from a Gaussian distribution
// with mean 0.0 and standard deviation 2.0
void GaussianRandomNumber() {
    uint32_t numRand = 3;
    vtkNew<vtkBoxMuellerRandomSequence> randomSequence;

    auto mean = 0.0;
    auto standardDeviation = 2.0;
    for (uint32_t i = 0; i < numRand; ++i) {
        auto a = randomSequence->GetScaledValue(mean, standardDeviation);
        randomSequence->Next();
        // std::cout << std::fixed << std::setw(9) << std::setprecision(6) << a << std::endl;
        SPDLOG_INFO("{:>9.6f}", a);
    }
    std::cout << std::endl;
}