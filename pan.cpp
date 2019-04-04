#include "pan.h"

Pan::Pan(PaintArea *pArea, int nCenterX, int nCenterY):m_pPaintArea(pArea)
{
    m_nCellsX=nCenterX/2;
    m_nCellsY=nCenterY/2;
    m_nSliceIndex=0;
    m_nPathIndex=0;
    m_nPlayIndex=0;
    m_nPosIndex=0;
    m_nCellWavePos=0;
    m_bRunStatus=false;
    m_bRunEnd=false;
    m_bPause=false;
    m_bStop=false;
    m_bDealEnd=false;
    m_nRunMode=RUN_NULL;
    m_pControl=m_pPaintArea->m_pAntiArea->m_pControl;
    m_pShape=m_pPaintArea->m_pAntiArea->m_pShape;
    m_sFilePath="/QAntiCollision/File/OldUnit/CheckCenter.txt";
    m_dZoom = ORIGNAL_ZOOM;//2D放大
    m_pCoord = new Coord;
    InitPan();
}

Pan::~Pan()
{
    delete m_pCoord;
    for (int i = 0; i < CELL_NUM;++i)
    {
        if (m_pCell[i]!=NULL)
        {
            delete m_pCell[i];
        }
    }
    CloseDrawTools();
}

void Pan::InitPan()
{
    InitDrawTools();
    InitCheckCell();
}

void Pan::InitCheckCell()
{
    for (int i = 0; i < CELL_NUM;i++)
    {
        m_nRunID[i] = -1;
        m_dRunRadius[i] = -1;
        m_dRunRadian[i] = -1;
    }
    ReadDataFile();
    for(int i=0;i<CELL_NUM;i++)
    {
        m_pCell[i] = new Cell(i + 1, STOP_STATUS, m_pCoord->m_dCoord[i][0], m_pCoord->m_dCoord[i][1]);
        m_pCell[i]->InitCell(this,STOP_RADIUS,STOP_RADIAN);
        m_bDealRuned[i] = true;
    }
    for(int j=0;j<m_nRunNum;j++)
    {
        ResetCell(m_nRunID[j], RUN_STATUS, m_dRunRadius[j], m_dRunRadian[j]);
    }
    SolutionBasis();
}

void Pan::ReadDataFile()
{
    QFile fp(m_sFilePath);
    if(fp.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        char c[100];
        fp.readLine(c,sizeof(c));
        int i=0;
        while(!fp.atEnd())
        {
            QString strLine=QString(fp.readLine()).trimmed();
            QStringList strList=strLine.split(QRegExp("\\s{1,}"));//把所有的多余的空格为间隔,正则表达式QRegExp("[\\s]+")
            bool b;
            m_nRunID[i]=strList[0].toInt(&b);
            m_dRunRadius[i]=strList[1].toDouble(&b);
            double dA=strList[2].toDouble(&b);
            m_dRunRadian[i]=dA/180.0*PI;//把度数转化为弧度，度数可读性好，弧度计算方便
            i++;
        }
        m_nRunNum = i;
    }
    fp.close();
}


QColor Pan::GetDefinedColor(int n)
{
    QColor col;
    switch (n)
    {
    case COLOR_NUM:	    col = QColor(0, 0, 0);	        break;//单元号颜色
    case COLOR_BREAK:	col = QColor(255, 0, 0);		break;//单元碰撞区域颜色
    case COLOR_RUN:	    col = QColor(0, 255, 0);	    break;//单元运行过的路径颜色
    case COLOR_COORD:	col = QColor(180, 0, 0);        break;//坐标轴颜色
    case COLOR_DISUSE:	col = QColor(230, 230, 230);	break;//不展开单元轴外形
    case COLOR_CIRCLE:	col = QColor(200, 200, 200);    break;//单元展开最外圆
    case COLOR_TARGET:	col = QColor(0, 0, 110);    	break;//浅黄色目标点
    case COLOR_BORDER:	col = QColor(0, 0, 100);     	break;//边界
    case COLOR_SHAFT:   col = QColor(0,0,0);            break;//运行轴外形
    case COLOR_CFILL:   col = QColor(255,235,210);      break;//中心轴填充
    case COLOR_EFILL:   col = QColor(210,225,255);      break;//偏心轴填充
    case COLOR_CENTER:  col = QColor(160,160,160);      break;//偏心轴填充

    default:			col = QColor(192, 192, 192);
    }
    return col;
}

