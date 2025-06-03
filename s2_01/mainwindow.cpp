#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <sstream>
#include <QJsonParseError>
#include <QDir>
#include <QApplication>
#include "recette.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Ajouter un menu pour choisir le format
    QMenuBar *menuBar = this->menuBar();
    QMenu *menuFichier = menuBar->addMenu("Fichier");

    QAction *actionOuvrir = menuFichier->addAction("Ouvrir fichier XML/JSON...");
    connect(actionOuvrir, &QAction::triggered, this, &MainWindow::ouvrirFichier);

    QAction *actionDefaut = menuFichier->addAction("Charger recettes par défaut");
    connect(actionDefaut, &QAction::triggered, this, &MainWindow::chargerRecettesParDefaut);

    menuFichier->addSeparator();

    QAction *actionQuitter = menuFichier->addAction("Quitter");
    connect(actionQuitter, &QAction::triggered, this, &QWidget::close);

    // Afficher un message d'aide
    qDebug() << "=== GESTIONNAIRE DE RECETTES ===";
    qDebug() << "Utilisez le menu 'Fichier' pour :";
    qDebug() << "- Ouvrir un fichier XML ou JSON";
    qDebug() << "- Charger les recettes par défaut";
    qDebug() << "===============================";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ouvrirFichier()
{
    QString fichier = QFileDialog::getOpenFileName(
        this,
        "Choisir un fichier de recettes",
        QDir::homePath(),
        "Fichiers de recettes (*.xml *.json);;Fichiers XML (*.xml);;Fichiers JSON (*.json);;Tous les fichiers (*)"
        );

    if (fichier.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(fichier);
    QString extension = fileInfo.suffix().toLower();

    qDebug() << "Ouverture du fichier:" << fichier;
    qDebug() << "Extension détectée:" << extension;

    if (extension == "xml") {
        chargerRecetteXML(fichier);
    } else if (extension == "json") {
        chargerRecetteJSON(fichier);
    } else {
        // Essayer de deviner le format en lisant le contenu
        QFile file(fichier);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString contenu = QTextStream(&file).readAll();
            file.close();

            // Détecter le format
            contenu = contenu.trimmed();
            if (contenu.startsWith("{") || contenu.startsWith("[")) {
                qDebug() << "Format JSON détecté automatiquement";
                chargerRecetteJSON(fichier);
            } else if (contenu.startsWith("<")) {
                qDebug() << "Format XML détecté automatiquement";
                chargerRecetteXML(fichier);
            } else {
                QMessageBox::warning(this, "Erreur",
                                     "Format de fichier non reconnu. Veuillez choisir un fichier XML ou JSON.");
            }
        }
    }
}

void MainWindow::chargerRecettesParDefaut()
{
    // Essayer de charger depuis les ressources (XML par défaut)
    QFile fichier(":/Recettes/recettes.xml");

    if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Chargement des recettes par défaut (XML)...";

        QTextStream stream(&fichier);
        QString contenu = stream.readAll();
        fichier.close();

        chargerRecetteDepuisContenu(contenu, "xml");
    } else {
        // Essayer JSON si XML n'est pas disponible
        QFile fichierJSON(":/Recettes/recettes.json");
        if (fichierJSON.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Chargement des recettes par défaut (JSON)...";

            QTextStream stream(&fichierJSON);
            QString contenu = stream.readAll();
            fichierJSON.close();

            chargerRecetteDepuisContenu(contenu, "json");
        } else {
            QMessageBox::information(this, "Information",
                                     "Aucun fichier de recettes par défaut trouvé.\n"
                                     "Utilisez 'Ouvrir fichier XML/JSON...' pour charger vos recettes.");
        }
    }
}

void MainWindow::chargerRecetteXML(const QString &cheminFichier)
{
    QFile fichier(cheminFichier);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur",
                             QString("Impossible d'ouvrir le fichier : %1").arg(cheminFichier));
        return;
    }

    QTextStream stream(&fichier);
    QString contenu = stream.readAll();
    fichier.close();

    chargerRecetteDepuisContenu(contenu, "xml");
}

void MainWindow::chargerRecetteJSON(const QString &cheminFichier)
{
    QFile fichier(cheminFichier);

    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur",
                             QString("Impossible d'ouvrir le fichier : %1").arg(cheminFichier));
        return;
    }

    QTextStream stream(&fichier);
    QString contenu = stream.readAll();
    fichier.close();

    chargerRecetteDepuisContenu(contenu, "json");
}

