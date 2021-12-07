#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <filesystem>
#include <memory>

class QDICOMFileSelectionEdit : public QWidget
{
    Q_OBJECT
   // Q_PROPERTY(QString filePromptText READ filePromptText WRITE setFilePromptText NOTIFY filePromptTextChanged)
   // Q_PROPERTY(std::filesystem::path filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
private:
    void* _d;
public:
    explicit QDICOMFileSelectionEdit(QWidget* parent = nullptr);
    //QDICOMFileSelectionEdit(QString filePromptText, QWidget* parent = nullptr);
public:
    virtual ~QDICOMFileSelectionEdit() override;
public:
    QGraphicsView* previewView();
signals:
    void filePathChanged(std::filesystem::path newFilePath);
private slots:
    void onFilePathEditingFinished();
    void onSelectionMenuRequested();
};
