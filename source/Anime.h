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

private:

    QString title;
    QString description;
};

#endif // ANIME_H
