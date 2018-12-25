#ifndef CELL_H
#define CELL_H

#include <QObject>
#include <QRect>
#include <QPainter>
#include <QPointF>
#include "mymethod.h"
#include "pan.h"

class Pan;
class Cell : public QObject
{
    Q_OBJECT
public:
    explicit Cell(int nID, bool bStatus, double dCenterX, double dCenterY);
    virtual ~Cell();
public:
    Pan*             m_pPan;
    QPainter*        m_pPainter;          //在此绘制
    QRect            m_rRect;             //单元所在的正方形
    int              m_nCenWave[WAVE_NUM];//中心轴运行的波形数据
    int              m_nEccWave[WAVE_NUM];//偏心轴运行的波形数据
private:
    const double m_dCen = 13;      //中心轴长度
    const double m_dEcc = 13.5;    //偏心轴长度
    const double m_dRunEcc = 8.5;  //偏心轴展开长度
    double m_dREcc;                //偏心轴外倒角圆弧半径

    QPointF m_ptCenP1;             //中心轴起始时4点
    QPointF m_ptCenP2;             //中心轴起始时4点
    QPointF m_ptCenP3;             //中心轴起始时4点
    QPointF m_ptCenP4;             //中心轴起始时4点
    double m_dLenghtP1;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP2;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP3;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dLenghtP4;            //中心轴起始时4点相当于坐标原点（圆心）的长度
    double m_dRadianP1;            //中心轴起始时4点相当于坐标x轴的偏转弧度
    double m_dRadianP2;            //中心轴起始时4点相当于坐标x轴的偏转弧度
    double m_dRadianP3;            //中心轴起始时4点相当于坐标x轴的偏转弧度
    double m_dRadianP4;            //中心轴起始时4点相当于坐标x轴的偏转弧度

    QPointF m_ptEccP1;             //偏心轴外形矩形点
    QPointF m_ptEccP2;             //偏心轴外形矩形点
    QPointF m_ptEccP3;             //偏心轴外形矩形点
    QPointF m_ptEccP4;             //偏心轴外形矩形点
    QPointF m_ptEccR1;             //偏心轴前端半圆中心点
    QPointF m_ptEccR2;             //偏心轴展开时旋转中心点
    QPointF m_ptEccR3;             //偏心轴后端半圆中心点

    QPainterPath m_pathCen;        //中心轴外形轨迹
    QPainterPath m_pathEcc;        //偏心轴外形轨迹
    QPainterPath m_pathArc;        //运行的圆弧轨迹

private:
    int          m_nID;                 //单元编号
    bool         m_bStatus;             //单元是否选中运行的，0否，1是
    double       m_dCenterX;            //单元中心X
    double       m_dCenterY;            //单元中心Y
    double       m_dRadius;             //单元运行半径
    double       m_dRunRadian;          //单元运行到指定点时的弧度rad,从单元圆心相对于x轴正方向
    int          m_nRunRadSlice;        //单元运行到指定点时的弧度片，rad*100,方便计算,相对于x轴正方向
    double       m_dStartRadian;        //单元根据半径展开后相对于x正轴得到的弧度
    int          m_nStartRadSlice;      //单元根据半径展开后相对于x正轴得到的弧度片，rad*100
    double       m_dCenRadian;          //中心轴需要运行的弧度rad，相对于x轴正方向
    int          m_nCenRadSlice;        //中心轴需要运行的弧度片，rad*100
    double       m_dEccRadian;          //偏心轴展开需要运行的弧度rad，相对于中心轴负方向
    int          m_nEccRadSlice;        //偏心轴展开需要运行的弧度片，rad*100
    double       m_dChamferDegree;      //画偏心轴外倒角圆弧的起始度数°
    int          m_nCurrentCenSlice;    //当前中心轴展开的弧度片，rad*100，相对于x轴正方向
    int          m_nCurrentEccSlice;    //当前偏心轴展开的弧度片，rad*100，相对于中心轴负方向
    int          m_nWavePos;            //轴波形位置
    bool         m_bRunStyle;           //运行方式，一般运行，处理运行
public:
    void    InitCell(Pan* pPan, double dRadius, double dRadian);
    QRect   CalRect();
    void    Draw();
    void    DrawArc();//运行半径小于中心轴m_dCen+偏心轴m_dEcc，运行要到达的角度从x正轴开始，小于等于2*PI
    void    DrawCenEcc();//画中心轴和偏心轴
    void    DrawUnexpanded();//画未展开的中心轴和偏心轴
    void    DrawTargetPos();//画目标点
    double  LengthByPoint(const QPointF pointF);
    double  LengthByPoint(const QPointF point1,const QPointF point2);
    void    CalCurrentRadSlice();
    QPointF CalPointBySlice(int nCenSlice,int nEccSlice);//通过中心轴，偏心轴弧度片，计算光纤点
    void    CreatePoint();
    void    CreatePath();
    void    CreateArc();

public:
    inline void SetPainter(QPainter* pPainter)      {m_pPainter = pPainter;}
    inline void SetCurrentCenSlice(int nSlice)      {m_nCurrentCenSlice=nSlice;}
    inline void SetCurrentEccSlice(int nSlice)      {m_nCurrentEccSlice=nSlice;}
    inline void SetRunStyle(bool bStyle)            {m_bRunStyle=bStyle;}
    inline int  GetCurrentCenSlice()                {return m_nCurrentCenSlice;}
    inline int  GetCurrentEccSlice()                {return m_nCurrentEccSlice;}
    inline int  GetCenRadSlice()                    {return m_nCenRadSlice;}
    inline int  GetEccRadSlice()                    {return m_nEccRadSlice;}
    inline int  GetRadius()                         {return m_dRadius;}
    inline double  GetCenterX()                     {return m_dCenterX;}
    inline double  GetCenterY()                     {return m_dCenterY;}
    inline bool    GetRunStatus()                   {return m_bStatus;}
    inline void SetWavePos(int nWavePos)            {m_nWavePos=nWavePos;}
    inline void SetEccRadius(double dRadius)        {m_dREcc=dRadius;}
    inline QPainterPath GetCenPath()                {return m_pathCen;}
    inline QPainterPath GetEccPath()                {return m_pathEcc;}
signals:

public slots:
};

#endif // CELL_H
