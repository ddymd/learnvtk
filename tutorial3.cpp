#include <vtkConeSource.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>

void tutorial3() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());
}
