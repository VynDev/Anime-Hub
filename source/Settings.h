#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>

class Settings
{
public:

public:
    Settings(const QString& settingsPath);

    QString GetPath() const;
    bool ShouldFetchImages() const;
    bool ShouldCache() const;
    QString GetTheme() const;

    void SetPath(const QString& path);
    void EnableImageFetching(bool state);
    void EnableCache(bool state);
    void SetTheme(const QString& theme);

    void Save();
    void Reload();

private:

    QString settingsPath;

    QString path;
    bool bShouldFetchImage;
    bool bShouldCache;
    QString theme;

};

#endif // SETTINGS_H
