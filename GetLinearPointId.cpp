#include <spdlog/spdlog.h>
#include <vtkNew.h>
#include <vtkStructuredData.h>
#include <vtkStructuredGrid.h>
#include <vtkType.h>

void GetLinearPointId(){
    // create a grid
    vtkNew<vtkStructuredGrid> structuredGrid;
    // set the dimensions of the grid
     {
        int dims[3] = {5,10,1};
        structuredGrid->SetDimensions(dims);
     }

    // retrieve the dimensions of the grid
    int dimensions[3];
    structuredGrid->GetDimensions(dimensions);
     {
        int queryPoint[3] = {4,0,0};
        vtkIdType pointId = vtkStructuredData::ComputePointId(dimensions, queryPoint);
        SPDLOG_INFO("PointId: {}", pointId);
     }

     {
        int queryPoint[3] = {0,1,0};
        vtkIdType pointId = vtkStructuredData::ComputePointId(dimensions, queryPoint);
        SPDLOG_INFO("PointId: {}", pointId);
     }
    std::cout << std::endl;  
}