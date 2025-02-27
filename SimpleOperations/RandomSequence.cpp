#include <spdlog/spdlog.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>
void RandomSequence() {
    // create a random sequence generator
    vtkNew<vtkMinimalStandardRandomSequence> sequence;
    // initialize the sequence
    sequence->SetSeed(1);

    // Get 3 random numbers
    double x = sequence->GetRangeValue(-1.0, 1.0);
    // double x = sequence->GetValue();
    sequence->Next();
    double y = sequence->GetRangeValue(-1.0, 1.0);
    // double y = sequence->GetValue();
    sequence->Next();
    double z = sequence->GetRangeValue(-1.0, 1.0);
    // double z = sequence->GetValue();

    SPDLOG_INFO("x: {} y: {} z: {}\n", x, y, z);
}
