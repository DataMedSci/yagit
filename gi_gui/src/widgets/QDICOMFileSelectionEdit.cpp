#include <array>

#include <QFileDialog>

#include <QDICOMFileSelectionEdit.hpp>

#include <ui_DICOMFileSelectionEdit.h>

const QStringList& selectionFilterAllowedExtensions()
{
    static const QStringList selectionFilterAllowedExtensions = {
            "DICOM (*.dcm *.DCM)",
            "IMA (*.ima *.IMA)",
            "All files (*.*)",
    };
    return selectionFilterAllowedExtensions;
}

const QString& selectionFilter()
{
    static const QString selectionFilter = selectionFilterAllowedExtensions().join(";;");
    return selectionFilter;
}

const QString& defaultSelectedSelectionFilter()
{
    return selectionFilterAllowedExtensions().front();
}

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

QGraphicsView* QDICOMFileSelectionEdit::previewView()
{
    return d(_d)->previewGraphicsView;
}

void QDICOMFileSelectionEdit::onFilePathEditingFinished()
{
    emit filePathChanged(std::filesystem::path(d(_d)->fileSelectionLineEdit->text().toStdString()));
}

void QDICOMFileSelectionEdit::onSelectionMenuRequested()
{
    static QString selectedSelectionFilter = defaultSelectedSelectionFilter(); // intentionally static

    QString selectedFile = QFileDialog::getOpenFileName(
        this,
        "Select DICOM/IMA file",
        QString(),
        selectionFilter(),
        &selectedSelectionFilter
    );

    if(!selectedFile.isNull()) // user cancelled
    {
        d(_d)->fileSelectionLineEdit->setText(selectedFile);
        emit filePathChanged(std::filesystem::path(selectedFile.toStdString()));
    }
}
