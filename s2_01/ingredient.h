#include <string>
#ifndef INGREDIENT_H
#define INGREDIENT_H


class Ingredient
{
private :
    std::string nom;
    double quantite; 
    std::string unite;
public:
    Ingredient()=delete;
    Ingredient(std::string _nom, double _quantite, std::string _unite);
    Ingredient(std::istream &is);
    ~Ingredient();

    inline std::string getNom() const {return nom;};
    inline void setNom(std::string s) {nom=s;};
    inline double getQuantite() const {return quantite;};
    inline void setQuantite(double d) {quantite=d;};
    inline std::string getUnite() const {return unite;};
    inline void setPoids(std::string u) {unite=u;};

    friend std::ostream& operator<<(std::ostream &os, const Ingredient& i);
};

#endif // INGREDIENT_H
