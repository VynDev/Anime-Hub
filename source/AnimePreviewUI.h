#ifndef ANIMEPREVIEWUI_H
#define ANIMEPREVIEWUI_H

#include <QWidget>
#include <QNetworkReply>
#include "Anime.h"

namespace Ui {
class AnimePreviewUI;
}

class Anime;

class AnimePreviewUI : public QWidget, public Anime
{
    Q_OBJECT

public:
    AnimePreviewUI(QWidget *parent = nullptr);
    AnimePreviewUI(const Anime& anime, QWidget *parent = nullptr);
    ~AnimePreviewUI();

    void Refresh();

    void paintEvent(QPaintEvent *);

private:
    Ui::AnimePreviewUI *ui;

    void ApplyCoverImageByUrl(const QString& coverImageUrl);
    void ApplyCoverImageByImage(const QString& coverImagePath); // [Todo]
};

#endif // ANIMEPREVIEWUI_H
