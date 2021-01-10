#include "AnimePreviewUI.h"
#include "ui_AnimePreviewUI.h"
#include "Anime.h"
#include <QtNetwork>
#include <iostream>
#include "AnimeHub.h"
#include <QPalette>

AnimePreviewUI::AnimePreviewUI(Anime* anime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimePreviewUI)
{
    ui->setupUi(this);
    ui->titleLabel->setText(anime->GetTitle());
    ui->descriptionLabel->setText(anime->GetDescription());
    ui->coverImageLabel->setText(anime->GetCoverImageUrl());
    ApplyCoverImageByUrl(anime->GetCoverImageUrl());

    QString genreString;
    for (const QString& genreName : anime->GetGenres()) {
        if (!genreString.isEmpty())
            genreString += ", ";
        genreString += genreName;
    }

    ui->genresLabel->setText(genreString);

    ui->statusLabel->setText(anime->GetStatus());
    ui->episodesLabel->setText(QString::number(anime->GetEpisodes()) + " episode(s)");

    ui->datesLabel->setText(QString::number(anime->GetStartDay()) + "/" + QString::number(anime->GetStartMonth()) + "/" + QString::number(anime->GetStartYear()) + " - " + QString::number(anime->GetEndDay()) + "/" + QString::number(anime->GetEndMonth()) + "/" + QString::number(anime->GetEndYear()));
}

void AnimePreviewUI::ApplyCoverImageByUrl(const QString& coverImageUrl) {
    QNetworkRequest coverImageRequest;
    coverImageRequest.setUrl(QUrl(coverImageUrl));
    QNetworkReply* coverImageReply = AnimeHub::GetNetworkManager().get(coverImageRequest);
    connect(coverImageReply, &QNetworkReply::finished, this, [=] {
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
        ui->coverImageLabel->setPixmap(coverImagePixmap);
        setMaximumHeight(coverImagePixmap.height());
    });
}

AnimePreviewUI::~AnimePreviewUI()
{
    delete ui;
}
