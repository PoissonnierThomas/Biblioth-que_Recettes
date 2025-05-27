#include <iostream>
#include <QFile>
#include <QDebug>
#include <QString>
#include "recette.h"

Recette::Recette(std::string sfichier) {
    QString nom_fichier;
    nom_fichier = QString::fromStdString(sfichier);
    QFile fichier(nom_fichier);
    if(!fichier.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Erreur" << fichier.errorString();
    }
}

Recette::Recette(std::string _nom, std::string _photo,std::string _categorie, std::vector<std::string> _description, size_t _nombre, double _prix, std::string _createur, Date _date, std::vector<Ingredient*> _Ingredients )
    : nom(_nom), photo(_photo), categorie("plat"), description(_description), nbConvives(_nombre), prix(_prix), createur(_createur), date(_date), ingredients(_Ingredients)
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

std::ostream& operator<<(std::ostream &os, const Recette &R)
{
    os << R.getNom() << ", un " << R.getCategorie() << " pour "<<R.getConvives() << " personnes"<<std::endl;
    os << "Cette recette vous est proposé par "<<R.getCreateur()<< " pour un prix de "<<R.getPrix() << " par personnes"<<std::endl;
    for (const std::string &s : R.getDescription())
        os << s << std::endl;
    os << "Les ingrédients nécessaires sont :"<<std::endl;
    for (const Ingredient* i : R.getIngredients())
    {
        os << *i<<std::endl;
    }
   return os;
}
