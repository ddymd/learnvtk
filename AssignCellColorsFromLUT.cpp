#include <fmt/format.h>
#include <iterator>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkType.h>
#include <vtkColorTransferFunction.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPlaneSource.h>
#include <vtkCellData.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
template<typename T> std::string PrintColor(T &rgb) {
    auto fbuf = fmt::memory_buffer();
    for (size_t i = 0; i < 3; ++i) {
        if (i < 2) {
            fmt::format_to(std::back_inserter(fbuf), "{:f} ", static_cast<double>(rgb[i]));
        } else {
            fmt::format_to(std::back_inserter(fbuf), "{:f}", static_cast<double>(rgb[i]));
        }
    }
    return fmt::to_string(fbuf);
}

// make a lookup table from a set of named colors
void MakeLUT(size_t const& tableSize, vtkLookupTable* lut) {
    vtkNew<vtkNamedColors> nc;
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(tableSize));
    lut->Build();
    // fill in a few  known colors, the rest will be generated if needed
    lut->SetTableValue(0, nc->GetColor4d("Black").GetData());
    lut->SetTableValue(1, nc->GetColor4d("Banana").GetData());
    lut->SetTableValue(2, nc->GetColor4d("Tomato").GetData());
    lut->SetTableValue(3, nc->GetColor4d("Wheat").GetData());
    lut->SetTableValue(4, nc->GetColor4d("Lavender").GetData());
    lut->SetTableValue(5, nc->GetColor4d("Flesh").GetData());
    lut->SetTableValue(6, nc->GetColor4d("Raspberry").GetData());
    lut->SetTableValue(7, nc->GetColor4d("Salmon").GetData());
    lut->SetTableValue(8, nc->GetColor4d("Mint").GetData());
    lut->SetTableValue(9, nc->GetColor4d("Peacock").GetData());
}

// use a color transfer function to generate the colors in the lookup table
void MakeLUTFromCTF(size_t const& tableSize, vtkLookupTable* lut) {
    vtkNew<vtkColorTransferFunction> ctf;
    ctf->SetColorSpaceToDiverging();
    // green to tan
    ctf->AddRGBPoint(0.0, 0.085, 0.532, 0.201);
    ctf->AddRGBPoint(0.5, 0.865, 0.865, 0.865);
    ctf->AddRGBPoint(1.0, 0.677, 0.492, 0.093);
    lut->SetNumberOfTableValues(static_cast<vtkIdType>(tableSize));
    lut->Build();
    for (size_t i = 0; i < tableSize; ++i) {
        double* rgb;
        rgb = ctf->GetColor(static_cast<double>(i)/tableSize);
        lut->SetTableValue(static_cast<vtkIdType>(i), rgb);
    }
}

// create the cell data using the colors from the lookup table
void MakeCellData(size_t const& tableSize, vtkLookupTable* lut, vtkUnsignedCharArray* colors) {
    for (size_t i = 1; i < tableSize; ++i) {
        double rgb[3];
        unsigned char ucrgb[3];
        // Get the interpolated color
        // of course you can use any function whose range is [0...1] to get the required color and assign it to a cell id
        // in this case we are just using the cell (id+1)/(talbeSize-1) to get the interpolated color
        lut->GetColor(static_cast<double>(i)/(tableSize-1), rgb);
        for (size_t j = 0; j < 3; ++j) {
            ucrgb[j] = static_cast<unsigned char>(rgb[j]*255);
        }
        colors->InsertNextTuple3(ucrgb[0], ucrgb[1], ucrgb[2]);
        // print out what we have
        SPDLOG_INFO("({}) ({})", PrintColor<double[3]>(rgb), PrintColor<unsigned char[3]>(ucrgb));
    }
}

