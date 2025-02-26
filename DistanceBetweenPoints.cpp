#include <cmath>
#include <vtkMath.h>
#include <spdlog/spdlog.h>

void DistanceBetweenPoints() {
    double p0[3] = {0, 0, 0};
    double p1[3] = {1, 1, 1};
    // 平方距离 Squared distance
    double squareDistance = vtkMath::Distance2BetweenPoints(p0, p1);
    // 欧几里德距离 Euclidean distance
    double distance = std::sqrt(squareDistance);
    // output
    SPDLOG_INFO("SquareDistance = {}", squareDistance);
    SPDLOG_INFO("Distance = {}\n", distance);
}
