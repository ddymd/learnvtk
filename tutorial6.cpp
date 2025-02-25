/**
 * Instroduces 3D widgets
 */
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3D.h>
#include <vtkBoxWidget.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace {
class vtkMyCallback : public vtkCommand {
public:
    static vtkMyCallback* New() {return new vtkMyCallback;}

    void Execute(vtkObject* caller, unsigned long, void*) override {
        vtkNew<vtkTransform> t;
        auto widget = reinterpret_cast<vtkBoxWidget*>(caller);
        widget->GetTransform(t);
        widget->GetProp3D()->SetUserTransform(t);
    }
};
}

void vtktutorial6() {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkConeSource> cone;
    cone->SetHeight(3.0);
    cone->SetRadius(1.0);
    cone->SetResolution(10);

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection(cone->GetOutputPort());

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper(coneMapper);
    coneActor->GetProperty()->SetColor(colors->GetColor3d("Bisque").GetData());

    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(coneActor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("Tutorial Step 6");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    vtkNew<vtkBoxWidget> boxWidget;
    boxWidget->SetInteractor(iren);
    boxWidget->SetPlaceFactor(1.25);
    boxWidget->GetOutlineProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    boxWidget->SetProp3D(coneActor);
    boxWidget->PlaceWidget();
    vtkNew<vtkMyCallback> callback;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    boxWidget->On();

    iren->Initialize();
    iren->Start();
}
