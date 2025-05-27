#include <string>
#ifndef INGREDIENT_H
#define INGREDIENT_H


class Ingredient
{
private :
    std::string nom;
    double quantite; // Si le booléen poids_état est vrai, la quantité sera exprimée en grammes, sinon en Litres.
    bool poids_etat;
public:
    Ingredient()=delete;
    Ingredient(std::string _nom, double _quantite, bool _etat);
    Ingredient(std::istream &is);
    ~Ingredient();

    inline std::string getNom() const {return nom;};
    inline void setNom(std::string s) {nom=s;};
    inline double getQuantite() const {return quantite;};
    inline void setQuantite(double d) {quantite=d;};
    inline bool getPoids() const {return poids_etat;};
    inline void setPoids(bool d) {poids_etat=d;};

    friend std::ostream& operator<<(std::ostream &os, const Ingredient& i);
};

#endif // INGREDIENT_H
