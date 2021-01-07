#include "AnimeHub.h"
#include "./ui_AnimeHub.h"
#include "Anime.h"
#include <iostream>
#include <QtNetwork/QHttpMultiPart>
#include <QByteArray>
#include <QFrame>
#include <QPushButton>
#include <QInputDialog>
#include "json-parser/JSON.h"
#include "AnimePreviewUI.h"

const QString DefaultListName = "Default";

AnimeHub::AnimeHub(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AnimeHub), selectedList(DefaultListName)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);

    lists[DefaultListName] = new QVector<Anime*>;
    Load();
}

AnimeHub::~AnimeHub()
{
    delete ui;
    delete manager;
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
            lists[QString::fromStdString(listJson->AsObject()["name"].AsString())]->push_back(anime);
        }
    }
    RefreshAnimeListUI();
    RefreshListsUI();
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
            //animeJson["coverImageUrl"] = anime->GetCoverImageURL().toStdString();
        }
    }
    json.Save("save.json");
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

    for (const Anime* anime : *lists[selectedList]) {
        AnimePreviewUI* animePreviewUI = new AnimePreviewUI(this);
        animeListPreviewListUIs.push_back(animePreviewUI);
        animePreviewUI->SetTitle(anime->GetTitle());
        ui->animeListLayout->addWidget(animePreviewUI);
    }
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
            QString coverImageUrl = QString::fromStdString(media->AsObject()["coverImage"].AsObject()["large"].AsString());
            std::cout << title.toStdString() << std::endl;
            AnimePreviewUI *preview = new AnimePreviewUI(this);
            animePreviewSearchUIs.push_back(preview);
            preview->SetTitle(title);
            preview->SetDescription(description);
            preview->SetCoverImageUrl(coverImageUrl);

            ui->animeSearchResult->addWidget(preview);

            coverImageUrl.replace("\\", "");

            QNetworkRequest coverImageRequest;
            coverImageRequest.setUrl(QUrl(coverImageUrl));
            auto* coverImageReply = manager->get(coverImageRequest);

            connect(coverImageReply, &QNetworkReply::finished, [=] {
                if (coverImageReply->error()) {
                    std::cout << "Error fetching image!" << std::endl;
                    std::cout << coverImageReply->errorString().toStdString() << std::endl;
                    std::cout << coverImageReply->error() << std::endl;
                    QString answer = coverImageReply->readAll();
                    std::cout << answer.toStdString() << std::endl;
                    return;
                }
                QByteArray coverImageData = coverImageReply->readAll();
                QPixmap coverImagePixmap;
                coverImagePixmap.loadFromData(coverImageData);
                preview->SetCoverImagePixmap(coverImagePixmap);
            });
        }
    });
}

void AnimeHub::on_newListButton_clicked()
{
    QString newListName = QInputDialog::getText(this, "New list", "Chose a name for the new list");
    if (lists.find(newListName) != lists.end())
        return ;
    lists[newListName] = new QVector<Anime*>;
    RefreshListsUI();
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
