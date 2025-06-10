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

    chargerRecettesParDefaut();

    statusBar()->showMessage(tr("Application started - Lecture mode"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupInterface()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

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

    // Recherche
    lineEditRecherche = new QLineEdit();
    lineEditRecherche->setPlaceholderText(tr("Search for a recipe..."));
    layoutMaster->addWidget(lineEditRecherche);

    // Liste
    listWidgetRecettes = new QListWidget();
    listWidgetRecettes->setAlternatingRowColors(true);
    listWidgetRecettes->setSelectionMode(QAbstractItemView::SingleSelection);
    layoutMaster->addWidget(listWidgetRecettes);

    // Boutons
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
    frameDetailRecette = new QFrame();
    frameDetailRecette->setFrameStyle(QFrame::StyledPanel);

    QVBoxLayout *layoutDetail = new QVBoxLayout(frameDetailRecette);
    layoutDetail->setSpacing(10);

    // En-tête
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

    // Contenu scrollable
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollContent = new QWidget();
    QVBoxLayout *layoutScroll = new QVBoxLayout(scrollContent);

    // Informations générales
    groupBoxInfos = new QGroupBox(tr("General informations"));
    QFormLayout *formInfos = new QFormLayout(groupBoxInfos);

    lineEditNom = new QLineEdit();
    lineEditNom->setEnabled(false);
    formInfos->addRow(tr("Name:"), lineEditNom);

    comboBoxCategorie = new QComboBox();
    comboBoxCategorie->addItems({tr("Starter"), tr("Main course"), tr("Dessert"), tr("Sweet"), tr("Soup")});
    comboBoxCategorie->setEnabled(false);
    formInfos->addRow(tr("Category:"), comboBoxCategorie);

    spinBoxNbPersonnes = new QSpinBox();
    spinBoxNbPersonnes->setRange(1, 20);
    spinBoxNbPersonnes->setEnabled(false);
    formInfos->addRow(tr("Nb. of people:"), spinBoxNbPersonnes);

    doubleSpinBoxPrix = new QDoubleSpinBox();
    doubleSpinBoxPrix->setRange(0.0, 999.99);
    doubleSpinBoxPrix->setDecimals(2);
    doubleSpinBoxPrix->setSuffix(" €");
    doubleSpinBoxPrix->setEnabled(false);
    formInfos->addRow(tr("Price/pers.:"), doubleSpinBoxPrix);

    lineEditCreateur = new QLineEdit();
    lineEditCreateur->setEnabled(false);
    formInfos->addRow(tr("Creator:"), lineEditCreateur);

    dateEditDate = new QDateEdit();
    dateEditDate->setCalendarPopup(true);
    dateEditDate->setEnabled(false);
    formInfos->addRow(tr("Date:"), dateEditDate);

    QHBoxLayout *layoutPhoto = new QHBoxLayout();
    lineEditPhoto = new QLineEdit();
    lineEditPhoto->setPlaceholderText(tr("Path to the picture"));
    lineEditPhoto->setEnabled(false);
    btnParcourirPhoto = new QPushButton(tr("Browse"));
    btnParcourirPhoto->setEnabled(false);
    layoutPhoto->addWidget(lineEditPhoto);
    layoutPhoto->addWidget(btnParcourirPhoto);
    formInfos->addRow(tr("Picture:"), layoutPhoto);

    layoutScroll->addWidget(groupBoxInfos);

    // Description
    groupBoxDescription = new QGroupBox(tr("Description"));
    QVBoxLayout *layoutDesc = new QVBoxLayout(groupBoxDescription);

    textEditDescription = new QTextEdit();
    textEditDescription->setMaximumHeight(120);
    textEditDescription->setEnabled(false);
    layoutDesc->addWidget(textEditDescription);

    layoutScroll->addWidget(groupBoxDescription);

    // Ingrédients avec image
    groupBoxIngredients = new QGroupBox(tr("Ingredients"));
    QHBoxLayout *layoutIngredientsPrincipal = new QHBoxLayout(groupBoxIngredients);

    // Partie gauche : tableau et boutons des ingrédients
    QWidget *widgetIngredientsGauche = new QWidget();
    QVBoxLayout *layoutIng = new QVBoxLayout(widgetIngredientsGauche);

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
    QStringList headers{tr("Name"), tr("Quantity"), tr("Unit")};
    tableWidgetIngredients->setHorizontalHeaderLabels(headers);
    tableWidgetIngredients->setAlternatingRowColors(true);
    tableWidgetIngredients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidgetIngredients->horizontalHeader()->setStretchLastSection(true);
    tableWidgetIngredients->setEnabled(false);
    tableWidgetIngredients->setToolTip(tr("Double-click on an ingredient to edit it"));

    layoutIng->addWidget(tableWidgetIngredients);

    // Partie droite : image de la recette
    QWidget *widgetImageDroite = new QWidget();
    QVBoxLayout *layoutImageDroite = new QVBoxLayout(widgetImageDroite);

    QLabel *labelTitreImage = new QLabel(tr("Recipe picture"));
    QFont fontTitreImage = labelTitreImage->font();
    fontTitreImage.setBold(true);
    labelTitreImage->setFont(fontTitreImage);
    labelTitreImage->setAlignment(Qt::AlignCenter);

    labelImageRecette = new QLabel();
    labelImageRecette->setAlignment(Qt::AlignCenter);
    labelImageRecette->setMinimumSize(200, 200);
    labelImageRecette->setMaximumSize(300, 300);
    labelImageRecette->setScaledContents(true);
    labelImageRecette->setStyleSheet(
        "QLabel { "
        "    border: 2px solid #cccccc; "
        "    border-radius: 8px; "
        "    background-color: #f8f8f8; "
        "    color: #888888; "
        "}"
        );
    labelImageRecette->setText(tr("No image available"));

    layoutImageDroite->addWidget(labelTitreImage);
    layoutImageDroite->addWidget(labelImageRecette);
    layoutImageDroite->addStretch();

    // Assemblage horizontal : tableau à gauche, image à droite
    layoutIngredientsPrincipal->addWidget(widgetIngredientsGauche, 2); // 2/3 de l'espace
    layoutIngredientsPrincipal->addWidget(widgetImageDroite, 1);       // 1/3 de l'espace

    layoutScroll->addWidget(groupBoxIngredients);

    scrollArea->setWidget(scrollContent);
    layoutDetail->addWidget(scrollArea);
}
void MainWindow::setupMenus()
{
    QMenu *menuFichier = menuBar()->addMenu(tr("File"));

    actionOuvrir = new QAction(tr("Open..."), this);
    actionOuvrir->setShortcut(QKeySequence::Open);
    menuFichier->addAction(actionOuvrir);

    actionSauvegarder = new QAction(tr("Save"), this);
    actionSauvegarder->setShortcut(QKeySequence::Save);
    menuFichier->addAction(actionSauvegarder);
    connect(actionSauvegarder, &QAction::triggered,
            this, &MainWindow::sauvegarderFichier);
    menuFichier->addSeparator();

    actionQuitter = new QAction(tr("Quit"), this);
    actionQuitter->setShortcut(QKeySequence::Quit);
    menuFichier->addAction(actionQuitter);

    QMenu *menuEdition = menuBar()->addMenu(tr("Edit"));

    actionModeEdition = new QAction(tr("Edition mode"), this);
    actionModeEdition->setCheckable(true);
    actionModeEdition->setShortcut(QKeySequence("Ctrl+E"));
    menuEdition->addAction(actionModeEdition);
}

void MainWindow::connecterSignaux()
{
    connect(listWidgetRecettes, &QListWidget::currentRowChanged,
            this, &MainWindow::onRecetteSelectionnee);

    connect(btnNouvelleRecette, &QPushButton::clicked,
            this, &MainWindow::onNouvelleRecette);
    connect(btnSupprimerRecette, &QPushButton::clicked,
            this, &MainWindow::onSupprimerRecette);
    connect(btnSauvegarder, &QPushButton::clicked,
            this, &MainWindow::onSauvegarder);
    connect(btnAnnuler, &QPushButton::clicked,
            this, &MainWindow::onAnnuler);

    connect(lineEditRecherche, &QLineEdit::textChanged,
            this, &MainWindow::onRechercheRecette);

    connect(btnAjouterIngredient, &QPushButton::clicked,
            this, &MainWindow::onAjouterIngredient);
    connect(btnSupprimerIngredient, &QPushButton::clicked,
            this, &MainWindow::onSupprimerIngredient);
    connect(tableWidgetIngredients, &QTableWidget::currentItemChanged,
            this, &MainWindow::onIngredientSelectionne);
    connect(tableWidgetIngredients, &QTableWidget::itemDoubleClicked,
            this, &MainWindow::onModifierIngredient);

    connect(actionModeEdition, &QAction::toggled,
            this, &MainWindow::onModeEditionToggled);
    connect(actionOuvrir, &QAction::triggered,
            this, &MainWindow::ouvrirFichier);
    connect(actionQuitter, &QAction::triggered,
            this, &QWidget::close);

    connect(btnParcourirPhoto, &QPushButton::clicked,
            this, &MainWindow::onParcourirPhoto);
}

void MainWindow::chargerRecettesParDefaut()
{
    QFile fichier(":/Recettes/recettes.xml");

    if (fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << tr("Default XML loading...");
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

QString MainWindow::categorieVersInterface(const std::string& categorie)
{
    if (categorie == "Entrée") return tr("Starter");
    if (categorie == "Plat") return tr("Main course");
    if (categorie == "Dessert") return tr("Dessert");
    if (categorie == "Entremet") return tr("Sweet");
    if (categorie == "Soupe") return tr("Soup");
    return QString::fromStdString(categorie);
}

void MainWindow::mettreAJourListeRecettes()
{
    listWidgetRecettes->clear();

    for (const auto& recette : recettes) {

        // Formatage du prix
        double prix = recette->getPrix();
        QString prixFormate;

        if (prix == 0.0) {
            prixFormate = "0,00 €";
        } else {
            prixFormate = QString::number(prix, 'f', 2).replace('.', ',') + " €";
        }

        // Formatage de la catégorie
        QString categorie;
        std::string categorieOriginale = recette->getCategorie();

        if (categorieOriginale == "Entrée") {
            categorie = tr("Starter");
        } else if (categorieOriginale == "Plat") {
            categorie = tr("Main course");
        } else if (categorieOriginale == "Dessert") {
            categorie = tr("Dessert");
        } else if (categorieOriginale == "Entremet") {
            categorie = tr("Sweet");
        } else if (categorieOriginale == "Soupe") {
            categorie = tr("Soup");
        } else {
            categorie = QString::fromStdString(categorieOriginale);
        }

        QString texteRecette = QString("%1 - %2 - %3 %4 - %5")
                                   .arg(QString::fromStdString(recette->getNom()))
                                   .arg(categorie)
                                   .arg(recette->getConvives())
                                   .arg(tr("people"))
                                   .arg(prixFormate);

        QListWidgetItem* item = new QListWidgetItem(texteRecette);
        item->setData(Qt::UserRole, QVariant::fromValue(recette.get()));
        listWidgetRecettes->addItem(item);
    }

    if (recettes.empty()) {
        QListWidgetItem* item = new QListWidgetItem(tr("No recipe available"));
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<Recette*>(nullptr)));
        item->setForeground(QColor::fromRgb(128, 128, 128));
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        listWidgetRecettes->addItem(item);
    }

    qDebug() << "Liste mise à jour avec" << recettes.size() << "recettes";
}

QString MainWindow::formatRecetteListe(const Recette* recette) const
{
    if (!recette) return "";

    return QString("%1 (%2)\n%3 %4 - %.2f€")
        .arg(QString::fromStdString(recette->getNom()))
        .arg(QString::fromStdString(recette->getCategorie()))
        .arg(recette->getConvives())
        .arg(tr("people"))
        .arg(recette->getPrix());
}

void MainWindow::onRecetteSelectionnee()
{
    int index = listWidgetRecettes->currentRow();

    if (index >= 0 && index < static_cast<int>(recettes.size())) {
        // Vérifier les modifications non sauvegardées
        if (donneesModifiees && !confirmerAbandonModifications()) {
            // Remettre la sélection précédente
            for (size_t i = 0; i < recettes.size(); ++i) {
                if (recettes[i].get() == recetteActuelle) {
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

    deconnecterSignauxModification();

    lineEditNom->setText(QString::fromStdString(recette->getNom()));

    // Traduction des catégories
    QString categorieActuelle = QString::fromStdString(recette->getCategorie());
    if (categorieActuelle == "Entrée") comboBoxCategorie->setCurrentText(tr("Starter"));
    else if (categorieActuelle == "Plat") comboBoxCategorie->setCurrentText(tr("Main course"));
    else if (categorieActuelle == "Dessert") comboBoxCategorie->setCurrentText(tr("Dessert"));
    else if (categorieActuelle == "Entremet") comboBoxCategorie->setCurrentText(tr("Sweet"));
    else if (categorieActuelle == "Soupe") comboBoxCategorie->setCurrentText(tr("Soup"));
    else comboBoxCategorie->setCurrentText(categorieActuelle);

    spinBoxNbPersonnes->setValue(static_cast<int>(recette->getConvives()));
    doubleSpinBoxPrix->setValue(recette->getPrix());
    lineEditCreateur->setText(QString::fromStdString(recette->getCreateur()));
    lineEditPhoto->setText(QString::fromStdString(recette->getPhoto()));

    Date dateRecette = recette->getDate();
    QDate qdate(static_cast<int>(dateRecette.annee),
                static_cast<int>(dateRecette.mois),
                static_cast<int>(dateRecette.jour));
    dateEditDate->setDate(qdate);

    const auto& descriptions = recette->getDescription();
    QString descriptionComplete;
    for (const auto& desc : descriptions) {
        if (!descriptionComplete.isEmpty()) {
            descriptionComplete += "\n";
        }
        descriptionComplete += QString::fromStdString(desc);
    }
    textEditDescription->setPlainText(descriptionComplete);

    mettreAJourTableIngredients();

    labelTitreDetail->setText(QString(tr("Details: %1"))
                                  .arg(QString::fromStdString(recette->getNom())));

    sauvegarderDonneesOriginales();

    donneesModifiees = false;
    btnSauvegarder->setEnabled(false);
    btnAnnuler->setEnabled(false);

    if (modeEdition) {
        connecterSignauxModification();
    }

    chargerImageRecette(QString::fromStdString(recette->getPhoto()));

    qDebug() << tr("Recipe displayed, Modified Data =") << donneesModifiees << ", modeEdition =" << modeEdition;
}

void MainWindow::mettreAJourTableIngredients()
{
    if (!recetteActuelle) return;

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

    tableWidgetIngredients->blockSignals(false);
}

void MainWindow::viderFormulaire()
{
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
    labelImageRecette->clear();
    labelImageRecette->setText(tr("No image available"));

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
    lineEditNom->setEnabled(activer);
    comboBoxCategorie->setEnabled(activer);
    spinBoxNbPersonnes->setEnabled(activer);
    doubleSpinBoxPrix->setEnabled(activer);
    lineEditCreateur->setEnabled(activer);
    lineEditPhoto->setEnabled(activer);
    dateEditDate->setEnabled(activer);
    textEditDescription->setEnabled(activer);
    tableWidgetIngredients->setEnabled(activer);

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
    if (!modeEdition) return;

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

    recetteActuelle->setNom(lineEditNom->text().toStdString());

    // Conversion des catégories traduites
    QString categorieSelectionnee = comboBoxCategorie->currentText();
    if (categorieSelectionnee == tr("Starter")) recetteActuelle->setCategorie("Entrée");
    else if (categorieSelectionnee == tr("Main course")) recetteActuelle->setCategorie("Plat");
    else if (categorieSelectionnee == tr("Dessert")) recetteActuelle->setCategorie("Dessert");
    else if (categorieSelectionnee == tr("Sweet")) recetteActuelle->setCategorie("Entremet");
    else if (categorieSelectionnee == tr("Soup")) recetteActuelle->setCategorie("Soupe");
    else recetteActuelle->setCategorie(categorieSelectionnee.toStdString());

    recetteActuelle->setConvives(static_cast<size_t>(spinBoxNbPersonnes->value()));
    recetteActuelle->setPrix(doubleSpinBoxPrix->value());
    recetteActuelle->setCreateur(lineEditCreateur->text().toStdString());
    recetteActuelle->setPhoto(lineEditPhoto->text().toStdString());

    QDate qdate = dateEditDate->date();
    Date nouvelleDate = {static_cast<size_t>(qdate.day()),
                         static_cast<size_t>(qdate.month()),
                         static_cast<size_t>(qdate.year())};
    recetteActuelle->setDate(nouvelleDate);

    QString description = textEditDescription->toPlainText();
    recetteActuelle->setDescription(description.toStdString());

    qDebug() << tr("Recipe saved:") << *recetteActuelle;
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

    donneesOriginales.ingredients.clear();
    for (const auto* ing : recetteActuelle->getIngredients()) {
        donneesOriginales.ingredients.push_back(const_cast<Ingredient*>(ing));
    }
}

void MainWindow::restaurerDonneesOriginales()
{
    if (!recetteActuelle) return;

    recetteActuelle->setNom(donneesOriginales.nom);
    recetteActuelle->setCategorie(donneesOriginales.categorie);
    recetteActuelle->setPhoto(donneesOriginales.photo);
    recetteActuelle->setCreateur(donneesOriginales.createur);
    recetteActuelle->setDescription(donneesOriginales.description);
    recetteActuelle->setConvives(donneesOriginales.nbConvives);
    recetteActuelle->setPrix(donneesOriginales.prix);
    recetteActuelle->setDate(donneesOriginales.date);

    afficherRecette(recetteActuelle);
}

bool MainWindow::confirmerAbandonModifications()
{
    if (!donneesModifiees) return true;

    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Modifications not saved"),
        tr("Modifications haven't been saved.\n"
           "Would you like to cancel them?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    return reponse == QMessageBox::Yes;
}

void MainWindow::onNouvelleRecette()
{
    if (donneesModifiees && !confirmerAbandonModifications()) {
        return;
    }

    Date dateActuelle = {static_cast<size_t>(QDate::currentDate().day()),
                         static_cast<size_t>(QDate::currentDate().month()),
                         static_cast<size_t>(QDate::currentDate().year())};

    auto nouvelleRecette = std::make_unique<Recette>(
        tr("New Recipe").toStdString(),
        "",
        "Plat",
        std::vector<std::string>{tr("Description of the new recipe").toStdString()},
        4,
        0.0,
        tr("User").toStdString(),
        dateActuelle,
        std::vector<Ingredient*>()
        );

    recettes.push_back(std::move(nouvelleRecette));
    mettreAJourListeRecettes();

    listWidgetRecettes->setCurrentRow(static_cast<int>(recettes.size()) - 1);

    actionModeEdition->setChecked(true); // Activer automatiquement le mode édition

    statusBar()->showMessage(tr("New recipe created"), 2000);
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

    QString unite = QInputDialog::getText(this, tr("Unit"),
                                          tr("Unit (French unit such as: g, ml, piece, etc.):"),
                                          QLineEdit::Normal, "g", &ok);

    if (!ok) return;

    try {
        Ingredient* nouvelIngredient = new Ingredient(nom.toStdString(),
                                                      quantite,
                                                      unite.toStdString());

        recetteActuelle->ajouterIngredient(nouvelIngredient);
        mettreAJourTableIngredients();
        onDonneesModifiees();

        statusBar()->showMessage(tr("Ingredient added successfully"), 2000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Error while adding the ingredient: %1")).arg(e.what()));
    }
}

void MainWindow::onSupprimerIngredient()
{
    int row = tableWidgetIngredients->currentRow();
    if (row < 0 || !modeEdition || !recetteActuelle) return;

    if (row >= static_cast<int>(recetteActuelle->getNombreIngredients())) return;

    QString nomIngredient = tableWidgetIngredients->item(row, 0)->text();

    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Delete the ingredient"),
        QString(tr("Are you sure you want to delete the '%1' ingredient?")).arg(nomIngredient),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reponse == QMessageBox::Yes) {
        try {
            recetteActuelle->retirerIngredientParIndex(row);
            mettreAJourTableIngredients();
            onDonneesModifiees();

            statusBar()->showMessage(QString(tr("'%1' ingredient deleted successfully")).arg(nomIngredient), 2000);

        } catch (const std::exception& e) {
            QMessageBox::warning(this, tr("Error"),
                                 QString(tr("Error while deleting: %1")).arg(e.what()));
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

    QString nomActuel = tableWidgetIngredients->item(row, 0)->text();
    QString quantiteActuelle = tableWidgetIngredients->item(row, 1)->text();
    QString uniteActuelle = tableWidgetIngredients->item(row, 2)->text();

    bool ok;

    QString nouveauNom = QInputDialog::getText(this, tr("Modify the ingredient"),
                                               tr("Name of the ingredient:"),
                                               QLineEdit::Normal, nomActuel, &ok);

    if (!ok || nouveauNom.isEmpty()) return;

    double nouvelleQuantite = QInputDialog::getDouble(this, tr("Modify the quantity"),
                                                      tr("Quantity:"),
                                                      quantiteActuelle.toDouble(),
                                                      0.0, 9999.99, 2, &ok);

    if (!ok) return;

    QString nouvelleUnite = QInputDialog::getText(this, tr("Modify the unit"),
                                                  tr("Unit (French unit such as: g, ml, piece, etc.):"),
                                                  QLineEdit::Normal, uniteActuelle, &ok);

    if (!ok) return;

    try {
        recetteActuelle->modifierIngredientParIndex(row,
                                                    nouveauNom.toStdString(),
                                                    nouvelleQuantite,
                                                    nouvelleUnite.toStdString());

        mettreAJourTableIngredients();
        tableWidgetIngredients->setCurrentCell(row, 0);
        onDonneesModifiees();

        statusBar()->showMessage(QString(tr("'%1' ingredient modified successfully")).arg(nouveauNom), 2000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Error while modifying: %1")).arg(e.what()));
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
        tr("Recipe files (*.xml *.json);;XML files (*.xml);;JSON files (*.json);;All files (*)")
        );

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
                             QString(tr("Cannot open the file: %1")).arg(fichier));
        return;
    }

    QTextStream stream(&file);
    QString contenu = stream.readAll();
    file.close();

    std::string contenuStd = contenu.toStdString();
    std::stringstream ss(contenuStd);

    try {
        recettes = Recette::chargerDepuisFlux(ss);
        mettreAJourListeRecettes();

        statusBar()->showMessage(QString(tr("XML file loaded: %1")).arg(fichier), 3000);

    } catch (const std::exception& e) {
        QMessageBox::warning(this, tr("Parsing error"),
                             QString(tr("Error while loading: %1")).arg(e.what()));
    }
}

void MainWindow::chargerRecetteJSON(const QString &fichier)
{
    QFile file(fichier);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             QString(tr("Cannot open the file: %1")).arg(fichier));
        return;
    }

    QTextStream stream(&file);
    QString contenu = stream.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(contenu.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, tr("JSON error"),
                             QString(tr("JSON parsing error: %1")).arg(parseError.errorString()));
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
                             QString(tr("Error while loading: %1")).arg(e.what()));
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


QString MainWindow::formaterPrix(double prix)
{
    if (prix == 0.0) return "0,00 €";
    if (prix < 0.0) return tr("Invalid price");

    return QString::number(prix, 'f', 2).replace('.', ',') + " €";
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

void MainWindow::sauvegarderFichier()
{
    if (recettes.empty()) {
        QMessageBox::warning(this, tr("Save"), tr("No recipe to save."));
        return;
    }

    QString fichier = QFileDialog::getSaveFileName(
        this,
        tr("Save recipe file"),
        "",
        tr("JSON files (*.json);;XML files (*.xml);;All files (*)")
        );

    if (fichier.isEmpty()) return;

    QFileInfo fileInfo(fichier);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "json") {
        sauvegarderRecettesJSON(fichier);
    } else if (extension == "xml") {
        sauvegarderRecettesXML(fichier);
    } else {
        QMessageBox::warning(this, tr("Error"),
                             tr("Please choose a valid XML or JSON file extension."));
    }
}

void MainWindow::sauvegarderRecettesJSON(const QString& fichier)
{
    QJsonArray array;

    for (const auto& recette : recettes) {
        QJsonObject obj;
        obj["nom"] = QString::fromStdString(recette->getNom());
        obj["photo"] = QString::fromStdString(recette->getPhoto());
        obj["categorie"] = QString::fromStdString(recette->getCategorie());
        obj["createur"] = QString::fromStdString(recette->getCreateur());
        obj["nombre_personnes"] = static_cast<int>(recette->getConvives());
        obj["prix"] = recette->getPrix();

        const Date& d = recette->getDate();
        obj["date"] = QString("%1/%2/%3").arg(d.jour).arg(d.mois).arg(d.annee);

        QJsonArray descArray;
        for (const auto& s : recette->getDescription()) {
            descArray.append(QString::fromStdString(s));
        }
        obj["description"] = descArray;

        QJsonArray ingArray;
        for (const auto* ing : recette->getIngredients()) {
            QJsonObject ingObj;
            ingObj["nom"] = QString::fromStdString(ing->getNom());
            ingObj["quantite"] = ing->getQuantite();
            ingObj["unite"] = QString::fromStdString(ing->getUnite());
            ingArray.append(ingObj);
        }
        obj["ingredients"] = ingArray;

        array.append(obj);
    }

    QJsonDocument doc(array);

    QFile file(fichier);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Save error"), tr("Cannot write to file."));
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    statusBar()->showMessage(tr("Recipes saved to JSON."), 3000);
}

void MainWindow::sauvegarderRecettesXML(const QString& fichier)
{
    QFile file(fichier);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Save error"), tr("Cannot write to file."));
        return;
    }

    QTextStream out(&file);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    for (const auto& recette : recettes) {
        out << "<recette>\n";
        out << "  <nom>" << QString::fromStdString(recette->getNom()) << "</nom>\n";
        out << "  <photo>" << QString::fromStdString(recette->getPhoto()) << "</photo>\n";
        out << "  <catégorie>" << QString::fromStdString(recette->getCategorie()) << "</catégorie>\n";
        out << "  <createur>" << QString::fromStdString(recette->getCreateur()) << "</createur>\n";
        out << "  <nombre_personnes>" << recette->getConvives() << "</nombre_personnes>\n";
        out << "  <prix>" << recette->getPrix() << "</prix>\n";

        const Date& d = recette->getDate();
        out << "  <date>" << d.jour << "/" << d.mois << "/" << d.annee << "</date>\n";

        for (const auto& desc : recette->getDescription()) {
            out << "  <description>" << QString::fromStdString(desc) << "</description>\n";
        }

        for (const auto* ing : recette->getIngredients()) {
            out << "  <ingredient>\n";
            out << "    <nom>" << QString::fromStdString(ing->getNom()) << "</nom>\n";
            out << "    <quantite>" << ing->getQuantite() << "</quantite>\n";
            out << "    <unite>" << QString::fromStdString(ing->getUnite()) << "</unite>\n";
            out << "  </ingredient>\n";
        }

        out << "</recette>\n\n";
    }

    file.close();
    statusBar()->showMessage(tr("Recipes saved to XML."), 3000);
}


void MainWindow::chargerImageRecette(const QString& nomImage)
{
    if (nomImage.isEmpty()) {
        labelImageRecette->clear();
        labelImageRecette->setText(tr("No image available"));
        return;
    }

    // Construire le chemin vers l'image dans les ressources
    QString cheminImage = QString(":/Recettes/images/%1").arg(nomImage);

    QPixmap pixmap(cheminImage);

    if (pixmap.isNull()) {
        // Si l'image n'est pas trouvée dans les ressources, essayer un chemin relatif
        QString cheminRelatif = QString("Recettes/images/%1").arg(nomImage);
        pixmap.load(cheminRelatif);
    }

    if (!pixmap.isNull()) {
        // Redimensionner l'image en conservant les proportions
        QPixmap pixmapRedimensionne = pixmap.scaled(
            labelImageRecette->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        labelImageRecette->setPixmap(pixmapRedimensionne);

        qDebug() << tr("Image loaded:") << nomImage;
    } else {
        labelImageRecette->clear();
        labelImageRecette->setText(tr("Image not found: %1").arg(nomImage));
        qDebug() << tr("Image not found:") << cheminImage;
    }
}

