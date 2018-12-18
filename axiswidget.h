#ifndef AXISWIDGET_H
#define AXISWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QStringList>
#include "mymethod.h"
#include "shapewidget.h"

class ShapeWidget;
class AxisWidget: public QWidget
{
    Q_OBJECT
public:
    AxisWidget(ShapeWidget *pShape);
public:
    virtual ~AxisWidget();
public:
    void paintEvent(QPaintEvent *e);
public:
    int    m_nWidth;         //几何内界面宽度
    int    m_nHeight;        //几何内界面高度
    int    m_nOffset;        //偏移量
public:
    QPainter    *m_pPainter;
    ShapeWidget *m_pShape;
public:
    QPoint       m_ptYStart;//出现问题不能用,问题是编译文件有问题,重新删除编译
    QPoint       m_ptYEnd;
    QPoint       m_ptXStart;
    QPoint       m_ptXEnd;
    QString      m_sTitle;
    QString      m_sID;
    bool         m_bCen;
    QString      m_sData;
public:
    void    DrawArrow();
    void    DrawAxisText();
    void    DrawAmplitude();
public slots:
    void    ReceiveValue(QString sID,bool bCen,QString sData);
};

#endif // AXISWIDGET_H
