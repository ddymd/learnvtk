#include <spdlog/spdlog.h>
#include <vtkNew.h>
#include <vtkStructuredPoints.h>
#include <vtkType.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
void StructuredPointsToUnstructuredGrid() {
    // create a structuredpoints
    vtkNew<vtkStructuredPoints> structuredPoints;
    // specify the size of the image data
    structuredPoints->SetDimensions(2,3,1);
    structuredPoints->AllocateScalars(VTK_DOUBLE, 1);
    const int *dims = structuredPoints->GetDimensions();
    SPDLOG_INFO("Dims: x: {} y: {} z: {}", dims[0], dims[1], dims[2]);
    SPDLOG_INFO("Number of points: {}", structuredPoints->GetNumberOfPoints());
    SPDLOG_INFO("Number of cells: {}", structuredPoints->GetNumberOfCells());

    // fill every entry of the image data with "2.0"
    for (int z = 0; z < dims[2]; ++z) {
        for (int y = 0; y < dims[1]; ++y) {
            for (int x = 0; x < dims[0]; ++x) {
                double *pixel = static_cast<double*>(structuredPoints->GetScalarPointer(x,y,z));
                pixel[0] = 2.0;
            }
        }
    }
    // copy the points from the StructuredPoints to the UnstructuredGrid
    vtkNew<vtkPoints> points;
    // retrieve the entries from the grid and print them to the screen
    for (vtkIdType i = 0; i < structuredPoints->GetNumberOfPoints(); ++i) {
        double p[3];
        structuredPoints->GetPoint(i, p);
        points->InsertNextPoint(p);
    }
    vtkNew<vtkUnstructuredGrid> ug;
    ug->SetPoints(points);
    vtkNew<vtkXMLUnstructuredGridWriter> write;
    write->SetFileName("build/output.vtu");
    write->SetInputData(ug);
    write->Write();
}
