#include <spdlog/spdlog.h>
#include <cstdint>
#include <vtkMapper.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkPlane.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

void BlankPoint() {
    vtkNew<vtkNamedColors> colors;
    // create grid
    vtkNew<vtkStructuredGrid> structuredGrid;

    vtkNew<vtkPoints> points;
    uint32_t gridSize = 8, counter = 0, ptIdx = 0;
    // create 5x5 grid of points
    for (uint32_t j = 0; j < gridSize; ++j) {
        for (uint32_t i = 0; i < gridSize; ++i) {
            // make one point higher than the rest
            if (i == 3 && j == 3) {
                points->InsertNextPoint(i, j, 2);
                SPDLOG_INFO("The different point is number {}", counter);
                ptIdx = counter;
            } else {
                points->InsertNextPoint(i, j, 0);
            }
            counter++;
        }
    }
    // specify the dimensions of the grid, set the points and blank one point
    structuredGrid->SetDimensions(gridSize, gridSize, 1 );
    structuredGrid->SetPoints(points);
    structuredGrid->BlankPoint(ptIdx);
    structuredGrid->Modified();

    // check
    auto isVisible = [&structuredGrid](int const& ptNum) {
        if (structuredGrid->IsPointVisible(ptNum)) {
            SPDLOG_INFO("Point: {} is visible", ptNum);
        } else {
            SPDLOG_INFO("Point: {} is not visible", ptNum);
        }
    };

    // should not be visible
    isVisible(ptIdx);
    // should be visible
    isVisible(7);

    // we need the geometry filter to ensure that the
    // blanked point and surrounding faces is missing
    vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
    geometryFilter->SetInputData(structuredGrid);

    // create a mapper and actor
    vtkNew<vtkDataSetMapper> gridMapper;
    // gridMapper->SetInputData(structuredGrid);
    gridMapper->SetInputConnection(geometryFilter->GetOutputPort());

    vtkNew<vtkActor> gridActor;
    gridActor->SetMapper(gridMapper);
    gridActor->GetProperty()->EdgeVisibilityOn();
    gridActor->GetProperty()->SetEdgeColor(colors->GetColor3d("Blue").GetData());

    // create a renderer, render window, and interactor
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BlankPoint");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // add the actor to the scene
    renderer->AddActor(gridActor);
    renderer->SetBackground(colors->GetColor3d("ForestGreen").GetData());

    // render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();
}