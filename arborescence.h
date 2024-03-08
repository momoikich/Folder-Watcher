#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QDir>
#include <QPair>
#include <QMap>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class arborescence; }
QT_END_NAMESPACE

class arborescence : public QMainWindow
{
    Q_OBJECT

public:
    arborescence(QWidget *parent = nullptr);
    ~arborescence();


    //fichier log
    QFile file_log;
    // stream text
    QTextStream out;
    //map qui va contenir comme key les liens dossiers et les sous dossiers .... et comme value (les sous dossiers de ce dossier et les fichiers dans
    // ce dossier)
    QMap<QString,QPair<QSet<QString>,QSet<QString>>> *dirs_map;
    // l'objectif de construire dircts à partir de notre path et ajouter tous les keys au path écouté par watch
    void construct_directories(const QString &path, QMap<QString,QPair<QSet<QString>,QSet<QString>>> *dircts,QFileSystemWatcher *watch);
    // supprimer de dircts tous les fichiers et dossiers dans notre path et de manière récurrente on va appliquer la même sur ses sous dossiers
    //    ainsi que remove les écoutes sur ses dossiers de notre watch
    //
    void delete_on_cascade(const QString &path, QMap<QString,QPair<QSet<QString>,QSet<QString>>> *dircts,QFileSystemWatcher *watch);

private slots:
    void on_searchButton_clicked();

    void NotifyChanges(const QString &path);

private:
    Ui::arborescence *ui;
    QFileSystemWatcher *watcher;
};
#endif // ARBORESCENCE_H
