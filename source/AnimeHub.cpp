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
    AnimeHub::manager = new QNetworkAccessManager(this);

    lists[DefaultListName] = new QVector<Anime*>;
    Load();
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
    if (!json.IsValid()) {
        std::cout << "Failed to load save.json" << std::endl;
        return ;
    }
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
    RefreshAnimeListUI();
    RefreshListsUI();
}

void AnimeHub::on_addAnimeButton_clicked()
{
   AddAnimeToList(DefaultListName, new Anime("ez"));
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
    ui->animeListLayout->setAlignment(Qt::AlignTop);
}

void AnimeHub::RefreshListsUI() {
    ui->listsComboBox->clear();
    for (auto& listName : lists.keys()) {
        ui->listsComboBox->addItem(listName);
    }
}

void AnimeHub::on_pushButton_clicked()
{
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
                  coverImage {\
                    large\
                 }\
                }\
              }\
            }\
        \",\
        \"variables\": {\
            \"search\": \"" + QByteArray(ui->searchLineEdit->text().isEmpty() ? "sword art online" : ui->searchLineEdit->text().toUtf8()) + "\"\
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
        std::cout << "Response!" << std::endl;
        std::cout << answer.toStdString() << std::endl;

        JSON::Object json(answer.toStdString(), JSON::SOURCE::CONTENT);
        if (!json.IsValid()) {
            std::cout << "Error while parsing json: " << json.GetError() << std::endl;
            return ;
        }
        std::cout << json["data"].AsObject()["Page"].AsObject()["media"].AsArray().ToString() << std::endl;
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

            Anime *anime = new Anime(title);
            anime->SetTitle(title);
            anime->SetDescription(description);
            anime->SetCoverImageByUrl(coverImageUrl);

            std::cout << title.toStdString() << std::endl;
            AnimePreviewUI *preview = new AnimePreviewUI(anime, this);
            animePreviewSearchUIs.push_back(preview);


            preview->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(preview, &AnimePreviewUI::customContextMenuRequested, [=] (const QPoint& pos) {
                // for most widgets
                QPoint globalPos = preview->mapToGlobal(pos);
                // for QAbstractScrollArea and derived classes you would use:
                // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

                QMenu myMenu;
                myMenu.addAction(title);

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

            ui->animeSearchResult->addWidget(preview);
        }
    });
}

void AnimeHub::on_newListButton_clicked()
{
    QString newListName = QInputDialog::getText(this, "New list", "Chose a name for the new list");
    if (newListName.isEmpty() || lists.find(newListName) != lists.end())
        return ;
    lists[newListName] = new QVector<Anime*>;
    RefreshListsUI();
    Save();
}

void AnimeHub::on_listsComboBox_currentTextChanged(const QString &listName)
{
    if (listName.isEmpty() || lists.find(listName) == lists.end())
        return ;
    SelectList(listName);
}

void AnimeHub::SelectList(const QString& listName) {
    selectedList = listName;
    RefreshAnimeListUI();
}


// Static methods

QNetworkAccessManager& AnimeHub::GetNetworkManager() {
    return *AnimeHub::manager;
}
