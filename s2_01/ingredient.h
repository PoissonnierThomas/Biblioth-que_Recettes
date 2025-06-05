#include <string>
#include <iostream>
#include <QDebug>
#ifndef INGREDIENT_H
#define INGREDIENT_H


class Ingredient
{
private :
    std::string nom;
    double quantite; 
    std::string unite;
public:
    Ingredient()=delete; // Interdire la création d'ingrédients sans paramètres
    Ingredient(std::string _nom, double _quantite, std::string _unite);
    Ingredient(std::istream &is); // Création d'ingrédient à partir d'une chaîne de caractères
    ~Ingredient();

    // Getteurs et Setteurs
    inline std::string getNom() const {return nom;};
    inline void setNom(std::string s) {nom=s;};
    inline double getQuantite() const {return quantite;};
    inline void setQuantite(double d) {quantite=d;};
    inline std::string getUnite() const {return unite;};
    inline void setUnite(std::string u) {unite=u;};

    // Fonctions d'affichage
    friend std::ostream& operator<<(std::ostream &os, const Ingredient& i);
    friend QDebug operator<<(QDebug debug, const Ingredient& i);
};

#endif // INGREDIENT_H
