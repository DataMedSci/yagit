#include <qdicomfileselectionedit.hpp>

#include <ui_DICOMFileSelectionEdit.h>

Ui::DICOMFileSelectionEdit* d(void* _d)
{
    return static_cast<Ui::DICOMFileSelectionEdit*>(_d);
}

QDICOMFileSelectionEdit::QDICOMFileSelectionEdit(QWidget* parent)
    : QWidget(parent)
    , _d(new Ui::DICOMFileSelectionEdit())
{
    d(_d)->setupUi(this);
}

QDICOMFileSelectionEdit::~QDICOMFileSelectionEdit()
{
    delete d(_d);
}

void QDICOMFileSelectionEdit::onFilePathEditingFinished()
{
    emit filePathChanged(std::filesystem::path(d(_d)->fileSelectionLineEdit->text().toStdString()));
}


