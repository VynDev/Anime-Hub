#include "Anime.h"

Anime::Anime(const QString& title) : title(title)
{

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

void Anime::SetTitle(const QString& title) {
    this->title = title;
}

void Anime::SetDescription(const QString& description) {
    this->description = description;
}

void Anime::SetCoverImageByUrl(const QString& coverImageUrl) {
    this->coverImageUrl = coverImageUrl;
}
