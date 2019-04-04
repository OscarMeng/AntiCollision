#ifndef SHAPEWIDGET_H
#define SHAPEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "pan.h"
#include "paintarea.h"
#include "axiswidget.h"

class Pan;
class PaintArea;
class AxisWidget;
class ShapeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShapeWidget(PaintArea *pPaintArea, Pan* pPan);
private:
    virtual ~ShapeWidget();
public:
    PaintArea   *m_pPaintArea;
    Pan         *m_pPan;
    QLabel      *m_pCellLabel;
    QLineEdit   *m_pCellIDEdit;
    QPushButton *m_pBtn;
    AxisWidget  *m_pAxis1;
    AxisWidget  *m_pAxis2;
    QHBoxLayout *m_pCellLayout;
    QGridLayout *m_pMainLayout;
public:
    bool    m_bCen;//中心轴
    bool    m_bEcc;//偏心轴

signals:

public slots:
    void    ReceiveShapeValue(QString sID,QString sCenData,QString sEccData);
    void    PressBtn();
};

#endif // SHAPEWIDGET_H
