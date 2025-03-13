#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkAxisActor.h>
#include <vtkProperty.h>
#include <vtkStringArray.h>
#include <vtkTextProperty.h>
// #include <vtkLabelProperty.h>
void AxisActor() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkSphereSource> source;
    source->SetPhiResolution(31);
    source->SetThetaResolution(31);
    source->Update();
    double bounds[6];
    source->GetOutput()->GetBounds(bounds);
    SPDLOG_INFO("Bounds: {}, {}, {}, {}, {}, {}", bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
    double center[3];
    source->GetOutput()->GetCenter(center);
    SPDLOG_INFO("Center: {}, {}, {}", center[0], center[1], center[2]);

    // create the axis actor
    vtkNew<vtkAxisActor> axis;
    axis->SetPoint1(-1.1, 0.0, 0.0);
    axis->SetPoint2(1.1, 0.0, 0.0);
    axis->SetTickLocationToBoth();
    axis->SetAxisTypeToX();
    axis->SetTitle("A Sphere");
    axis->SetTitleScale(0.2);
    axis->TitleVisibilityOn();

    // axis->SetTickVisibility(false);
    axis->SetMajorTickSize(0.05);
    axis->SetMinorTickSize(1);
    // axis->DrawGridlinesOff();
    axis->GetTitleTextProperty()->SetColor(colors->GetColor3d("banana").GetData());
    axis->GetLabelTextProperty()->SetColor(colors->GetColor3d("orange").GetData());

    vtkNew<vtkStringArray> labels;
    labels->SetNumberOfTuples(1);
    labels->SetValue(0, "x Axis");

    axis->SetLabels(labels);
    axis->SetLabelScale(0.1);
    axis->MinorTicksVisibleOn();
    axis->SetDeltaMajor(0, 0.1);
    axis->SetCalculateTitleOffset(0);
    axis->SetCalculateLabelOffset(0);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(source->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetDiffuseColor(colors->GetColor3d("Tomato").GetData());

    // create the renderwindow, renderer and bot actors
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("AxisActor");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);

    axis->SetCamera(renderer->GetActiveCamera());

    renderer->AddActor(actor);
    renderer->AddActor(axis);

    renderer->SetBackground(colors->GetColor4d("SlateGray").GetData());
    renderWindow->SetSize(640,480);
    renderer->ResetCamera();
    renderer->ResetCameraClippingRange();

    // render the image
    renderWindow->Render();

    interactor->Initialize();
    interactor->Start();
}
