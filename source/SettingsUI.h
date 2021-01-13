#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <QMainWindow>
#include "Settings.h"

namespace Ui {
class SettingsUI;
}

class SettingsUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsUI(Settings *settings, QWidget *parent = nullptr);
    ~SettingsUI();

private slots:
    void on_saveButton_clicked();

    void on_systemThemeCheckBox_stateChanged(int arg1);

private:
    Ui::SettingsUI *ui;

    Settings *settings;
};

#endif // SETTINGSUI_H
