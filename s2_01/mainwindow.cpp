#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <sstream>
#include <QJsonParseError>
#include <QTableWidgetItem>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , recetteActuelle(nullptr)
    , modeEdition(false)
    , donneesModifiees(false)
{
    setWindowTitle(tr("Recipe Manager - S2.01"));
    setMinimumSize(1000, 700);
    resize(1200, 800);

    setupInterface();
    setupMenus();
    connecterSignaux();
    appliquerStyle();

    // Charger les données au démarrage
    chargerRecettesParDefaut();

    statusBar()->showMessage(tr("Application started - Lecture mode"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupInterface()
{
    // Widget central
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal avec splitter
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(mainSplitter);

    setupMasterPanel();
    setupDetailPanel();

    mainSplitter->addWidget(frameListeRecettes);
    mainSplitter->addWidget(frameDetailRecette);
    mainSplitter->setSizes({350, 850});
}

void MainWindow::setupMasterPanel()
{
    // === FRAME LISTE RECETTES ===
    frameListeRecettes = new QFrame();
    frameListeRecettes->setFrameStyle(QFrame::StyledPanel);
    frameListeRecettes->setMinimumWidth(300);
    frameListeRecettes->setMaximumWidth(400);

    QVBoxLayout *layoutMaster = new QVBoxLayout(frameListeRecettes);
    layoutMaster->setSpacing(10);

    // Titre
    labelTitreListe = new QLabel(tr("Recipe list"));
    labelTitreListe->setAlignment(Qt::AlignCenter);
    QFont fontTitre = labelTitreListe->font();
    fontTitre.setPointSize(14);
    fontTitre.setBold(true);
    labelTitreListe->setFont(fontTitre);
    layoutMaster->addWidget(labelTitreListe);

    // Barre de recherche
    lineEditRecherche = new QLineEdit();
    lineEditRecherche->setPlaceholderText(tr(" Search for a recipe ..."));
    layoutMaster->addWidget(lineEditRecherche);

    // Liste des recettes
    listWidgetRecettes = new QListWidget();
    listWidgetRecettes->setAlternatingRowColors(true);
    listWidgetRecettes->setSelectionMode(QAbstractItemView::SingleSelection);
    layoutMaster->addWidget(listWidgetRecettes);

    // Boutons d'action
    QHBoxLayout *layoutBoutons = new QHBoxLayout();

    btnNouvelleRecette = new QPushButton(tr("New"));
    btnSupprimerRecette = new QPushButton(tr("Delete"));
    btnSupprimerRecette->setEnabled(false);

    layoutBoutons->addWidget(btnNouvelleRecette);
    layoutBoutons->addWidget(btnSupprimerRecette);

    layoutMaster->addLayout(layoutBoutons);
}

void MainWindow::setupDetailPanel()
{
    // === FRAME DETAIL RECETTE ===
    frameDetailRecette = new QFrame();
    frameDetailRecette->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *layoutDetail = new QVBoxLayout(frameDetailRecette);
    layoutDetail->setSpacing(10);

    // En-tête avec titre et boutons
    QHBoxLayout *layoutHeader = new QHBoxLayout();

    labelTitreDetail = new QLabel(tr("Recipe details"));
    QFont fontDetail = labelTitreDetail->font();
    fontDetail.setPointSize(16);
    fontDetail.setBold(true);
    labelTitreDetail->setFont(fontDetail);

    btnSauvegarder = new QPushButton(tr("Save"));
    btnAnnuler = new QPushButton(tr("Cancel"));
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);

    layoutHeader->addWidget(labelTitreDetail);
    layoutHeader->addStretch();
    layoutHeader->addWidget(btnSauvegarder);
    layoutHeader->addWidget(btnAnnuler);

    layoutDetail->addLayout(layoutHeader);

    // Zone de contenu avec scroll
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollContent = new QWidget();
    QVBoxLayout *layoutScroll = new QVBoxLayout(scrollContent);

    // === INFORMATIONS GENERALES ===
    groupBoxInfos = new QGroupBox(tr("General informations"));
    QFormLayout *formInfos = new QFormLayout(groupBoxInfos);

    lineEditNom = new QLineEdit();
    lineEditNom->setEnabled(false);
    formInfos->addRow(tr("Name :"), lineEditNom);

    comboBoxCategorie = new QComboBox();
    comboBoxCategorie->addItems({"Entrée", "Plat", "Dessert", "Entremet", "Soupe"});
    comboBoxCategorie->setEnabled(false);
    formInfos->addRow(tr("Category :"), comboBoxCategorie);

    spinBoxNbPersonnes = new QSpinBox();
    spinBoxNbPersonnes->setRange(1, 20);
    spinBoxNbPersonnes->setEnabled(false);
    formInfos->addRow(tr("Nb. of people :"), spinBoxNbPersonnes);

    doubleSpinBoxPrix = new QDoubleSpinBox();
    doubleSpinBoxPrix->setRange(0.0, 999.99);
    doubleSpinBoxPrix->setDecimals(2);
    doubleSpinBoxPrix->setSuffix(" €");
    doubleSpinBoxPrix->setEnabled(false);
    formInfos->addRow(tr("Price/pers. :"), doubleSpinBoxPrix);

    lineEditCreateur = new QLineEdit();
    lineEditCreateur->setEnabled(false);
    formInfos->addRow(tr("Creator :"), lineEditCreateur);

    dateEditDate = new QDateEdit();
    dateEditDate->setCalendarPopup(true);
    dateEditDate->setEnabled(false);
    formInfos->addRow("Date :", dateEditDate);

    QHBoxLayout *layoutPhoto = new QHBoxLayout();
    lineEditPhoto = new QLineEdit();
    lineEditPhoto->setPlaceholderText(tr("Path to te picture"));
    lineEditPhoto->setEnabled(false);
    btnParcourirPhoto = new QPushButton(tr("Browse"));
    btnParcourirPhoto->setEnabled(false);
    layoutPhoto->addWidget(lineEditPhoto);
    layoutPhoto->addWidget(btnParcourirPhoto);
    formInfos->addRow(tr("Picture :"), layoutPhoto);

    layoutScroll->addWidget(groupBoxInfos);

    // === DESCRIPTION ===
    groupBoxDescription = new QGroupBox(tr("Description"));
    QVBoxLayout *layoutDesc = new QVBoxLayout(groupBoxDescription);

    textEditDescription = new QTextEdit();
    textEditDescription->setMaximumHeight(120);
    textEditDescription->setEnabled(false);
    layoutDesc->addWidget(textEditDescription);

    layoutScroll->addWidget(groupBoxDescription);

    // === INGREDIENTS ===
    groupBoxIngredients = new QGroupBox(tr("Ingredients"));
    QVBoxLayout *layoutIng = new QVBoxLayout(groupBoxIngredients);

    QHBoxLayout *layoutBoutonsIng = new QHBoxLayout();
    btnAjouterIngredient = new QPushButton(tr("Add"));
    btnSupprimerIngredient = new QPushButton(tr("Delete"));
    btnAjouterIngredient->setEnabled(false);
    btnSupprimerIngredient->setEnabled(false);

    layoutBoutonsIng->addWidget(btnAjouterIngredient);
    layoutBoutonsIng->addWidget(btnSupprimerIngredient);
    layoutBoutonsIng->addStretch();

    layoutIng->addLayout(layoutBoutonsIng);

    tableWidgetIngredients = new QTableWidget();
    tableWidgetIngredients->setColumnCount(3);
    QStringList headers{"Nom", "Quantité", "Unité"};
    tableWidgetIngredients->setHorizontalHeaderLabels(headers);
    tableWidgetIngredients->setAlternatingRowColors(true);
    tableWidgetIngredients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidgetIngredients->horizontalHeader()->setStretchLastSection(true);
    tableWidgetIngredients->setEnabled(false);

    // Tooltip pour indiquer le double-clic
    tableWidgetIngredients->setToolTip(tr("Double-click on an ingredient to edit it"));

    layoutIng->addWidget(tableWidgetIngredients);

    layoutScroll->addWidget(groupBoxIngredients);

    scrollArea->setWidget(scrollContent);
    layoutDetail->addWidget(scrollArea);
}

void MainWindow::setupMenus()
{
    // Menu Fichier
    QMenu *menuFichier = menuBar()->addMenu(tr("File"));

    actionOuvrir = new QAction(tr("Open ..."), this);
    actionOuvrir->setShortcut(QKeySequence::Open);
    menuFichier->addAction(actionOuvrir);

    actionSauvegarder = new QAction(tr("Save"), this);
    actionSauvegarder->setShortcut(QKeySequence::Save);
    menuFichier->addAction(actionSauvegarder);

    menuFichier->addSeparator();

    actionQuitter = new QAction(tr("Quit"), this);
    actionQuitter->setShortcut(QKeySequence::Quit);
    menuFichier->addAction(actionQuitter);

    // Menu Edition
    QMenu *menuEdition = menuBar()->addMenu(tr("Edit"));

    actionModeEdition = new QAction(tr("Edition mode"), this);
    actionModeEdition->setCheckable(true);
    actionModeEdition->setShortcut(QKeySequence("Ctrl+E"));
    menuEdition->addAction(actionModeEdition);
}

void MainWindow::connecterSignaux()
{
    // Liste des recettes
    connect(listWidgetRecettes, &QListWidget::currentRowChanged,
            this, &MainWindow::onRecetteSelectionnee);

    // Boutons principaux
    connect(btnNouvelleRecette, &QPushButton::clicked,
            this, &MainWindow::onNouvelleRecette);
    connect(btnSupprimerRecette, &QPushButton::clicked,
            this, &MainWindow::onSupprimerRecette);
    connect(btnSauvegarder, &QPushButton::clicked,
            this, &MainWindow::onSauvegarder);
    connect(btnAnnuler, &QPushButton::clicked,
            this, &MainWindow::onAnnuler);

    // Recherche
    connect(lineEditRecherche, &QLineEdit::textChanged,
            this, &MainWindow::onRechercheRecette);

    // Ingrédients
    connect(btnAjouterIngredient, &QPushButton::clicked,
            this, &MainWindow::onAjouterIngredient);
    connect(btnSupprimerIngredient, &QPushButton::clicked,
            this, &MainWindow::onSupprimerIngredient);
    connect(tableWidgetIngredients, &QTableWidget::currentItemChanged,
            this, &MainWindow::onIngredientSelectionne);
    connect(tableWidgetIngredients, &QTableWidget::itemDoubleClicked,
            this, &MainWindow::onModifierIngredient);

    // Actions du menu
    connect(actionModeEdition, &QAction::toggled,
            this, &MainWindow::onModeEditionToggled);
    connect(actionOuvrir, &QAction::triggered,
            this, &MainWindow::ouvrirFichier);
    connect(actionQuitter, &QAction::triggered,
            this, &QWidget::close);

    // Autres
    connect(btnParcourirPhoto, &QPushButton::clicked,
            this, &MainWindow::onParcourirPhoto);
}

void MainWindow::chargerRecettesParDefaut()
{
    QFile fichier(":/Recettes/recettes.xml");

    if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << tr("Default xml loading ...");
        QTextStream stream(&fichier);
        QString contenu = stream.readAll();
        fichier.close();

        std::string contenuStd = contenu.toStdString();
        std::stringstream ss(contenuStd);

        try {
            auto recette = std::make_unique<Recette>(ss);
            qDebug() << tr("Charged recipe:") << *recette;

            recettes.push_back(std::move(recette));
            mettreAJourListeRecettes();

        } catch (const std::exception& e) {
            qDebug() << tr("Error parsing XML:") << e.what();
        }
    } else {
        qDebug() << tr("Resource file not found");
        statusBar()->showMessage(tr("No default recipe - Use File > Open"));
    }
}

void MainWindow::mettreAJourListeRecettes()
{
    // Bloquer temporairement les signaux pour éviter les changements de sélection intempestifs
    listWidgetRecettes->blockSignals(true);

    int ancienneSelection = listWidgetRecettes->currentRow();
    listWidgetRecettes->clear();

    for (const auto& recette : recettes) {
        QString texte = formatRecetteListe(recette.get());
        listWidgetRecettes->addItem(texte);
    }

    // Restaurer la sélection ou sélectionner la première recette
    if (!recettes.empty()) {
        if (ancienneSelection >= 0 && ancienneSelection < listWidgetRecettes->count()) {
            listWidgetRecettes->setCurrentRow(ancienneSelection);
        } else {
            listWidgetRecettes->setCurrentRow(0);
        }
    }

    // Réactiver les signaux
    listWidgetRecettes->blockSignals(false);
}

QString MainWindow::formatRecetteListe(const Recette* recette) const
{
    if (!recette) return "";

    return QString("%1 (%2)\n%3 personnes - %.2f€")
        .arg(QString::fromStdString(recette->getNom()))
        .arg(QString::fromStdString(recette->getCategorie()))
        .arg(recette->getConvives())
        .arg(recette->getPrix());
}

void MainWindow::onRecetteSelectionnee()
{
    int index = listWidgetRecettes->currentRow();

    if (index >= 0 && index < static_cast<int>(recettes.size())) {
        // Vérifier les modifications non sauvegardées AVANT de changer
        if (donneesModifiees && !confirmerAbandonModifications()) {
            // Remettre la sélection sur l'ancienne recette
            for (size_t i = 0; i < recettes.size(); ++i) {
                if (recettes[i].get() == recetteActuelle) {
                    // Bloquer temporairement le signal pour éviter la récursion
                    listWidgetRecettes->blockSignals(true);
                    listWidgetRecettes->setCurrentRow(static_cast<int>(i));
                    listWidgetRecettes->blockSignals(false);
                    return;
                }
            }
        }

        recetteActuelle = recettes[index].get();
        afficherRecette(recetteActuelle);
        btnSupprimerRecette->setEnabled(true);

        qDebug() << tr("Selected recipe:") << QString::fromStdString(recetteActuelle->getNom());

    } else {
        recetteActuelle = nullptr;
        viderFormulaire();
        btnSupprimerRecette->setEnabled(false);
    }
}

void MainWindow::afficherRecette(Recette* recette)
{
    if (!recette) {
        viderFormulaire();
        return;
    }

    // 1. DÉCONNECTER TOUS LES SIGNAUX (même si pas en mode édition)
    deconnecterSignauxModification();

    // 2. REMPLIR LES CHAMPS (sans déclencher de signaux)
    lineEditNom->setText(QString::fromStdString(recette->getNom()));
    comboBoxCategorie->setCurrentText(QString::fromStdString(recette->getCategorie()));
    spinBoxNbPersonnes->setValue(static_cast<int>(recette->getConvives()));
    doubleSpinBoxPrix->setValue(recette->getPrix());
    lineEditCreateur->setText(QString::fromStdString(recette->getCreateur()));
    lineEditPhoto->setText(QString::fromStdString(recette->getPhoto()));

    // Date
    Date dateRecette = recette->getDate();
    QDate qdate(static_cast<int>(dateRecette.annee),
                static_cast<int>(dateRecette.mois),
                static_cast<int>(dateRecette.jour));
    dateEditDate->setDate(qdate);

    // Description
    const auto& descriptions = recette->getDescription();
    QString descriptionComplete;
    for (const auto& desc : descriptions) {
        if (!descriptionComplete.isEmpty()) {
            descriptionComplete += "\n";
        }
        descriptionComplete += QString::fromStdString(desc);
    }
    textEditDescription->setPlainText(descriptionComplete);

    // Mettre à jour les ingrédients
    mettreAJourTableIngredients();

    // Titre
    labelTitreDetail->setText(QString(tr("Details : %1"))
                                  .arg(QString::fromStdString(recette->getNom())));

    // 3. SAUVEGARDER LES DONNÉES ORIGINALES
    sauvegarderDonneesOriginales();

    // 4. REMETTRE L'ÉTAT À "NON MODIFIÉ" AVANT DE RECONNECTER
    donneesModifiees = false;
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);

    // 5. RECONNECTER LES SIGNAUX SEULEMENT SI EN MODE ÉDITION
    if (modeEdition) {
        connecterSignauxModification();
    }

    qDebug() << tr("Recipe displayed, Modified Data =") << donneesModifiees << ", modeEdition =" << modeEdition;
}