void MainWindow::chargerRecetteDepuisContenu(const QString &contenu, const QString &format)
{
    try {
        if (format.toLower() == "xml") {
            qDebug() << "Parsing XML...";
            qDebug() << "Contenu du fichier XML:";
            qDebug() << contenu;

            // Convertir en std::stringstream pour le constructeur existant
            std::string contenuStd = contenu.toStdString();
            std::stringstream ss(contenuStd);

            Recette recette(ss);
            qDebug() << "Recette XML chargée avec succès:" << recette;

            // Afficher chaque ingrédient individuellement
            qDebug() << "Liste des ingrédients (XML):";
            for (const Ingredient* ing : recette.getIngredients()) {
                qDebug() << *ing;
            }

        } else if (format.toLower() == "json") {
            qDebug() << "Parsing JSON...";

            QJsonParseError parseError;
            QJsonDocument document = QJsonDocument::fromJson(contenu.toUtf8(), &parseError);

            if (parseError.error != QJsonParseError::NoError) {
                QMessageBox::warning(this, "Erreur JSON",
                                     QString("Erreur de parsing JSON : %1").arg(parseError.errorString()));
                return;
            }

            if (document.isObject()) {
                // Un seul objet recette
                parseRecetteJSON(document.object());
            } else if (document.isArray()) {
                // Tableau de recettes
                QJsonArray recettesArray = document.array();
                qDebug() << "Nombre de recettes dans le fichier JSON:" << recettesArray.size();

                for (const QJsonValue &value : recettesArray) {
                    if (value.isObject()) {
                        parseRecetteJSON(value.toObject());
                    }
                }
            } else {
                QMessageBox::warning(this, "Erreur", "Format JSON invalide");
            }
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erreur de parsing",
                             QString("Erreur lors du chargement : %1").arg(e.what()));
        qDebug() << "Exception lors du parsing:" << e.what();
    } catch (...) {
        QMessageBox::warning(this, "Erreur", "Erreur inconnue lors du chargement du fichier");
        qDebug() << "Erreur inconnue lors du parsing";
    }
}

void MainWindow::parseRecetteJSON(const QJsonObject &recetteObj)
{
    try {
        // Extraire les informations de base
        std::string nom = recetteObj["nom"].toString().toStdString();
        std::string photo = recetteObj["photo"].toString().toStdString();
        std::string categorie = recetteObj["categorie"].toString().toStdString();
        std::string createur = recetteObj["createur"].toString().toStdString();

        size_t nbConvives = static_cast<size_t>(recetteObj["nombre_personnes"].toInt());
        double prix = recetteObj["prix"].toDouble();

        // Extraire la date
        Date date = parseDateJSON(recetteObj["date"].toString());

        // Extraire la description
        std::vector<std::string> descriptions;
        if (recetteObj["description"].isString()) {
            descriptions.push_back(recetteObj["description"].toString().toStdString());
        } else if (recetteObj["description"].isArray()) {
            QJsonArray descArray = recetteObj["description"].toArray();
            for (const QJsonValue &descValue : descArray) {
                descriptions.push_back(descValue.toString().toStdString());
            }
        }

        // Extraire les ingrédients
        std::vector<Ingredient*> ingredients;
        QJsonArray ingredientsArray = recetteObj["ingredients"].toArray();

        for (const QJsonValue &ingValue : ingredientsArray) {
            QJsonObject ingObj = ingValue.toObject();

            std::string nomIng = ingObj["nom"].toString().toStdString();
            double quantite = ingObj["quantite"].toDouble();
            std::string unite = ingObj["unite"].toString().toStdString();

            Ingredient* ingredient = new Ingredient(nomIng, quantite, unite);
            ingredients.push_back(ingredient);
        }

        // Créer la recette
        Recette recette(nom, photo, categorie, descriptions, nbConvives, prix, createur, date, ingredients);

        qDebug() << "Recette JSON chargée avec succès:" << recette;

        // Afficher chaque ingrédient individuellement
        qDebug() << "Liste des ingrédients (JSON):";
        for (const Ingredient* ing : recette.getIngredients()) {
            qDebug() << *ing;
        }

    } catch (const std::exception& e) {
        qDebug() << "Erreur lors du parsing de la recette JSON:" << e.what();
        throw;
    }
}

Date MainWindow::parseDateJSON(const QString &dateStr)
{
    // Format attendu : "DD/MM/YYYY" ou "YYYY-MM-DD"
    Date date = {1, 1, 2023}; // valeur par défaut

    if (dateStr.contains("/")) {
        // Format DD/MM/YYYY
        QStringList parts = dateStr.split("/");
        if (parts.size() == 3) {
            date.jour = static_cast<size_t>(parts[0].toInt());
            date.mois = static_cast<size_t>(parts[1].toInt());
            date.annee = static_cast<size_t>(parts[2].toInt());
        }
    } else if (dateStr.contains("-")) {
        // Format YYYY-MM-DD
        QStringList parts = dateStr.split("-");
        if (parts.size() == 3) {
            date.annee = static_cast<size_t>(parts[0].toInt());
            date.mois = static_cast<size_t>(parts[1].toInt());
            date.jour = static_cast<size_t>(parts[2].toInt());
        }
    }

    return date;
}
