#ifndef ANIMEHUB_H
#define ANIMEHUB_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "AnimePreviewUI.h"

class Anime;

QT_BEGIN_NAMESPACE
namespace Ui { class AnimeHub; }
QT_END_NAMESPACE

class AnimeHub : public QMainWindow
{
    Q_OBJECT

public:
    AnimeHub(QWidget *parent = nullptr);
    ~AnimeHub();

    static QNetworkAccessManager& GetNetworkManager();

    void Load();
    void Save();

    void SelectList(const QString& listName);

    const QVector<Anime *>& GetAnimes() const;
    void AddAnimeToList(const QString& listName, Anime* anime);

    void RefreshAnimeListUI();
    void RefreshListsUI();

private slots:
    void on_addAnimeButton_clicked();
    void on_pushButton_clicked();

    void on_newListButton_clicked();

    void on_listsComboBox_currentTextChanged(const QString &arg1);

public slots:


private:
    Ui::AnimeHub *ui;
    static QNetworkAccessManager *manager;

    QVector<Anime *> animes; // deprecated

    QMap<QString, QVector<Anime *>*> lists;

    QVector<AnimePreviewUI *> animeListPreviewListUIs;

    QVector<AnimePreviewUI *> animePreviewSearchUIs;

    QString selectedList;
};
#endif // ANIMEHUB_H
