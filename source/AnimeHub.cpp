#include "AnimeHub.h"
#include "./ui_AnimeHub.h"
#include "Anime.h"
#include <iostream>
#include <QtNetwork/QHttpMultiPart>
#include <QByteArray>
#include <QFrame>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include "json-parser/JSON.h"
#include "AnimePreviewUI.h"

const QString DefaultListName = "Default";

QNetworkAccessManager* AnimeHub::manager = nullptr;

AnimeHub::AnimeHub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AnimeHub), selectedList(DefaultListName)
{
    ui->setupUi(this);
    ui->animeListLayout->setAlignment(Qt::AlignTop);

    AnimeHub::manager = new QNetworkAccessManager(this);

    Load();
    if (lists.isEmpty())
        lists[DefaultListName] = new QVector<Anime*>;

    RefreshListsUI();
    RefreshAnimeListUI();
}

AnimeHub::~AnimeHub()
{
    delete ui;
    delete AnimeHub::manager;
}

void AnimeHub::Save() {
    JSON::Object json;

    auto& listsJson = json.AddArray("lists");

    for (auto& listName : lists.keys()) {
        auto& listJson = listsJson.AddObject();
        listJson["name"] = listName.toStdString();
        auto& animesJson = listJson.AddArray("animes");
        for (Anime* anime : *lists[listName]) {
            auto& animeJson = animesJson.AddObject();
            animeJson["title"] = anime->GetTitle().toStdString();
            animeJson["description"] = anime->GetDescription().toStdString();
            animeJson["coverImageUrl"] = anime->GetCoverImageUrl().toStdString();
        }
    }
    json.Save("save.json");
}

void AnimeHub::Load() {
    JSON::Object json("save.json");
    if (json.IsValid()) {
        for (auto& listJson : json["lists"].AsArray().GetElements()) {
            lists[QString::fromStdString(listJson->AsObject()["name"].AsString())] = new QVector<Anime*>;
            auto& animesJson = listJson->AsObject()["animes"].AsArray();
            for (int i = 0; i < animesJson.GetLenth(); ++i) {
                Anime* anime = new Anime(QString::fromStdString(animesJson[i].AsObject()["title"].AsString()));
                anime->SetDescription(QString::fromStdString(animesJson[i].AsObject()["description"].AsString()));
                anime->SetCoverImageByUrl(QString::fromStdString(animesJson[i].AsObject()["coverImageUrl"].AsString()));
                lists[QString::fromStdString(listJson->AsObject()["name"].AsString())]->push_back(anime);
            }
        }
        return ;
    }
    else {
        std::cout << "Failed to load save.json" << std::endl;
    }
}

const QVector<Anime *>& AnimeHub::GetAnimes() const {
    return animes;
}

void AnimeHub::AddAnimeToList(const QString& listName, Anime* anime) {
    std::cout << "Adding anime" << std::endl;

    lists[listName]->push_back(anime);
    RefreshAnimeListUI();
    Save();
}

void AnimeHub::CreateList(const QString& listName) {
    if (listName.isEmpty() || lists.find(listName) != lists.end())
        return ;
    lists[listName] = new QVector<Anime*>;
    RefreshListsUI();
    Save();
}

void AnimeHub::SelectList(const QString& listName) {
    selectedList = listName;
    RefreshAnimeListUI();
}

void AnimeHub::SetupAnimePreviewSearchContextMenu(AnimePreviewUI* animePreviewUI, Anime* anime) {
    animePreviewUI->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(animePreviewUI, &AnimePreviewUI::customContextMenuRequested, [=] (const QPoint& pos) {
        // for most widgets
        QPoint globalPos = animePreviewUI->mapToGlobal(pos);
        // for QAbstractScrollArea and derived classes you would use:
        // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

        QMenu myMenu;
        myMenu.addAction(anime->GetTitle());

        QMap<QAction*, QString> actions;
        for (auto listName : lists.keys()) {
            actions[myMenu.addAction("Add to '" + listName + "'")] = listName;
        }

        QAction* selectedAction = myMenu.exec(globalPos);

        for (auto action : actions.keys()) {
            if (selectedAction == action) {
                AddAnimeToList(actions[action], anime);
            }
        }
    });
}

