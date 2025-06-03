#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <sstream>
#include "recette.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Charger et afficher les recettes au démarrage
    chargerRecettes();
}

void MainWindow::chargerRecettes()
{
    // Utiliser le bon chemin selon votre fichier ressources.qrc
    QFile fichier(":/Recettes/recettes.xml");

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erreur: Impossible d'ouvrir le fichier de ressources" << fichier.errorString();
        return;
    }

    // Lire le contenu du fichier
    QTextStream stream(&fichier);
    QString contenu = stream.readAll();
    fichier.close();

    qDebug() << "Contenu du fichier XML lu:";
    qDebug() << contenu;

    // Convertir en std::stringstream pour utiliser avec votre constructeur
    std::string contenuStd = contenu.toStdString();
    std::stringstream ss(contenuStd);

    try {
        Recette recette(ss);
        qDebug() << "Recette chargée:" << recette;

        // Afficher chaque ingrédient individuellement
        qDebug() << "Liste des ingrédients:";
        for (const Ingredient* ing : recette.getIngredients()) {
            qDebug() << *ing;
        }

    } catch (const std::exception& e) {
        qDebug() << "Exception lors du parsing de la recette:" << e.what();
    } catch (...) {
        qDebug() << "Erreur inconnue lors du parsing de la recette";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
