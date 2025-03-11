#include <spdlog/spdlog.h>
template<typename T>
void PrintColor(T &rgb) {
    for (size_t i = 0; i < 3; ++i) {
        if (i < 2) {
            SPDLOG_INFO("{:f} ", static_cast<double>(rgb[i]));
        } else {
            SPDLOG_INFO("{:f}", static_cast<double>(rgb[i]));
        }
    }
}

void AssignCellColorsFromLUT() {

}
