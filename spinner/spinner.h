#ifndef SPINNER_H
#define SPINNER_H

#include <QWidget>

namespace Ui {
class Spinner;
}

class Spinner : public QWidget
{
    Q_OBJECT

public:
    explicit Spinner(QWidget *parent = 0);
    ~Spinner();

private:
    Ui::Spinner *ui;
};

#endif // SPINNER_H
