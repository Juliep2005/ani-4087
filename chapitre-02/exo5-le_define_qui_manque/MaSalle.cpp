#include "../include/MaClasse.hpp"

#ifdef MA_CLASSE_COMPLETE

#include <iostream>

MaClasse::MaClasse()
{
}

void MaClasse::afficher()
{
    std::cout << "Classe complète" << std::endl;
}

#endif