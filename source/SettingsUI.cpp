#include "SettingsUI.h"
#include "ui_SettingsUI.h"
#include "AnimeHub.h"

SettingsUI::SettingsUI(Settings *settings, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsUI), settings(settings)
{
    ui->setupUi(this);

    ui->pathLineEdit->setText(settings->GetPath());
    ui->fetchImagesCheckBox->setChecked(settings->ShouldFetchImages());
    ui->noCacheCheckBox->setChecked(!settings->ShouldCache());
    ui->systemThemeCheckBox->setChecked(settings->ShouldUseSystemTheme());
}

SettingsUI::~SettingsUI()
{
    delete ui;
}

void SettingsUI::on_saveButton_clicked()
{
    settings->SetPath(ui->pathLineEdit->text());
    settings->UseSystemTheme(ui->systemThemeCheckBox->isChecked());
    settings->EnableImageFetching(ui->fetchImagesCheckBox->isChecked());
    settings->EnableCache(!ui->noCacheCheckBox->isChecked());
    settings->Save();
    close();
}

void SettingsUI::on_systemThemeCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked) {
        static_cast<AnimeHub *>(parent())->ResetStyle();
    }
    else {
        static_cast<AnimeHub *>(parent())->LoadStyle(":/styles/vyn-dark.qss");
    }
}
