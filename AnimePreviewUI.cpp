#include "AnimePreviewUI.h"
#include "ui_AnimePreviewUI.h"

AnimePreviewUI::AnimePreviewUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnimePreviewUI)
{
    ui->setupUi(this);
}

AnimePreviewUI::~AnimePreviewUI()
{
    delete ui;
}

void AnimePreviewUI::SetTitle(const QString& title) {
    ui->titleLabel->setText(title);
}

void AnimePreviewUI::SetDescription(const QString& description) {
    ui->descriptionLabel->setText(description);
}

void AnimePreviewUI::SetCoverImageUrl(const QString& coverImageUrl) {
    ui->coverImageLabel->setText(coverImageUrl);
}

void AnimePreviewUI::SetCoverImagePixmap(const QPixmap& coverImagePixmap) {
    ui->coverImageLabel->setPixmap(coverImagePixmap);
}
