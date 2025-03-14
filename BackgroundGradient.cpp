#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
void BackgroundGradient() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkSphereSource> source;
    source->Update();

    // create mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());

    // create renderer, renderwindow and renderwindow interactor
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->GradientBackgroundOn();
    renderer->SetBackground(colors->GetColor3d("Banana").GetData());
    renderer->SetBackground2(colors->GetColor3d("Tomato").GetData());
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("BackgroundGradient");
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    renWin->Render();
    iren->Start();
}
