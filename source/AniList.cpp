#include "AniList.h"

AniList::AniList(QObject *parent) : QObject(parent)
{
	
}

// Basically, converts the "\u75a2" like sequences into real unicode (because they are escaped)
void HandleUnicode(QString* text) {
	QRegularExpression rx("(\\\\u([0-9a-fA-F]{4}))");
	QRegularExpressionMatch rxMatch;
	int pos = 0;
	while ((rxMatch = rx.match(*text, pos)).hasMatch()) {
		(*text).replace(rxMatch.capturedStart(), 6, QChar(rxMatch.captured(2).toUShort(0, 16)));
		pos = rxMatch.capturedStart() + 1;
	}
	text->replace(QRegularExpression("\\\\(n|r)"), "");
}

void AniList::GraphQLQuery(QString body, std::function<void(JSON::Object)> onSuccess, std::function<void()> onFailure) {
	
	body.replace(QRegularExpression("\t"), " "); // It seems that AniList GraphQL doesn't work with tabs
	QByteArray bodyRequest = QByteArray(body.toUtf8());
	
	QNetworkRequest request;
	request.setUrl(QUrl("https://graphql.anilist.co"));
	request.setRawHeader("Content-Type", "application/json");
	request.setRawHeader("Content-Length", QByteArray::number(bodyRequest.size()));

	auto* reply = http.post(request, bodyRequest);
	connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error()) {
			std::cerr << "Error!" << std::endl;
			std::cerr << reply->errorString().toStdString() << std::endl;
			std::cerr << reply->error() << std::endl;
			QString answer = reply->readAll();
			std::cerr << answer.toStdString() << std::endl;
			onFailure();
			return;
		}
		QString answer = reply->readAll();
		HandleUnicode(&answer);
		//std::cout << "Response:" << std::endl << answer.toStdString() << std::endl;

		JSON::Object json(answer.toStdString(), JSON::SOURCE::CONTENT);
		if (!json.IsValid()) {
			std::cerr << "Error while parsing json: " << json.GetError() << std::endl;
			onFailure();
			return ;
		}
		onSuccess(std::move(json));
	});
}

void AniList::SearchAnimes(const QString& title, std::function<void(QVector<Anime>)> onSuccess, std::function<void()> onFailure) {
	QString body =
	"{\
		\"query\": \"\
			query ($id: Int, $page: Int, $perPage: Int, $search: String) {\
				Page(page: $page, perPage: $perPage) {\
					pageInfo {\
						total\
						currentPage\
						lastPage\
						hasNextPage\
						perPage\
					}\
					media(id: $id, search: $search, type: ANIME) {\
						" + QString(GRAPHQL_ANILIST_MEDIA_CONTENT) + "\
					}\
				}\
			}\
		\",\
		\"variables\": {\
			\"search\": \"" + title + "\"\
		}\
	}";

	GraphQLQuery(body, [=](JSON::Object json) {
		auto& medias = json["data"].AsObject()["Page"].AsObject()["media"].AsArray().GetElements();

		QVector<Anime> animes;

		// This loop will check each field of the media before adding it to the Anime object.
		for (auto& media : medias) {
			Anime anime;
			if (media->IsObject() && media->AsObject()["title"].IsObject() && media->AsObject()["title"].AsObject()["romaji"].IsString()) {
				anime.SetTitle(QString::fromStdString(media->AsObject()["title"].AsObject()["romaji"].AsString()));
			}
			else {
			   anime.SetTitle("[Unknown]");
			}

			if (media->IsObject() && media->AsObject()["description"].IsString()) {
				anime.SetDescription(QString::fromStdString(media->AsObject()["description"].AsString()));
			}
			else {
				anime.SetDescription("[No description provided]");
			}

			if (media->IsObject() && media->AsObject()["coverImage"].IsObject() && media->AsObject()["coverImage"].AsObject()["large"].IsString()) {
				if (GetSettings().ShouldFetchImages())
					anime.SetCoverImageByUrl(QString::fromStdString(media->AsObject()["coverImage"].AsObject()["large"].AsString()).replace("\\", ""));
			}
			else {
				anime.SetCoverImageByUrl("[No image]");
			}

			if (media->IsObject() && media->AsObject()["status"].IsString()) {
				anime.SetStatus(QString::fromStdString(media->AsObject()["status"].AsString()));
			}
			else {
				anime.SetStatus("[Unknown status]");
			}

			if (media->IsObject() && media->AsObject()["episodes"].IsNumber()) {
				anime.SetEpisodes(media->AsObject()["episodes"].AsNumber());
			}
			else {
				anime.SetEpisodes(-1);
			}

			if (media->IsObject() && media->AsObject()["startDate"].IsObject()) {
				JSON::Object& startDateJson = media->AsObject()["startDate"].AsObject();
				anime.SetStartYear(startDateJson["year"].IsNumber() ? startDateJson["year"].AsNumber() : -1);
				anime.SetStartMonth(startDateJson["month"].IsNumber() ? startDateJson["month"].AsNumber() : -1);
				anime.SetStartDay(startDateJson["day"].IsNumber() ? startDateJson["day"].AsNumber() : -1);
			}

			if (media->IsObject() && media->AsObject()["endDate"].IsObject()) {
				JSON::Object& endDateJson = media->AsObject()["startDate"].AsObject();
				anime.SetEndYear(endDateJson["year"].IsNumber() ? endDateJson["year"].AsNumber() : -1);
				anime.SetEndMonth(endDateJson["month"].IsNumber() ? endDateJson["month"].AsNumber() : -1);
				anime.SetEndDay(endDateJson["day"].IsNumber() ? endDateJson["day"].AsNumber() : -1);
			}

			if (media->IsObject() && media->AsObject()["genres"].IsArray()) {
				for (int i = 0; i < media->AsObject()["genres"].AsArray().GetLenth(); ++i) {
					if (media->AsObject()["genres"].AsArray()[i].IsString())
						anime.AddGenre(QString::fromStdString(media->AsObject()["genres"].AsArray()[i].AsString()));
				}
			}
			animes.push_back(anime);
		}
		onSuccess(animes);
	}, [=]{
		onFailure();
	});
}
	
