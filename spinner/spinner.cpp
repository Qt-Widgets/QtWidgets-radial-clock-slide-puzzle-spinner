#include "spinner.h"
#include "ui_spinner.h"

Spinner::Spinner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Spinner)
{
    ui->setupUi(this);
}

Spinner::~Spinner()
{
    delete ui;
}
