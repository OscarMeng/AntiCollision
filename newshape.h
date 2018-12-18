#ifndef NEWSHAPE_H
#define NEWSHAPE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "newpan.h"
#include "paintarea.h"
#include "axiswidget.h"

class NewPan;
class PaintArea;
class AxisWidget;
class NewShape : public QWidget
{
    Q_OBJECT
public:
    explicit NewShape(PaintArea *pPaintArea, NewPan* pNewPan);
private:
    virtual ~NewShape();
public:
    PaintArea   *m_pPaintArea;
    NewPan      *m_pNewPan;
    QLabel      *m_pCellLabel;
    QLineEdit   *m_pCellIDEdit;
    QPushButton *m_pCenBtn;
    QPushButton *m_pEccBtn;
    AxisWidget  *m_pAxis;
    QTextEdit   *m_pTextEdit;
    QHBoxLayout *m_pCellLayout;
    QGridLayout *m_pMainLayout;
public:
    QString   m_sCen;
    QString   m_sEcc;
    QString   m_sID;
    bool      m_bCenBtn;//默认中心轴数据
public:

signals:
    void    SendValue(QString sID,bool bCen,QString sData);
public slots:
    void    ReceiveCenShape(QString sID,QString sData);
    void    ReceiveEccShape(QString sID,QString sData);
    void    PressCenBtn();
    void    PressEccBtn();
};

#endif // NEWSHAPE_H
