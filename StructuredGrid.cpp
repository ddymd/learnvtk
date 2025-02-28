#include <cstdint>
#include <fmt/format.h>
#include <iterator>
#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkIdList.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkType.h>
#include <vtkCellIterator.h>
#include <vtkLookupTable.h>
#include <vtkCamera.h>

void StructuredGrid() {
    vtkNew<vtkNamedColors> colors;
    size_t nx = 2, ny = 3, nz = 2;
    auto dataSize = nx * ny * nz;

    vtkNew<vtkDoubleArray> pointValues;
    pointValues->SetNumberOfComponents(1);
    pointValues->SetNumberOfTuples(dataSize);
    for (size_t i = 0; i < dataSize; ++i) {
        pointValues->SetValue(i, i);
    }
    auto numberOfCells = (nx-1)*(ny-1)*(nz-1);
    vtkNew<vtkDoubleArray> cellValues;
    cellValues->SetNumberOfTuples(numberOfCells);
    for (size_t i = 0; i < numberOfCells; ++i) {
        cellValues->SetValue(i, i);
    }

    vtkNew<vtkPoints> points;
    double x = 0.0, y = 0.0, z = 0.0;
    for (unsigned int k = 0; k < nz; ++k) {
        z += 2.0;
        for (unsigned int j = 0; j < ny; ++j) {
            y += 1.0;
            for (unsigned int i = 0; i < nx; ++i) {
                x += 0.5;
                points->InsertNextPoint(x, y, z);
            }
        }
    }

    // create a grid
    vtkNew<vtkStructuredGrid> structuredGrid;
    // specify the dimensions of the grid
    structuredGrid->SetDimensions(static_cast<int32_t>(nx), static_cast<int32_t>(ny), static_cast<int32_t>(nz));
    structuredGrid->SetPoints(points);
    structuredGrid->GetCellData()->SetScalars(cellValues);
    structuredGrid->GetPointData()->SetScalars(pointValues);

    // the key is the cell id and the value is a set of corresponding point ids.
    std::map<vtkIdType, std::set<vtkIdType>> cellPointIds;
    vtkCellIterator *it = structuredGrid->NewCellIterator();
    for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextCell()) {
        vtkIdList *pointIds = it->GetPointIds();
        std::set<vtkIdType> ptIds;
        for (vtkIdType *id = pointIds->begin(); id != pointIds->end(); ++id) {
            ptIds.insert(*id);
        }
        cellPointIds[it->GetCellId()] = ptIds;
    }
    it->Delete();

    auto out = fmt::memory_buffer();
    fmt::format_to(std::back_inserter(out), "Cells and their points\n");
    for (auto const& cell: cellPointIds) {
        fmt::format_to(std::back_inserter(out), "Cell Id: {} Point Ids: ", cell.first);
        for (auto id = cell.second.begin(); id != cell.second.end(); ++id) {
            if (id != std::prev(cell.second.end())) {
                fmt::format_to(std::back_inserter(out), "{}, ", *id);
            } else {
                fmt::format_to(std::back_inserter(out), "{}\n", *id);
            }
        }
    }
    SPDLOG_INFO("{}", fmt::to_string(out));
    out.clear();

    // the key is the point Id and the value is a set of corresponding cell Ids
    std::map<vtkIdType, std::set<vtkIdType>> commonPointIds; // = cellPointIds.begin()->second;
    for (auto const& cell : cellPointIds) {
        for (auto pointId: cell.second) {
            commonPointIds[pointId].insert(cell.first);
        }
    }
    fmt::format_to(std::back_inserter(out), "Point Ids shared between cells\n");
    for (auto point = commonPointIds.begin(); point != commonPointIds.end(); ++point) {
        if (point->second.size() > 1) {
            fmt::format_to(std::back_inserter(out), "Point Id: {} CellIds: ", point->first);
            for (auto cellId = point->second.begin(); cellId != point->second.end(); ++cellId) {
                if (cellId != std::prev(point->second.end())) {
                    fmt::format_to(std::back_inserter(out), "{}, ", *cellId);
                } else {
                    fmt::format_to(std::back_inserter(out), "{}\n", *cellId);
                }
            }
        }
    }
    SPDLOG_INFO("{}", fmt::to_string(out));

    // use the point data
    // map the scalar values in the image to colors with a lookup table
    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(dataSize);
    lut->Build();

    // create a mapper and actor
    vtkNew<vtkDataSetMapper> mapper;
    mapper->SetInputData(structuredGrid);
    mapper->SetLookupTable(lut);
    mapper->SetScalarRange(0, dataSize-1);
    mapper->ScalarVisibilityOn();


    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    // create a renderer, render window, and interactor
    vtkNew<vtkRenderer> ren;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren);
    vtkNew<vtkRenderWindowInteractor> iRen;
    iRen->SetRenderWindow(renWin);

    // add the actor to the scene
    ren->AddActor(actor);
    ren->SetBackground(colors->GetColor3d("SlateGray").GetData());

    ren->GetActiveCamera()->Azimuth(60);
    ren->GetActiveCamera()->Elevation(-45);
    ren->GetActiveCamera()->Dolly(1.2);
    ren->ResetCamera();

    // render and interact
    renWin->Render();
    renWin->SetSize(800, 800);
    renWin->SetWindowName("StructuredGrid");
    iRen->Start();
}