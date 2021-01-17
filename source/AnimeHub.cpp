#include "AnimeHub.h"
#include "./ui_AnimeHub.h"

#include <QtNetwork/QHttpMultiPart>
#include <QByteArray>
#include <QFrame>
#include <QPushButton>
#include <QInputDialog>
#include <QMenu>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QMessageBox>

#include <iostream>
#include "json-parser/JSON.h"
#include "AnimePreviewUI.h"
#include "Anime.h"
#include "SettingsUI.h"
#include "AboutUI.h"

// [Todo] Fix pattern design errors and add comments when needed.

QNetworkAccessManager* AnimeHub::manager = nullptr;

AnimeHub::AnimeHub(QWidget *parent) : QMainWindow(parent)
    , ui(new Ui::AnimeHub)
{
    ui->setupUi(this);

    AnimeHub::manager = new QNetworkAccessManager(this);
    settingsUI = new SettingsUI(&settings, this);
    aboutUI = new AboutUI(this);

    if (!settings.ShouldUseSystemTheme())
        LoadStyle(":/styles/vyn-dark.qss");

    Load(); // Anime lists
    if (lists.isEmpty())
        lists["Watched"]; // Generate a default list

    RefreshListsUI();
    RefreshAnimeListUI();

    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(OpenSettings()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(OpenAbout()));
    ui->animeListLayout->setAlignment(Qt::AlignTop);
}

AnimeHub::~AnimeHub() {
    delete ui;
    delete AnimeHub::manager;
    delete settingsUI;
    delete aboutUI;
    for (auto& list : lists) {
        for (int i = 0; i < list.size(); ++i) {
            delete list[i];
        }
    }
}

void AnimeHub::ResetStyle() {
    setStyleSheet("");
}

void AnimeHub::LoadStyle(const QString& file) {
    QFile style(file);
    style.open(QFile::ReadOnly);
    setStyleSheet(style.readAll());
}

void AnimeHub::OpenSettings() {
    std::cout << "Opening settings" << std::endl;
    settingsUI->show();
}

void AnimeHub::OpenAbout() {
    std::cout << "Opening about" << std::endl;
    aboutUI->show();
}

void AnimeHub::Save() {
    JSON::Object json;

    auto& listsJson = json.AddArray("lists");

    for (auto& listName : lists.keys()) {
        auto& listJson = listsJson.AddObject();
        listJson["name"] = listName.toStdString();
        auto& animesJson = listJson.AddArray("animes");
        for (Anime* anime : lists[listName]) {
            auto& animeJson = animesJson.AddObject();
            animeJson["title"] = anime->GetTitle().toStdString();
            animeJson["description"] = anime->GetDescription().toStdString();
            animeJson["coverImageUrl"] = anime->GetCoverImageUrl().toStdString();
            animeJson["episodes"] = anime->GetEpisodes();
            animeJson["status"] = anime->GetStatus().toStdString();
            animeJson.AddArray("genres");
            for (const QString& genreName : anime->GetGenres()) {
                animeJson["genres"].AsArray().AddElement(genreName.toStdString());
            }
            animeJson.AddObject("startDate");
            animeJson["startDate"].AsObject()["year"] = anime->GetStartYear();
            animeJson["startDate"].AsObject()["month"] = anime->GetStartMonth();
            animeJson["startDate"].AsObject()["day"] = anime->GetStartDay();
            animeJson.AddObject("endDate");
            animeJson["endDate"].AsObject()["year"] = anime->GetEndYear();
            animeJson["endDate"].AsObject()["month"] = anime->GetEndMonth();
            animeJson["endDate"].AsObject()["day"] = anime->GetEndDay();
        }
    }
    json.Save(settings.GetPath().toStdString());
}

