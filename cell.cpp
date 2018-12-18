#include "cell.h"

Cell::Cell(int nID, int nStatus, double dCenterX, double dCenterY)
: m_nID(nID), m_nStatus(nStatus), m_dCenterX(dCenterX), m_dCenterY(dCenterY)
{
    m_nCurrentCenDegree=0;
    m_nCurrentEccDegree=0;
    m_dDrawEccAngle=0;
    m_nWavePos=0;
    m_bRunStatus=true;
    m_dREcc=MAX_RADIUS;
    m_ptCenP1=QPointF(-2.75,-3.9);
    m_ptCenP2=QPointF(-2.75,-6.9);
    m_ptCenP3=QPointF(10.25,-6.9);
    m_ptCenP4=QPointF(10.25,-3.9);
    m_ptEccP1=QPointF(0,0);
    m_ptEccP2=QPointF(0,0);
    m_ptEccP3=QPointF(0,0);
    m_ptEccP4=QPointF(0,0);
    m_ptEccR1=QPointF(0,0);
    m_ptEccR2=QPointF(0,0);
    m_ptEccR3=QPointF(0,0);
    m_dLenghtP1=LengthByPoint(m_ptCenP1);
    m_dLenghtP2=LengthByPoint(m_ptCenP2);
    m_dLenghtP3=LengthByPoint(m_ptCenP3);
    m_dLenghtP4=LengthByPoint(m_ptCenP4);
    m_dAngleP1=atan(m_ptCenP1.y()/m_ptCenP1.x())+PI;
    m_dAngleP2=atan(m_ptCenP2.y()/m_ptCenP2.x())+PI;
    m_dAngleP3=atan(m_ptCenP3.y()/m_ptCenP3.x())+2*PI;
    m_dAngleP4=atan(m_ptCenP4.y()/m_ptCenP4.x())+2*PI;
    for(int i=0;i<WAVE_NUM;i++)
    {
        m_nCenWave[i]=BYTE_NULL;
        m_nEccWave[i]=BYTE_NULL;
    }
}

Cell::~Cell()
{

}

//运行的点最终半径和角度
void Cell::InitCell(Pan *pPan, double dRadius, int nAngle)
{
    m_pPan = pPan;
    m_nRunAngle = nAngle;
    if(dRadius<=0)
    {
        m_dRadius =0;
        m_dStartAngle=acos(0);
    }
    else if(dRadius>=2*m_dRunEcc)
    {
        m_dRadius=2*m_dRunEcc;
        m_dStartAngle=acos(1);
    }
    else
    {
        m_dRadius = dRadius;
        m_dStartAngle = acos((m_dRadius*m_dRadius + m_dRunEcc*m_dRunEcc - m_dRunEcc*m_dRunEcc)
                            /(2 * m_dRadius*m_dRunEcc));//三角形余弦定理
    }
    m_dEccRunAngle=PI-2*m_dStartAngle;
    m_nEccRunDegree=int(m_dEccRunAngle/PI*180)*SPLIT_DEGREE;
    m_nStartDegree=int(m_dStartAngle/PI*180)*SPLIT_DEGREE;
    m_nRunDegree=m_nRunAngle*SPLIT_DEGREE;
}

QRect Cell::CalRect()
{
    return m_pPan->Radius2Rect(m_dCenterX, m_dCenterY, m_dRadius);
}

void Cell::Draw()
{
    //画圆
    m_rRect = CalRect();//每重画一次，都要计算一次
    if(m_nStatus==RUN_STATUS)
    {
        DrawArc();//画运行轨迹
        DrawCenEcc();//画展开的中心轴和偏心轴
        DrawTargetPos();//画目标点
    }
    else
    {
        m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_DISUSE]));
        m_pPainter->drawEllipse(m_rRect);//画未展开圆
        DrawUnexpanded();
    }
    //写单元数
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_NUM]));
    QFont font;
    font.setPointSize(10);
    m_pPainter->setFont(font);
    m_pPainter->drawText(m_rRect,Qt::AlignCenter,QString::number(m_nID,10));
}

