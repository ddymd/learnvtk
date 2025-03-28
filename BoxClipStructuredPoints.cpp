#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkLookupTable.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>
#include <vtkBoxClipDataSet.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
void BoxClipStructuredPoints(int argc, char** argv) {
    if (argc < 2) {
        SPDLOG_INFO("Usage: {} file.mhd e.g. HeadMRVolume.mhd", argv[0]);
        return;
    }
    vtkNew<vtkNamedColors> colors;
    // read the data
    vtkNew<vtkMetaImageReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();

    double bounds[6];
    reader->GetOutput()->GetBounds(bounds);
    double range[2];
    reader->GetOutput()->GetScalarRange(range);

    double minBoxPoint[3];
    double maxBoxPoint[3];
    minBoxPoint[0] = (bounds[1]-bounds[0]) / 2.0 + bounds[0];
    minBoxPoint[1] = (bounds[3]-bounds[2]) / 2.0 + bounds[2];
    minBoxPoint[2] = (bounds[5]-bounds[4]) / 2.0 + bounds[4];
    maxBoxPoint[0] = bounds[1];
    maxBoxPoint[1] = bounds[3];
    maxBoxPoint[2] = bounds[5];

    vtkNew<vtkBoxClipDataSet> boxClip;
    boxClip->SetInputConnection(reader->GetOutputPort());

    const double minusx[] = {-1.0, -0.5, 0.0};
    const double minusy[] = {0.0, -1.0, 0.0};
    const double minusz[] = {0.0, 0.0, -1.0};
    const double plusx[] = {1.0, 0.0, 0.0};
    const double plusy[] = {0.0, 1.0, 0.0};
    const double plusz[] = {0.0, 0.0, 1.0};

    boxClip->SetBoxClip(minusx, minBoxPoint, minusy, minBoxPoint, minusz, minBoxPoint, plusx, maxBoxPoint, plusy, maxBoxPoint, plusz, maxBoxPoint);
    boxClip->GenerateClippedOutputOn();

    // define a lut
    vtkNew<vtkLookupTable> lut1;
    lut1->SetHueRange(0.667, 0);

    vtkNew<vtkDataSetMapper> mapperIn;
    mapperIn->SetInputConnection(boxClip->GetOutputPort(0));
    mapperIn->SetScalarRange(reader->GetOutput()->GetScalarRange());
    mapperIn->SetLookupTable(lut1);
    mapperIn->SetColorModeToMapScalars();

    vtkNew<vtkActor> actorIn;
    actorIn->SetMapper(mapperIn);

    vtkNew<vtkDataSetMapper> mapperOut;
    mapperOut->SetInputConnection(boxClip->GetOutputPort(1));
    mapperOut->SetScalarRange(reader->GetOutput()->GetScalarRange());
    mapperOut->SetLookupTable(lut1);
    mapperOut->SetColorModeToMapScalars();

    // move the outside actor
    vtkNew<vtkActor> actorOut;
    actorOut->SetMapper(mapperOut);
    actorOut->AddPosition(-0.5*(maxBoxPoint[0]-minBoxPoint[0]), -0.5*(maxBoxPoint[1]-minBoxPoint[1]), -0.5*(maxBoxPoint[2]-minBoxPoint[2]));

    // create renderer, render window and interactor
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("Silver").GetData());
    renderer->UseHiddenLineRemovalOn();

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640, 480);
    renderWindow->SetWindowName("BoxClipStructuredPoints");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);
    // add the actors to the scene
    renderer->AddActor(actorIn);
    renderer->AddActor(actorOut);

    // generate an interesting view
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(120);
    renderer->GetActiveCamera()->Elevation(30);
    renderer->GetActiveCamera()->Dolly(1.0);
    renderer->ResetCameraClippingRange();

    // render and interact
    renderWindow->Render();
    iren->Start();
}
