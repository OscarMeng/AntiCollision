#include "axiswidget.h"

AxisWidget::AxisWidget(ShapeWidget *pShape):m_pShape(pShape)
{
    QPalette bgpal = palette();
    bgpal.setColor(QPalette::Background, QColor (170, 230 , 200));
    setPalette(bgpal);
    setAutoFillBackground(true);
    setMinimumSize(400,100);
    m_ptYStart=QPoint(0,0);
    m_ptYEnd=QPoint(0,0);
    m_ptXStart=QPoint(0,0);
    m_ptXEnd=QPoint(0,0);
    m_sID=1;
    m_bCenEcc=true;
    m_sData="";
    m_sTitle="";

}

AxisWidget::~AxisWidget()
{
    delete m_pPainter;
}

void AxisWidget::paintEvent(QPaintEvent *e)
{
    //窗口尺寸在show之后才是实际尺寸
    m_nWidth=this->width();
    m_nHeight=this->height();
    m_nOffset=30;
    m_pPainter=new QPainter(this);
    m_pPainter->setRenderHint(QPainter::Antialiasing,true);//反走样
    m_pPainter->begin(this);
    m_ptYStart=QPoint(m_nOffset,10);
    m_ptYEnd=QPoint(m_nOffset,m_nHeight-30);
    m_ptXStart=QPoint(m_nOffset,(m_nHeight-20)/2);
    m_ptXEnd=QPoint(m_nWidth-10,(m_nHeight-20)/2);

    QPen penAxis(QColor(65, 70, 80), 1.2, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    m_pPainter->setPen(penAxis);
    //坐标轴
    m_pPainter->drawLine(m_ptXStart,m_ptXEnd);
    m_pPainter->drawLine(m_ptYStart,m_ptYEnd);
    //箭头
    DrawArrow();
    //xy轴标语
    DrawAxisText();
    //画Y轴幅值
    DrawAmplitude();
    m_pPainter->end();
}

void AxisWidget::DrawAxisText()
{
    QFont font;
    font.setFamily("黑体");
    font.setPixelSize(50);
    font.setPointSize(10);
    m_pPainter->setFont(font);

    m_pPainter->rotate(-90);//以(0,0)点坐标原点选择，最后右下角坐标显示,x,y轴为旋转之后的
    m_pPainter->drawText(-m_nHeight/2-15,m_nOffset/2,tr("Y轴幅值"));
    m_pPainter->rotate(90);
    m_pPainter->drawText(m_ptXEnd.x()-50,m_ptXEnd.y()+40,tr("X轴位置"));
    m_pPainter->drawText(m_ptXStart.x()-10,m_ptXStart.y()+5,tr("0"));
    m_pPainter->drawText(m_ptXStart.x()-10,m_ptXStart.y()-m_nHeight/4+5,tr("1"));
    m_pPainter->drawText(m_ptXStart.x()-14,m_ptXStart.y()+m_nHeight/4+5,tr("-1"));
    m_pPainter->drawLine(m_ptXStart.x(),m_ptXStart.y()-m_nHeight/4,
                         m_ptXStart.x()+5,m_ptXStart.y()-m_nHeight/4);
    m_pPainter->drawLine(m_ptXStart.x(),m_ptXStart.y()+m_nHeight/4,
                         m_ptXStart.x()+5,m_ptXStart.y()+m_nHeight/4);
    if(m_bCenEcc)
        m_sTitle="中心轴";
    else
        m_sTitle="偏心轴";
    m_pPainter->drawText(m_nWidth/2-20,m_nHeight-10,m_sTitle);
}

void AxisWidget::DrawAmplitude()
{
    if(!m_sData.isEmpty())
    {
        QPen penAmp(QColor(255, 0, 0), 1.4, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
        QPen penDash(QColor(100, 0, 0), 0.5, Qt::DashLine,Qt::SquareCap,Qt::MiterJoin);
        QPen penText(QColor(65, 70, 80), 1.0, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);

        QStringList strList=m_sData.split("");//分隔
        QPointF point;
        bool b0=false;
        bool b1=false;
        for(int i=0;i<strList.size();i++)
        {
            if(strList[i]=="0")
            {
                point=QPointF(m_ptXStart.x()+i*0.5,m_ptXStart.y());
                m_pPainter->setPen(penAmp);
                m_pPainter->drawPoint(point);
                if(b1)
                {
                    m_pPainter->setPen(penDash);
                    m_pPainter->drawLine(m_ptXStart.x()+i*0.5-0.25,m_ptXStart.y(),
                                         m_ptXStart.x()+i*0.5-0.25,m_ptXStart.y()-m_nHeight/4);
                    m_pPainter->setPen(penText);
                    m_pPainter->drawLine(m_ptXStart.x()+i*0.5-0.25,m_ptXStart.y(),
                                         m_ptXStart.x()+i*0.5-0.25,m_ptXStart.y()-5);
                    QString s=QString::number(i,10);
                    m_pPainter->drawText(m_ptXStart.x()+i*0.5-0.25,m_ptXStart.y()+20,tr(s));
                }
            }
            else if(strList[i]=="1")
            {
                point=QPointF(m_ptXStart.x()+i*0.5,m_ptXStart.y()-m_nHeight/4);
                m_pPainter->setPen(penAmp);
                m_pPainter->drawPoint(point);
            }
        }
    }
}

void AxisWidget::ReceiveValue(QString sID, bool bCenEcc, QString sData)
{
    m_sID=sID;
    m_bCenEcc=bCenEcc;
    m_sData=sData;
}

void AxisWidget::DrawArrow()
{
    int nArrow=7;
    QPoint ptYArrowLeft(m_ptYStart.x()-nArrow*tan(PI/6),m_ptYStart.y()+nArrow*tan(PI/3));
    QPoint ptYArrowRight(m_ptYStart.x()+nArrow*tan(PI/6),m_ptYStart.y()+nArrow*tan(PI/3));
    QPoint ptXArrowUp(m_ptXEnd.x()-nArrow*tan(PI/3),m_ptXEnd.y()-nArrow*tan(PI/6));
    QPoint ptXArrowDown(m_ptXEnd.x()-nArrow*tan(PI/3),m_ptXEnd.y()+nArrow*tan(PI/6));
    m_pPainter->drawLine(m_ptYStart,ptYArrowLeft);
    m_pPainter->drawLine(m_ptYStart,ptYArrowRight);
    m_pPainter->drawLine(m_ptXEnd,ptXArrowUp);
    m_pPainter->drawLine(m_ptXEnd,ptXArrowDown);
}
