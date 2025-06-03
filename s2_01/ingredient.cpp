#include <ostream>
#include <iostream>
#include <string>
#include <QDebug>
#include "ingredient.h"

Ingredient::Ingredient(std::string _nom, double _quantite, std::string  _unite)
    : nom(_nom), quantite(_quantite), unite(_unite)
{
}


Ingredient::Ingredient(std::istream &is) {
    std::string line;

    // Lire jusqu'à trouver l'ouverture <ingredient>
    while (std::getline(is, line)) {
        if (line.find("<ingredient>") != std::string::npos) {
            break;
        }
    }

    // Lire les éléments de l'ingrédient
    while (std::getline(is, line)) {
        // Fin de l'ingrédient
        if (line.find("</ingredient>") != std::string::npos) {
            break;
        }

        // Extraire le nom
        if (line.find("<nom>") != std::string::npos) {
            size_t start = line.find("<nom>") + 5;
            size_t end = line.find("</nom>");
            nom = line.substr(start, end - start);
        }

        // Extraire la quantité
        if (line.find("<quantite>") != std::string::npos) {
            size_t start = line.find("<quantite>") + 10;
            size_t end = line.find("</quantite>");
            std::string quantiteStr = line.substr(start, end - start);
            quantite = std::stod(quantiteStr);
        }

        // Extraire l'unité (si présente)
        if (line.find("<unite>") != std::string::npos) {
            size_t start = line.find("<unite>") + 7;
            size_t end = line.find("</unite>");
            unite = line.substr(start, end - start);
        }
    }

    // L'unité n'est pas présente dans le XML, on met une valeur par défaut
    if (unite.empty()) {
        unite = "g"; // grammes par défaut
    }
}

Ingredient::~Ingredient(){}


std::ostream& operator<<(std::ostream &os, const Ingredient& i)
{
    os << i.getQuantite() << i.getUnite() << " de " << i.getNom() ;
    return os;
}



QDebug operator<<(QDebug debug, const Ingredient& i)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Ingredient("
                    << QString::fromStdString(i.getNom()) << ": "
                    << i.getQuantite() << " "
                    << QString::fromStdString(i.getUnite()) << ")";
    return debug;
}
