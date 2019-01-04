#ifndef ANTICOLLISIONAREA_H
#define ANTICOLLISIONAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include "paintarea.h"
#include "controlwidget.h"
#include "shapewidget.h"

class PaintArea;
class ControlWidget;
class ShapeWidget;
class AntiCollisionArea : public QScrollArea
{
    Q_OBJECT
public:
    AntiCollisionArea(ControlWidget *pControl,ShapeWidget *pShape);
private:
    virtual ~AntiCollisionArea();
private:
    void resizeEvent(QResizeEvent *);
public:
    void SetWheelScroll(int nW, int nH, QPoint ptOrg, QPoint ptNew);
    void SetMidBtnMove(int nMoveX,int nMoveY);
    void SetScrollBarPos(int nW, int nVScroll);
public:

public:
    PaintArea     *m_pPaintArea;
    ControlWidget *m_pControl;
    ShapeWidget   *m_pShape;
    int            m_nBarWidth;//滚动条宽度
};

#endif // ANTICOLLISONAREA_H
