#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

namespace {
class vtkMyCallback : public vtkCommand {
public:
    static vtkMyCallback* New() {return new vtkMyCallback;}

    void Execute(vtkObject* caller, unsigned long, void*) override {
        auto renderer = reinterpret_cast<vtkRenderer*>(caller);
        auto campos = renderer->GetActiveCamera()->GetPosition();
        std::cout << campos[0] << " " << campos[1] << " " << campos[2] << std::endl;
    }
    vtkMyCallback() = default;
};
}

void vtktutorial2() {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(coneActor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("Tutorial_Step2");

    vtkNew<vtkMyCallback> mo1;
    ren1->AddObserver(vtkCommand::StartEvent, mo1);

    for (int i = 0; i < 360; ++i)
    {
        // Render the image
        renWin->Render();
        // Rotate the active camera by one degree.
        ren1->GetActiveCamera()->Azimuth(1);
    }
}