void Pan::InitDrawTools()
{
    QColor col;
    for (int i = 0; i < COLOR_AMOUNT; i++)
    {
        col = GetDefinedColor(i);
        m_cColor[i] = col;
        m_pSolidPen[i] = new QPen(col, 2.2, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
        m_pDashPen[i] = new QPen(col, 1, Qt::DashLine,Qt::SquareCap,Qt::MiterJoin);
        m_pDddPen[i] = new QPen(col, 1, Qt::DashDotDotLine,Qt::SquareCap,Qt::MiterJoin);
        m_bBrush[i] = new QBrush(col);
    }
}

void Pan::CloseDrawTools()
{
    for (int i = 0; i < COLOR_AMOUNT; i++)
    {
        delete m_pSolidPen[i];
        delete m_pDashPen[i];
        delete m_pDddPen[i];
        delete m_bBrush[i];
    }
}

QPoint Pan::Op2Vp(const double dx, const double dy)
{
    double x = (dx * CELL_RATIO + m_nCellsX)* m_dZoom;
    double y = (dy * CELL_RATIO + m_nCellsY)* m_dZoom;
    int nx, ny;
    if (x - floor(x) < 0.5)
        nx = (int)floor(x);
    else
        nx = (int)floor(x) + 1;
    if (y - floor(y) < 0.5)
        ny = (int)floor(y);
    else
        ny = (int)floor(y) + 1;

    return QPoint(nx, ny);
}

QPointF Pan::Op2Vp(const QPointF pointF)
{
    double x = (pointF.x() * CELL_RATIO + m_nCellsX)* m_dZoom;
    double y = (pointF.y() * CELL_RATIO + m_nCellsY)* m_dZoom;
    return QPointF(x, y);
}

QPoint Pan::Vp2Op(const QPoint &point)
{
    double x = (point.x() / m_dZoom - m_nCellsX) / CELL_RATIO;
    double y = (point.y() / m_dZoom - m_nCellsY) / CELL_RATIO;

    int nx, ny;
    if (x - floor(x) < 0.5)
        nx = (int)floor(x);
    else
        nx = (int)floor(x) + 1;
    if (y - floor(y) < 0.5)
        ny = (int)floor(y);
    else
        ny = (int)floor(y) + 1;

    return QPoint(nx, ny);
}

void Pan::GetCellID(const QPoint &point, int &nID)
{
    QPoint p=Vp2Op(point);
    for(int i=0;i<CELL_NUM;i++)
    {
        double dR=sqrt((m_pCell[i]->GetCenterX()-p.x())*(m_pCell[i]->GetCenterX()-p.x())
                      +(m_pCell[i]->GetCenterY()-p.y())*(m_pCell[i]->GetCenterY()-p.y()));
        if(dR<=m_pCell[i]->GetRadius())
        {
            nID=i+1;
            return;
        }
    }
}

void Pan::CellCenEccValue(QPoint &point)
{
    int nID=0;
    GetCellID(point,nID);
    if(nID>0)
    {
        QString sc="";
        QString se="";
        for(int i=1;i<=WAVE_NUM;i++)
        {
            if(CellCenPosValue(nID,i)!=BYTE_NULL)
            {
                sc+=QString::number(CellCenPosValue(nID,i));
            }
            if(CellEccPosValue(nID,i)!=BYTE_NULL)
            {
                se+=QString::number(CellEccPosValue(nID,i));
            }
        }
        emit SendShapeValue(QString::number(nID,10),sc,se);
    }
}

QRect Pan::Radius2Rect(double dx, double dy, double dR)
{
    double left = dx - dR;
    double top = dy - dR;
    double right = dx + dR;
    double bottom = dy + dR;

    QPoint leftTop = Op2Vp(left, top);
    QPoint rightBottom = Op2Vp(right, bottom);
    //矩形，起始点，宽和高
    return QRect(leftTop.x(),leftTop.y(),
                 rightBottom.x()-leftTop.x(),rightBottom.y()-leftTop.y());
}

void Pan::Draw()
{
    //不运行，不处理，区域放大移动时，创建单元
    m_pPainter->setRenderHint(QPainter::Antialiasing,true);//反走样
    if(!m_bRunStatus)
    {
        for (int i = 1; i <= CELL_NUM;i++)
        {
            CreateCellPath(i);
        }
        CheckIntersects();
    }
    //画单元,运行时已经生成单元轮廓轨迹
    for(int i=0;i<CELL_NUM;i++)
    {
        m_pCell[i]->SetPainter(m_pPainter);//只能在画之前设置
        m_pCell[i]->Draw();
    }
    //画中心圆，外圆
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    m_pPainter->drawEllipse(Radius2Rect(0,0,m_dRPanCenter));
    m_pPainter->drawEllipse(Radius2Rect(0,0,m_dRPan));
    //画中心轴线
    m_pPainter->setPen(*m_pDddPen[COLOR_COORD]);
    m_pPainter->drawLine(Op2Vp(0,m_dRPan+20.0),Op2Vp(0,-m_dRPan-20.0));
    m_pPainter->drawLine(Op2Vp(m_dRPan+20.0,0),Op2Vp(-m_dRPan-20.0,0));
    //画框
    QPen* pen= new QPen(QColor(0, 0, 0), 4.5, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
    m_pPainter->setPen(*pen);
    double ds1=18;
    double ds2=44.1;
    double dl=73;
    m_pPainter->drawLine(Op2Vp(ds1,-ds2),Op2Vp(ds1,-ds2-dl));
    m_pPainter->drawLine(Op2Vp(ds1,-ds2-dl),Op2Vp(ds1+dl,-ds2-dl));
    m_pPainter->drawLine(Op2Vp(ds1+dl,-ds2-dl),Op2Vp(ds1+dl,-ds2));
    m_pPainter->drawLine(Op2Vp(ds1+dl,-ds2),Op2Vp(ds1,-ds2));
    //画碰撞区域
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    for(int i=0;i<m_nPathIndex;i++)
    {
        m_pPainter->drawPath(m_pathInsertects[i]);
        m_pPainter->fillPath(m_pathInsertects[i],*m_bBrush[COLOR_BREAK]);
    }
}

int Pan::CellDistance(int nID, int mID)
{
    int nDistance=int(sqrt((m_pCell[nID-1]->GetCenterX()-m_pCell[mID-1]->GetCenterX())
                          *(m_pCell[nID-1]->GetCenterX()-m_pCell[mID-1]->GetCenterX())
                          +(m_pCell[nID-1]->GetCenterY()-m_pCell[mID-1]->GetCenterY())
                          *(m_pCell[nID-1]->GetCenterY()-m_pCell[mID-1]->GetCenterY())));
    return nDistance;
}

int Pan::CenTargetSlice(int nID)
{
    return m_pCell[nID-1]->GetCenRadSlice();
}

int Pan::EccTargetSlice(int nID)
{
    return m_pCell[nID-1]->GetEccRadSlice();
}

int Pan::CalCenSlice(int nID,int nPos)
{
    int nSlice=0;
    for(int i=1;i<=nPos;i++)
    {
        int nValue=CellCenPosValue(nID,i);
        if(nValue==BYTE_NULL)
            break;
        nSlice+=nValue;
    }
    return nSlice;
}

int Pan::CalEccSlice(int nID, int nPos)
{
    int nSlice=0;
    for(int i=1;i<=nPos;i++)
    {
        int nValue=int(CellEccPosValue(nID,i));
        if(nValue==BYTE_NULL)
            break;
        nSlice+=nValue;
    }
    return nSlice;
}

void Pan::SetCenPosValue(int nID, int nPos, int nValue)
{
    m_pCell[nID-1]->m_nCenWave[nPos-1]=nValue;
}

void Pan::SetEccPosValue(int nID, int nPos, int nValue)
{
    m_pCell[nID-1]->m_nEccWave[nPos-1]=nValue;
}

int Pan::CellCenFinalPos(int nID)
{
    int nPos=0;
    for(int i=0;i<WAVE_NUM;i++)
    {
        nPos=i+1;
        if(m_pCell[nID-1]->m_nCenWave[i]==BYTE_NULL)
            break;
    }
    return nPos;
}

int Pan::CellEccFinalPos(int nID)
{
    int nPos=0;
    for(int i=0;i<WAVE_NUM;i++)
    {
        nPos=i+1;
        if(m_pCell[nID-1]->m_nEccWave[i]==BYTE_NULL)
            break;
    }
    return nPos;
}

QPainterPath Pan::CellCenPath(int nID)
{
    return m_pCell[nID-1]->GetCenPath();
}

QPainterPath Pan::CellEccPath(int nID)
{
    return m_pCell[nID-1]->GetEccPath();
}

void Pan::SetCellWavePos(int nID, int nPos)
{
    m_pCell[nID-1]->SetWavePos(nPos);
}

void Pan::SetCellRunStyle(int nID, int nStyle)
{
    m_pCell[nID-1]->SetRunStyle(nStyle);
}

void Pan::SetCellCenSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentCenSlice(nSlice);
}

void Pan::SetCellEccSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentEccSlice(nSlice);
}