void AnimeHub::SearchAnime(const QString& animeName) {
    std::cout << "Fetching page" << std::endl;

    QByteArray body =
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
                media(id: $id, search: $search) {\
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
                 }\
                }\
              }\
            }\
        \",\
        \"variables\": {\
            \"search\": \"" + QByteArray(animeName.toUtf8()) + "\"\
        }\
    }";

    QNetworkRequest request;
    request.setUrl(QUrl("https://graphql.anilist.co"));
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", QByteArray::number(body.size()));

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
        std::cout << "Response:" << std::endl << answer.toStdString() << std::endl;

        JSON::Object json(answer.toStdString(), JSON::SOURCE::CONTENT);
        if (!json.IsValid()) {
            std::cout << "Error while parsing json: " << json.GetError() << std::endl;
            return ;
        }
        auto& medias = json["data"].AsObject()["Page"].AsObject()["media"].AsArray().GetElements();

        while (!animePreviewSearchUIs.isEmpty()) {
            AnimePreviewUI* animePreviewUI = animePreviewSearchUIs.takeLast();
            ui->animeSearchResult->removeWidget(animePreviewUI);
            delete animePreviewUI;
        }

        for (auto& media : medias) {
            QString title = QString::fromStdString(media->AsObject()["title"].AsObject()["romaji"].AsString());
            QString description = !media->AsObject()["description"].IsNull() ? QString::fromStdString(media->AsObject()["description"].AsString()) : "null";
            QString coverImageUrl = QString::fromStdString(media->AsObject()["coverImage"].AsObject()["large"].AsString()).replace("\\", "");
            QString status = QString::fromStdString(media->AsObject()["status"].AsString());
            int episodes = !media->AsObject()["episodes"].IsNull() ? media->AsObject()["episodes"].AsNumber() : -1;

            JSON::Object& startDateJson = media->AsObject()["startDate"].AsObject();
            int startYear = startDateJson["year"].IsNull() ? -1 : startDateJson["year"].AsNumber();
            int startMonth = startDateJson["month"].IsNull() ? -1 : startDateJson["month"].AsNumber();
            int startDay = startDateJson["day"].IsNull() ? -1 : startDateJson["day"].AsNumber();

            JSON::Object& endDateJson = media->AsObject()["endDate"].AsObject();
            int endYear = endDateJson["year"].IsNull() ? -1 : endDateJson["year"].AsNumber();
            int endMonth = endDateJson["month"].IsNull() ? -1 : endDateJson["month"].AsNumber();
            int endDay = endDateJson["day"].IsNull() ? -1 : endDateJson["day"].AsNumber();

            Anime *anime = new Anime(title);
            anime->SetDescription(description);
            anime->SetCoverImageByUrl(coverImageUrl);
            for (int i = 0; i < media->AsObject()["genres"].AsArray().GetLenth(); ++i) {
                anime->AddGenre(QString::fromStdString(media->AsObject()["genres"].AsArray()[i].AsString()));
            }
            anime->SetStatus(status);
            anime->SetEpisodes(episodes);

            anime->SetStartYear(startYear);
            anime->SetStartMonth(startMonth);
            anime->SetStartDay(startDay);

            anime->SetEndYear(endYear);
            anime->SetEndMonth(endMonth);
            anime->SetEndDay(endDay);

            AnimePreviewUI *preview = new AnimePreviewUI(anime, this);
            animePreviewSearchUIs.push_back(preview);
            ui->animeSearchResult->addWidget(preview);



            SetupAnimePreviewSearchContextMenu(preview, anime);
        }
    });
}

// Events

void AnimeHub::on_addAnimeButton_clicked()
{
   AddAnimeToList(DefaultListName, new Anime("ez"));
}

void AnimeHub::on_searchButton_clicked()
{
    SearchAnime(ui->searchLineEdit->text().isEmpty() ? "sword art online" : ui->searchLineEdit->text());
}

void AnimeHub::on_searchLineEdit_returnPressed()
{
    ui->searchButton->click();
}

void AnimeHub::on_listsComboBox_currentTextChanged(const QString &listName)
{
    if (listName.isEmpty() || lists.find(listName) == lists.end())
        return ;
    SelectList(listName);
}

void AnimeHub::on_newListButton_clicked()
{
    QString newListName = QInputDialog::getText(this, "New list", "Chose a name for the new list");
    CreateList(newListName);
}

// UI related methods

void AnimeHub::RefreshAnimeListUI() {
    while (!animeListPreviewListUIs.isEmpty()) {
        AnimePreviewUI* animePreviewUI = animeListPreviewListUIs.takeLast();
        ui->animeListLayout->removeWidget(animePreviewUI);
        delete animePreviewUI;
    }

    for (Anime* anime : *lists[selectedList]) {
        AnimePreviewUI* animePreviewUI = new AnimePreviewUI(anime, this);
        animeListPreviewListUIs.push_back(animePreviewUI);
        ui->animeListLayout->addWidget(animePreviewUI);
    }
}

void AnimeHub::RefreshListsUI() {
    ui->listsComboBox->clear();
    for (auto& listName : lists.keys()) {
        ui->listsComboBox->addItem(listName);
    }
}

// Static methods

QNetworkAccessManager& AnimeHub::GetNetworkManager() {
    return *AnimeHub::manager;
}
