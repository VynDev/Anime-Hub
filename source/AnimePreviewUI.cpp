#include "AnimePreviewUI.h"
#include "ui_AnimePreviewUI.h"
#include "Anime.h"
#include <QtNetwork>
#include <iostream>
#include "AnimeHub.h"

AnimePreviewUI::AnimePreviewUI(Anime* anime, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimePreviewUI)
{
    ui->setupUi(this);
    ui->titleLabel->setText(anime->GetTitle());
    ui->descriptionLabel->setText(anime->GetDescription());
    ui->coverImageLabel->setText(anime->GetCoverImageUrl());
    ApplyCoverImageByUrl(anime->GetCoverImageUrl());
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
