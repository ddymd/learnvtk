
#include <cstdint>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointerBase.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkRenderWindow.h>

namespace {
vtkSmartPointer<vtkActor> PointToGlyph(vtkPoints* points, double const& scale) {
    auto bounds = points->GetBounds();

    // for (int i = 0; i < 6; ++i) {
    //     std::cout << bounds[i] << std::endl;
    // }

    double maxLen = 0;
    for (int i = 0; i < 3; ++i) {
        maxLen = std::max(bounds[i+1]-bounds[i], maxLen);
    }
    vtkNew<vtkSphereSource> sphereSource;
    sphereSource->SetRadius(scale*maxLen);

    vtkNew<vtkPolyData> pd;
    pd->SetPoints(points);

    vtkNew<vtkGlyph3DMapper> mapper;
    mapper->SetInputData(pd);
    mapper->SetSourceConnection(sphereSource->GetOutputPort());
    mapper->ScalarVisibilityOff();
    mapper->ScalingOff();

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    return actor;
}
}

void VisualizeStructuredGrid() {
    vtkNew<vtkNamedColors> colors;
    //create a grid
    vtkNew<vtkStructuredGrid> structuredGrid;
    vtkNew<vtkPoints> points;
    uint32_t numi = 2;
    uint32_t numj = 3;
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
    SPDLOG_INFO("There are {} points.", structuredGrid->GetNumberOfPoints());
    SPDLOG_INFO("There are {} cells.", structuredGrid->GetNumberOfCells());

    vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
    geometryFilter->SetInputData(structuredGrid);
    geometryFilter->Update();

    // create a mapper and actor
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(geometryFilter->GetOutputPort());
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(1);

    // visualize
    // map the points to spheres
    auto sphereActor = PointToGlyph(geometryFilter->GetOutput()->GetPoints(), 0.02);
    sphereActor->GetProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("VisualizeStructedGrid");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->AddActor(sphereActor);

    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());
    renderWindow->Render();
    renderWindowInteractor->Start();
}
