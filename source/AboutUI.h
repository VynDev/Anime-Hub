#ifndef ABOUTUI_H
#define ABOUTUI_H

#include <QMainWindow>

namespace Ui {
class AboutUI;
}

class AboutUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit AboutUI(QWidget *parent = nullptr);
    ~AboutUI();

private:
    Ui::AboutUI *ui;
};

#endif // ABOUTUI_H
