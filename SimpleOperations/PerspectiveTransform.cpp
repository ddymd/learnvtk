#include <vtkNew.h>
#include <vtkMatrix4x4.h>
#include <vtkPerspectiveTransform.h>
#include <vtkTransform.h>
#include <spdlog/spdlog.h>
void PerspectiveTransform() {
    vtkNew<vtkMatrix4x4> m;
    m->SetElement(0, 0, 1);
    m->SetElement(0, 1, 2);
    m->SetElement(0, 2, 3);
    m->SetElement(0, 3, 4);

    m->SetElement(1, 0, 2);
    m->SetElement(1, 1, 2);
    m->SetElement(1, 2, 3);
    m->SetElement(1, 3, 4);

    m->SetElement(2, 0, 3);
    m->SetElement(2, 1, 2);
    m->SetElement(2, 2, 3);
    m->SetElement(2, 3, 4);

    m->SetElement(3, 0, 4);
    m->SetElement(3, 1, 2);
    m->SetElement(3, 2, 3);
    m->SetElement(3, 3, 4);

    vtkNew<vtkPerspectiveTransform> perspectiveTransform;
    perspectiveTransform->SetMatrix(m);

    vtkNew<vtkTransform> transform;
    transform->SetMatrix(m);

    double p[3];
    p[0] = 1.0;
    p[1] = 2.0;
    p[2] = 3.0;

    double normalProjection[3];
    transform->TransformPoint(p, normalProjection);
    SPDLOG_INFO("Standard projection: {} {} {}", normalProjection[0], normalProjection[1], normalProjection[2]);

    double perspectiveProjection[3];
    perspectiveTransform->TransformPoint(p, perspectiveProjection);
    SPDLOG_INFO("Standard projection: {} {} {}\n", perspectiveProjection[0], perspectiveProjection[1], perspectiveProjection[2]);
}