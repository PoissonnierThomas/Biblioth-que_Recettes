#include <string>
#include <vector>
#include "ingredient.h"
#ifndef RECETTE_H
#define RECETTE_H


struct Date {
    size_t jour;
    size_t mois;
    size_t annee;
};


class Recette
{
private :
    std::string nom;
    std::string photo;  // Contient le nom de l'emplacement de la photo correspondante
    std::string categorie; // Contient le nom de la catégorie ( Entrée, Plat, Dessert, Entremet, Soupe)
    std::string description; // Contient une description des étapes de la recette
    size_t nbConvives; // Contient le nombre de personnes prévu par la recette initiale
    double prix; // Contient le prix unitaire que coûte la recette
    std::string createur; // Contient le nom du créateur de la recette
    Date date; // Date de dépôt de la recette sur l'appli
    std::vector<Ingredient*> ingredients; // Contient les différents ingrédients, dans lesquels on retrouve le nom et la quantité
public:
    Recette()=delete;
    Recette(std::istream &is);
    Recette(std::string _nom, std::string _photo,std::string _categorie, std::string _description, size_t _nombre, double _prix, std::string _createur, Date _date, std::vector<Ingredient*> _Ingredients );
    ~Recette();
    inline std::string getString() const {return nom;};
    inline void setString(std::string s) {nom=s;};
    inline std::string getCategorie() const {return categorie;};
    void setCategorie(std::string s);
    inline std::string getDescription() const {return description;};
    inline void setDescription(std::string s) {description=s;};
    inline size_t getConvives() const {return nbConvives;};
    inline void setConvives(size_t i) {nbConvives=i;};
    inline double getPrix() const {return prix;};
    inline void setPrix(double d) {prix=d;};
    inline std::string getCreateur() const {return createur;};
    inline void setCreateur(std::string s) {createur=s;};
    inline Date getDate() const {return date;};
    inline void setDate(Date d) {date=d;};
    void afficherIngredients();
    void ajouterIngredient(Ingredient* i);
    void retirerIngredient(Ingredient* i);
    void modifierIngredient(Ingredient* i);

};

#endif // RECETTE_H