void MainWindow::mettreAJourTableIngredients()
{
    if (!recetteActuelle) return;

    // Bloquer les signaux de la table pendant la mise à jour
    tableWidgetIngredients->blockSignals(true);

    const auto& ingredients = recetteActuelle->getIngredients();
    tableWidgetIngredients->setRowCount(static_cast<int>(ingredients.size()));

    for (size_t i = 0; i < ingredients.size(); ++i) {
        const Ingredient* ing = ingredients[i];

        QTableWidgetItem* itemNom = new QTableWidgetItem(
            QString::fromStdString(ing->getNom()));
        QTableWidgetItem* itemQuantite = new QTableWidgetItem(
            QString::number(ing->getQuantite()));
        QTableWidgetItem* itemUnite = new QTableWidgetItem(
            QString::fromStdString(ing->getUnite()));

        tableWidgetIngredients->setItem(static_cast<int>(i), 0, itemNom);
        tableWidgetIngredients->setItem(static_cast<int>(i), 1, itemQuantite);
        tableWidgetIngredients->setItem(static_cast<int>(i), 2, itemUnite);
    }

    redimensionnerColonnes();

    // Réactiver les signaux
    tableWidgetIngredients->blockSignals(false);
}

void MainWindow::viderFormulaire()
{
    // Déconnecter avant de vider pour éviter les signaux parasites
    deconnecterSignauxModification();

    lineEditNom->clear();
    comboBoxCategorie->setCurrentIndex(0);
    spinBoxNbPersonnes->setValue(1);
    doubleSpinBoxPrix->setValue(0.0);
    lineEditCreateur->clear();
    lineEditPhoto->clear();
    dateEditDate->setDate(QDate::currentDate());
    textEditDescription->clear();
    tableWidgetIngredients->setRowCount(0);
    labelTitreDetail->setText(tr("Select a recipe"));

    // Remettre l'état propre
    donneesModifiees = false;
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);
}

