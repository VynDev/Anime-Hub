#ifndef ANIME_H
#define ANIME_H
#include <QString>
#include <QVector>

class Anime
{
public:
    Anime();

    const QString& GetTitle() const;
    const QString& GetDescription() const;
    const QString& GetSynopsis() const; // Alias for GetDescription()
    const QString& GetCoverImageUrl() const;
    const QVector<QString>& GetGenres() const;
    const QString& GetStatus() const;
    int GetEpisodes() const;
    int GetStartYear() const;
    int GetStartMonth() const;
    int GetStartDay() const;
    int GetEndYear() const;
    int GetEndMonth() const;
    int GetEndDay() const;

    void SetTitle(const QString& title);
    void SetDescription(const QString& description);
    void SetCoverImageByUrl(const QString& coverImageUrl);
    void SetGenres(const QVector<QString>& genres);
    void AddGenre(const QString& genreName);
    void SetStatus(const QString& status);
    void SetEpisodes(int episodes);
    void SetStartYear(int year);
    void SetStartMonth(int month);
    void SetStartDay(int day);
    void SetEndYear(int year);
    void SetEndMonth(int month);
    void SetEndDay(int day);

private:

    QString title;
    QString description;
    QString coverImageUrl;
    QVector<QString> genres;
    QString status;
    int episodes;

    int startYear, startMonth, startDay;
    int endYear, endMonth, endDay;
};

#endif // ANIME_H
