#include <iostream>
#include <vector>
#include "recette.h"

int main()              // Fichier ajouté pour tester les classes sans avoir à utiliser l'interface Qt
{
    // Test sur les getteurs et setteurs de Recette
    std::vector<Recette> Recettes;
    Date d;
    std::vector<Ingredient *> i;
    i.push_back(new Ingredient ("Patate",500,true));
    Ingredient I1("Crème",0.3,false);
    std::vector<std::string> v;
    v.push_back("Un écrasé de pommes de terre accompagné de crèmes");
    Recette R("Purée","purée.png","Plat",v,4,2.5,"Poissonnier",d,i);
    std::cout<<R;
    std::cout<<I1<<std::endl;
    return 0;
}