void MainWindow::onModeEditionToggled(bool actif)
{
    modeEdition = actif;
    activerEdition(actif);

    if (actif) {
        connecterSignauxModification();
        statusBar()->showMessage(tr("Edition mode active"));
    } else {
        deconnecterSignauxModification();
        statusBar()->showMessage(tr("Lecture mode"));

        if (donneesModifiees && !confirmerAbandonModifications()) {
            actionModeEdition->setChecked(true);
            return;
        }

        if (donneesModifiees) {
            restaurerDonneesOriginales();
        }
    }
}

void MainWindow::activerEdition(bool activer)
{
    // Champs de saisie
    lineEditNom->setEnabled(activer);
    comboBoxCategorie->setEnabled(activer);
    spinBoxNbPersonnes->setEnabled(activer);
    doubleSpinBoxPrix->setEnabled(activer);
    lineEditCreateur->setEnabled(activer);
    lineEditPhoto->setEnabled(activer);
    dateEditDate->setEnabled(activer);
    textEditDescription->setEnabled(activer);
    tableWidgetIngredients->setEnabled(activer);

    // Boutons
    btnAjouterIngredient->setEnabled(activer);
    btnSupprimerIngredient->setEnabled(activer && tableWidgetIngredients->currentRow() >= 0);
    btnParcourirPhoto->setEnabled(activer);
    btnSauvegarder->setEnabled(activer && donneesModifiees);
    btnAnnuler->setEnabled(activer && donneesModifiees);
}

