#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Configuration de l'application
    app.setApplicationName("Gestionnaire de Recettes");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("S2.01 - Groupe 19");

    QTranslator translator;

    /*/if (translator.load(":/translations/S2_01_en_FR.qm")) {
    //    app.installTranslator(&translator);
    //    qDebug() << "Traduction française chargée avec succès";
    } else {
        qDebug() << "Impossible de charger la traduction française";
        qDebug() << "Vérifiez que le fichier S2_01_en_FR.qm existe dans le répertoire";
    }*/

    MainWindow window;
    window.show();

    return app.exec();
}
