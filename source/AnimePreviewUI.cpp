#include "AnimePreviewUI.h"
#include "ui_AnimePreviewUI.h"
#include "Anime.h"
#include <QtNetwork>
#include <iostream>
#include "AnimeHub.h"
#include <QPalette>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

// Function used to format number in a "date" format.
QString DateNumber(int number) {
    if (number < 0)
        return "?";
    return (number < 10 ? "0" : "") + QString::number(number);
}

// [Todo] Replace the Anime* anime to a reference, because we don't need to store it and it's misleading for the caller.
AnimePreviewUI::AnimePreviewUI(Anime* anime, QWidget *parent) : QWidget(parent),
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

    ui->datesLabel->setText(DateNumber(anime->GetStartDay()) + "/" + DateNumber(anime->GetStartMonth()) + "/" + DateNumber(anime->GetStartYear()) + " - " + DateNumber(anime->GetEndDay()) + "/" + DateNumber(anime->GetEndMonth()) + "/" + DateNumber(anime->GetEndYear()));
}

AnimePreviewUI::~AnimePreviewUI() {
    delete ui;
}

// This method directly fetch the image with the given url, so it may takes some times before it applies
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

// This method must exist for Qss to work.
void AnimePreviewUI::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
