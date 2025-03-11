
// this does the actual work: updates the prob.
// callback for the interaction.
#include <iomanip>
#include <ios>
#include <sstream>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkCallbackCommand.h>
#include <vtkPointWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSphereSource.h>
#include <vtkProbeFilter.h>
#include <vtkGlyph3D.h>
#include <vtkConeSource.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <spdlog/spdlog.h>
class vtkmyPWCallback : public vtkCallbackCommand {
public:
    vtkmyPWCallback() = default;
    static vtkmyPWCallback* New() {
        return new vtkmyPWCallback;
    }
    virtual void Execute(vtkObject* caller, unsigned long, void*) {
        vtkPointWidget *pointWidget = reinterpret_cast<vtkPointWidget*>(caller);
        pointWidget->GetPolyData(this->PolyData);
        double position[3];
        pointWidget->GetPosition(position);
        std::ostringstream text;
        text << "cursor: " << std::fixed << std::setprecision(4) << position[0] << ", " << position[1] << ", " << position[2];
        auto ftext = fmt::format("cursor: {:.4f}, {:.4f}, {:.4f}", position[0], position[1], position[2]);
        this->PositionActor->SetInput(text.str().c_str());
        // this->PositionActor->SetInput(ftext.c_str());
        this->CursorActor->VisibilityOn();
    }

    vtkPolyData* PolyData {nullptr};
    vtkActor* CursorActor {nullptr};
    vtkTextActor* PositionActor {nullptr};

};

void Arbitrary3DCursor(int argc, char** argv) {
    vtkSmartPointer<vtkPolyData> inputPolyData;
    if (argc > 1) {
        vtkNew<vtkXMLPolyDataReader> reader;
        reader->SetFileName(argv[1]);
        reader->Update();
        inputPolyData = reader->GetOutput();
    } else {
        vtkNew<vtkSphereSource> sphereSource;
        sphereSource->SetPhiResolution(15);
        sphereSource->SetThetaResolution(15);
        sphereSource->Update();
        inputPolyData = sphereSource->GetOutput();
    }

    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkPolyData> point;
    vtkNew<vtkProbeFilter> probe;
    probe->SetInputData(point);
    probe->SetSourceData(inputPolyData);

    // create glyph
    vtkNew<vtkConeSource> cone;
    cone->SetResolution(16);
    vtkNew<vtkGlyph3D> glyph;
    glyph->SetInputConnection(probe->GetOutputPort());
    glyph->SetSourceConnection(cone->GetOutputPort());
    glyph->SetVectorModeToUseVector();
    glyph->SetScaleModeToDataScalingOff();
    glyph->SetScaleFactor(inputPolyData->GetLength()*0.1);

    vtkNew<vtkPolyDataMapper> glyphMapper;
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    vtkNew<vtkActor> glyphActor;
    glyphActor->SetMapper(glyphMapper);
    glyphActor->VisibilityOn();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(inputPolyData);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetRepresentationToWireframe();
    actor->GetProperty()->SetColor(colors->GetColor3d("gold").GetData());

    vtkNew<vtkTextActor> textActor;
    textActor->GetTextProperty()->SetFontSize(12);
    textActor->SetPosition(10,20);
    textActor->SetInput("cursor:");
    textActor->GetTextProperty()->SetColor(colors->GetColor3d("White").GetData());

    // create the render window renderer and both actors
    vtkNew<vtkRenderer> ren1;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // the set interactor method is show 3D widgets are associated with the render
    // window interactor. internally, set interactor sets up a bunch of callbacks
    // using the command/observer mechanism (AddObserver())
    vtkNew<vtkmyPWCallback> myCallback;
    myCallback->PolyData = point;
    myCallback->CursorActor = glyphActor;
    myCallback->PositionActor = textActor;

    // the point widget is used probe the dataset
    vtkNew<vtkPointWidget> pointWidget;
    pointWidget->SetInteractor(iren);
    pointWidget->SetInputData(inputPolyData);
    pointWidget->AllOff();
    pointWidget->PlaceWidget();
    pointWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

    ren1->AddActor(glyphActor);
    ren1->AddActor(actor);
    ren1->AddActor2D(textActor);

    // add the actors to the renderer, set the background and size
    ren1->GradientBackgroundOn();
    ren1->SetBackground(colors->GetColor3d("SlateGray").GetData());
    ren1->SetBackground2(colors->GetColor3d("Wheat").GetData());

    renWin->SetSize(300, 300);
    renWin->SetWindowName("Arbitrary3DCursor");
    renWin->Render();
    pointWidget->On();

    // render the image
    iren->Initialize();
    renWin->Render();

    iren->Start();
}