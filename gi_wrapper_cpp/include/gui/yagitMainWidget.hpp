#pragma once

#include <array>

#include <QWidget>
#include <QDoubleSpinBox>

class YAGITMainWidget final : public QWidget
{
    Q_OBJECT
private:
    void* _d;

    QList<QDoubleSpinBox*> _precisionDependentSpinBoxes;
public:
    enum Dimension
    {
        d1D,        // 1D
        d2D,        // 2D
        d3D,        // 3D
        d2_5D,      // 2.5D
        d2Df3D,     // 2D from 3D
    };

    enum Plane
    {
        None,
        XY,
        XZ,
        YZ,
        YX = XY,
        ZX = XZ,
        ZY = YZ,
    };

    enum SaveMode
    {
        AllTags,
        AnyTag,
        AllExceptPrivate,
    };
public:
    explicit YAGITMainWidget(QWidget* parent = nullptr);
public:
    virtual ~YAGITMainWidget() override;
public:
    qreal distanceToAgreement() const;
    uint precision() const;
    Dimension dimension() const;
    Plane plane() const;
    SaveMode saveMode() const;
private slots:
    // GENERAL
    //void distanceToAgreementChanged(double newDistanceToAgreement);
    //void doseDifferenceChanged(double newDoseDifference);
    //void referenceValueChanged(double newReferenceValue);
    //void initialGammaIndexValueChanged(double newInitialGammaIndexValue);
    //void referenceScaleChanged(double newScale);
    //void referenceBiasChanged(double newBias);
    //void targetScaleChanged(double newScale);
    //void targetBiasChanged(double newBias);
    // DIMENSIONS
    void dimensionItemIndexChanged(int newItemIndex);
    //void planeItemIndexChanged(int newItemIndex);
    //void referenceSliceValueChanged(int newSliceIndex);
    //void targetSliceValueChanged(int newSliceIndex);
    // MISCELLANEOUS
    //void algorithmVersionItemIndexChanged(int newItemIndex);
    void precisionValueChanged(int newPrecision);
    //void filterChanged(QString newValue);
    //void saveModeItemIndexChanged(int newItemIndex);
};
