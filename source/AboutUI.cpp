#include "AboutUI.h"
#include "ui_AboutUI.h"

AboutUI::AboutUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutUI)
{
    ui->setupUi(this);
}

AboutUI::~AboutUI()
{
    delete ui;
}