void Pan::SetCellArc()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        m_pCell[m_nRunID[i]-1]->CreateArc();
    }
}

void Pan::SetCellFinalValue(int nID, int nStyle)
{
    SetCellCenSlice(nID,CenTargetSlice(nID));
    SetCellEccSlice(nID,EccTargetSlice(nID));
    SetCellRunStyle(nID,nStyle);
}

int Pan::CellCenPosValue(int nID, int nPos)
{
    return m_pCell[nID-1]->m_nCenWave[nPos-1];
}

int Pan::CellEccPosValue(int nID, int nPos)
{
    return m_pCell[nID-1]->m_nEccWave[nPos-1];
}

bool Pan::CellRunStatus(int nID)
{
    return m_pCell[nID-1]->GetRunStatus();
}

void Pan::CreateCellPath(int nID)
{
     m_pCell[nID-1]->CreatePath();
}

void Pan::PlayRun()
{
    killTimer(m_nComTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nSliceIndex++;
        m_nPlayIndex++;
        m_nUpdateIndex++;
        m_bRunEnd=true;//每次都进行假设运行完成
        break;
    case 1:
        RunCellValue();
        m_nPlayIndex++;
        break;
    case 2:
        if(m_bRunEnd)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            SetCellArc();
            m_pPaintArea->update();
            emit ShowText("运行完成！");
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
        break;
    case 3:
        CheckIntersects();
        m_nPlayIndex++;
        break;
    case 4:
        if(m_bPause)
        {
            m_bRunStatus=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
        break;
    case 5:
        if(m_bStop)
        {
            m_nPlayIndex=0;
            m_bStop=false;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
        break;
    case 6:
        m_nPlayIndex=0;
        if(m_nUpdateIndex==10)
        {
            m_nUpdateIndex=0;
            SetCellArc();
            m_pPaintArea->update();
        }
        break;
    default:
        return;
    }
    m_nComTimerID=this->startTimer(RUN_TIME);
}

void Pan::PlayDeal()
{
    killTimer(m_nLastTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nPosIndex++;
        m_nPlayIndex++;
        m_nUpdateIndex++;
    case 1:
        if(m_nPosIndex<=m_nCellWavePos)
        {
            for(int i=1;i<=CELL_NUM;i++)
            {
                SetCellWavePos(i,m_nPosIndex);
                SetCellRunStyle(i,RUN_LAST);
                CreateCellPath(i);
            }
        }
        else
        {
            m_nPosIndex=0;
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            SetCellArc();
            m_pPaintArea->update();
            emit ShowText("运行结束！");
            return;
        }
        m_nPlayIndex++;
    case 2:
        CheckIntersects();
        m_nPlayIndex++;
    case 3:
        if(m_bPause)
        {
            m_bRunStatus=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
    case 4:
        if(m_bStop)
        {
            m_nPosIndex=0;
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            m_bStop=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
    case 5:
        if(m_nUpdateIndex==10)
        {
            m_nUpdateIndex=0;
            SetCellArc();
            m_pPaintArea->update();
        }
        m_nPlayIndex++;
    case 6:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nLastTimerID=this->startTimer(RUN_TIME);
}

void Pan::RunCellValue()
{
    for (int i = 1; i <= CELL_NUM;i++)
    {
        //运行的单元进行给值
        if(CellRunStatus(i))
        {
            //中心轴先展开
            if(m_nSliceIndex <= CenTargetSlice(i))
            {
                SetCellCenSlice(i,m_nSliceIndex);
                m_bRunEnd=false;
            }
            //中心轴剩余展开度数与偏心轴度数相同或小于时，偏心轴开始展开
            if(CenTargetSlice(i)-m_nSliceIndex<EccTargetSlice(i))
            {
                if(CenTargetSlice(i)>=EccTargetSlice(i)&&m_nSliceIndex<=CenTargetSlice(i))
                {
                    SetCellEccSlice(i,m_nSliceIndex-CenTargetSlice(i)+EccTargetSlice(i));
                    m_bRunEnd=false;
                }
                else if(CenTargetSlice(i)<EccTargetSlice(i)&&m_nSliceIndex<=EccTargetSlice(i))
                {
                    SetCellEccSlice(i,m_nSliceIndex);
                    m_bRunEnd=false;
                }
            }
            else
            {
                SetCellEccSlice(i,0);
            }
        }
        SetCellRunStyle(i,RUN_COM);
        CreateCellPath(i);
    }
}

void Pan::CheckIntersects()
{
    //检测可能发生碰撞的两个单元，单元与其周围的单元进行检测
    m_nPathIndex=0;
    for (int i = 0; i < m_nRunNum;i++)
    {
        for(int j = 1;j <= CELL_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(nDistance==INT_SPACE)
            {
                QPainterPath path1=CellCenPath(m_nRunID[i]);
                QPainterPath path2=CellEccPath(m_nRunID[i]);
                QPainterPath path3=CellCenPath(j);
                QPainterPath path4=CellEccPath(j);
                bool b1=path1.intersects(path4);
                bool b2=path2.intersects(path3);
                bool b3=path2.intersects(path4);
                if(b1)
                {
                    m_pathInsertects[m_nPathIndex]=path1.intersected(path4);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j,true,false);
                }
                if(b2)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path3);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j,false,true);
                }
                if(b3)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path4);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j,false,false);
                }
            }
        }
    }
}


