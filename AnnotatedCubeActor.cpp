#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkProperty.h>
void AnnotatedCubeActor() {
    vtkNew<vtkNamedColors> colors;
    // set up the renderer, window and interactor
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("Wheat").GetData());

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(640,480);
    renderWindow->SetWindowName("AnnotatedCubeActor");

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(renderWindow);
    
    vtkNew<vtkAnnotatedCubeActor> cube;
    cube->SetFaceTextScale(2.0/3.0);
    cube->SetXPlusFaceText("A");
    cube->SetXMinusFaceText("P");
    cube->SetYPlusFaceText("L");
    cube->SetYMinusFaceText("R");
    cube->SetZPlusFaceText("S");
    cube->SetZMinusFaceText("I");

    // change the vector text colors
    cube->GetTextEdgesProperty()->SetColor(colors->GetColor3d("Black").GetData());
    cube->GetTextEdgesProperty()->SetLineWidth(4);

    cube->GetXPlusFaceProperty()->SetColor(colors->GetColor3d("Turquoise").GetData());
    cube->GetXMinusFaceProperty()->SetColor(colors->GetColor3d("Turquoise").GetData());

    cube->GetYPlusFaceProperty()->SetColor(colors->GetColor3d("Mint").GetData());
    cube->GetYMinusFaceProperty()->SetColor(colors->GetColor3d("Mint").GetData());

    cube->GetZPlusFaceProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
    cube->GetZMinusFaceProperty()->SetColor(colors->GetColor3d("Tomato").GetData());

    renderer->AddActor(cube);

    // set up an interesting view
    vtkCamera* camera = renderer->GetActiveCamera();
    camera->SetViewUp(0,0,1);
    camera->SetFocalPoint(0,0,0);
    camera->SetPosition(4.5,4.5,2.5);
    renderer->ResetCamera();
    camera->Dolly(1.0);
    renderer->ResetCameraClippingRange();
    renderWindow->Render();
    interactor->Start();
}