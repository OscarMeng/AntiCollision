#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStackedWidget>
#include <QTime>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include "anticollisionarea.h"
#include "controlwidget.h"
#include "shapewidget.h"
#include "pan.h"
#include "newantiarea.h"
#include "newcontrol.h"
#include "newshape.h"
#include "newpan.h"

class Pan;
class NewPan;
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();
public:
    bool    m_bUnitStyle;//单元类型，旧新单元
private:
    AntiCollisionArea* m_pAntiArea;
    ControlWidget*     m_pCtrlWidget;
    ShapeWidget*       m_pShapeWidget;
    NewAntiArea*       m_pNewAnti;
    NewControl*        m_pNewControl;
    NewShape*          m_pNewShape;
    QMenu*             m_pCellType;
    QMenu*             m_pCellEdit;
    QAction*           m_pOldAct;
    QAction*           m_pNewAct;
    QAction*           m_pPosAct;
    QMenuBar*          m_pMenuBar;
    QStackedWidget*    m_pAntiStack;
    QStackedWidget*    m_pControlStack;
    QStackedWidget*    m_pShapeStack;
signals:

public slots:
    void SetOldUnit();
    void SetNewUnit();
    void SetUnitPos();//随机撒点

};

#endif // MAINWIDGET_H