void Pan::PlaySolution()
{
    killTimer(m_nPlanTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nCellWavePos++;
        m_nPlayIndex++;
        m_bDealEnd=true;
        m_nProgressIndex++;
        m_nUpdateIndex++;
    case 1:
        if(m_nCellWavePos>WAVE_NUM)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            emit ShowText("处理出错！");
            return;
        }
        m_nPlayIndex++;
    case 2:
        DealCellPos();
        if(m_nProgressIndex==10)
        {
            m_nProgressIndex=0;
            emit DealProgress();
        }
        m_nPlayIndex++;
    case 3:
        if(m_bDealEnd)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            SetCellArc();
            m_pPaintArea->update();
            emit DealProgress();
            emit ShowText("处理完成！");
            return;
        }
        else
        {
            DealSolution();
            m_nPlayIndex++;
        }
    case 4:
        if(m_bPause)
        {
            m_bRunStatus=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
    case 5:
        if(m_bStop)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            m_bStop=false;
            return;
        }
        else
        {
            m_nPlayIndex++;
        }
    case 6:
        if(m_nUpdateIndex==10)
        {
            m_nUpdateIndex=0;
            SetCellArc();
            m_pPaintArea->update();
        }
        m_nPlayIndex++;
    case 7:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nPlanTimerID=this->startTimer(RUN_TIME);
}

void Pan::DealCellPos()
{
    //位置向前1，位置暂停0，然后进行碰撞判断
    for(int i=1;i<=CELL_NUM;i++)
    {
        //运行的单元,进行波形位置值处理
        if(CellRunStatus(i))
        {
            if(CenTargetSlice(i)>CalCenSlice(i,m_nCellWavePos))
            {
                m_bDealEnd=false;
            }
            if(EccTargetSlice(i)>CalEccSlice(i,m_nCellWavePos))
            {
                m_bDealEnd=false;
            }
            if(CellCenPosValue(i,m_nCellWavePos)==BYTE_NULL)
            {
                SetCenPosValue(i,m_nCellWavePos,BYTE_STOP);
            }
            if(CellEccPosValue(i,m_nCellWavePos)==BYTE_NULL)
            {
                SetEccPosValue(i,m_nCellWavePos,BYTE_STOP);
            }
            if(CenTargetSlice(i)==CalCenSlice(i,m_nCellWavePos)&&
               EccTargetSlice(i)==CalEccSlice(i,m_nCellWavePos))
            {
                m_bDealRuned[i-1]=true;
            }
        }
        SetCellWavePos(i,m_nCellWavePos);
        SetCellRunStyle(i,RUN_PLAN);
        CreateCellPath(i);
    }
}

