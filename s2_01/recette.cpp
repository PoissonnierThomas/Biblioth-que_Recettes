#include <iostream>
#include <fstream>
#include <sstream>
#include <QFile>
#include <QDebug>
#include <QString>
#include "recette.h"

Recette::Recette(std::string sfichier) {
    std::ifstream fichier(sfichier);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << sfichier << std::endl;
        // Initialiser avec des valeurs par défaut
        nom = "Recette inconnue";
        photo = "";
        categorie = "plat";
        nbConvives = 1;
        prix = 0.0;
        createur = "Inconnu";
        date = {1, 1, 2023};
        return;
    }

    // Utiliser le constructeur istream
    *this = Recette(fichier);
    fichier.close();
}

Recette::Recette(std::istream &is) {
    std::string line;

    // Valeurs par défaut
    categorie = "plat";
    nbConvives = 1;
    prix = 0.0;
    date = {1, 1, 2023};

    // Lire jusqu'à trouver l'ouverture <recette>
    while (std::getline(is, line)) {
        if (line.find("<recette>") != std::string::npos) {
            break;
        }
    }

    // Lire les éléments de la recette
    while (std::getline(is, line)) {
        // Fin de la recette
        if (line.find("</recette>") != std::string::npos) {
            break;
        }

        // Extraire le nom
        if (line.find("<nom>") != std::string::npos) {
            size_t start = line.find("<nom>") + 5;
            size_t end = line.find("</nom>");
            nom = line.substr(start, end - start);
        }

        // Extraire la photo
        if (line.find("<photo>") != std::string::npos) {
            size_t start = line.find("<photo>") + 7;
            size_t end = line.find("</photo>");
            photo = line.substr(start, end - start);
        }

        // Extraire la catégorie
        if (line.find("<catégorie>") != std::string::npos) {
            size_t start = line.find("<catégorie>") + 11;
            size_t end = line.find("</catégorie>");
            std::string cat = line.substr(start, end - start);
            try {
                setCategorie(cat);
            } catch (...) {
                categorie = "plat"; // valeur par défaut en cas d'erreur
            }
        }

        // Extraire la description
        if (line.find("<description>") != std::string::npos) {
            size_t start = line.find("<description>") + 13;
            size_t end = line.find("</description>");
            std::string desc = line.substr(start, end - start);
            description.push_back(desc);
        }

        // Extraire le nombre de personnes
        if (line.find("<nombre_personnes>") != std::string::npos) {
            size_t start = line.find("<nombre_personnes>") + 18;
            size_t end = line.find("</nombre_personnes>");
            std::string nbStr = line.substr(start, end - start);
            nbConvives = std::stoul(nbStr);
        }

        // Extraire le prix
        if (line.find("<prix>") != std::string::npos) {
            size_t start = line.find("<prix>") + 6;
            size_t end = line.find("</prix>");
            std::string prixStr = line.substr(start, end - start);
            prix = std::stod(prixStr);
        }

        // Extraire le créateur
        if (line.find("<createur>") != std::string::npos) {
            size_t start = line.find("<createur>") + 10;
            size_t end = line.find("</createur>");
            createur = line.substr(start, end - start);
        }

        // Extraire la date (format DD/MM/YYYY)
        if (line.find("<date>") != std::string::npos) {
            size_t start = line.find("<date>") + 6;
            size_t end = line.find("</date>");
            std::string dateStr = line.substr(start, end - start);

            // Transformer la date au format DD/MM/YYYY
            std::stringstream ss(dateStr);
            std::string item;
            std::vector<std::string> dateTokens;

            while (std::getline(ss, item, '/')) {
                dateTokens.push_back(item);
            }

            if (dateTokens.size() == 3) {
                date.jour = std::stoul(dateTokens[0]);
                date.mois = std::stoul(dateTokens[1]);
                date.annee = std::stoul(dateTokens[2]);
            }
        }

        // Traiter les ingrédients
        if (line.find("<ingredient>") != std::string::npos) {
            // Remettre le flux au début de cette ligne pour que le constructeur d'Ingredient puisse la lire
            is.seekg(-(long)line.length() - 1, std::ios::cur);

            // Créer un nouvel ingrédient
            Ingredient* ingredient = new Ingredient(is);
            ingredients.push_back(ingredient);
        }
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

QDebug operator<<(QDebug debug, const Date& d)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << d.jour << "/" << d.mois << "/" << d.annee;
    return debug;
}



QDebug operator<<(QDebug debug, const Recette& r)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "\n=== RECETTE ===\n"
                    << "Nom: " << QString::fromStdString(r.getNom()) << "\n"
                    << "Catégorie: " << QString::fromStdString(r.getCategorie()) << "\n"
                    << "Pour " << r.getConvives() << " personnes\n"
                    << "Prix: " << r.getPrix() << "€ par personne\n"
                    << "Créateur: " << QString::fromStdString(r.getCreateur()) << "\n"
                    << "Date: " << r.getDate() << "\n"
                    << "Photo: " << QString::fromStdString(r.getPhoto()) << "\n"  // Décommentée
                    << "Description: ";

    const auto& desc = r.getDescription();
    if (!desc.empty()) {
        debug << QString::fromStdString(desc[0]);
    }

    debug << "\nIngrédients:\n";
    const auto& ingredients = r.getIngredients();
    for (const Ingredient* ingredient : ingredients) {
        debug << "  - " << *ingredient << "\n";
    }
    debug << "===============\n";

    return debug;
}
