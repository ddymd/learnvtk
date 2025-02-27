
// Projects a point onto a plane
#include <vtkNew.h>
#include <vtkPlane.h>
#include <spdlog/spdlog.h>
void ProjectPointPlane() {
    vtkNew<vtkPlane> plane;
    plane->SetOrigin(0,0,0);
    plane->SetNormal(0,0,1);

    double p[3] = {23.1, 54.6, 9.2};
    double origin[3] = {0, 0, 0};
    double normal[3] = {0, 0, 1};
    double projected[3];

    plane->ProjectPoint(p, origin, normal, projected);

    SPDLOG_INFO("Projected: {} {} {}\n", projected[0], projected[1], projected[2]);
}
