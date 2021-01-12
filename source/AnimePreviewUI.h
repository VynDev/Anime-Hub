#ifndef ANIMEPREVIEWUI_H
#define ANIMEPREVIEWUI_H

#include <QWidget>
#include <QNetworkReply>

namespace Ui {
class AnimePreviewUI;
}

class Anime;

class AnimePreviewUI : public QWidget
{
    Q_OBJECT

public:
    AnimePreviewUI(Anime* anime, QWidget *parent = nullptr);
    ~AnimePreviewUI();

    void paintEvent(QPaintEvent *);

private:
    Ui::AnimePreviewUI *ui;

    void ApplyCoverImageByUrl(const QString& coverImageUrl);
};

#endif // ANIMEPREVIEWUI_H
