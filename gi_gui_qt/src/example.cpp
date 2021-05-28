#include <example.hpp>
#include <QMessageBox>

ExampleWidget::ExampleWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);

    QObject::connect(pushButton, &QPushButton::released, this, [this]() {
        QMessageBox::information(this, "Released", "Pushbutton released", QMessageBox::StandardButton::Ok);
        });

    QObject::connect(pushButton, &QPushButton::released, this, &ExampleWidget::onButtonReleased);
    QObject::connect(pushButton, &QPushButton::released, this, &ExampleWidget::buttonReleased); // signal redirection
}

void ExampleWidget::onButtonReleased()
{
    emit buttonReleased(/* additional arguments if signal has them */);
}