bool Pan::DetectCollision(int nID, int mID, bool* bCE)
{
    bool bValue=false;
    CreateCellPath(nID);
    CreateCellPath(mID);
    QPainterPath path1=CellCenPath(nID);
    QPainterPath path2=CellEccPath(nID);
    QPainterPath path3=CellCenPath(mID);
    QPainterPath path4=CellEccPath(mID);
    bCE[0]=path1.intersects(path4);//n中心轴与m偏心轴
    bCE[1]=path2.intersects(path3);//n偏心轴与m中心轴
    bCE[2]=path2.intersects(path4);//n偏心轴与m偏心轴
    if(bCE[0]||bCE[1]||bCE[2])
    {
        bValue=true;
    }
    return bValue;
}

void Pan::DealSolution()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= CELL_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(nDistance==INT_SPACE)
            {
                bool bDeal=false;
                bool bCE[3]={false,false,false};
                while(DetectCollision(m_nRunID[i],j,bCE))
                {
                    DealCollision(m_nRunID[i],j,m_nCellWavePos,bCE);
                    bDeal=true;
                }
                if(bDeal)
                {
                    DealNearCell(m_nRunID[i],j,m_nCellWavePos);
                }
            }
        }
    }
}

void Pan::DealCollision(int nID, int mID, int nPos, bool* bCE)
{
    //n中心轴与m偏心轴相碰，偏心轴一定是展开的
    //n偏心轴与m中心轴相碰，偏心轴一定是展开的
    //n偏心轴与m偏心轴相碰，偏心轴至少有一个是展开的
    if(bCE[0])
    {
        DealCenMethod(nID,mID,nPos);
    }
    if(bCE[1])
    {
        DealCenMethod(mID,nID,nPos);
    }
    if(bCE[2])
    {
        //nID已到达过目标位置，mID未到达过目标位置
        if(m_bDealRuned[nID-1]&&!m_bDealRuned[mID-1])
        {
            DealEccMethod(nID,mID,nPos);
        }
        //mID已到达过目标位置，nID未到达过目标位置
        else if(m_bDealRuned[mID-1]&&!m_bDealRuned[nID-1])
        {
            DealEccMethod(mID,nID,nPos);
        }
        //nID已到达过目标位置，mID已到达过目标位置
        else if(m_bDealRuned[nID-1]&&m_bDealRuned[mID-1])
        {
            if(CalCenSlice(nID,nPos)==CenTargetSlice(nID))
            {
                DealEccMethod(nID,mID,nPos);
            }
            else
            {
                DealEccMethod(mID,nID,nPos);
            }
        }
        //nID未到达过目标位置，mID未到达过目标位置
        else
        {
            if(EccTargetSlice(nID)-CalEccSlice(nID,nPos)<=EccTargetSlice(mID)-CalEccSlice(mID,nPos))
            {
                DealEccMethod(nID,mID,nPos);
            }
            else
            {
                DealEccMethod(mID,nID,nPos);
            }
        }
    }
}

void Pan::DealCenMethod(int nID, int mID, int nPos)
{
    /// nID单元中心轴与mID单元偏心轴碰撞，以mID单元在中心轴目标位置时其偏心轴从0到目标位置转动，
    /// 偏心轴扫过的区域与nID单元中心轴目标位置是否碰撞标签，处理mID单元中心轴、偏心轴
    int nBasis=m_mapResult[QString::number(mID,10)+QString::number(nID,10)];
    switch(nBasis)
    {
    //mID单元偏心轴扫过的区域与nID单元中心轴目标位置无碰撞
    case BASIS_NULL:
        DealEcc(mID,nPos);
        break;
    //mID单元偏心轴扫过的区域与nID单元中心轴目标位置碰撞
    case BASIS_CEN:
        if(CalCenSlice(mID,nPos)>0)
        {
            DealCen(mID,nPos);
        }
        else
        {
            DealEcc(mID,nPos);
        }
        break;
    //mID单元偏心轴扫过的区域与nID单元偏心轴目标位置碰撞
    case BASIS_ECC:
        DealEcc(mID,nPos);
        break;
    //mID单元偏心轴扫过的区域与nID单元目标位置都碰撞
    case BASIS_CEC:
        if(CalCenSlice(mID,nPos)>0)
        {
            DealCen(mID,nPos);
        }
        else
        {
            DealEcc(mID,nPos);
        }
        break;
    default:
        break;
    }
}

