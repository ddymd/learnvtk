#include <vtkFloatingPointExceptions.h>
#include <spdlog/spdlog.h>

#ifdef _MSC_VER
#pragma warning(disable : 4723)
#endif

void FloatingPointExceptions() {
    vtkFloatingPointExceptions::Enable();
    double x = 0.0;
    double y = 1.0 / x;
    // std::cout << "x: " << x << " y: " << y << std::endl;
    SPDLOG_INFO("x: {} y: {}\n", x, y);

}