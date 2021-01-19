#ifndef ANILISTIMPORTUI_H
#define ANILISTIMPORTUI_H

#include <QMainWindow>
#include "AniList.h"

namespace JSON {
	class Object;
}

namespace Ui {
class AniListImportUI;
}

class AniListImportUI : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit AniListImportUI(QWidget *parent = nullptr);
	~AniListImportUI();
	
	void ImportFromAnilist(const QString& anilistListName, const QString& targetListName);
	
	void closeEvent(QCloseEvent* closeEvent) override;
	
private slots:
	void on_findUserButton_clicked();
	void on_importButton_clicked();
	
private:
	Ui::AniListImportUI *ui;
	
	AniList aniList;
	QMap<QString, QVector<Anime>> lists;
};

#endif // ANILISTIMPORTUI_H
