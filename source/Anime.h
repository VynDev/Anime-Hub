#ifndef ANIME_H
#define ANIME_H
#include <QString>

class Anime
{
public:
    Anime(const QString& title);

    const QString& GetTitle() const;
    const QString& GetDescription() const;
    const QString& GetSynopsis() const; // Alias for GetDescription()
    const QString& GetCoverImageUrl() const;

    void SetTitle(const QString& title);
    void SetDescription(const QString& description);
    void SetCoverImageByUrl(const QString& coverImageUrl);

private:

    QString title;
    QString description;
    QString coverImageUrl;
};

#endif // ANIME_H
