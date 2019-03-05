#include "anticollisionarea.h"

AntiCollisionArea::AntiCollisionArea(ControlWidget *pControl, ShapeWidget *pShape)
:m_pControl(pControl),m_pShape(pShape)
{
    viewport()->setPalette(QPalette(Qt:: white));
    viewport()->setAutoFillBackground(true);
    setMinimumSize(800,800);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);  //控件大小 小于 视窗大小时，默认不会显示滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);    //强制显示滚动条
    m_nBarWidth=20;
    int width=this->width()-m_nBarWidth;
    int height=this->height()-m_nBarWidth;
    m_pPaintArea=new PaintArea(this,width,height);//Anti是scroll窗口在上面，paint是widget画图窗口在下面
    setWidget(m_pPaintArea);
}

AntiCollisionArea::~AntiCollisionArea()
{

}

void AntiCollisionArea::resizeEvent(QResizeEvent *)
{
    int width=this->width()-m_nBarWidth;
    int height=this->height()-m_nBarWidth;
    m_pPaintArea->resize(width,height);//scroll窗体改变的时候，paint窗体也要改变
    m_pPaintArea->m_pPan->SetCellCenter(width/2,height/2);//scroll窗体改变，以中心点为0点，中心的位置也在改变
}

void AntiCollisionArea::SetWheelScroll(int nW, int nH, QPoint ptOrg, QPoint ptNew)
{
    int nHMax=horizontalScrollBar()->maximum();
    int nVMax=verticalScrollBar()->maximum();
    int width=this->width()-m_nBarWidth;
    int height=this->height()-m_nBarWidth;
    int nHScroll=int(double(ptNew.x()-ptOrg.x())/double(nW-width)*nHMax);
    int nVScroll=int(double(ptNew.y()-ptOrg.y())/double(nH-height)*nVMax);
    SetScrollBarPos(nHScroll,nVScroll);
}

void AntiCollisionArea::SetMidBtnMove(int nMoveX, int nMoveY)
{
    int nHValue=horizontalScrollBar()->value()+nMoveX;
    int nVValue=verticalScrollBar()->value()+nMoveY;
    SetScrollBarPos(nHValue,nVValue);
}

void AntiCollisionArea::SetScrollBarPos(int nHScroll, int nVScroll)
{
    horizontalScrollBar()->valueChanged(nHScroll);
    horizontalScrollBar()->setSliderPosition(nHScroll);  //setValue()对水平滑动条的新位置进行设置
    verticalScrollBar()->valueChanged(nVScroll);
    verticalScrollBar()->setSliderPosition(nVScroll);    //setValue()对垂直滑动条的新位置进行设置
    horizontalScrollBar()->update();
    verticalScrollBar()->update();
}


