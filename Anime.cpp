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
