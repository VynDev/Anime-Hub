#include "AniListImport.h"
#include "ui_AniListImport.h"
#include "json-parser/JSON.h"
#include "AnimeHub.h"
#include <QtNetwork>
#include <QByteArray>

AniListImport::AniListImport(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::AniListImport)
{
	ui->setupUi(this);
	
}

AniListImport::~AniListImport()
{
	delete ui;
}

void AniListImport::on_findUserButton_clicked()
{
	QByteArray body =
    "{\
        \"query\": \"\
            query {\
                MediaListCollection(userName: \\\"Vyn\\\", type: ANIME) {\
                    lists {\
                        name\
                        entries {\
                            id\
                            media {\
                                " + QByteArray(GRAPHQL_ANILIST_MEDIA_CONTENT) + "\
                            }\
                        }\
                    }\
                }\
            }\",\
        \"variables\": null\
    }";
	std::cout << body.toStdString() << std::endl;
	
	QNetworkRequest request;
	request.setUrl(QUrl("https://graphql.anilist.co"));
	request.setRawHeader("Content-Type", "application/json");
	request.setRawHeader("Content-Length", QByteArray::number(body.size()));
	
	QNetworkAccessManager *manager = &AnimeHub::GetNetworkManager();

	auto* reply = manager->post(request, body);
	connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error()) {
			std::cout << "Error!" << std::endl;
			std::cout << reply->errorString().toStdString() << std::endl;
			std::cout << reply->error() << std::endl;
			QString answer = reply->readAll();
			std::cout << answer.toStdString() << std::endl;
			return;
		}
		QString answer = reply->readAll();
		QRegularExpression rx("(\\\\u([0-9a-fA-F]{4}))");
		QRegularExpressionMatch rxMatch;
		int pos = 0;
		while ((rxMatch = rx.match(answer, pos)).hasMatch()) {
			answer.replace(rxMatch.capturedStart(), 6, QChar(rxMatch.captured(2).toUShort(0, 16)));
			pos = rxMatch.capturedStart() + 1;
		}

		answer.replace(QRegularExpression("\\\\(n|r)"), "");

		std::cout << "Response:" << std::endl << answer.toStdString() << std::endl;

		json = new JSON::Object(answer.toStdString(), JSON::CONTENT);

		std::cout << "count: " <<
			(*json)["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray()[0].AsObject()["entries"].AsArray().GetLenth()
		<< std::endl;
		
		auto& entries = (*json)["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray().GetElements();
		
		for (auto& entry : entries) {
			ui->aniListListsComboBox->addItem(QString::fromStdString(entry->AsObject()["name"].AsString()));
		}
	});
}

void AniListImport::on_importButton_clicked()
{
	ImportFromAnilist("Vyn", "Completed", ui->newListLineEdit->text());
	close();
}

