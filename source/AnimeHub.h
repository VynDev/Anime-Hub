#ifndef ANIMEHUB_H
#define ANIMEHUB_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include "AnimePreviewUI.h"
#include "Settings.h"

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

    void CreateList(const QString& listName);
    void SelectList(const QString& listName);

    const QVector<Anime *>& GetAnimes() const;
    void AddAnimeToList(const QString& listName, Anime* anime);

    void SetupAnimePreviewSearchContextMenu(AnimePreviewUI* animePreviewUI, Anime* anime);

    void SearchAnime(const QString& animeName);

    void RefreshAnimeListUI();
    void RefreshListsUI();

    void ResetStyle();
    void LoadStyle(const QString& file);

private slots:
    void on_addAnimeButton_clicked();
    void on_searchButton_clicked();
    void on_newListButton_clicked();
    void on_listsComboBox_currentTextChanged(const QString &arg1);
    void on_searchLineEdit_returnPressed();

public slots:

    void OpenSettings();

private:
    Ui::AnimeHub *ui;

    Settings settings;

    static QNetworkAccessManager *manager;

    QVector<Anime *> animes; // deprecated

    QMap<QString, QVector<Anime *>*> lists;

    QVector<AnimePreviewUI *> animeListPreviewListUIs;

    QVector<AnimePreviewUI *> animePreviewSearchUIs;

    QString selectedList;
};
#endif // ANIMEHUB_H
