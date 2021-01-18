#ifndef ANILISTIMPORT_H
#define ANILISTIMPORT_H

#include <QMainWindow>

namespace JSON {
	class Object;
}

namespace Ui {
class AniListImport;
}

class AniListImport : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit AniListImport(QWidget *parent = nullptr);
	~AniListImport();
	
	void ImportFromAnilist(const QString& userName, const QString& anilistListName, const QString& targetListName);
	
private slots:
	void on_findUserButton_clicked();
	void on_importButton_clicked();
	
private:
	Ui::AniListImport *ui;
	
	JSON::Object *json;
};

#endif // ANILISTIMPORT_H
