#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
void BackfaceCulling() {
    vtkNew<vtkNamedColors> colors;
    // sphere
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());

    vtkNew<vtkProperty> backProperty;
    backProperty->SetColor(colors->GetColor3d("Gold").GetData());
    actor->SetBackfaceProperty(backProperty);
    // since back face culling is on, when you zoom into the sphere, you won't see the gold colored backface
    actor->GetProperty()->BackfaceCullingOn();

    // visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BackfaceCulling");

    vtkNew<vtkRenderWindowInteractor> renderWIndowInteractor;
    renderWIndowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("MistyRose").GetData());

    renderWindow->Render();
    renderWIndowInteractor->Start();
}
