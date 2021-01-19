#include "AniListImportUI.h"
#include "ui_AniListImportUI.h"
#include "json-parser/JSON.h"
#include "AnimeHub.h"
#include <QtNetwork>
#include <QByteArray>
#include <QMessageBox>

AniListImportUI::AniListImportUI(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::AniListImportUI)
{
	ui->setupUi(this);
	
}

AniListImportUI::~AniListImportUI()
{
	delete ui;
}

void AniListImportUI::on_findUserButton_clicked()
{
	if (ui->userNameLineEdit->text().isEmpty())
		return ;
	std::cout << "Fetching user" << std::endl;
	aniList.FetchUserLists(ui->userNameLineEdit->text(), [&] (QMap<QString, QVector<Anime>> lists) {
		std::cout << "Received lists" << std::endl;
		for (const QString& listName : lists.keys()) {
			ui->aniListListsComboBox->addItem(listName);
		}
		this->lists = std::move(lists);
		ui->userNameLineEdit->setEnabled(false);
		ui->aniListListsComboBox->setEnabled(true);
		ui->newListLineEdit->setEnabled(true);
		ui->importButton->setEnabled(true);
	}, [&]{
		QMessageBox::warning(this, "Error", "Couldn't fetch animes");
	});
}

void AniListImportUI::on_importButton_clicked()
{
	if (ui->newListLineEdit->text().isEmpty())
		return ;
	ui->aniListListsComboBox->setEnabled(false);
	ui->newListLineEdit->setEnabled(false);
	ui->importButton->setEnabled(false);
	ImportFromAnilist(ui->aniListListsComboBox->currentText(), ui->newListLineEdit->text());
	close();
}

void AniListImportUI::ImportFromAnilist(const QString& anilistListName, const QString& targetListName) {
	std::cout << "Importing '" << anilistListName.toStdString() << "' from Anilist to '" << targetListName.toStdString() << "'" << std::endl;
	
	AnimeHub* animeHub = static_cast<AnimeHub*>(parent());
	
	if (!animeHub->HasList(targetListName))
	{
		animeHub->CreateList(targetListName);
		animeHub->RefreshListsUI();
	}
	
	QVector<Anime>& list = lists[anilistListName];
	
	for (const Anime& anime : list) {
		animeHub->AddAnimeToList(targetListName, anime);
	}
	animeHub->RefreshAnimeListUI();
	animeHub->Save();
}

void AniListImportUI::closeEvent(QCloseEvent* closeEvent) {
	deleteLater();
}
