#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
//如果设置了固定尺寸，各布局窗口大小的最小值要适合固定尺寸
//    setFixedSize(1300,900);
    setWindowTitle(tr("LAMOST碰撞处理"));
    m_bUnitStyle=true;
    m_pAntiArea=new AntiCollisionArea(m_pCtrlWidget,m_pShapeWidget);
    m_pCtrlWidget=new ControlWidget(m_pAntiArea->m_pPaintArea->m_pPan);
    m_pShapeWidget=new ShapeWidget(m_pAntiArea->m_pPaintArea,m_pAntiArea->m_pPaintArea->m_pPan);

    m_pNewAnti=new NewAntiArea(m_pNewControl,m_pNewShape);
    m_pNewControl=new NewControl(m_pNewAnti->m_pNewPaint->m_pNewPan);
    m_pNewShape=new NewShape(m_pNewAnti->m_pNewPaint,m_pNewAnti->m_pNewPaint->m_pNewPan);

    m_pAntiStack=new QStackedWidget(this);
    m_pAntiStack->setFrameStyle(QFrame::Panel|QFrame::Raised);
    m_pAntiStack->addWidget(m_pNewAnti);
    m_pAntiStack->addWidget(m_pAntiArea);
    m_pControlStack=new QStackedWidget(this);
    m_pControlStack->setFrameStyle(QFrame::Panel|QFrame::Raised);
    m_pControlStack->addWidget(m_pNewControl);
    m_pControlStack->addWidget(m_pCtrlWidget);
    m_pShapeStack=new QStackedWidget(this);
    m_pShapeStack->setFrameStyle(QFrame::Panel|QFrame::Raised);
    m_pShapeStack->addWidget(m_pNewShape);
    m_pShapeStack->addWidget(m_pShapeWidget);

    m_pCellType = new QMenu(tr("单元类型"));
    m_pOldAct = new QAction(tr("25.6mm单元"),this);
    m_pOldAct->setShortcut(Qt::CTRL | Qt::Key_O );
    m_pOldAct->setStatusTip(tr("单元间距25.6"));
    m_pNewAct=new QAction(tr("45mm单元"),this);
    m_pNewAct->setShortcut(tr("Ctrl+N"));
    m_pNewAct->setStatusTip(tr("单元间距45"));
    m_pCellType->addAction(m_pOldAct);
    m_pCellType->addAction(m_pNewAct);
    connect(m_pOldAct,SIGNAL(triggered()),this,SLOT(SetOldUnit()));
    connect(m_pNewAct,SIGNAL(triggered()),this,SLOT(SetNewUnit()));
    m_pCellEdit=new QMenu(tr("编辑"));
    m_pPosAct=new QAction(tr("随机撒点"));
    m_pCellEdit->addAction(m_pPosAct);
    connect(m_pPosAct,SIGNAL(triggered(bool)),this,SLOT(SetUnitPos()));
    m_pMenuBar = new QMenuBar(this);
    m_pMenuBar->addMenu(m_pCellType);
    m_pMenuBar->addMenu(m_pCellEdit);
    m_pMenuBar->setStyleSheet("QMenuBar{background-color: rgb(230,230,230);"
                              "border: 1px solid white inset;}");
    //整体的布局
    QGridLayout *mainLayout=new QGridLayout(this);
    mainLayout->setMargin(2);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(m_pMenuBar,0,0,1,1);
    mainLayout->addWidget(m_pAntiStack,1,0,2,1);
    mainLayout->addWidget(m_pControlStack,0,1,2,1);
    mainLayout->addWidget(m_pShapeStack,2,1,1,1);
    mainLayout->setRowStretch(1,1);
    mainLayout->setRowStretch(2,1);
    mainLayout->setColumnStretch(0,3);
    mainLayout->setColumnStretch(1,2);
}

MainWidget::~MainWidget()
{

}

void MainWidget::SetOldUnit()
{
    setWindowTitle(tr("LAMOST碰撞处理"));
    m_bUnitStyle=true;
    m_pAntiStack->setCurrentIndex(0);
    m_pControlStack->setCurrentIndex(0);
    m_pShapeStack->setCurrentIndex(0);
}

void MainWidget::SetNewUnit()
{
    setWindowTitle(tr("LAMOST新单元碰撞处理"));
    m_bUnitStyle=false;
    m_pAntiStack->setCurrentIndex(1);
    m_pControlStack->setCurrentIndex(1);
    m_pShapeStack->setCurrentIndex(1);
}

void MainWidget::SetUnitPos()
{
    QTime t=QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);

    if(m_bUnitStyle)
    {
        QDateTime time=QDateTime::currentDateTime();
        QString st=time.toString("yyyyMMddhhmmss");
        QFile fp("/QAntiCollision/File/OldUnit/RandomPoint/"+st+".txt");
        int nCell[]={
                     15,16,17,18,19,20,21,22,
                     26,27,28,29,30,31,32,33,34,
                     40,41,42,43,44,45,46,47,48,
                     54,55,56,57,58,59,60,61,62,
//                     8,9,10,
//                     15,16,17,18,19,20,21,22,
//                     26,27,28,29,30,31,32,33,34,
//                     38,39,40,41,42,43,44,45,46,47,48,49,
//                     52,53,54,55,56,57,58,59,60,61,62,63,64,
//                     67,68,69,70,71,72,73,74,75,76,77,78,79,80
                    };
        if(fp.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QTextStream output(&fp);
            QString str;
            str.sprintf("%-8s%-15s%-10s","CellID","Radius","Angle");
            output<<str<<endl;
            for(int i=0;i<int(sizeof(nCell)/sizeof(nCell[0]));i++)
            {
                int nStart=1600;
                double dR=double(nStart+rand()%(4500-nStart))/100;            //展开后单元中心到光纤点距离半径0~17mm
                double dA=double(rand()%36000)/100;                           //运行到的度数0~360°
                str.sprintf("%-8d%-15f%-10f",nCell[i],dR,dA);
                output<<str<<endl;
            }
        }
        fp.close();
    }
}
