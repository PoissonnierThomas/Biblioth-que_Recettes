#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <QFile>
#include <QDebug>
#include "ingredient.h"
#ifndef RECETTE_H
#define RECETTE_H

struct Date { // Structure de représentation simple des dates
    size_t jour;
    size_t mois;
    size_t annee;
};

QDebug operator<<(QDebug debug, const Date& d);

class Recette
{
private :
    std::string nom;
    std::string photo;  // Contient le nom de l'emplacement de la photo correspondante
    std::string categorie; // Contient le nom de la catégorie ( Entrée, Plat, Dessert, Entremet, Soupe)
    std::vector<std::string> description; // Contient une description des étapes de la recette
    size_t nbConvives; // Contient le nombre de personnes prévu par la recette initiale
    double prix; // Contient le prix unitaire que coûte la recette
    std::string createur; // Contient le nom du créateur de la recette
    Date date; // Date de dépôt de la recette sur l'appli
    std::vector<Ingredient*> ingredients; // Contient les différents ingrédients, dans lesquels on retrouve le nom et la quantité

public:
    Recette()=delete; // Interdire la création de recette sans paramètre
    Recette(std::string nom_fichier); // Création de recette à partir du nom de fichier
    Recette(std::istream &is); // Création de recette à partir d'un flux
    Recette(std::string _nom, std::string _photo,std::string _categorie, std::vector<std::string> _description, size_t _nombre, double _prix, std::string _createur, Date _date, std::vector<Ingredient*> _Ingredients );
    ~Recette();

    // Getters et Setters
    inline std::string getNom() const {return nom;};
    inline void setNom(std::string s) {nom=s;};
    inline std::string getCategorie() const {return categorie;};
    void setCategorie(std::string s);
    inline std::vector<std::string> getDescription() const {return description;};
    inline void setDescription(std::string s) {description.clear(); description.push_back(s);};
    inline size_t getConvives() const {return nbConvives;};
    inline void setConvives(size_t i) {nbConvives=i;};
    inline double getPrix() const {return prix;};
    inline void setPrix(double d) {prix=d;};
    inline std::string getCreateur() const {return createur;};
    inline void setCreateur(std::string s) {createur=s;};
    inline Date getDate() const {return date;};
    inline void setDate(Date d) {date=d;};
    inline std::string getPhoto() const {return photo;};
    inline void setPhoto(std::string s) {photo=s;};
    inline size_t getNombreIngredients() const {return ingredients.size();};

    // Gestion des ingrédients
    inline std::vector<Ingredient*> getIngredients() const {return ingredients;};
    inline void ajouterIngredient(Ingredient* i) {ingredients.push_back(i);};
    void retirerIngredient(Ingredient* i);
    void retirerIngredientParIndex(int index);
    void modifierIngredient(Ingredient* ancien, Ingredient* nouveau);
    void modifierIngredientParIndex(int index, const std::string& nom, double quantite, const std::string& unite);
    void viderIngredients();
    static std::vector<std::unique_ptr<Recette>> chargerDepuisFlux(std::istream& is);

    // Opérateurs
    friend std::ostream& operator<<(std::ostream &os, const Recette &R);
    friend QDebug operator<<(QDebug debug, const Recette& R);
};

#endif // RECETTE_H
