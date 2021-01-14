#include "Anime.h"

// All methods in this class are self explanatory and simple.

Anime::Anime(const QString& title) : title(title) {

}

const QString& Anime::GetTitle() const {
    return title;
}

const QString& Anime::GetDescription() const {
    return description;
}

// Alias for GetDescription()
const QString& Anime::GetSynopsis() const {
    return GetDescription();
}

const QString& Anime::GetCoverImageUrl() const {
    return coverImageUrl;
}

const QVector<QString>& Anime::GetGenres() const {
    return genres;
}

int Anime::GetEpisodes() const {
    return episodes;
}

const QString& Anime::GetStatus() const {
    return status;
}

int Anime::GetStartYear() const {
    return startYear;
}

int Anime::GetStartMonth() const {
    return startMonth;
}

int Anime::GetStartDay() const {
    return startDay;
}

int Anime::GetEndYear() const {
    return endYear;
}

int Anime::GetEndMonth() const {
    return endMonth;
}

int Anime::GetEndDay() const {
    return endDay;
}

void Anime::SetTitle(const QString& title) {
    this->title = title;
}

void Anime::SetDescription(const QString& description) {
    this->description = description;
}

void Anime::SetCoverImageByUrl(const QString& coverImageUrl) {
    this->coverImageUrl = coverImageUrl;
}

void Anime::AddGenre(const QString& genreName) {
    genres.push_back(genreName);
}

void Anime::SetStatus(const QString &status) {
    this->status = status;
}

void Anime::SetEpisodes(int episodes) {
    this->episodes = episodes;
}

void Anime::SetStartYear(int year) {
    this->startYear = year;
}

void Anime::SetStartMonth(int month) {
    this->startMonth = month;
}

void Anime::SetStartDay(int day) {
    this->startDay = day;
}

void Anime::SetEndYear(int year) {
    this->endYear = year;
}

void Anime::SetEndMonth(int month) {
    this->endMonth = month;
}

void Anime::SetEndDay(int day) {
    this->endDay = day;
}