void MainWindow::connecterSignauxModification()
{
    connect(lineEditNom, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    connect(comboBoxCategorie, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDonneesModifiees);
    connect(spinBoxNbPersonnes, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onDonneesModifiees);
    connect(doubleSpinBoxPrix, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onDonneesModifiees);
    connect(lineEditCreateur, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    connect(lineEditPhoto, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    connect(dateEditDate, &QDateEdit::dateChanged, this, &MainWindow::onDonneesModifiees);
    connect(textEditDescription, &QTextEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    connect(tableWidgetIngredients, &QTableWidget::itemChanged, this, &MainWindow::onDonneesModifiees);
}

void MainWindow::deconnecterSignauxModification()
{
    disconnect(lineEditNom, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    disconnect(comboBoxCategorie, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &MainWindow::onDonneesModifiees);
    disconnect(spinBoxNbPersonnes, QOverload<int>::of(&QSpinBox::valueChanged),
               this, &MainWindow::onDonneesModifiees);
    disconnect(doubleSpinBoxPrix, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
               this, &MainWindow::onDonneesModifiees);
    disconnect(lineEditCreateur, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    disconnect(lineEditPhoto, &QLineEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    disconnect(dateEditDate, &QDateEdit::dateChanged, this, &MainWindow::onDonneesModifiees);
    disconnect(textEditDescription, &QTextEdit::textChanged, this, &MainWindow::onDonneesModifiees);
    disconnect(tableWidgetIngredients, &QTableWidget::itemChanged, this, &MainWindow::onDonneesModifiees);
}

void MainWindow::onDonneesModifiees()
{
    if (!modeEdition) return; // Ignorer si pas en mode édition

    donneesModifiees = true;
    btnSauvegarder->setEnabled(true);
    btnAnnuler->setEnabled(true);
    statusBar()->showMessage(tr("Modifications not saved"), 2000);

    qDebug() << tr("Modified data detected");
}

void MainWindow::onSauvegarder()
{
    if (!recetteActuelle || !modeEdition) return;

    sauvegarderRecetteActuelle();
    sauvegarderDonneesOriginales();

    donneesModifiees = false;
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);

    mettreAJourListeRecettes();
    statusBar()->showMessage(tr("Recipe saved"), 2000);
}

void MainWindow::onAnnuler()
{
    if (!confirmerAbandonModifications()) return;

    restaurerDonneesOriginales();

    donneesModifiees = false;
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);

    statusBar()->showMessage(tr("Modifications canceled"), 2000);
}

void MainWindow::sauvegarderRecetteActuelle()
{
    if (!recetteActuelle) return;

    // Sauvegarder les modifications des champs de base
    recetteActuelle->setNom(lineEditNom->text().toStdString());
    recetteActuelle->setCategorie(comboBoxCategorie->currentText().toStdString());
    recetteActuelle->setConvives(static_cast<size_t>(spinBoxNbPersonnes->value()));
    recetteActuelle->setPrix(doubleSpinBoxPrix->value());
    recetteActuelle->setCreateur(lineEditCreateur->text().toStdString());
    recetteActuelle->setPhoto(lineEditPhoto->text().toStdString());

    // Date
    QDate qdate = dateEditDate->date();
    Date nouvelleDate = {static_cast<size_t>(qdate.day()),
                         static_cast<size_t>(qdate.month()),
                         static_cast<size_t>(qdate.year())};
    recetteActuelle->setDate(nouvelleDate);

    // Description
    QString description = textEditDescription->toPlainText();
    recetteActuelle->setDescription(description.toStdString());

    // Les ingrédients sont déjà sauvegardés automatiquement lors de l'ajout/suppression
    // via les méthodes onAjouterIngredient() et onSupprimerIngredient()

    qDebug() << tr("Recipe saved :") << *recetteActuelle;
}

void MainWindow::sauvegarderDonneesOriginales()
{
    if (!recetteActuelle) return;

    donneesOriginales.nom = recetteActuelle->getNom();
    donneesOriginales.categorie = recetteActuelle->getCategorie();
    donneesOriginales.photo = recetteActuelle->getPhoto();
    donneesOriginales.createur = recetteActuelle->getCreateur();

    const auto& descriptions = recetteActuelle->getDescription();
    donneesOriginales.description = descriptions.empty() ? "" : descriptions[0];

    donneesOriginales.nbConvives = recetteActuelle->getConvives();
    donneesOriginales.prix = recetteActuelle->getPrix();
    donneesOriginales.date = recetteActuelle->getDate();

    // Copier les ingrédients
    donneesOriginales.ingredients.clear();
    for (const auto* ing : recetteActuelle->getIngredients()) {
        donneesOriginales.ingredients.push_back(const_cast<Ingredient*>(ing));
    }
}

void MainWindow::restaurerDonneesOriginales()
{
    if (!recetteActuelle) return;

    // Restaurer les données originales
    recetteActuelle->setNom(donneesOriginales.nom);
    recetteActuelle->setCategorie(donneesOriginales.categorie);
    recetteActuelle->setPhoto(donneesOriginales.photo);
    recetteActuelle->setCreateur(donneesOriginales.createur);
    recetteActuelle->setDescription(donneesOriginales.description);
    recetteActuelle->setConvives(donneesOriginales.nbConvives);
    recetteActuelle->setPrix(donneesOriginales.prix);
    recetteActuelle->setDate(donneesOriginales.date);

    // Réafficher la recette
    afficherRecette(recetteActuelle);
}

bool MainWindow::confirmerAbandonModifications()
{
    if (!donneesModifiees) return true;

    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Modifications not saved"),
        tr("Modifications haven't been saved.\n"
        "Would you like to cancel it ?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    return reponse == QMessageBox::Yes;
}

void MainWindow::onNouvelleRecette()
{
    // Vérifier les modifications non sauvegardées
    if (donneesModifiees && !confirmerAbandonModifications()) {
        return;
    }

    // Créer une nouvelle recette
    Date dateActuelle = {static_cast<size_t>(QDate::currentDate().day()),
                         static_cast<size_t>(QDate::currentDate().month()),
                         static_cast<size_t>(QDate::currentDate().year())};

    auto nouvelleRecette = std::make_unique<Recette>(
        "Nouvelle Recette",
        "",
        "Plat",
        std::vector<std::string>{"Description de la nouvelle recette"},
        4,
        0.0,
        "Utilisateur",
        dateActuelle,
        std::vector<Ingredient*>()
        );

    // Ajouter à la liste
    recettes.push_back(std::move(nouvelleRecette));
    mettreAJourListeRecettes();

    // Sélectionner la nouvelle recette
    listWidgetRecettes->setCurrentRow(static_cast<int>(recettes.size()) - 1);

    // Activer le mode édition automatiquement
    actionModeEdition->setChecked(true);

    statusBar()->showMessage(tr("new recipe created"), 2000);
}

void MainWindow::onSupprimerRecette()
{
    int index = listWidgetRecettes->currentRow();
    if (index < 0 || index >= static_cast<int>(recettes.size())) return;

    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Delete the recipe"),
        QString(tr("Are you sure you want to delete the '%1' recipe?"))
            .arg(QString::fromStdString(recettes[index]->getNom())),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reponse == QMessageBox::Yes) {
        recettes.erase(recettes.begin() + index);
        mettreAJourListeRecettes();
        viderFormulaire();
        recetteActuelle = nullptr;
        statusBar()->showMessage(tr("Recipe deleted"), 2000);
    }
}

void MainWindow::onRechercheRecette(const QString &texte)
{
    // Filtrer la liste selon le texte de recherche
    for (int i = 0; i < listWidgetRecettes->count(); ++i) {
        QListWidgetItem* item = listWidgetRecettes->item(i);
        bool visible = texte.isEmpty() ||
                       item->text().contains(texte, Qt::CaseInsensitive);
        item->setHidden(!visible);
    }
}

void MainWindow::onAjouterIngredient()
{
    if (!modeEdition || !recetteActuelle) return;

    bool ok;
    QString nom = QInputDialog::getText(this, tr("New ingredient"),
                                        tr("Name of the ingredient:"),
                                        QLineEdit::Normal, "", &ok);

    if (!ok || nom.isEmpty()) return;

    double quantite = QInputDialog::getDouble(this, tr("Quantity"),
                                              tr("Quantity:"),
                                              1.0, 0.0, 9999.99, 2, &ok);

    if (!ok) return;

    QString unite = QInputDialog::getText(this, tr("Unity="),
                                          tr("Unity (French unity such as :g, ml, pièce, etc.):"),
                                          QLineEdit::Normal, "g", &ok);

    if (!ok) return;

    try {
        // Créer le nouvel ingrédient
        Ingredient* nouvelIngredient = new Ingredient(nom.toStdString(),
                                                      quantite,
                                                      unite.toStdString());

        // L'ajouter à la recette actuelle
        recetteActuelle->ajouterIngredient(nouvelIngredient);

        // Mettre à jour l'affichage de la table
        mettreAJourTableIngredients();

        // Marquer comme modifié
        onDonneesModifiees();

        statusBar()->showMessage(tr("Ingredient added successfully"), 2000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Error while adding the ingredient : %1")).arg(e.what()));
    }
}

void MainWindow::onSupprimerIngredient()
{
    int row = tableWidgetIngredients->currentRow();
    if (row < 0 || !modeEdition || !recetteActuelle) return;

    // Vérifier qu'il y a des ingrédients
    if (row >= static_cast<int>(recetteActuelle->getNombreIngredients())) return;

    // Récupérer le nom de l'ingrédient pour la confirmation
    QString nomIngredient = tableWidgetIngredients->item(row, 0)->text();

    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Delete the ingredient"),
        QString(tr("Are you sure you want to delete the '%1' ingredient ?")).arg(nomIngredient),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reponse == QMessageBox::Yes) {
        try {
            // Supprimer l'ingrédient de la recette par index (plus simple et sûr)
            recetteActuelle->retirerIngredientParIndex(row);

            // Mettre à jour l'affichage
            mettreAJourTableIngredients();

            // Marquer comme modifié
            onDonneesModifiees();

            statusBar()->showMessage(QString(tr("'%1' ingredient deleted successfully")).arg(nomIngredient), 2000);

        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Error while deleting : %1")).arg(e.what()));
        }
    }
}

void MainWindow::onIngredientSelectionne()
{
    bool hasSelection = tableWidgetIngredients->currentRow() >= 0;
    btnSupprimerIngredient->setEnabled(modeEdition && hasSelection);
}

void MainWindow::onModifierIngredient()
{
    if (!modeEdition || !recetteActuelle) return;

    int row = tableWidgetIngredients->currentRow();
    if (row < 0 || row >= static_cast<int>(recetteActuelle->getNombreIngredients())) return;

    // Récupérer les valeurs actuelles
    QString nomActuel = tableWidgetIngredients->item(row, 0)->text();
    QString quantiteActuelle = tableWidgetIngredients->item(row, 1)->text();
    QString uniteActuelle = tableWidgetIngredients->item(row, 2)->text();

    bool ok;

    // Modifier le nom
    QString nouveauNom = QInputDialog::getText(this, tr("Modify the ingredient"),
                                               tr("Name of the ingredient:"),
                                               QLineEdit::Normal, nomActuel, &ok);

    if (!ok || nouveauNom.isEmpty()) return;

    // Modifier la quantité
    double nouvelleQuantite = QInputDialog::getDouble(this, tr("Modify the quantity"),
                                                      tr("Quantity:"),
                                                      quantiteActuelle.toDouble(),
                                                      0.0, 9999.99, 2, &ok);

    if (!ok) return;

    // Modifier l'unité
    QString nouvelleUnite = QInputDialog::getText(this, tr("Modify the unity"),
                                                  tr("Unity (French unity such as :g, ml, pièce, etc.):"),
                                                  QLineEdit::Normal, uniteActuelle, &ok);

    if (!ok) return;

    try {
        // Modifier l'ingrédient dans la recette
        recetteActuelle->modifierIngredientParIndex(row,
                                                    nouveauNom.toStdString(),
                                                    nouvelleQuantite,
                                                    nouvelleUnite.toStdString());

        // Mettre à jour l'affichage
        mettreAJourTableIngredients();

        // Sélectionner la ligne modifiée
        tableWidgetIngredients->setCurrentCell(row, 0);

        // Marquer comme modifié
        onDonneesModifiees();

        statusBar()->showMessage(QString(tr("'%1' ingredient modified successfully")).arg(nouveauNom), 2000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Error while modifying : %1")).arg(e.what()));
    }
}

void MainWindow::onParcourirPhoto()
{
    QString fichier = QFileDialog::getOpenFileName(
        this,
        tr("Select a picture"),
        "",
        tr("Pictures (*.png *.jpg *.jpeg *.bmp *.gif)")
        );

    if (!fichier.isEmpty()) {
        lineEditPhoto->setText(fichier);
    }
}

void MainWindow::ouvrirFichier()
{
    if (donneesModifiees && !confirmerAbandonModifications()) {
        return;
    }

    QString fichier = QFileDialog::getOpenFileName(
        this,
        tr("Open a recipe file"),
        "",
        tr("Recipe files (*.xml *.json);;XML files (*.xml);;JSON files (*.json);;All files (*)"
           ));

    if (fichier.isEmpty()) return;

    QFileInfo fileInfo(fichier);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "xml") {
        chargerRecetteXML(fichier);
    } else if (extension == "json") {
        chargerRecetteJSON(fichier);
    } else {
        QMessageBox::warning(this, tr("Error"),
                             tr("Unrecognized file format. Please choose an XML or JSON file."));
    }
}

void MainWindow::chargerRecetteXML(const QString &fichier)
{
    QFile file(fichier);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Cannot open the file : %1")).arg(fichier));
        return;
    }

    QTextStream stream(&file);
    QString contenu = stream.readAll();
    file.close();

    std::string contenuStd = contenu.toStdString();
    std::stringstream ss(contenuStd);

    try {
        auto recette = std::make_unique<Recette>(ss);
        recettes.clear();
        recettes.push_back(std::move(recette));
        mettreAJourListeRecettes();

        statusBar()->showMessage(QString(tr("XML file charged: %1")).arg(fichier), 3000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Parsing error"),
                             QString(tr("Error while loading : %1")).arg(e.what()));
    }
}

void MainWindow::chargerRecetteJSON(const QString &fichier)
{
    QFile file(fichier);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Cannot open the file : %1")).arg(fichier));
        return;
    }

    QTextStream stream(&file);
    QString contenu = stream.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(contenu.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, tr("Error JSON"),
                             QString(tr("Parsing JSON error : %1")).arg(parseError.errorString()));
        return;
    }

    recettes.clear();

    try {
        if (document.isObject()) {
            parseRecetteJSON(document.object());
        } else if (document.isArray()) {
            QJsonArray recettesArray = document.array();
            for (const QJsonValue &value : recettesArray) {
                if (value.isObject()) {
                    parseRecetteJSON(value.toObject());
                }
            }
        }

        mettreAJourListeRecettes();
        statusBar()->showMessage(QString(tr("JSON file loaded: %1")).arg(fichier), 3000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Parsing error"),
                             QString(tr("Error while loading : %1")).arg(e.what()));
    }
}

void MainWindow::parseRecetteJSON(const QJsonObject &recetteObj)
{
    std::string nom = recetteObj["nom"].toString().toStdString();
    std::string photo = recetteObj["photo"].toString().toStdString();
    std::string categorie = recetteObj["categorie"].toString().toStdString();
    std::string createur = recetteObj["createur"].toString().toStdString();

    size_t nbConvives = static_cast<size_t>(recetteObj["nombre_personnes"].toInt());
    double prix = recetteObj["prix"].toDouble();

    Date date = parseDateJSON(recetteObj["date"].toString());

    std::vector<std::string> descriptions;
    if (recetteObj["description"].isString()) {
        descriptions.push_back(recetteObj["description"].toString().toStdString());
    } else if (recetteObj["description"].isArray()) {
        QJsonArray descArray = recetteObj["description"].toArray();
        for (const QJsonValue &descValue : descArray) {
            descriptions.push_back(descValue.toString().toStdString());
        }
    }

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

    auto recette = std::make_unique<Recette>(nom, photo, categorie, descriptions,
                                             nbConvives, prix, createur, date, ingredients);

    recettes.push_back(std::move(recette));
}

Date MainWindow::parseDateJSON(const QString &dateStr)
{
    Date date = {1, 1, 2023};

    if (dateStr.contains("/")) {
        QStringList parts = dateStr.split("/");
        if (parts.size() == 3) {
            date.jour = static_cast<size_t>(parts[0].toInt());
            date.mois = static_cast<size_t>(parts[1].toInt());
            date.annee = static_cast<size_t>(parts[2].toInt());
        }
    } else if (dateStr.contains("-")) {
        QStringList parts = dateStr.split("-");
        if (parts.size() == 3) {
            date.annee = static_cast<size_t>(parts[0].toInt());
            date.mois = static_cast<size_t>(parts[1].toInt());
            date.jour = static_cast<size_t>(parts[2].toInt());
        }
    }

    return date;
}

void MainWindow::redimensionnerColonnes()
{
    if (tableWidgetIngredients->columnCount() >= 3) {
        tableWidgetIngredients->setColumnWidth(0, 200);
        tableWidgetIngredients->setColumnWidth(1, 100);
        // La dernière colonne s'étend automatiquement
    }
}

void MainWindow::appliquerStyle()
{
    // Style moderne pour l'application
    setStyleSheet(
        "QFrame { "
        "    border: 1px solid #d0d0d0; "
        "    border-radius: 5px; "
        "    background-color: #fafafa; "
        "} "
        "QGroupBox { "
        "    font-weight: bold; "
        "    border: 2px solid #cccccc; "
        "    border-radius: 5px; "
        "    margin-top: 1ex; "
        "    padding-top: 10px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 10px; "
        "    padding: 0 5px 0 5px; "
        "} "
        "QPushButton { "
        "    background-color: #e3f2fd; "
        "    border: 1px solid #2196f3; "
        "    border-radius: 4px; "
        "    padding: 5px 10px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #bbdefb; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #90caf9; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #f5f5f5; "
        "    color: #9e9e9e; "
        "    border: 1px solid #e0e0e0; "
        "} "
        "QListWidget::item:selected { "
        "    background-color: #2196f3; "
        "    color: white; "
        "} "
        "QTableWidget { "
        "    gridline-color: #e0e0e0; "
        "    selection-background-color: #e3f2fd; "
        "} "
        );
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // Adapter la taille du panel master selon la taille de la fenêtre
    if (width() < 1000) {
        frameListeRecettes->setMaximumWidth(300);
    } else {
        frameListeRecettes->setMaximumWidth(400);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (donneesModifiees && !confirmerAbandonModifications()) {
        event->ignore();
        return;
    }

    event->accept();
}
