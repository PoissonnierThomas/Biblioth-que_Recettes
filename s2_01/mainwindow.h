#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QFrame>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCloseEvent>
#include <QResizeEvent>
#include <memory>
#include <vector>
#include "recette.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Gestion des recettes
    void onRecetteSelectionnee();
    void onNouvelleRecette();
    void onSupprimerRecette();
    void onRechercheRecette(const QString &texte);

    // Mode édition
    void onModeEditionToggled(bool actif);
    void onSauvegarder();
    void onAnnuler();

    // Gestion des ingrédients
    void onAjouterIngredient();
    void onSupprimerIngredient();
    void onIngredientSelectionne();
    void onModifierIngredient();

    // Fichiers
    void ouvrirFichier();
    void onParcourirPhoto();

    // Détection des modifications
    void onDonneesModifiees();

private:
    // === WIDGETS PRINCIPAUX ===
    QWidget *centralWidget;
    QSplitter *mainSplitter;

    // === PANEL MASTER (LISTE) ===
    QFrame *frameListeRecettes;
    QLabel *labelTitreListe;
    QLineEdit *lineEditRecherche;
    QListWidget *listWidgetRecettes;
    QPushButton *btnNouvelleRecette;
    QPushButton *btnSupprimerRecette;

    // === PANEL DETAIL (EDITION) ===
    QFrame *frameDetailRecette;
    QLabel *labelTitreDetail;
    QPushButton *btnSauvegarder;
    QPushButton *btnAnnuler;
    QScrollArea *scrollArea;
    QWidget *scrollContent;

    // === INFORMATIONS GENERALES ===
    QGroupBox *groupBoxInfos;
    QLineEdit *lineEditNom;
    QComboBox *comboBoxCategorie;
    QSpinBox *spinBoxNbPersonnes;
    QDoubleSpinBox *doubleSpinBoxPrix;
    QLineEdit *lineEditCreateur;
    QDateEdit *dateEditDate;
    QLineEdit *lineEditPhoto;
    QPushButton *btnParcourirPhoto;

    // === DESCRIPTION ===
    QGroupBox *groupBoxDescription;
    QTextEdit *textEditDescription;

    // === INGREDIENTS ===
    QGroupBox *groupBoxIngredients;
    QTableWidget *tableWidgetIngredients;
    QPushButton *btnAjouterIngredient;
    QPushButton *btnSupprimerIngredient;

    // === MENUS ===
    QAction *actionOuvrir;
    QAction *actionSauvegarder;
    QAction *actionQuitter;
    QAction *actionModeEdition;

    // === DONNEES ===
    std::vector<std::unique_ptr<Recette>> recettes;
    Recette* recetteActuelle;
    bool modeEdition;
    bool donneesModifiees;

    // Sauvegarde pour annulation
    struct DonneesOriginales {
        std::string nom, categorie, photo, createur, description;
        size_t nbConvives;
        double prix;
        Date date;
        std::vector<Ingredient*> ingredients;
    } donneesOriginales;

    // === METHODES PRIVEES ===
    void setupInterface();
    void setupMenus();
    void setupMasterPanel();
    void setupDetailPanel();
    void connecterSignaux();

    // Gestion des données
    void chargerRecettesParDefaut();
    void chargerRecetteXML(const QString &fichier);
    void chargerRecetteJSON(const QString &fichier);
    void parseRecetteJSON(const QJsonObject &obj);
    Date parseDateJSON(const QString &dateStr);

    // Affichage
    void mettreAJourListeRecettes();
    void afficherRecette(Recette* recette);
    void viderFormulaire();
    void mettreAJourTableIngredients();
    QString formatRecetteListe(const Recette* recette) const;

    // Edition
    void activerEdition(bool activer);
    void connecterSignauxModification();
    void deconnecterSignauxModification();
    void sauvegarderRecetteActuelle();
    void sauvegarderDonneesOriginales();
    void restaurerDonneesOriginales();
    bool confirmerAbandonModifications();

    // Utilitaires
    void redimensionnerColonnes();
    void appliquerStyle();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINWINDOW_H