void Pan::DealEccMethod(int nID, int mID, int nPos)
{
    /// nID单元偏心轴与mID单元偏心轴相碰，以nID单元在中心轴目标位置时其偏心轴从0到目标位置转动，
    /// 偏心轴扫过的区域与mID单元目标位置的碰撞情况为依据，处理nID、mID单元情况
    int nBasis=m_mapResult[QString::number(nID,10)+QString::number(mID,10)];
    switch(nBasis)
    {
    //nID单元偏心轴扫过的区域与mID单元目标位置无碰撞
    case BASIS_NULL:
        if(CalEccSlice(nID,nPos)>0)
        {
            DealEcc(nID,nPos);
        }
        else
        {
            DealEcc(mID,nPos);
        }
        break;
    //nID单元偏心轴扫过的区域与mID单元中心轴目标位置碰撞
    case BASIS_CEN:
        if(CalEccSlice(mID,nPos)>0)
        {
            DealEcc(mID,nPos);
        }
        else
        {
            DealCen(nID,nPos);
        }
        break;
    //nID单元偏心轴扫过的区域与mID单元偏心轴目标位置碰撞
    case BASIS_ECC:
        DealCen(nID,nPos);
        break;
    //nID单元偏心轴扫过的区域与mID单元目标位置都碰撞
    case BASIS_CEC:
        if(CalCenSlice(mID,nPos)<CenTargetSlice(mID))
        {
            if(CalEccSlice(mID,nPos)>0)
            {
                DealEcc(mID,nPos);
            }
        }
        else
        {
            DealCen(nID,nPos);
        }
        break;
    default:
        break;
    }
}

void Pan::SolutionBasis()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= CELL_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(m_nRunID[i]!=j&&nDistance==INT_SPACE)
            {
                QString key=QString::number(m_nRunID[i],10)+QString::number(j,10);
                int nResult=DealBasis(m_nRunID[i],j);
                m_mapResult.insert(key,nResult);
            }
        }
    }
}

int Pan::DealBasis(int nID, int mID)
{
    //nID单元中心轴在目标位置，偏心轴从0到目标位置是否与mID的中心轴/偏心轴目标位置碰撞
    int nResult=BASIS_NULL;
    SetCellCenSlice(nID,CenTargetSlice(nID));
    SetCellCenSlice(mID,CenTargetSlice(mID));
    SetCellEccSlice(mID,EccTargetSlice(mID));
    SetCellRunStyle(nID,RUN_COM);
    SetCellRunStyle(mID,RUN_COM);
    CreateCellPath(mID);
    QPainterPath path1=CellCenPath(mID);
    QPainterPath path2=CellEccPath(mID);
    bool b1=false;
    bool b2=false;
    for(int nSlice=0;nSlice<=EccTargetSlice(nID);nSlice++)
    {
        SetCellEccSlice(nID,nSlice);
        CreateCellPath(nID);
        QPainterPath path3=CellEccPath(nID);
        if(path1.intersects(path3))
        {
            b1=true;
        }
        if(path2.intersects(path3))
        {
            b2=true;
        }
    }
    if(b1&&!b2)
    {
        nResult=BASIS_CEN;
    }
    else if(!b1&&b2)
    {
        nResult=BASIS_ECC;
    }
    else if(b1&&b2)
    {
        nResult=BASIS_CEC;
    }
    return nResult;
}

void Pan::DealNearCell(int nID, int mID, int nPos)
{
    for(int i=1;i<=CELL_NUM;i++)
    {
        int nL1=CellDistance(nID,i);
        int nL2=CellDistance(mID,i);
        if(nL1==INT_SPACE)
        {
           DealEachPos(nID,i,nPos);
        }
        if(nL2==INT_SPACE)
        {
           DealEachPos(mID,i,nPos);
        }
    }
}

void Pan::DealEachPos(int nID, int mID, int nPos)
{
    for(int i=1;i<=nPos;i++)
    {
        SetCellWavePos(nID,i);
        SetCellWavePos(mID,i);
        bool bDeal=false;
        bool bCE[3]={false,false,false};
        while(DetectCollision(nID,mID,bCE))
        {
            DealCollision(nID,mID,i,bCE);
            bDeal=true;
        }
        if(bDeal)
        {
            DealNearCell(nID,mID,i);
        }
    }
}

void Pan::DealCen(int nID, int nPos)
{
    //处理之前把波形分段大于2，即为3段，最多只有3段，交换第2、3段移至成两段
    DealCenWave(nID,2);
    int nMove=0;
    for(int i=0;i<DEAL_SLICE;i++)
    {
        while(CellCenPosValue(nID,nMove)!=BYTE_RUN
              &&nMove<nPos)
        {
            nMove++;
        }
        if(CellCenPosValue(nID,nMove)==BYTE_RUN)
        {
            SetCenPosValue(nID,nMove,BYTE_STOP);
            int nEnd=CellCenFinalPos(nID);
            SetCenPosValue(nID,nEnd,BYTE_RUN);
        }
    }
    //处理之后，为3段的不处理，为4段，交换第2、3段移至成两段
    //（4段：首先已到达目标位置，1段：处理的暂停DEAL_SLICE个位置；2段：到达目标位置运行的少DEAL_SLICE个的位置；
    // 3段：到达目标位置后暂停的DEAL_SLICE个位置；4段：补充的DEAL_SLICE个位置后到达目标位置）
    DealCenWave(nID,3);
}

