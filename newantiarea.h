#ifndef NEWANTIAREA_H
#define NEWANTIAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QResizeEvent>
#include "newpaint.h"
#include "newcontrol.h"
#include "newshape.h"

class NewPaint;
class NewControl;
class NewShape;
class NewAntiArea : public QScrollArea
{
    Q_OBJECT
public:
    NewAntiArea(NewControl *pControl,NewShape *pShape);
private:
    virtual ~NewAntiArea();
private:
    void resizeEvent(QResizeEvent *);
public:
    void SetWheelScroll(int nW, int nH, QPoint ptOrg, QPoint ptNew);
    void SetMidBtnMove(int nMoveX,int nMoveY);
    void SetScrollBarPos(int nHScroll, int nVScroll);
public:

public:
    NewPaint     *m_pNewPaint;
    NewControl   *m_pNewControl;
    NewShape     *m_pNewShape;
    int           m_nBarWidth;//滚动条宽度
};

#endif // NEWANTIAREA_H
