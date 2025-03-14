#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
void BackgroundColor() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkSphereSource> source;
    source->SetCenter(0.0,0.0,0.0);
    source->SetRadius(4.0);
    source->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("SeaGreen").GetData());
    
    // Visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BackgroundColor");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("Plum").GetData());

    renderWindow->Render();
    iren->Start();
}
