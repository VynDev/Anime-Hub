#ifndef ANILIST_H
#define ANILIST_H
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QMap>
#include <QtNetwork>
#include <functional>
#include <iostream>
#include "json-parser/JSON.h"
#include "Settings.h"
#include "Anime.h"

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

class AniList : public QObject
{
	Q_OBJECT
public:
	explicit AniList(QObject *parent = nullptr);
	
	void GraphQLQuery(QString body_, std::function<void(JSON::Object)> onSuccess, std::function<void()> onFailure);

	void SearchAnimes(const QString& title, std::function<void(QVector<Anime>)> onSuccess, std::function<void()> onFailure);
	void FetchUserLists(const QString userName, std::function<void(QMap<QString, QVector<Anime>>)> onSuccess, std::function<void()> onFailure);
	
signals:
	
private:
	
	QNetworkAccessManager http;
	
};

#endif // ANILIST_H
