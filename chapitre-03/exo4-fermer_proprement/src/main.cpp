#include <NKWindow/NKWindow.h>
#include <NKEvent/NkEvent.h>
#include <NKWindow/NKMain.h>

int nkmain(const nkentseu::NkEntryState &state){
    nkentseu::NkWindowConfig config;

    config.title = "Ma salle Juliette";
    config.width = 1280;
    config.height = 720;
    config.minimizable = true;

    nkentseu::NkWindow fenetre(config);

    if (!fenetre.IsValid()){
        return 1;
    }

    bool tourne = true;

    nkentseu::NkEventSystem &evenements = nkentseu::NkEvents();

    evenements.AddEventCallback<nkentseu::NkWindowCloseEvent>([&](nkentseu::NkWindowCloseEvent *){
            tourne = false;
        }
    );

    evenements.AddEventCallback<nkentseu::NkKeyPressEvent>( [&](nkentseu::NkKeyPressEvent *event){ 
        if (event->GetKey() == nkentseu::NkKey::NK_ESCAPE){ 
            tourne = false; 
        } 
    } );

    while (tourne){ 
        evenements.PollEvents(); 
    }

    fenetre.Close();

    return 0;
}