void AniListImport::ImportFromAnilist(const QString& userName, const QString& anilistListName, const QString& targetListName) {
	std::cout << "Importing list from Anilist" << std::endl;
	
	AnimeHub* animeHub = static_cast<AnimeHub*>(parent());
	
	if (!animeHub->HasList(targetListName))
	{
		animeHub->CreateList(targetListName);
		animeHub->RefreshListsUI();
	}
	
	// Please keep the whitespaces with a REAL SPACE, NOT TAB for graphQL queries for the moment
	QByteArray body =
    "{\
        \"query\": \"\
            query {\
                MediaListCollection(userName: \\\"Vyn\\\", type: ANIME) {\
                    lists {\
                        name\
                        entries {\
                            id\
                            media {\
                                " + QByteArray(GRAPHQL_ANILIST_MEDIA_CONTENT) + "\
                            }\
                        }\
                    }\
                }\
            }\",\
        \"variables\": null\
    }";
	std::cout << body.toStdString() << std::endl;
	
	QNetworkRequest request;
	request.setUrl(QUrl("https://graphql.anilist.co"));
	request.setRawHeader("Content-Type", "application/json");
	request.setRawHeader("Content-Length", QByteArray::number(body.size()));

	/*auto* reply = manager->post(request, body);
	connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error()) {
			std::cout << "Error!" << std::endl;
			std::cout << reply->errorString().toStdString() << std::endl;
			std::cout << reply->error() << std::endl;
			QString answer = reply->readAll();
			std::cout << answer.toStdString() << std::endl;
			return;
		}
		QString answer = reply->readAll();
		QRegularExpression rx("(\\\\u([0-9a-fA-F]{4}))");
		QRegularExpressionMatch rxMatch;
		int pos = 0;
		while ((rxMatch = rx.match(answer, pos)).hasMatch()) {
			answer.replace(rxMatch.capturedStart(), 6, QChar(rxMatch.captured(2).toUShort(0, 16)));
			pos = rxMatch.capturedStart() + 1;
		}

		answer.replace(QRegularExpression("\\\\(n|r)"), "");

		std::cout << "Response:" << std::endl << answer.toStdString() << std::endl;

		JSON::Object json(answer.toStdString(), JSON::CONTENT);*/
		JSON::Object json("./importlist.json");

		std::cout << "count: " <<
			json["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray()[0].AsObject()["entries"].AsArray().GetLenth()
		<< std::endl;
		
		auto& medias = json["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray()[0].AsObject()["entries"].AsArray().GetElements();
		
		for (auto& entry : medias) {
			auto& media_ = entry->AsObject()["media"].AsObject();
			auto* media = &media_;
            Anime *anime = new Anime;
            if (media->IsObject() && media->AsObject()["title"].IsObject() && media->AsObject()["title"].AsObject()["romaji"].IsString()) {
                anime->SetTitle(QString::fromStdString(media->AsObject()["title"].AsObject()["romaji"].AsString()));
            }
            else {
               anime->SetTitle("[Unknown]");
            }

            if (media->IsObject() && media->AsObject()["description"].IsString()) {
                anime->SetDescription(QString::fromStdString(media->AsObject()["description"].AsString()));
            }
            else {
                anime->SetDescription("[No description provided]");
            }

            if (media->IsObject() && media->AsObject()["coverImage"].IsObject() && media->AsObject()["coverImage"].AsObject()["large"].IsString()) {
                if (animeHub->GetSettings().ShouldFetchImages())
                    anime->SetCoverImageByUrl(QString::fromStdString(media->AsObject()["coverImage"].AsObject()["large"].AsString()).replace("\\", ""));
            }
            else {
                anime->SetCoverImageByUrl("[No image]");
            }

            if (media->IsObject() && media->AsObject()["status"].IsString()) {
                anime->SetStatus(QString::fromStdString(media->AsObject()["status"].AsString()));
            }
            else {
                anime->SetStatus("[Unknown status]");
            }

            if (media->IsObject() && media->AsObject()["episodes"].IsNumber()) {
                anime->SetEpisodes(media->AsObject()["episodes"].AsNumber());
            }
            else {
                anime->SetEpisodes(-1);
            }

            if (media->IsObject() && media->AsObject()["startDate"].IsObject()) {
                JSON::Object& startDateJson = media->AsObject()["startDate"].AsObject();
                anime->SetStartYear(startDateJson["year"].IsNumber() ? startDateJson["year"].AsNumber() : -1);
                anime->SetStartMonth(startDateJson["month"].IsNumber() ? startDateJson["month"].AsNumber() : -1);
                anime->SetStartDay(startDateJson["day"].IsNumber() ? startDateJson["day"].AsNumber() : -1);
            }

            if (media->IsObject() && media->AsObject()["endDate"].IsObject()) {
                JSON::Object& endDateJson = media->AsObject()["startDate"].AsObject();
                anime->SetEndYear(endDateJson["year"].IsNumber() ? endDateJson["year"].AsNumber() : -1);
                anime->SetEndMonth(endDateJson["month"].IsNumber() ? endDateJson["month"].AsNumber() : -1);
                anime->SetEndDay(endDateJson["day"].IsNumber() ? endDateJson["day"].AsNumber() : -1);
            }

            if (media->IsObject() && media->AsObject()["genres"].IsArray()) {
                for (int i = 0; i < media->AsObject()["genres"].AsArray().GetLenth(); ++i) {
                    if (media->AsObject()["genres"].AsArray()[i].IsString())
                        anime->AddGenre(QString::fromStdString(media->AsObject()["genres"].AsArray()[i].AsString()));
                }
            }
			
			animeHub->AddAnimeToList(targetListName, *anime);
        }
		animeHub->RefreshAnimeListUI();
		animeHub->Save();
	//});
}
