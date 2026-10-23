#include <NKWindow/Core/NkWindow.h>
#include <NKWindow/NKMain.h>
#include <NKEvent/NkEventSystem.h>
#include <NKEvent/NkKeyboardEvent.h>

namespace {

class MouseCapture {
public:
    explicit MouseCapture(nkentseu::NkWindow& window) : window_(window) {}

    void Set(bool captured) {
        if (captured == captured_) {
            return;
        }

        if (captured) {
            window_.ShowMouse(false);
            window_.CaptureMouse(true);
            window_.ClipMouseToClient(true);
        } else {
            window_.ClipMouseToClient(false);
            window_.CaptureMouse(false);
            window_.ShowMouse(true);
        }
        captured_ = captured;
    }

private:
    nkentseu::NkWindow& window_;
    bool captured_ = false;
};

}

int nkmain(const nkentseu::NkEntryState&) {
    nkentseu::NkWindowConfig config;
    config.title = "Chapitre 6 - capture du pointeur";
    config.width = 1280;
    config.height = 720;

    nkentseu::NkWindow window(config);
    if (!window.IsValid()) {
        return 1;
    }

    MouseCapture mouse(window);
    mouse.Set(true);

    bool running = true;
    nkentseu::NkEventSystem& events = nkentseu::NkEvents();
    auto keyCallback = events.AddEventCallbackGuard<nkentseu::NkKeyPressEvent>(
        [&](nkentseu::NkKeyPressEvent* event) {
            if (event->GetKey() == nkentseu::NkKey::NK_ESCAPE) {
                mouse.Set(false);
            } else if (event->GetKey() == nkentseu::NkKey::NK_E) {
                mouse.Set(true);
            }
        }, window.GetId());
    auto closeCallback = events.AddEventCallbackGuard<nkentseu::NkWindowCloseEvent>(
        [&](nkentseu::NkWindowCloseEvent*) {
            running = false;
        }, window.GetId());

    while (running && window.IsOpen()) {
        events.PollEvents();
    }

    mouse.Set(false);
    window.Close();
    return 0;
}