void AssignCellColorsFromLUT() {
    vtkNew<vtkNamedColors> nc;
    // provide some geometry
    int resolution = 16;
    vtkNew<vtkPlaneSource> plane11;
    plane11->SetXResolution(resolution);
    plane11->SetYResolution(resolution);

    vtkNew<vtkPlaneSource> plane12;
    plane12->SetXResolution(resolution);
    plane12->SetYResolution(resolution);

    // create a lookup table to map cell data to colors
    vtkNew<vtkLookupTable> lut1;
    vtkNew<vtkLookupTable> lut2;
    int tableSize = std::max(resolution*resolution+1, 10);

    // force an update so we can set cell data
    plane11->Update();
    plane12->Update();

    MakeLUT(tableSize, lut1);
    MakeLUTFromCTF(tableSize, lut2);

    vtkNew<vtkUnsignedCharArray> colorData1;
    colorData1->SetName("colors");
    colorData1->SetNumberOfComponents(3);
    SPDLOG_INFO("Using a lookup table from a set of named colors.");
    MakeCellData(tableSize, lut1, colorData1);
    // then use SetScalars() to add it to the vtkPolyData structure, this will then be interpreted as a color table
    plane11->GetOutput()->GetCellData()->SetScalars(colorData1);

    vtkNew<vtkUnsignedCharArray> colorData2;
    colorData2->SetName("colors");
    colorData2->SetNumberOfComponents(3);
    SPDLOG_INFO("Using a lookup table created from a color transfer function.");
    MakeCellData(tableSize, lut2, colorData2);
    plane12->GetOutput()->GetCellData()->SetScalars(colorData2);

    // setup actor and mapper
    vtkNew<vtkPolyDataMapper> mapper11;
    mapper11->SetInputConnection(plane11->GetOutputPort());
    // now, instead of doing this
    // mapper11->SetScalarRange(0, tableSize-1);
    // mapper11->SetLookupTable(lut1);
    // we can just use the color data that we created from the lookup table and assigned to the cells
    mapper11->SetScalarModeToUseCellData();
    mapper11->Update();

    vtkNew<vtkPolyDataMapper> mapper12;
    mapper12->SetInputConnection(plane12->GetOutputPort());
    mapper12->SetScalarModeToUseCellData();
    mapper12->Update();

    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetFileName("pdlut.vtp");
    writer->SetInputData(mapper11->GetInput());
    // this is set so we can see the data in a text editor
    writer->Write();
    writer->SetFileName("pdctf.vtp");
    writer->SetInputData(mapper12->GetInput());
    writer->Write();

    vtkNew<vtkActor> actor11;
    actor11->SetMapper(mapper11);
    vtkNew<vtkActor> actor12;
    actor12->SetMapper(mapper12);

    // let's read in the data we wrote out
    vtkNew<vtkXMLPolyDataReader> reader1;
    reader1->SetFileName("pdlut.vtp");

    vtkNew<vtkXMLPolyDataReader> reader2;
    reader2->SetFileName("pdctf.vtp");

    vtkNew<vtkPolyDataMapper> mapper21;
    mapper21->SetInputConnection(reader1->GetOutputPort());
    mapper21->SetScalarModeToUseCellData();
    mapper21->Update();
    vtkNew<vtkActor> actor21;
    actor21->SetMapper(mapper11);

    vtkNew<vtkPolyDataMapper> mapper22;
    mapper22->SetInputConnection(reader2->GetOutputPort());
    mapper22->SetScalarModeToUseCellData();
    mapper22->Update();
    vtkNew<vtkActor> actor22;
    actor22->SetMapper(mapper22);

    // define viewport ranges
    double viewport11[4] = {0.0,0.0,0.5,0.5};
    double viewport12[4] = {0.0,0.5,0.5,1.0};
    double viewport21[4] = {0.5,0.0,1.0,0.5};
    double viewport22[4] = {0.5,0.5,1.0,1.0};

    // set up the renderers
    vtkNew<vtkRenderer> ren11;
    vtkNew<vtkRenderer> ren12;
    vtkNew<vtkRenderer> ren21;
    vtkNew<vtkRenderer> ren22;

    // setup the render windows
    vtkNew<vtkRenderWindow> renWin;
    renWin->SetSize(600,600);
    renWin->SetWindowName("AssignCellColorsFromLUT");

    renWin->AddRenderer(ren11);
    renWin->AddRenderer(ren12);
    renWin->AddRenderer(ren21);
    renWin->AddRenderer(ren22);
    ren11->SetViewport(viewport11);
    ren12->SetViewport(viewport12);
    ren21->SetViewport(viewport21);
    ren22->SetViewport(viewport22);
    ren11->SetBackground(nc->GetColor3d("MidnightBlue").GetData());
    ren12->SetBackground(nc->GetColor3d("MidnightBlue").GetData());
    ren21->SetBackground(nc->GetColor3d("MidnightBlue").GetData());
    ren22->SetBackground(nc->GetColor3d("MidnightBlue").GetData());
    ren11->AddActor(actor11);
    ren12->AddActor(actor12);
    ren21->AddActor(actor21);
    ren22->AddActor(actor22);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);
    renWin->Render();
    iren->Start();
}
