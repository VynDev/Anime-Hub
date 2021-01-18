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

// Please keep the whitespaces with a REAL SPACE, NOT TAB for graphQL queries for the moment
#define GRAPHQL_ANILIST_MEDIA_CONTENT "\
    id\
    title {\
        romaji\
    }\
    description\
    episodes\
    genres\
    status\
    startDate {\
        year\
        month\
        day\
    }\
    endDate {\
        year\
        month\
        day\
    }\
    coverImage {\
        large\
    }"

class Anime;
class SettingsUI;
class AboutUI;
class AniListImport;

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
	
	const Settings& GetSettings() const;

	void Load();
	void Save();

	void CreateList(const QString& listName);
	void SelectList(const QString& listName);
	void DeleteList(const QString& listName);
	bool HasList(const QString& listName) const;

	const QVector<Anime *>& GetAnimes() const;
	void AddAnimeToList(const QString& listName, const Anime& anime);
	void RemoveAnimeFromList(const QString& listName, int index);

	void SetupAnimePreviewSearchContextMenu(AnimePreviewUI* animePreviewUI);
	void SetupAnimePreviewListContextMenu(AnimePreviewUI* animePreviewUI, int index);

	void SearchAnime(const QString& animeName);

	void RefreshAnimeListUI();
	void RefreshListsUI();

	void ResetStyle();
	void LoadStyle(const QString& file);

	void ImportFromAnilist(const QString& userName, const QString& anilistListName, const QString& targetListName);

private slots:
	void on_searchButton_clicked();
	void on_newListButton_clicked();
	void on_listsComboBox_currentTextChanged(const QString &arg1);
	void on_searchLineEdit_returnPressed();
	void on_deleteListButton_clicked();

public slots:

	void OpenSettings();
	void OpenAbout();
	void OpenAnilistImport();

private:
	Ui::AnimeHub *ui;

	Settings settings;

	SettingsUI *settingsUI;
	AboutUI *aboutUI;
	AniListImport *aniListImport;

	static QNetworkAccessManager *manager;

	QVector<Anime *> animes; // deprecated
	QMap<QString, QVector<Anime *>> lists;
	QVector<AnimePreviewUI *> animeListPreviewListUIs;
	QVector<AnimePreviewUI *> animePreviewSearchUIs;
	QString selectedList;
};
#endif // ANIMEHUB_H
