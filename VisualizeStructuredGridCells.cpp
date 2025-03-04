#include <cstdint>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkShrinkFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
void VisualizeStructuredGridCells() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkStructuredGrid> structuredGrid;
    vtkNew<vtkPoints> points;
    uint32_t numi = 3;
    uint32_t numj = 4;
    uint32_t numk = 5;
    for (uint32_t k = 0; k < numk; ++k) {
        for (uint32_t j = 0; j < numj; ++j) {
            for (uint32_t i = 0; i < numi; ++i) {
                points->InsertNextPoint(i,j,k);
            }
        }
    }
    structuredGrid->SetDimensions(numi, numj, numk);
    structuredGrid->SetPoints(points);
    SPDLOG_INFO("There are {} points before shrinking.", structuredGrid->GetNumberOfPoints());
    SPDLOG_INFO("There are {} cells before shrinking.", structuredGrid->GetNumberOfCells());

    vtkNew<vtkShrinkFilter> shrinkFilter;
    shrinkFilter->SetInputData(structuredGrid);
    shrinkFilter->SetShrinkFactor(0.8);
    shrinkFilter->Update();

    SPDLOG_INFO("There are {} points after shrinking.", shrinkFilter->GetOutput()->GetNumberOfPoints());
    SPDLOG_INFO("There are {} cells after shrinking.", shrinkFilter->GetOutput()->GetNumberOfCells());

    // Note: there are more points after shrinking because cells no longer share points
    // create a mapper and actor
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputConnection(shrinkFilter->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    // create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("VisualizeStructuredGridCells");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // add the actor to the scene
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    // render and interact
    renderWindow->Render();
    auto camera = renderer->GetActiveCamera();
    camera->SetPosition(9.56681, 4.14093, -0.26565);
    camera->SetFocalPoint(1, 1.5, 2);
    camera->SetViewUp(-0.172781, 0.94846,-0.26565);
    camera->SetDistance(9.7592);
    camera->SetClippingRange(5.84116, 14.7147);

    renderWindowInteractor->Start();
}