void Cell::DrawArc()
{
    //画最后的落点所在的圆
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_CIRCLE]));
    m_pPainter->drawArc(m_rRect,0,360*SPLIT_DEGREE);
    //画运行的轨迹
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_RUN]));
    m_pPainter->drawPath(m_pathArc);
}

void Cell::DrawCenEcc()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pPan->m_pDddPen[COLOR_COORD]));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_dCenterX,m_dCenterY),m_pPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_ptEccR2),m_pPan->Op2Vp(m_ptEccR1));

    //中心轴外形    偏心轴外形
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_SHAFT]));
    m_pPainter->drawPath(m_pathCen);
    m_pPainter->drawPath(m_pathEcc);
}

void Cell::DrawUnexpanded()
{
    //展开痕迹
    m_pPainter->setPen(*(m_pPan->m_pDddPen[COLOR_DISUSE]));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_dCenterX,m_dCenterY),m_pPan->Op2Vp(m_ptEccR2));
    m_pPainter->drawLine(m_pPan->Op2Vp(m_ptEccR2),m_pPan->Op2Vp(m_ptEccR1));
    //中心轴外形    偏心轴外形
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_DISUSE]));
    m_pPainter->drawPath(m_pathCen);
    m_pPainter->drawPath(m_pathEcc);
}

void Cell::DrawTargetPos()
{
    //目标点
    m_pPainter->setPen(*(m_pPan->m_pSolidPen[COLOR_TARGET]));
    double dDegree=double(m_nRunDegree-m_nStartDegree)/SPLIT_DEGREE/180.0*PI+m_dStartAngle;
    QPointF p=QPointF(m_dCenterX+m_dRadius*cos(dDegree),m_dCenterY-m_dRadius*sin(dDegree));
    double d=1.0;
    QPointF p1=QPointF(p.x()-d,p.y()-d);
    QPointF p2=QPointF(p.x()+d,p.y()-d);
    QPointF p3=QPointF(p.x()-d,p.y()+d);
    QPointF p4=QPointF(p.x()+d,p.y()+d);
    m_pPainter->drawLine(m_pPan->Op2Vp(p1),m_pPan->Op2Vp(p4));
    m_pPainter->drawLine(m_pPan->Op2Vp(p2),m_pPan->Op2Vp(p3));
}

double Cell::LengthByPoint(const QPointF pointF)
{
    return sqrt(pointF.x()*pointF.x()+pointF.y()*pointF.y());
}

void Cell::CalCurrentCenEccDegree()
{
    int nCenDegree=0;
    int nEccDegree=0;
    for(int i=0;i<m_nWavePos;i++)
    {
        int n=m_nCenWave[i];
        int m=m_nEccWave[i];
        if(n!=BYTE_NULL)
        {
            nCenDegree+=n*SPLIT_DEGREE;
        }
        if(m!=BYTE_NULL)
        {
            nEccDegree+=m*SPLIT_DEGREE;
        }
    }
    m_nCurrentCenDegree=nCenDegree;
    m_nCurrentEccDegree=nEccDegree;
}