void Pan::DealEcc(int nID, int nPos)
{
    //处理之前把波形分段大于2，即为3段，最多只有3段，交换第2、3段移至成两段
    DealEccWave(nID,2);
    int nMove=0;
    for(int i=0;i<DEAL_SLICE;i++)
    {
        while(CellEccPosValue(nID,nMove)!=BYTE_RUN
              &&nMove<nPos)
        {
            nMove++;
        }
        if(CellEccPosValue(nID,nMove)==BYTE_RUN)
        {
            SetEccPosValue(nID,nMove,BYTE_STOP);
            int nEnd=CellEccFinalPos(nID);
            SetEccPosValue(nID,nEnd,BYTE_RUN);
        }
    }
    //处理之后，为3段的不处理，为4段，交换第2、3段移至成两段
    //（4段：首先已到达目标位置，1段：处理的暂停DEAL_SLICE个位置；2段：到达目标位置运行的少DEAL_SLICE个的位置；
    // 3段：到达目标位置后暂停的DEAL_SLICE个位置；4段：补充的DEAL_SLICE个位置后到达目标位置）
    DealEccWave(nID,3);
}

void Pan::DealCenWave(int nID, int nPart)
{
    QVector<QVector<int>> vWave(10);
    int nFlag=CellCenPosValue(nID,1);
    int nRow=1;
    for(int i=1;i<=WAVE_NUM;i++)
    {
        int nValue=CellCenPosValue(nID,i);
        if(nValue!=BYTE_NULL)
        {
            if(nFlag!=nValue)
            {
                nFlag=nValue;
                nRow++;
            }
            vWave[nRow-1].push_back(nValue);
        }
    }
    if(nRow>nPart)
    {
        vWave[1].swap(vWave[2]);
        int n=1;
        for(int j=0;j<nRow;j++)
        {
            for(int k=0;k<vWave[j].size();k++)
            {
                SetCenPosValue(nID,n,vWave[j][k]);
                n++;
            }
        }
    }
}

void Pan::DealEccWave(int nID, int nPart)
{
    QVector<QVector<int>> vWave(10);
    int nFlag=CellEccPosValue(nID,1);
    int nRow=1;
    for(int i=1;i<=WAVE_NUM;i++)
    {
        int nValue=CellEccPosValue(nID,i);
        if(nValue!=BYTE_NULL)
        {
            if(nFlag!=nValue)
            {
                nFlag=nValue;
                nRow++;
            }
            vWave[nRow-1].push_back(nValue);
        }
    }
    if(nRow>nPart)
    {
        vWave[1].swap(vWave[2]);
        int n=1;
        for(int j=0;j<nRow;j++)
        {
            for(int k=0;k<vWave[j].size();k++)
            {
                SetEccPosValue(nID,n,vWave[j][k]);
                n++;
            }
        }
    }
}

void Pan::ResetCell(int nID, bool bStatus,double dRadius,double dRadian)
{
    delete m_pCell[nID-1];
    m_pCell[nID-1] = new Cell(nID, bStatus, m_pCoord->m_dCoord[nID-1][0], m_pCoord->m_dCoord[nID-1][1]);
    m_pCell[nID-1]->InitCell(this,dRadius,dRadian);
    m_bDealRuned[nID-1] = !bStatus;
}


void Pan::CommonRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nSliceIndex=0;
    m_nUpdateIndex=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_COM;
    m_nComTimerID=this->startTimer(RUN_TIME);
}

void Pan::PauseRun()
{
    if(m_nRunMode==RUN_COM)
    {
        if(m_bPause)
        {
            m_bRunStatus=true;
            m_bPause=false;
            m_nPlayIndex=4;
            m_nComTimerID=this->startTimer(RUN_TIME);
        }
        else
        {
            m_bRunStatus=false;
            m_bPause=true;
        }
    }
    else if(m_nRunMode==RUN_PLAN)
    {
        if(m_bPause)
        {
            m_bRunStatus=true;
            m_bPause=false;
            m_nPlayIndex=4;
            m_nPlanTimerID=this->startTimer(RUN_TIME);
        }
        else
        {
            m_bRunStatus=false;
            m_bPause=true;
        }
    }
    else if(m_nRunMode==RUN_LAST)
    {
        if(m_bPause)
        {
            m_bRunStatus=true;
            m_bPause=false;
            m_nPlayIndex=3;
            m_nLastTimerID=this->startTimer(RUN_TIME);
        }
        else
        {
            m_bRunStatus=false;
            m_bPause=true;
        }
    }
}

void Pan::StopRun()
{
    m_bStop=true;
}

void Pan::ResetRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nSliceIndex=0;//转过度数设为0
    m_nPlayIndex=0;
    m_bRunStatus=false;
    for (int i = 0; i < m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],0);
        SetCellEccSlice(m_nRunID[i],0);
        SetCellRunStyle(m_nRunID[i],RUN_COM);
    }
    m_pPaintArea->update();
}

void Pan::CheckEndStatus()
{
    for(int i=1;i<=CELL_NUM;i++)
    {
        SetCellFinalValue(i);
    }
    m_pPaintArea->update();
}

