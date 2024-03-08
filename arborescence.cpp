#include "arborescence.h"
#include "ui_arborescence.h"
#include <QFileDialog>

arborescence::arborescence(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::arborescence)
{
    ui->setupUi(this);
}

arborescence::~arborescence()
{
    delete ui;
}


void arborescence::on_searchButton_clicked()
{
    QString directory = QFileDialog::getExistingDirectory(this, "Sélectionner un répertoire", QDir::homePath());
        if (!directory.isEmpty()) {
            // répertoire du build de projet
            QDir::setCurrent(".");
            file_log.setFileName("log.txt");

            watcher = new QFileSystemWatcher(this);
            dirs_map = new QMap<QString,QPair<QSet<QString>,QSet<QString>>>();
            construct_directories(directory,dirs_map,watcher);
            QString message = QString("[%1] : Répertoire \"%2\" sélectionné pour surveillance.\n")
                                  .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"))
                                  .arg(directory);

            out.setDevice(&file_log);
            if(file_log.open(QIODevice::Append | QIODevice::Text)) {


                out << message.toStdString().c_str();
                out << "\nmerci";
            }

            if(file_log.open(QIODevice::Append | QIODevice::Text)) {



                out << message.toStdString().c_str();
                out << "\n";
            }

            ui->log->append(message);

            ui->LinkDirectory->setText(directory);

            disconnect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(NotifyChanges(QString)));


            connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(NotifyChanges(QString)));

            file_log.close();
        }
        return;
}

