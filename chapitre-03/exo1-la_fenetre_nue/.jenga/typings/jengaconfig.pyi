# =============================================================================
# jengaconfig.pyi — STUB GENERE par Jenga (au build / `jenga ide`). NE PAS EDITER.
# Declare les symboles des fichiers de config charges via useconfig(), pour que
# l'editeur (Pyright/Pylance) les COLORE et donne go-to-def / autocomplete.
# =============================================================================
from typing import Any

KIT_BUILT: Any
KIT_COMPILER: Any
KIT_DEPENDS: Any
KIT_JENGA: Any
KIT_MODULES: Any
KIT_NAME: Any
KIT_ROOT: Any
KIT_STDLIB: Any
KIT_SYSTEM_LIBS: Any
KIT_TARGETS: Any
KIT_WORKSPACE: Any
def usekit(*args: Any, **kwargs: Any) -> Any: ...
def user3d(*args: Any, **kwargs: Any) -> Any: ...
