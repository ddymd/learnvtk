#include <vtkLine.h>
#include <spdlog/spdlog.h>

void DistancePointToLine() {
    double lineP0[3] = {0, 0, 0};
    double lineP1[3] = {2, 0, 0};

    double p0[3] = {1, 0, 0};
    double p1[3] = {1, 2, 0};
    {
        double dist0 = vtkLine::DistanceToLine(p0, lineP0, lineP1);
        SPDLOG_INFO("Dist0: {}", dist0);
        double dist1 = vtkLine::DistanceToLine(p1, lineP0, lineP1);
        SPDLOG_INFO("Dist1: {}", dist1);
    }

    {
        double t, closest[3];
        double dist0 = vtkLine::DistanceToLine(p0, lineP0, lineP1, t, closest);
        SPDLOG_INFO("Dist0: {} closet point: [{}, {}, {}] t: {}", dist0, closest[0], closest[1], closest[2], t);

        double dist1 = vtkLine::DistanceToLine(p1, lineP0, lineP1, t, closest);
        SPDLOG_INFO("Dist1: {} closet point: [{}, {}, {}] t: {}\n", dist1, closest[0], closest[1], closest[2], t);
    }
}