#include <string>
#ifndef INGREDIENT_H
#define INGREDIENT_H


class Ingredient
{
private :
    std::string nom;
    double quantite; // Si le booléen poids est vrai, la quantité sera exprimée en grammes, sinon en Litres.
    bool poids;
public:
    Ingredient();
    ~Ingredient();

    inline std::string getNom() const {return nom;};
    inline void setNom(std::string s) {nom=s;};
    inline double getQuantite() const {return quantite;};
    inline void setQuantite(double d) {quantite=d;};
    inline bool getPoids() const {return poids;};
    inline void setPoids(bool d) {poids=d;};
};

#endif // INGREDIENT_H
