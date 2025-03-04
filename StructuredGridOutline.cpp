
#include <cstdint>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkProperty.h>
void StructuredGridOutline() {
    vtkNew<vtkNamedColors> colors;
    // create a grid
    vtkNew<vtkStructuredGrid> structuredGrid;

    vtkNew<vtkPoints> points;
    uint32_t numi = 2;
    uint32_t numj = 2;
    uint32_t numk = 2;
    for (uint32_t k = 0; k < numk; ++k) {
        for (uint32_t j = 0; j < numj; ++j) {
            for (uint32_t i = 0; i < numi; ++i) {
                points->InsertNextPoint(i,j,k);
            }
        }
    }
    // specify the dimensions of the grid
    structuredGrid->SetDimensions(numi, numj, numk);
    structuredGrid->SetPoints(points);

    SPDLOG_INFO("There are {} points", structuredGrid->GetNumberOfPoints());
    SPDLOG_INFO("There are {} cells", structuredGrid->GetNumberOfCells());

    vtkNew<vtkStructuredGridOutlineFilter> outlineFilter;
    outlineFilter->SetInputData(structuredGrid);
    outlineFilter->Update();

    // create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(outlineFilter->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // visualize
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("StructuredGridOutline");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();
}
