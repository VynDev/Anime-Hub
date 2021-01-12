#include "SettingsUI.h"
#include "ui_SettingsUI.h"


SettingsUI::SettingsUI(Settings *settings, QWidget *parent) :
    QMainWindow(parent), settings(settings),
    ui(new Ui::SettingsUI)
{
    ui->setupUi(this);

    ui->pathLineEdit->setText(settings->GetPath());
    ui->fetchImagesCheckBox->setChecked(settings->ShouldFetchImages());
    ui->noCacheCheckBox->setChecked(!settings->ShouldCache());
}

SettingsUI::~SettingsUI()
{
    delete ui;
}

void SettingsUI::on_saveButton_clicked()
{
    settings->SetPath(ui->pathLineEdit->text());
    settings->SetTheme(ui->themeComboBox->currentText());
    settings->EnableImageFetching(ui->fetchImagesCheckBox->isChecked());
    settings->EnableCache(!ui->noCacheCheckBox->isChecked());
    settings->Save();
    close();
}
