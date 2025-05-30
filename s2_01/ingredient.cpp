#include <ostream>
#include "ingredient.h"

Ingredient::Ingredient(std::string _nom, double _quantite, std::string  _unite)
    : nom(_nom), quantite(_quantite), unite(_unite)
{
}


Ingredient::Ingredient(std::istream &is)
{

}

Ingredient::~Ingredient(){}


std::ostream& operator<<(std::ostream &os, const Ingredient& i)
{
    os << i.getQuantite() << i.getUnite() << " de " << i.getNom() ;
    return os;
}