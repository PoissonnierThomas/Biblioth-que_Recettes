#include "recette.h"

Recette::Recette(std::istream &is) {
}

Recette::Recette(std::string _nom, std::string _photo,std::string _categorie, std::string _description, size_t _nombre, double _prix, std::string _createur, Date _date, std::vector<Ingredient*> _Ingredients )
    : nom(_nom), photo(_photo), categorie("plat"), description(_description), nbConvives(_nombre), prix(_prix), createur(_createur), date(_date)
{    
    setCategorie(_categorie);
}
Recette::~Recette()
{
    for (Ingredient* i : ingredients)
        delete i;
}

void Recette::setCategorie(std::string s)
{
    if (s == "Entrée" || s == "Plat" ||s == "Dessert" || s == "Entremet" || s == "Soupe")
        categorie=s;
    else
        throw ("La catégorie n'est pas valable");
}
