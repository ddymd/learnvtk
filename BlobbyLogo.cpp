// use implicit modeller to create the VTK logo
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkContourFilter.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataReader.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkImplicitModeller.h>
void BlobbyLogo(int argc, char** argv) {
    if (argc < 4) {
        SPDLOG_INFO("Usage: {} v.vtk t.vtk k.vtk", argv[0]);
        return;
    }
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("BlobbyLogo");
    renWin->SetSize(300, 300);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // read the geometry file containing the letter v
    vtkNew<vtkPolyDataReader> letterV;
    letterV->SetFileName(argv[1]);

    // read the geometry file containing the letter t
    vtkNew<vtkPolyDataReader> letterT;
    letterT->SetFileName(argv[2]);

    // read the geometry file containing the letter k
    vtkNew<vtkPolyDataReader> letterK;
    letterK->SetFileName(argv[3]);

    // create a transform and transform filter for each letter
    vtkNew<vtkTransform> Vtransform;
    vtkNew<vtkTransformPolyDataFilter> VtransformFilter;
    VtransformFilter->SetInputConnection(letterV->GetOutputPort());
    VtransformFilter->SetTransform(Vtransform);

    vtkNew<vtkTransform> Ttransform;
    vtkNew<vtkTransformPolyDataFilter> TtransformFilter;
    TtransformFilter->SetInputConnection(letterT->GetOutputPort());
    TtransformFilter->SetTransform(Ttransform);

    vtkNew<vtkTransform> Ktransform;
    vtkNew<vtkTransformPolyDataFilter> KtransformFilter;
    KtransformFilter->SetInputConnection(letterK->GetOutputPort());
    KtransformFilter->SetTransform(Ktransform);

    // now append them all
    vtkNew<vtkAppendPolyData> appendAll;
    appendAll->AddInputConnection(VtransformFilter->GetOutputPort());
    appendAll->AddInputConnection(TtransformFilter->GetOutputPort());
    appendAll->AddInputConnection(KtransformFilter->GetOutputPort());

    // create normals
    vtkNew<vtkPolyDataNormals> logoNormals;
    logoNormals->SetInputConnection(appendAll->GetOutputPort());
    logoNormals->SetFeatureAngle(60);

    // map to rendering primitives
    vtkNew<vtkPolyDataMapper> logoMapper;
    logoMapper->SetInputConnection(logoNormals->GetOutputPort());
    // now an actor
    vtkNew<vtkActor> logo;
    logo->SetMapper(logoMapper);

    // now create an implicit model of the same letter
    vtkNew<vtkImplicitModeller> blobbyLogoImp;
    blobbyLogoImp->SetInputConnection(appendAll->GetOutputPort());
    blobbyLogoImp->SetMaximumDistance(0.075);
    blobbyLogoImp->SetSampleDimensions(64,64,64);
    blobbyLogoImp->SetAdjustDistance(0.05);

    // extract an iso surface
    vtkNew<vtkContourFilter> blobbyLogoIso;
    blobbyLogoIso->SetInputConnection(blobbyLogoImp->GetOutputPort());
    blobbyLogoIso->SetValue(1, 1.5);

    // map to rendering primitives
    vtkNew<vtkPolyDataMapper> blobbyLogoMapper;
    blobbyLogoMapper->SetInputConnection(blobbyLogoIso->GetOutputPort());
    blobbyLogoMapper->ScalarVisibilityOff();

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkProperty> tomato;
    tomato->SetDiffuseColor(colors->GetColor3d("tomato").GetData());
    tomato->SetSpecular(0.3);
    tomato->SetSpecularPower(20);

    vtkNew<vtkProperty> banana;
    banana->SetDiffuseColor(colors->GetColor3d("banana").GetData());
    banana->SetDiffuse(0.7);
    banana->SetSpecular(0.4);
    banana->SetSpecularPower(20);

    vtkNew<vtkActor> blobbyLogo;
    blobbyLogo->SetMapper(blobbyLogoMapper);
    blobbyLogo->SetProperty(banana);

    // position the letters
    Vtransform->Translate(-16.0, 0.0, 12.5);
    Vtransform->RotateY(40);

    Ktransform->Translate(14.0, 0.0, 0.0);
    Ktransform->RotateY(-40);

    // move the polygonal letters to the front
    logo->SetProperty(tomato);
    logo->SetPosition(0,0,6);
    renderer->AddActor(logo);
    renderer->AddActor(blobbyLogo);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
    renderer->Render();
    iren->Start();
}
