#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Configuration de l'application
    app.setApplicationName("Gestionnaire de Recettes");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("S2.01 - Groupe 19");

    // Créer et afficher la fenêtre principale
    MainWindow window;
    window.show();

    return app.exec();
}