void Cell::CreatePoint()
{
    if(!m_bRunStatus)
    {
        CalCurrentCenEccDegree();
    }
    //中心轴外形矩形4点
    double dCenCurrent=double(m_nCurrentCenDegree)/SPLIT_DEGREE/180.0*PI;//中心轴转动的角度，也就是从x轴正方向开始转动角度
    double dPA1=dCenCurrent+m_dAngleP1;
    double dPA2=dCenCurrent+m_dAngleP2;
    double dPA3=dCenCurrent+m_dAngleP3;
    double dPA4=dCenCurrent+m_dAngleP4;
    m_ptCenP1=QPointF(m_dCenterX+m_dLenghtP1*cos(dPA1),m_dCenterY-m_dLenghtP1*sin(dPA1));
    m_ptCenP2=QPointF(m_dCenterX+m_dLenghtP2*cos(dPA2),m_dCenterY-m_dLenghtP2*sin(dPA2));
    m_ptCenP3=QPointF(m_dCenterX+m_dLenghtP3*cos(dPA3),m_dCenterY-m_dLenghtP3*sin(dPA3));
    m_ptCenP4=QPointF(m_dCenterX+m_dLenghtP4*cos(dPA4),m_dCenterY-m_dLenghtP4*sin(dPA4));

    //偏心轴外形矩形4点，中心3点
    double dCenDistance=m_dEcc-2*m_dREcc-m_dRunEcc;
    double dEccCurrent=double(m_nCurrentEccDegree)/SPLIT_DEGREE/180.0*PI;//偏心轴转动的角度，也就是从x轴负方向开始转动角度
    double dRadius=sqrt(m_dRunEcc*m_dRunEcc+m_dRunEcc*m_dRunEcc
                        -2*m_dRunEcc*m_dRunEcc*cos(dEccCurrent));
    double dRadiusAngle=(PI-dEccCurrent)/2.0;
    m_ptEccR1=QPointF(m_dCenterX+dRadius*cos(dRadiusAngle+dCenCurrent),
                   m_dCenterY-dRadius*sin(dRadiusAngle+dCenCurrent));
    m_ptEccR2=QPointF(m_dCenterX+m_dRunEcc*cos(dCenCurrent),
                   m_dCenterY-m_dRunEcc*sin(dCenCurrent));

    double dCenA=asin(abs(m_ptEccR1.y()-m_ptEccR2.y())/m_dRunEcc);
    double dAngle=PI/2.0-dCenA;
    double dCX1=m_ptEccR1.x();
    double dCY1=m_ptEccR1.y();
    double dCX2=m_ptEccR2.x();
    double dCY2=m_ptEccR2.y();
    if(dCX1>=dCX2&&dCY1<=dCY2)
    {
        m_dDrawEccAngle=dCenA/PI*180.0-90.0;
        m_ptEccR3=QPointF(m_ptEccR2.x()-dCenDistance*cos(dCenA),m_ptEccR2.y()+dCenDistance*sin(dCenA));
        m_ptEccP1=QPointF(m_ptEccR1.x()-m_dREcc*cos(dAngle),m_ptEccR1.y()-m_dREcc*sin(dAngle));
        m_ptEccP2=QPointF(m_ptEccR1.x()+m_dREcc*cos(dAngle),m_ptEccR1.y()+m_dREcc*sin(dAngle));
        m_ptEccP3=QPointF(m_ptEccR3.x()+m_dREcc*cos(dAngle),m_ptEccR3.y()+m_dREcc*sin(dAngle));
        m_ptEccP4=QPointF(m_ptEccR3.x()-m_dREcc*cos(dAngle),m_ptEccR3.y()-m_dREcc*sin(dAngle));
    }
    else if(dCX1<=dCX2&&dCY1<=dCY2)
    {
        m_dDrawEccAngle=90.0-dCenA/PI*180.0;
        m_ptEccR3=QPointF(m_ptEccR2.x()+dCenDistance*cos(dCenA),m_ptEccR2.y()+dCenDistance*sin(dCenA));
        m_ptEccP1=QPointF(m_ptEccR1.x()-m_dREcc*cos(dAngle),m_ptEccR1.y()+m_dREcc*sin(dAngle));
        m_ptEccP2=QPointF(m_ptEccR1.x()+m_dREcc*cos(dAngle),m_ptEccR1.y()-m_dREcc*sin(dAngle));
        m_ptEccP3=QPointF(m_ptEccR3.x()+m_dREcc*cos(dAngle),m_ptEccR3.y()-m_dREcc*sin(dAngle));
        m_ptEccP4=QPointF(m_ptEccR3.x()-m_dREcc*cos(dAngle),m_ptEccR3.y()+m_dREcc*sin(dAngle));
    }
    else if(dCX1<=dCX2&&dCY1>=dCY2)
    {
        m_dDrawEccAngle=90.0+dCenA/PI*180.0;
        m_ptEccR3=QPointF(m_ptEccR2.x()+dCenDistance*cos(dCenA),m_ptEccR2.y()-dCenDistance*sin(dCenA));
        m_ptEccP1=QPointF(m_ptEccR1.x()+m_dREcc*cos(dAngle),m_ptEccR1.y()+m_dREcc*sin(dAngle));
        m_ptEccP2=QPointF(m_ptEccR1.x()-m_dREcc*cos(dAngle),m_ptEccR1.y()-m_dREcc*sin(dAngle));
        m_ptEccP3=QPointF(m_ptEccR3.x()-m_dREcc*cos(dAngle),m_ptEccR3.y()-m_dREcc*sin(dAngle));
        m_ptEccP4=QPointF(m_ptEccR3.x()+m_dREcc*cos(dAngle),m_ptEccR3.y()+m_dREcc*sin(dAngle));
    }
    else
    {
        m_dDrawEccAngle=270.0-dCenA/PI*180.0;
        m_ptEccR3=QPointF(m_ptEccR2.x()-dCenDistance*cos(dCenA),m_ptEccR2.y()-dCenDistance*sin(dCenA));
        m_ptEccP1=QPointF(m_ptEccR1.x()+m_dREcc*cos(dAngle),m_ptEccR1.y()-m_dREcc*sin(dAngle));
        m_ptEccP2=QPointF(m_ptEccR1.x()-m_dREcc*cos(dAngle),m_ptEccR1.y()+m_dREcc*sin(dAngle));
        m_ptEccP3=QPointF(m_ptEccR3.x()-m_dREcc*cos(dAngle),m_ptEccR3.y()+m_dREcc*sin(dAngle));
        m_ptEccP4=QPointF(m_ptEccR3.x()+m_dREcc*cos(dAngle),m_ptEccR3.y()-m_dREcc*sin(dAngle));
    }
}