void AnimeHub::Load() {
    JSON::Object json(settings.GetPath().toStdString());
    if (json.IsValid()) {
        for (auto& listJson : json["lists"].AsArray().GetElements()) {
            lists[QString::fromStdString(listJson->AsObject()["name"].AsString())];
            auto& animesJson = listJson->AsObject()["animes"].AsArray();
            for (int i = 0; i < animesJson.GetLenth(); ++i) {
                Anime* anime = new Anime;
                anime->SetTitle(QString::fromStdString(animesJson[i].AsObject()["title"].AsString()));
                anime->SetDescription(QString::fromStdString(animesJson[i].AsObject()["description"].AsString()));
                anime->SetCoverImageByUrl(QString::fromStdString(animesJson[i].AsObject()["coverImageUrl"].AsString()));
                anime->SetStatus(QString::fromStdString(animesJson[i].AsObject()["status"].AsString()));

                anime->SetEpisodes(animesJson[i].AsObject()["episodes"].AsNumber());

                for (auto& genreJson : animesJson[i].AsObject()["genres"].AsArray().GetElements()) {
                    anime->AddGenre(QString::fromStdString(genreJson->AsString()));
                }

                anime->SetStartYear(animesJson[i].AsObject()["startDate"].AsObject()["year"].AsNumber());
                anime->SetStartMonth(animesJson[i].AsObject()["startDate"].AsObject()["month"].AsNumber());
                anime->SetStartDay(animesJson[i].AsObject()["startDate"].AsObject()["day"].AsNumber());

                anime->SetEndYear(animesJson[i].AsObject()["endDate"].AsObject()["year"].AsNumber());
                anime->SetEndMonth(animesJson[i].AsObject()["endDate"].AsObject()["month"].AsNumber());
                anime->SetEndDay(animesJson[i].AsObject()["endDate"].AsObject()["day"].AsNumber());

                lists[QString::fromStdString(listJson->AsObject()["name"].AsString())].push_back(anime);
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

void AnimeHub::AddAnimeToList(const QString& listName, const Anime& anime) {
    std::cout << "Adding anime" << std::endl;

    lists[listName].push_back(new Anime(anime));
    RefreshAnimeListUI();
    Save();
}

void AnimeHub::RemoveAnimeFromList(const QString &listName, int index) {
    std::cout << "Removing anime" << std::endl;

    delete lists[listName][index];
    lists[listName].remove(index, 1);
    RefreshAnimeListUI();
    Save();
}

void AnimeHub::CreateList(const QString& listName) {
    if (listName.isEmpty() || lists.find(listName) != lists.end())
        return ;
    lists[listName];
    RefreshListsUI();
    Save();
}

void AnimeHub::SelectList(const QString& listName) {
    selectedList = listName;
    RefreshAnimeListUI();
}

void AnimeHub::DeleteList(const QString &listName) {
    lists.remove(listName);
    RefreshListsUI();
    Save();
}

void AnimeHub::SetupAnimePreviewSearchContextMenu(AnimePreviewUI* animePreviewUI) {
    animePreviewUI->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(animePreviewUI, &AnimePreviewUI::customContextMenuRequested, [=] (const QPoint& pos) {
        QPoint globalPos = animePreviewUI->mapToGlobal(pos);
        QMenu myMenu;

        myMenu.addAction(animePreviewUI->GetTitle());

        QHash<QAction*, QString> actions;
        for (const QString& listName : lists.keys()) {
            actions[myMenu.addAction("Add to '" + listName + "'")] = listName;
        }

        QAction* selectedAction = myMenu.exec(globalPos);

        for (auto action : actions.keys()) {
            if (selectedAction == action) {
                AddAnimeToList(actions[action], *animePreviewUI);
            }
        }
    });
}

void AnimeHub::SetupAnimePreviewListContextMenu(AnimePreviewUI* animePreviewUI, int index) {
    animePreviewUI->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(animePreviewUI, &AnimePreviewUI::customContextMenuRequested, [=] (const QPoint& pos) {
        QPoint globalPos = animePreviewUI->mapToGlobal(pos);
        QMenu myMenu;

        myMenu.addAction(animePreviewUI->GetTitle());
        QAction *removeAction = myMenu.addAction("Remove from the list");
        QAction* selectedAction = myMenu.exec(globalPos);

        if (selectedAction == removeAction) {
            QMessageBox msgBox;
            msgBox.setText("Do you really want to delete '" + animePreviewUI->GetTitle() + "' from the list ?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            auto answer = msgBox.exec();
            if (answer == QMessageBox::Yes)
                RemoveAnimeFromList(selectedList, index);
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
                media(id: $id, search: $search, type: ANIME) {\
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
        QRegularExpression rx("(\\\\u([0-9a-fA-F]{4}))");
        QRegularExpressionMatch rxMatch;
        int pos = 0;
        while ((rxMatch = rx.match(answer, pos)).hasMatch()) {
            answer.replace(rxMatch.capturedStart(), 6, QChar(rxMatch.captured(2).toUShort(0, 16)));
            pos = rxMatch.capturedStart() + 1;
        }

        answer.replace(QRegularExpression("\\\\(n|r)"), "");

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

        // This loop will check each field of the media before adding it to the Anime object.
        for (auto& media : medias) {
            AnimePreviewUI *anime = new AnimePreviewUI(this);
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
                if (settings.ShouldFetchImages())
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

            anime->Refresh();
            anime->setObjectName("animePreviewUI");
            animePreviewSearchUIs.push_back(anime);
            ui->animeSearchResult->addWidget(anime);

            SetupAnimePreviewSearchContextMenu(anime);
        }
    });
}

// Events

void AnimeHub::on_searchButton_clicked() {
    SearchAnime(ui->searchLineEdit->text().isEmpty() ? "sword art online" : ui->searchLineEdit->text());
}

void AnimeHub::on_searchLineEdit_returnPressed() {
    ui->searchButton->click();
}

void AnimeHub::on_listsComboBox_currentTextChanged(const QString &listName) {
    if (listName.isEmpty() || lists.find(listName) == lists.end())
        return ;
    SelectList(listName);
}

void AnimeHub::on_newListButton_clicked() {
    QString newListName = QInputDialog::getText(this, "New list", "Chose a name for the new list");
    CreateList(newListName);
}

void AnimeHub::on_deleteListButton_clicked() {
    QMessageBox msgBox;
    msgBox.setText("Do you really want to delete the current list ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    auto answer = msgBox.exec();
    if (answer == QMessageBox::Yes)
        DeleteList(ui->listsComboBox->currentText());
}

// UI related methods

void AnimeHub::RefreshAnimeListUI() {
    while (!animeListPreviewListUIs.isEmpty()) {
        AnimePreviewUI* animePreviewUI = animeListPreviewListUIs.takeLast();
        ui->animeListLayout->removeWidget(animePreviewUI);
        delete animePreviewUI;
    }

    for (int i = 0; i < lists[selectedList].length(); ++i) {
        AnimePreviewUI* animePreviewUI = new AnimePreviewUI(*(lists[selectedList][i]), this);
        animeListPreviewListUIs.push_back(animePreviewUI);
        ui->animeListLayout->addWidget(animePreviewUI);
        SetupAnimePreviewListContextMenu(animePreviewUI, i);
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
