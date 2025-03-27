#include <fmt/format.h>
#include <iterator>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <vector>
#include <vtkActor.h>
#include <vtkContourFilter.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkWarpVector.h>
#include <vtkConnectivityFilter.h>
#include <vtkGeometryFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCamera.h>

void Blow(int argc, char** argv) {
    fmt::memory_buffer out;
    if (argc < 2) {
        fmt::format_to(std::back_inserter(out), "\nUsage: {} filename [data_point] e.g. blow.vtk\n", argv[0]);
        fmt::format_to(std::back_inserter(out), "where: filename is blow.vtk.\n");
        fmt::format_to(std::back_inserter(out), "       data_point allows you to specify which frame is to be displayed.\n");
        fmt::format_to(std::back_inserter(out), "       If data_point < 0 or data_point > 9 all ten frames are then displayed.");
        SPDLOG_INFO(out.data());
        return;
    }

    // default is to display all ten frames.
    auto dataPoint = -1;
    std::string fileName = argv[1];
    if (argc > 2) dataPoint = atoi(argv[2]);

    vtkNew<vtkNamedColors> colors;
    std::vector<std::string> thickness;
    std::vector<std::string> displacement;
    for (auto i = 0; i < 10; ++i) {
        std::ostringstream os;
        os << i;
        thickness.push_back("thickness"+os.str());
        displacement.push_back("displacement"+os.str());
        os.str("");
    }

    std::vector<vtkSmartPointer<vtkDataSetReader>> reader;
    std::vector<vtkSmartPointer<vtkWarpVector>> warp;
    std::vector<vtkSmartPointer<vtkConnectivityFilter>> connect;
    std::vector<vtkSmartPointer<vtkGeometryFilter>> mold;
    std::vector<vtkSmartPointer<vtkDataSetMapper>> moldMapper;
    std::vector<vtkSmartPointer<vtkActor>> moldActor;
    std::vector<vtkSmartPointer<vtkConnectivityFilter>> connect2;
    std::vector<vtkSmartPointer<vtkGeometryFilter>> parison;
    std::vector<vtkSmartPointer<vtkPolyDataNormals>> normals2;
    std::vector<vtkSmartPointer<vtkPolyDataMapper>> parisonMapper;
    std::vector<vtkSmartPointer<vtkActor>> parisonActor;
    std::vector<vtkSmartPointer<vtkContourFilter>> cf;
    std::vector<vtkSmartPointer<vtkPolyDataMapper>> contourMapper;
    std::vector<vtkSmartPointer<vtkActor>> contours;
    std::vector<vtkSmartPointer<vtkRenderer>> ren;

    vtkNew<vtkLookupTable> lut;
    lut->SetHueRange(0.0, 0.66667);
    for (auto i = 0; i < 10; ++i) {
        // create the reader and warp the data vith vectors.
        reader.push_back(vtkSmartPointer<vtkDataSetReader>::New());
        reader[i]->SetFileName(fileName.c_str());
        reader[i]->SetScalarsName(thickness[i].c_str());
        reader[i]->SetVectorsName(displacement[i].c_str());
        reader[i]->Update();

        warp.push_back(vtkSmartPointer<vtkWarpVector>::New());
        warp[i]->SetInputData(reader[i]->GetUnstructuredGridOutput());

        // extract the mold from the mesh using connectivity
        connect.push_back(vtkSmartPointer<vtkConnectivityFilter>::New());
        connect[i]->SetInputConnection(warp[i]->GetOutputPort());
        connect[i]->SetExtractionModeToSpecifiedRegions();
        connect[i]->AddSpecifiedRegion(0);
        connect[i]->AddSpecifiedRegion(1);

        mold.push_back(vtkSmartPointer<vtkGeometryFilter>::New());
        mold[i]->SetInputConnection(connect[i]->GetOutputPort());
        moldMapper.push_back(vtkSmartPointer<vtkDataSetMapper>::New());
        moldMapper[i]->SetInputConnection(mold[i]->GetOutputPort());
        moldMapper[i]->ScalarVisibilityOff();

        moldActor.push_back(vtkSmartPointer<vtkActor>::New());
        moldActor[i]->SetMapper(moldMapper[i]);
        moldActor[i]->GetProperty()->SetColor(colors->GetColor3d("ivory_black").GetData());
        moldActor[i]->GetProperty()->SetRepresentationToWireframe();

        // extract the parison from the mesh using connectivity
        connect2.push_back(vtkSmartPointer<vtkConnectivityFilter>::New());
        connect2[i]->SetInputConnection(warp[i]->GetOutputPort());
        connect2[i]->SetExtractionModeToSpecifiedRegions();
        connect2[i]->AddSpecifiedRegion(2);

        parison.push_back(vtkSmartPointer<vtkGeometryFilter>::New());
        parison[i]->SetInputConnection(connect2[i]->GetOutputPort());

        normals2.push_back(vtkSmartPointer<vtkPolyDataNormals>::New());
        normals2[i]->SetInputConnection(parison[i]->GetOutputPort());
        normals2[i]->SetFeatureAngle(60);

        parisonMapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
        parisonMapper[i]->SetInputConnection(normals2[i]->GetOutputPort());
        parisonMapper[i]->SetLookupTable(lut);
        parisonMapper[i]->SetScalarRange(0.12, 1.0);
        parisonActor.push_back(vtkSmartPointer<vtkActor>::New());
        parisonActor[i]->SetMapper(parisonMapper[i]);

        cf.push_back(vtkSmartPointer<vtkContourFilter>::New());
        cf[i]->SetInputConnection(connect2[i]->GetOutputPort());
        cf[i]->SetValue(0, 0.5);
        contourMapper.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
        contourMapper[i]->SetInputConnection(cf[i]->GetOutputPort());

        contours.push_back(vtkSmartPointer<vtkActor>::New());
        contours[i]->SetMapper(contourMapper[i]);

        ren.push_back(vtkSmartPointer<vtkRenderer>::New());
        ren[i]->AddActor(moldActor[i]);
        ren[i]->AddActor(parisonActor[i]);
        ren[i]->AddActor(contours[i]);
        ren[i]->SetBackground(colors->GetColor3d("AliceBlue").GetData());
        ren[i]->GetActiveCamera()->SetPosition(50.973277, 12.298821, 29.102547);
        ren[i]->GetActiveCamera()->SetFocalPoint(0.141547, 12.298821, -0.245166);
        ren[i]->GetActiveCamera()->SetViewUp(-0.5, 0.0, 0.866025);
        ren[i]->GetActiveCamera()->SetClippingRange(36.640827, 78.61468);
    }

    // create the renderwindow and render window interactor
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    auto rendererSizeX = 750;
    auto rendererSizeY = 400;
    auto renWinScale = 0.5;
    renWin->SetWindowName("Blow");
    if (dataPoint >= 0 && dataPoint < 10) {
        renWin->AddRenderer(ren[dataPoint]);
        renWin->SetSize(rendererSizeX, rendererSizeY);
    } else {
        auto gridDimensionX = 2;
        auto gridDimensionY = 5;
        renWin->SetSize(int(rendererSizeX*gridDimensionX*renWinScale), int(rendererSizeY*gridDimensionY*renWinScale));
        // add and position the renders to the render window.
        for (auto row = 0; row < gridDimensionY; ++row) {
            for (auto col = 0; col < gridDimensionX; ++col) {
                auto idx = row * gridDimensionX + col;
                auto x0 = double(col) / gridDimensionX;
                auto y0 = double(gridDimensionY-row-1)/gridDimensionY;
                auto x1 = double(col+1)/gridDimensionX;
                auto y1 = double(gridDimensionY-row)/gridDimensionY;
                renWin->AddRenderer(ren[idx]);
                ren[idx]->SetViewport(x0, y0, x1, y1);
            }
        }
    }
    renWin->Render();
    iren->Initialize();
    iren->Start();
}
