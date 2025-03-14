#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkMapper.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkType.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkProperty.h>
#include <vtkShepardMethod.h>
#include <vtkContourFilter.h>
#include <vtkColorTransferFunction.h>
void BackgroundTexture() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(100.0, 0.0, 0.0);
    points->InsertNextPoint(300.0, 0.0, 0.0);
    // setup colors
    unsigned char white[3] = {255,255,255};
    unsigned char black[3] = {0,0,0};

    vtkNew<vtkUnsignedCharArray> vertexColors;
    vertexColors->SetNumberOfComponents(3);
    vertexColors->SetName("Colors");
    vertexColors->InsertNextTypedTuple(black);
    vertexColors->InsertNextTypedTuple(white);

    // create a scalar array for the point data, each value represents the distance
    // of the vertices from the first vertex
    vtkNew<vtkFloatArray> values;
    values->SetNumberOfComponents(1);
    values->SetName("Values");
    values->InsertNextValue(0.0);
    values->InsertNextValue(1.0);

    // we must make two objects, because the ShepardMethod uses the ActiveScalars
    // as does the renderer
    vtkNew<vtkPolyData> polydataToProcess;
    polydataToProcess->SetPoints(points);
    polydataToProcess->GetPointData()->SetScalars(values);

    vtkNew<vtkPolyData> polydataToVisualize;
    polydataToVisualize->SetPoints(points);
    polydataToVisualize->GetPointData()->SetScalars(vertexColors);

    vtkNew<vtkVertexGlyphFilter> vertexGlyphFilter;
    vertexGlyphFilter->AddInputData(polydataToVisualize);
    vertexGlyphFilter->Update();

    // create a mapper and actor
    vtkNew<vtkPolyDataMapper> vertsMapper;
    // vertsMapper->ScalarVisibilityOff();
    vertsMapper->SetInputConnection(vertexGlyphFilter->GetOutputPort());

    vtkNew<vtkActor> vertsActor;
    vertsActor->SetMapper(vertsMapper);
    vertsActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    vertsActor->GetProperty()->SetPointSize(3);

    // create a Shepard filter to interpolate the vertices over a regularized image grid
    vtkNew<vtkShepardMethod> shepard;
    shepard->SetInputData(polydataToProcess);
    shepard->SetSampleDimensions(2,2,2);
    shepard->SetModelBounds(100,300,-10,10,-10,10);
    shepard->SetMaximumDistance(1);

    // contour the shepard generated image at 3 isovalues
    // the accuracy of the results are highly dependent on how the shepard filter is set up
    vtkNew<vtkContourFilter> contourFilter;
    contourFilter->SetNumberOfContours(3);
    contourFilter->SetValue(0, 0.25);
    contourFilter->SetValue(1, 0.5);
    contourFilter->SetValue(2, 0.75);
    contourFilter->SetInputConnection(shepard->GetOutputPort());
    contourFilter->Update();

    // create a mapper and actor for the resulting isosurfaces
    vtkNew<vtkPolyDataMapper> contourMapper;
    contourMapper->SetInputConnection(contourFilter->GetOutputPort());
    contourMapper->ScalarVisibilityOn();
    contourMapper->SetColorModeToMapScalars();

    vtkNew<vtkActor> contourActor;
    contourActor->SetMapper(contourMapper);
    contourActor->GetProperty()->SetAmbient(1);
    contourActor->GetProperty()->SetSpecular(0);
    contourActor->GetProperty()->SetDiffuse(0);

    // report the results of the interpolation
    double* range = contourFilter->GetOutput()->GetScalarRange();
    SPDLOG_INFO("Shepard interpolation:");
    SPDLOG_INFO("contour output scalar range: {}, {}", range[0], range[1]);

    vtkIdType nCells = contourFilter->GetOutput()->GetNumberOfCells();
    double bounds[6];
    for (vtkIdType i = 0; i < nCells; ++i) {
        if (i % 2) {
            contourFilter->GetOutput()->GetCellBounds(i, bounds);
            SPDLOG_INFO("cell {}, x position: {}", i, bounds[0]);
        }
    }
    // create a transfer function to color the isosurfaces
    vtkNew<vtkColorTransferFunction> lut;
    lut->SetColorSpaceToRGB();
    lut->AddRGBPoint(range[0], 0, 0, 0); // black
    lut->AddRGBPoint(range[1], 1, 1, 1); // white
    lut->SetScaleToLinear();

    contourMapper->SetLookupTable(lut);
    // create a renderer, render window and interactor
    vtkNew<vtkRenderer> renderer;
    renderer->GradientBackgroundOn();
    renderer->SetBackground(colors->GetColor3d("Blue").GetData());
    renderer->SetBackground2(colors->GetColor3d("Magenta").GetData());
    renderer->AddActor(contourActor);
    renderer->AddActor(vertsActor);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("BackgroundTexture");
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);

    // position the camera so that the image produced is viewable
    vtkCamera* camera = renderer->GetActiveCamera();
    camera->SetPosition(450,100,100);
    camera->SetFocalPoint(200,0,0);
    camera->SetViewUp(0,0,1);

    renderWindow->Render();
    iren->Start();
}
