#include <vtkActor.h>
#include <vtkConeSource.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

/**
 * How to use multiple renderers within a render window
 */
void tutorial3() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(coneActor);
    ren1->SetBackground(colors->GetColor3d("RoyalBlue").GetData());
    ren1->SetViewport(0.0, 0.0, 0.5, 1.0);

    vtkNew<vtkRenderer> ren2;
    ren2->AddActor(coneActor);
    ren2->SetBackground(colors->GetColor3d("DodgerBlue").GetData());
    ren2->SetViewport(0.5, 0.0, 1.0, 1.0);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->AddRenderer(ren2);
    renWin->SetSize(600, 300);
    renWin->SetWindowName("Tutorial Step3");

    ren1->ResetCamera();
    ren1->GetActiveCamera()->Azimuth(90);

    for (int i = 0; i < 360; ++i) {
        renWin->Render();
        ren1->GetActiveCamera()->Azimuth(1);
        ren2->GetActiveCamera()->Azimuth(1);
    }
}
