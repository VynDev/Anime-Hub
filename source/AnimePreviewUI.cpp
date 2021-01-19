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
AnimePreviewUI::AnimePreviewUI(QWidget *parent) : QWidget(parent),
    ui(new Ui::AnimePreviewUI)
{
    ui->setupUi(this);

}

AnimePreviewUI::AnimePreviewUI(const Anime& anime, QWidget *parent) : QWidget(parent),
    ui(new Ui::AnimePreviewUI)
{
    ui->setupUi(this);

    SetTitle(anime.GetTitle());
    SetDescription(anime.GetDescription());
    SetCoverImageByUrl(anime.GetCoverImageUrl());
    SetGenres(anime.GetGenres());
    SetEpisodes(anime.GetEpisodes());
    SetStartDay(anime.GetStartDay());
    SetStartMonth(anime.GetStartMonth());
    SetStartDay(anime.GetStartDay());
    SetEndDay(anime.GetStartDay());
    SetEndMonth(anime.GetEndMonth());
    SetEndDay(anime.GetEndDay());
    SetStatus(anime.GetStatus());
    Refresh();
}

AnimePreviewUI::~AnimePreviewUI() {
    delete ui;
}

void AnimePreviewUI::Refresh() {
    ui->titleLabel->setText(GetTitle());
    ui->descriptionLabel->setText(GetDescription());
    ui->coverImageLabel->setText(GetCoverImageUrl());
	if (GetSettings().ShouldFetchImages())
		ApplyCoverImageByUrl(GetCoverImageUrl());

    QString genreString;
    for (const QString& genreName : GetGenres()) {
        if (!genreString.isEmpty())
            genreString += ", ";
        genreString += genreName;
    }

    ui->genresLabel->setText(genreString);

    ui->statusLabel->setText(GetStatus());
    ui->episodesLabel->setText(QString::number(GetEpisodes()) + " episode(s)");

    ui->datesLabel->setText(DateNumber(GetStartDay()) + "/" + DateNumber(GetStartMonth()) + "/" + DateNumber(GetStartYear()) + " - " + DateNumber(GetEndDay()) + "/" + DateNumber(GetEndMonth()) + "/" + DateNumber(GetEndYear()));
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
