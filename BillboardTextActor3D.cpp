#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>

namespace {
void ActorCallback(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData)) {
    auto textActor = static_cast<vtkBillboardTextActor3D*>(clientData);
    auto actor = static_cast<vtkActor*>(caller);
    auto label = fmt::format("{:.3f}, {:.3f}, {:.3f}\n", actor->GetPosition()[0], actor->GetPosition()[1], actor->GetPosition()[2]);
    textActor->SetPosition(actor->GetPosition());
    textActor->SetInput(label.c_str());
}
void RandomPosition(double p[3], double const& min_r, double const& max_r, vtkMinimalStandardRandomSequence* rng) {
    for (auto i = 0; i < 3; ++i) {
        p[i] = rng->GetRangeValue(min_r, max_r);
        rng->Next();
    }
}
}

void BillboardTextActor3D() {
    vtkNew<vtkNamedColors> colors;
    // for testing
    vtkNew<vtkMinimalStandardRandomSequence> rng;
    // rng->SetSeed(8775070);
    rng->SetSeed(5127);

    // create renderer
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(colors->GetColor3d("DarkSlateGray").GetData());

    // render window
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(renderer);
    renWin->SetWindowName("BillboardTextActor3D");

    // interactor
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // create a sphere
    vtkNew<vtkSphereSource> source;
    source->SetCenter(0.0, 0.0, 0.0);
    source->SetRadius(1.0);
    
    auto min_r = -10.0;
    auto max_r = 10.0;
    for (auto i = 0; i < 10; ++i) {
        if (0 == i) {
            // create an actor representing the origin
            vtkNew<vtkPolyDataMapper> mapper;
            mapper->SetInputConnection(source->GetOutputPort());

            vtkNew<vtkActor> actor;
            actor->SetMapper(mapper);
            actor->SetPosition(0,0,0);
            actor->GetProperty()->SetColor(colors->GetColor3d("Peacock").GetData());
            renderer->AddActor(actor);
        }

        // create mapper
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->AddInputConnection(source->GetOutputPort());
        // create actor
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->SetPosition(0,0,0);
        actor->GetProperty()->SetColor(colors->GetColor3d("MistyRose").GetData());

        // setup the text and add it to the renderer
        vtkNew<vtkBillboardTextActor3D> textActor;
        textActor->SetInput("");
        textActor->SetPosition(actor->GetPosition());
        textActor->GetTextProperty()->SetFontSize(12);
        textActor->GetTextProperty()->SetColor(colors->GetColor3d("Gold").GetData());
        textActor->GetTextProperty()->SetJustificationToCentered();

        renderer->AddActor(actor);
        renderer->AddActor(textActor);

        // if you want to use a callback, do this:
        vtkNew<vtkCallbackCommand> actorCallback;
        actorCallback->SetCallback(ActorCallback);
        actorCallback->SetClientData(textActor);
        actor->AddObserver(vtkCommand::ModifiedEvent, actorCallback);
        // Otherwise do this:
        // auto label = fmt::format("{:.3f}, {:.3f}, {:.3f}\n", actor->GetPosition()[0], actor->GetPosition()[1], actor->GetPosition()[2]);
        // textActor->SetPosition(actor->GetPosition());
        // textActor->SetInput(label.c_str());

        double position[3];
        RandomPosition(position, min_r, max_r, rng);
        actor->SetPosition(position);
    }
    iren->Start();
}