void arborescence::NotifyChanges(const QString &path){
        QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
        QPair<QSet<QString>,QSet<QString>> pair_dir_files = dirs_map->value(path);
        QDir dir(path);
        //nouveaux ficihiers à l'état actuel de notre path
        QStringList fileList = dir.entryList(QDir::Files);
        //nouveaux dossiers à l'état actuel de notre path
        QStringList dirList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        QSet<QString> dirnewSet(dirList.begin(),dirList.end());
        QSet<QString> filenewSet(fileList.begin(),fileList.end());
        //update de la value de notre dirs_map
        QPair<QSet<QString>,QSet<QString>> pair_update(dirnewSet,filenewSet);
        //anciens fichiers dans notre path
        QSet<QString> fileSet = pair_dir_files.second;
        //anciens dossiers dans notre path
        QSet<QString> dirSet = pair_dir_files.first;
        // opérations de différence sur les ensembles
        QSet<QString> diffdir = dirnewSet - dirSet;
        QSet<QString> diffile = filenewSet - fileSet;
        QSet<QString> anc_diffile = fileSet - filenewSet;
        QSet<QString> anc_difdir = dirSet - dirnewSet;

        QString path_str;
        QString new_path_str;


        if (fileSet.size() == filenewSet.size()) {
            // si la l'ensemble des nouveaux fichiers dans notre path et la l'ensemble des anciens fichiers dans notre path ont la même taille
            // donc il y'a une possibilté qu'un fichier path notre path (peut être qu'on a touché un dossier)

            for (const QString &str : anc_diffile) {
                QString message = QString("[%1] : Le fichier : %2/%3 est renommé\n").arg(time).arg(path).arg(str);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    out << message.toStdString().c_str();

                    out << "\n";
                }

                ui->log->append(message);

            }


        }
        else if (fileSet.size() > filenewSet.size()) {
            // si la taille de l'ensemble des nouveaux fichiers dans notre path est inférieur (strictement) à la l'ensemble des anciens fichiers
            // dans notre path donc fichier un fichier est supprimé dans notre path

            for (const QString &str : anc_diffile) {
                QString message = QString("[%1] : Le fichier : %2/%3 est supprimé\n").arg(time).arg(path).arg(str);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    out << message.toStdString().c_str();

                    out << "\n";
                }

                ui->log->append(message);
            }


        }

        else {
            // si la taille de l'ensemble des nouveaux fichiers est supérieur (strictement) à la l'ensemble des anciens fichiers
            // donc fichier un fichier est ajouté dans notre path
            for (const QString &str : diffile) {
                QString message = QString("[%1] : Le fichier : %2/%3 est ajouté\n").arg(time).arg(path).arg(str);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    out << message.toStdString().c_str();

                    out << "\n";
                }

                ui->log->append(message);
            }

        }

        if (dirSet.size() == dirnewSet.size()) {
            // si la l'ensemble des nouveaux dossiers dans notre path et la l'ensemble des anciens dossiers dans notre path ont la même taille
            // donc il y'a une possibilté qu'un fichier path notre path (peut être qu'on a touché un dossier)
            for (const QString &str : anc_difdir) {
                path_str = QString("%1/%2").arg(path).arg(str);
                QString message = QString("[%1] : Le dossier : %2 est renommé\n").arg(time).arg(path_str);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    out << message.toStdString().c_str();

                    out << "\n";
                }

                delete_on_cascade(path_str, dirs_map,watcher);
                ui->log->append(message);
            }

            for (const QString &str : diffdir) {
                new_path_str = QString("%1/%2").arg(path).arg(str);

                QMap<QString,QPair<QSet<QString>,QSet<QString>>> *new_map = new QMap<QString,QPair<QSet<QString>,QSet<QString>>>();
                construct_directories(new_path_str,new_map,watcher);
                // Ajout de new_map à dirs_map en utilisant la méthode insert
                for (auto it = new_map->constBegin(); it != new_map->constEnd(); ++it) {
                    dirs_map->insert(it.key(), it.value());
                }

            }




        }
        else if (dirSet.size() > dirnewSet.size()) {
            // si la taille de l'ensemble des nouveaux fichiers dans notre path est inférieur (strictement) à la l'ensemble des anciens fichiers
            // dans notre path donc fichier un fichier est supprimé dans notre path
            for (const QString &str : anc_difdir) {
                path_str = QString("%1/%2").arg(path).arg(str);
                QString message = QString("[%1] : Le dossier : %2 est supprimé\n").arg(time).arg(path_str);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    out << message.toStdString().c_str();

                    out << "\n";
                }

                delete_on_cascade(path_str,dirs_map,watcher);
                ui->log->append(message);
            }


        }

        else {
            // si la taille de l'ensemble des nouveaux fichiers dans notre path est supérieur (strictement) à la l'ensemble des anciens fichiers
            // dans notre path donc fichier un fichier est ajouté dans notre path
            for (const QString &str : diffdir) {
                QString new_dossier = QString("%1/%2").arg(path).arg(str);
                watcher->addPath(new_dossier);
                QString message = QString("[%1] : Le dossier : %2 est ajouté\n").arg(time).arg(new_dossier);
                if(file_log.open(QIODevice::Append | QIODevice::Text)) {
                    file_log.write(message.toStdString().c_str());
                    QTextStream out(&file_log);
                    out << "\n";
                }

                QSet<QString> dir_dossier = QSet<QString>();
                QSet<QString> file_dossier = QSet<QString>();
                QPair<QSet<QString>,QSet<QString>> pair_added(dir_dossier,file_dossier);
                dirs_map->insert(new_dossier,pair_added);
                ui->log->append(message);
            }

        }

        (*dirs_map)[path] = pair_update;
        file_log.close();
        return;


}

void arborescence::construct_directories(const QString &path, QMap<QString,QPair<QSet<QString>,QSet<QString>>> *dirs_map, QFileSystemWatcher *watch){
        watch->addPath(path);
        QDir directory(path);
        QStringList list_dirs = directory.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        QStringList list_files = directory.entryList(QDir::Files);
        QSet<QString> set_dirs(list_dirs.begin(),list_dirs.end());
        QSet<QString> set_files(list_files.begin(),list_files.end());
        QPair<QSet<QString>,QSet<QString>> pair(set_dirs,set_files);
        // Initialisation de dirs_map
        dirs_map->insert(path,pair);
        for(const QString subdirect : list_dirs) {
            construct_directories(QString("%1/%2").arg(path).arg(subdirect),dirs_map,watch);
        }
        return;
}

void arborescence::delete_on_cascade(const QString &path,QMap<QString,QPair<QSet<QString>,QSet<QString>>> *dircts,QFileSystemWatcher *watch) {
        watch->removePath(path);
        QPair<QSet<QString>,QSet<QString>> pair_delete = dircts->value(path);
        dircts->remove(path);
        QSet<QString> dires = pair_delete.first;
        for (const QString subdir : dires)  {
            delete_on_cascade(QString("%1/%2").arg(path).arg(subdir),dircts,watch);
        }
        return;


}