void Pan::DealRandom()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= CELL_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(nDistance==INT_SPACE)
            {
                SetCellFinalValue(m_nRunID[i]);
                SetCellFinalValue(j);
                bool bCE[3]={false,false,false};
                DetectCollision(m_nRunID[i],j,bCE);
                if(bCE[0])
                {
                    ResetCell(j);
                }
                else if(bCE[1])
                {
                    ResetCell(m_nRunID[i]);
                }
                else if(bCE[2])
                {
                    if(m_bDealRuned[j-1])
                    {
                        ResetCell(m_nRunID[i]);
                    }
                    else if(m_bDealRuned[m_nRunID[i]-1])
                    {
                        ResetCell(j);
                    }
                    else
                    {
                        double iRadius=m_pCell[m_nRunID[i]-1]->GetRadius();
                        double iRadian=m_pCell[m_nRunID[i]-1]->GetRunRadian();
                        double jRadius=m_pCell[j-1]->GetRadius();
                        double jRadian=m_pCell[j-1]->GetRunRadian();
                        double icx=m_pCell[m_nRunID[i]-1]->GetCenterX();
                        double icy=m_pCell[m_nRunID[i]-1]->GetCenterY();
                        double jcx=m_pCell[j-1]->GetCenterX();
                        double jcy=m_pCell[j-1]->GetCenterY();
                        double ix=icx+iRadius*cos(iRadian);
                        double iy=icy-iRadius*sin(iRadian);
                        double jx=jcx+jRadius*cos(jRadian);
                        double jy=jcy-jRadius*sin(jRadian);
                        double iR=sqrt((jx-icx)*(jx-icx)+(jy-icy)*(jy-icy));
                        double jR=sqrt((ix-jcx)*(ix-jcx)+(iy-jcy)*(iy-jcy));
                        if(int(iR)==INT_SPACE||int(jR)==INT_SPACE)
                        {
                            ResetCell(m_nRunID[i]);
                            break;
                        }
                        double iRad=asin((icy-jy)/iR);
                        double jRad=asin((jcy-iy)/jR);
                        if(iRad>=0.0)
                        {
                            if(icx>jx)
                                iRad=PI-iRad;
                        }
                        else if(iRad<0)
                        {
                            if(icx>jx)
                                iRad=PI+fabs(iRad);
                            else
                                iRad=2*PI+iRad;
                        }
                        if(jRad>=0.0)
                        {
                            if(jcx>ix)
                                jRad=PI-jRad;
                        }
                        else if(jRad<0)
                        {
                            if(jcx>ix)
                                jRad=PI+fabs(jRad);
                            else
                                jRad=2*PI+jRad;
                        }
                        ResetCell(m_nRunID[i], RUN_STATUS, iR, iRad);
                        ResetCell(j, RUN_STATUS, jR, jRad);
                        SetCellFinalValue(m_nRunID[i]);
                        SetCellFinalValue(j);
                        if(DetectCollision(m_nRunID[i],j,bCE))
                        {
                            ResetCell(j, RUN_STATUS, jRadius, jRadian);
                            ResetCell(m_nRunID[i]);
                        }
                    }
                }
            }
        }
    }
    QVector<int> vID;
    QVector<int> vRadius;
    QVector<int> vRadian;
    for (int i = 0; i < m_nRunNum;i++)
    {
        if(!m_bDealRuned[m_nRunID[i]-1])
        {
            vID.push_back(m_nRunID[i]);
            vRadius.push_back(m_dRunRadius[i]);
            vRadian.push_back(m_dRunRadian[i]);
        }
        m_nRunID[i] = -1;
        m_dRunRadius[i] = -1;
        m_dRunRadian[i] = -1;
    }
    for(int j=0;j<vID.size();j++)
    {
        m_nRunID[j] = vID[j];
        m_dRunRadius[j] = vRadius[j];
        m_dRunRadian[j] = vRadian[j];
    }
    m_nRunNum=vID.size();
    SolutionBasis();
    m_pPaintArea->update();
}

void Pan::OverInitPan(QString sPath)
{
    m_sFilePath=sPath;
    for (int i = 0; i < CELL_NUM;++i)
    {
        if (m_pCell[i]!=NULL)
        {
            delete m_pCell[i];
        }
    }
    InitCheckCell();
    m_pPaintArea->update();
}

void Pan::PlanRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nCellWavePos=0;
    m_nProgressIndex=0;
    m_nUpdateIndex=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_PLAN;
    for(int j=0;j<m_nRunNum;j++)
    {
        m_bDealRuned[m_nRunID[j]-1] = false;
    }
//    m_nCellWavePos=700;
//    QTime time;
//    time.start();
//    DealCellPos();
//    int n=time.elapsed();

//    time.start();
//    DealSolution();
//    n=time.elapsed();


//    time.start();
//    CreateCellPath(18);
//    n=time.elapsed();


//    time.start();
//    DetectCollision(17,18);
//    n=time.elapsed();
//    m_nCellWavePos=100;
    m_nPlanTimerID=this->startTimer(RUN_TIME);
//    PlaySolution();
}

void Pan::LastRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nPosIndex=0;
    m_nUpdateIndex=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_LAST;
    m_nLastTimerID=this->startTimer(RUN_TIME);
}

void Pan::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_nComTimerID)
    {
        PlayRun();
    }
    else if(event->timerId()==m_nLastTimerID)
    {
        PlayDeal();
    }
    else if(event->timerId()==m_nPlanTimerID)
    {
        PlaySolution();
    }
}
