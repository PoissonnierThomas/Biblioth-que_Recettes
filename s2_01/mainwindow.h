#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "recette.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ouvrirFichier();
    void chargerRecettesParDefaut();

private:
    Ui::MainWindow *ui;

    // Méthodes de chargement
    void chargerRecetteXML(const QString &cheminFichier);
    void chargerRecetteJSON(const QString &cheminFichier);
    void chargerRecetteDepuisContenu(const QString &contenu, const QString &format);

    // Méthodes utilitaires JSON
    void parseRecetteJSON(const QJsonObject &recetteObj);
    Date parseDateJSON(const QString &dateStr);
};

#endif // MAINWINDOW_H