void Cell::CreatePath()
{
    CreatePoint();
    //中心轴外形
    QPainterPath cenPath;
    cenPath.moveTo(m_pPan->Op2Vp(m_ptCenP1));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP2));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP3));
    cenPath.lineTo(m_pPan->Op2Vp(m_ptCenP4));
    cenPath.closeSubpath();
    m_pathCen=cenPath;
    //偏心轴外形
    QPainterPath eccPath;
    eccPath.arcMoveTo(m_pPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dREcc),m_dDrawEccAngle);
    eccPath.arcTo(m_pPan->Radius2Rect(m_ptEccR1.x(), m_ptEccR1.y(), m_dREcc),m_dDrawEccAngle,180.0);
    eccPath.lineTo(m_pPan->Op2Vp(m_ptEccP4));
    eccPath.arcTo(m_pPan->Radius2Rect(m_ptEccR3.x(), m_ptEccR3.y(), m_dREcc),m_dDrawEccAngle+180.0,180.0);
    eccPath.closeSubpath();
//    eccPath.moveTo(m_pPan->Op2Vp(m_ptEccP1));
//    eccPath.lineTo(m_pPan->Op2Vp(m_ptEccP2));
//    eccPath.lineTo(m_pPan->Op2Vp(m_ptEccP3));
//    eccPath.lineTo(m_pPan->Op2Vp(m_ptEccP4));
//    eccPath.lineTo(m_pPan->Op2Vp(m_ptEccP1));
    m_pathEcc=eccPath;
    //画运行轨迹
    CreateArc();
}

