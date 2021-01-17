#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>

class Settings
{
public:

public:
    Settings();

    QString GetPath() const;
    bool ShouldFetchImages() const;
    bool ShouldCache() const;
    bool ShouldUseSystemTheme () const;

    void SetPath(const QString& path);
    void EnableImageFetching(bool state);
    void EnableCache(bool state);
    void UseSystemTheme(bool state);

    void Save();
    void Reload();

private:

    QString settingsPath;

    QString path;
    bool bShouldFetchImage = true;
    bool bShouldCache = true;
    bool bShouldUseSystemTheme = false;

};

#endif // SETTINGS_H
