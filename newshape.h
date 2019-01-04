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
#include "newpaint.h"
#include "axiswidget.h"

class NewPan;
class NewPaint;
class AxisWidget;
class NewShape : public QWidget
{
    Q_OBJECT
public:
    explicit NewShape(NewPaint *pPaint, NewPan* pPan);
private:
    virtual ~NewShape();
public:
    NewPaint    *m_pNewPaint;
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