void AniList::FetchUserLists(const QString userName, std::function<void(QMap<QString, QVector<Anime>>)> onSuccess, std::function<void()> onFailure) {
	QString body =
    "{\
        \"query\": \"\
            query {\
                MediaListCollection(userName: \\\"" + userName + "\\\", type: ANIME) {\
                    lists {\
                        name\
                        entries {\
                            id\
                            media {\
                                " + QString(GRAPHQL_ANILIST_MEDIA_CONTENT) + "\
                            }\
                        }\
                    }\
                }\
            }\",\
        \"variables\": null\
    }";

	GraphQLQuery(body, [=](JSON::Object json) {
	
		QMap<QString, QVector<Anime>> lists;
		QVector<Anime> animes;
			
		for (int i = 0; i < json["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray().GetLenth(); ++i) {
			auto &listJson = json["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray()[i].AsObject();
			auto& entries = json["data"].AsObject()["MediaListCollection"].AsObject()["lists"].AsArray()[i].AsObject()["entries"].AsArray().GetElements();
			lists[QString::fromStdString(listJson["name"].AsString())];
			QVector<Anime>& list = lists[QString::fromStdString(listJson["name"].AsString())];
			for (auto& entry : entries) {
				auto& media_ = entry->AsObject()["media"].AsObject();
				auto* media = &media_;
				Anime anime;
				if (media->IsObject() && media->AsObject()["title"].IsObject() && media->AsObject()["title"].AsObject()["romaji"].IsString()) {
					anime.SetTitle(QString::fromStdString(media->AsObject()["title"].AsObject()["romaji"].AsString()));
				}
				else {
				   anime.SetTitle("[Unknown]");
				}
	
				if (media->IsObject() && media->AsObject()["description"].IsString()) {
					anime.SetDescription(QString::fromStdString(media->AsObject()["description"].AsString()));
				}
				else {
					anime.SetDescription("[No description provided]");
				}
	
				if (media->IsObject() && media->AsObject()["coverImage"].IsObject() && media->AsObject()["coverImage"].AsObject()["large"].IsString()) {
					if (GetSettings().ShouldFetchImages())
						anime.SetCoverImageByUrl(QString::fromStdString(media->AsObject()["coverImage"].AsObject()["large"].AsString()).replace("\\", ""));
				}
				else {
					anime.SetCoverImageByUrl("[No image]");
				}
	
				if (media->IsObject() && media->AsObject()["status"].IsString()) {
					anime.SetStatus(QString::fromStdString(media->AsObject()["status"].AsString()));
				}
				else {
					anime.SetStatus("[Unknown status]");
				}
	
				if (media->IsObject() && media->AsObject()["episodes"].IsNumber()) {
					anime.SetEpisodes(media->AsObject()["episodes"].AsNumber());
				}
				else {
					anime.SetEpisodes(-1);
				}
	
				if (media->IsObject() && media->AsObject()["startDate"].IsObject()) {
					JSON::Object& startDateJson = media->AsObject()["startDate"].AsObject();
					anime.SetStartYear(startDateJson["year"].IsNumber() ? startDateJson["year"].AsNumber() : -1);
					anime.SetStartMonth(startDateJson["month"].IsNumber() ? startDateJson["month"].AsNumber() : -1);
					anime.SetStartDay(startDateJson["day"].IsNumber() ? startDateJson["day"].AsNumber() : -1);
				}
	
				if (media->IsObject() && media->AsObject()["endDate"].IsObject()) {
					JSON::Object& endDateJson = media->AsObject()["startDate"].AsObject();
					anime.SetEndYear(endDateJson["year"].IsNumber() ? endDateJson["year"].AsNumber() : -1);
					anime.SetEndMonth(endDateJson["month"].IsNumber() ? endDateJson["month"].AsNumber() : -1);
					anime.SetEndDay(endDateJson["day"].IsNumber() ? endDateJson["day"].AsNumber() : -1);
				}
	
				if (media->IsObject() && media->AsObject()["genres"].IsArray()) {
					for (int i = 0; i < media->AsObject()["genres"].AsArray().GetLenth(); ++i) {
						if (media->AsObject()["genres"].AsArray()[i].IsString())
							anime.AddGenre(QString::fromStdString(media->AsObject()["genres"].AsArray()[i].AsString()));
					}
				}
				
				list.push_back(anime);
			}
		}
		
		onSuccess(lists);
	}, [=]{
		onFailure();
	});
}
