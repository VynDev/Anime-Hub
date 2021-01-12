#include "Settings.h"
#include <iostream>
#include <QFile>
#include "json-parser/JSON.h"

Settings::Settings(const QString& settingsPath) : settingsPath(settingsPath)
{
    Reload();
}

void Settings::Save() {
    JSON::Object settingsJson;

    settingsJson["path"] = path.toStdString();
    settingsJson["fetchImages"] = bShouldFetchImage;
    settingsJson["noCache"] = !bShouldCache;
    settingsJson["theme"] = theme.toStdString();

    settingsJson.Save(settingsPath.toStdString());
}

void Settings::Reload() {
    JSON::Object settingsJson(settingsPath.toStdString());

    if (!settingsJson.IsValid()) {
        std::cout << "Couldn't open settings.json" << std::endl;
        return;
    }

    path = QString::fromStdString(settingsJson["path"].AsString());
    bShouldFetchImage = settingsJson["fetchImages"].AsBool();
    bShouldCache = !settingsJson["noCache"].AsBool();
    theme = QString::fromStdString(settingsJson["theme"].AsString());
}

QString Settings::GetPath() const {
    return path;
}

bool Settings::ShouldFetchImages() const {
    return bShouldFetchImage;
}

bool Settings::ShouldCache() const {
    return bShouldCache;
}

QString Settings::GetTheme() const {
    return theme;
}


void Settings::SetPath(const QString& path) {
    this->path = path;
}

void Settings::EnableImageFetching(bool state) {
    this->bShouldFetchImage = state;
}

void Settings::EnableCache(bool state) {
    this->bShouldCache = state;
}

void Settings::SetTheme(const QString& theme) {
    this->theme = theme;
}