void Cell::CreateArc()
{
    //运行轨迹，中心轴、偏心轴根据碰撞处理进行运行展开
    QPainterPath arcPath;
    if(m_bRunStatus)
    {
        double dRadius=0;
        double dSpread=0;
        QPointF p=QPointF(m_dCenterX,m_dCenterY);
        arcPath.moveTo(m_pPan->Op2Vp(p));
        //当前总共运行的度数进行分割
        if(m_nRunDegree-m_nStartDegree>=m_nEccRunDegree)
        {
            for(int nCen=1;nCen<=m_nCurrentCenDegree;nCen++)
            {
                int nEcc=0;
                if(m_nRunDegree-m_nStartDegree-nCen<m_nEccRunDegree)
                {
                    if(nCen<=m_nRunDegree-m_nStartDegree)
                    {
                        nEcc=nCen-m_nRunDegree+m_nStartDegree+m_nEccRunDegree;
                    }
                }
                if(nCen>m_nCurrentCenDegree)
                    nCen=m_nCurrentCenDegree;
                if(nEcc>m_nEccRunDegree)
                   nEcc=m_nEccRunDegree;
                double dCenCurrent=double(nCen)/SPLIT_DEGREE/180.0*PI;
                double dEccCurrent=double(nEcc)/SPLIT_DEGREE/180.0*PI;
                dRadius=sqrt(m_dRunEcc*m_dRunEcc+m_dRunEcc*m_dRunEcc-2*m_dRunEcc*m_dRunEcc*cos(dEccCurrent));
                dSpread=(PI-dEccCurrent)/2.0;
                p=QPointF(m_dCenterX+dRadius*cos(dCenCurrent+dSpread),m_dCenterY-dRadius*sin(dCenCurrent+dSpread));
                arcPath.lineTo(m_pPan->Op2Vp(p));
            }
        }
        else
        {
            for(int nCen=1,nEcc=1;nCen<=m_nCurrentCenDegree||nEcc<=m_nCurrentEccDegree;nCen++,nEcc++)
            {
                if(nCen>m_nCurrentCenDegree)
                    nCen=m_nCurrentCenDegree;
                if(nEcc>m_nEccRunDegree)
                    nEcc=m_nEccRunDegree;
                double dCenCurrent=double(nCen)/SPLIT_DEGREE/180.0*PI;
                double dEccCurrent=double(nEcc)/SPLIT_DEGREE/180.0*PI;
                dRadius=sqrt(m_dRunEcc*m_dRunEcc+m_dRunEcc*m_dRunEcc-2*m_dRunEcc*m_dRunEcc*cos(dEccCurrent));
                dSpread=(PI-dEccCurrent)/2.0;
                p=QPointF(m_dCenterX+dRadius*cos(dCenCurrent+dSpread),m_dCenterY-dRadius*sin(dCenCurrent+dSpread));
                arcPath.lineTo(m_pPan->Op2Vp(p));
            }
        }
    }
    else
    {
        double dRadius=0;
        double dSpread=0;
        QPointF p=QPointF(m_dCenterX,m_dCenterY);
        arcPath.moveTo(m_pPan->Op2Vp(p));
        int nCurrentCen=0;
        int nCurrentEcc=0;
        int nLastCen=0;
        int nLastEcc=0;
        for(int nC=0,nE=0;nC<m_nWavePos&&nE<m_nWavePos;nC++,nE++)
        {
            if(m_nCenWave[nC]!=BYTE_NULL)
                nCurrentCen+=m_nCenWave[nC]*SPLIT_DEGREE;
            if(m_nEccWave[nE]!=BYTE_NULL)
                nCurrentEcc+=m_nEccWave[nE]*SPLIT_DEGREE;
            double dDiffCen=double(nCurrentCen-nLastCen)/SPLIT_DEGREE;
            double dDiffEcc=double(nCurrentEcc-nLastEcc)/SPLIT_DEGREE;
            for(int n=1;n<=SPLIT_DEGREE;n++)
            {
                double dCenCurrent=double(nLastCen+n*dDiffCen)/SPLIT_DEGREE/180.0*PI;
                double dEccCurrent=double(nLastEcc+n*dDiffEcc)/SPLIT_DEGREE/180.0*PI;
                dRadius=sqrt(m_dRunEcc*m_dRunEcc+m_dRunEcc*m_dRunEcc-2*m_dRunEcc*m_dRunEcc*cos(dEccCurrent));
                dSpread=(PI-dEccCurrent)/2.0;
                p=QPointF(m_dCenterX+dRadius*cos(dCenCurrent+dSpread),m_dCenterY-dRadius*sin(dCenCurrent+dSpread));
                arcPath.lineTo(m_pPan->Op2Vp(p));
            }
            nLastCen=nCurrentCen;
            nLastEcc=nCurrentEcc;
        }
    }
    m_pathArc=arcPath;
}
