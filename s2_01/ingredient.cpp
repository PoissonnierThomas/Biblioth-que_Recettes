#include <ostream>
#include "ingredient.h"

Ingredient::Ingredient(std::string _nom, double _quantite, bool _etat)
    : nom(_nom), quantite(_quantite), poids_etat(_etat)
{
}


Ingredient::Ingredient(std::istream &is)
{

}

Ingredient::~Ingredient(){}


std::ostream& operator<<(std::ostream &os, const Ingredient& i)
{
    if (i.getPoids()==true)
        os << i.getQuantite() << " grammes de " << i.getNom();
    else 
        os << i.getQuantite() << " litres de " << i.getNom();
    return os;
}