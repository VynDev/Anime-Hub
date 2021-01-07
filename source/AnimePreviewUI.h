#ifndef ANIMEPREVIEWUI_H
#define ANIMEPREVIEWUI_H

#include <QWidget>

namespace Ui {
class AnimePreviewUI;
}

class AnimePreviewUI : public QWidget
{
    Q_OBJECT

public:
    explicit AnimePreviewUI(QWidget *parent = nullptr);
    ~AnimePreviewUI();

    void SetTitle(const QString& name);
    void SetDescription(const QString& description);
    void SetCoverImageUrl(const QString& coverImageUrl);
    void SetCoverImagePixmap(const QPixmap& coverImagePixmap);

private:
    Ui::AnimePreviewUI *ui;
};

#endif // ANIMEPREVIEWUI_H
