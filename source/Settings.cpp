#include "Settings.h"
#include <iostream>
#include <QFile>
#include "json-parser/JSON.h"
#include <QDir>

// [Todo] Remove settingsPath arg
Settings::Settings(const QString& settingsPath) : settingsPath(QDir::homePath() + "/.config/Anime-Hub/settings.json"), path(QDir::homePath() + "/.config/Anime-Hub/save.json") {
    QDir().mkpath(QDir::homePath() + "/.config/Anime-Hub");
    Reload();
}

void Settings::Save() {
    JSON::Object settingsJson;

    settingsJson["path"] = path.toStdString();
    settingsJson["fetchImages"] = bShouldFetchImage;
    settingsJson["noCache"] = !bShouldCache;
    settingsJson["useSystemTheme"] = bShouldUseSystemTheme;

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
    bShouldUseSystemTheme = settingsJson["useSystemTheme"].AsBool();
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

bool Settings::ShouldUseSystemTheme() const {
    return bShouldUseSystemTheme;
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

void Settings::UseSystemTheme(bool state) {
    this->bShouldUseSystemTheme = state;
}
