#include <QWidget>
#include <ui_example.h>

class ExampleWidget : public QWidget, private Ui::ExampleWidget
{
    Q_OBJECT
public:
    explicit ExampleWidget(QWidget* parent = nullptr);
signals:
    void buttonReleased();
private slots:
    void onButtonReleased();
};
