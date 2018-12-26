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
    m_bDealEnd=false;
    m_pControl=m_pPaintArea->m_pAntiArea->m_pControl;
    m_pShape=m_pPaintArea->m_pAntiArea->m_pShape;
    m_sFilePath="/QAntiCollision/File/CheckCenter.txt";
    m_dZoom = ORIGNAL_ZOOM;//2D放大
    for (int i = 0; i < CELL_NUM;i++)
    {
        m_nRunID[i] = -1;
        m_dRunRadius[i] = -1;
        m_dRunRadian[i] = -1;
        m_bCellDeal[i] = false;
    }
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
    ReadDataFile();
    for(int i=0;i<CELL_NUM;i++)
    {
        m_pCell[i] = new Cell(i + 1, STOP_STATUS, m_pCoord->m_dCoord[i][0], m_pCoord->m_dCoord[i][1]);
        m_pCell[i]->InitCell(this,STOP_RADIUS,STOP_RADIAN);
    }
    for(int j=0;j<m_nRunNum;j++)
    {
        delete m_pCell[m_nRunID[j]-1];
        m_pCell[m_nRunID[j]-1] = new Cell(m_nRunID[j], RUN_STATUS, m_pCoord->m_dCoord[m_nRunID[j]-1][0],
                                          m_pCoord->m_dCoord[m_nRunID[j]-1][1]);
        m_pCell[m_nRunID[j]-1]->InitCell(this, m_dRunRadius[j], m_dRunRadian[j]);
    }
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
            m_dRunRadian[i]=dA/180*PI;//把度数转化为弧度，度数可读性好，弧度计算方便
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
    case COLOR_NUM:	    col = QColor(255, 255, 255);	break;//白色单元号颜色
    case COLOR_BREAK:	col = QColor(255, 0, 0);		break;//红色单元碰撞区域颜色
    case COLOR_RUN:	    col = QColor(0, 225, 0);	    break;//绿色单元运行过的路径颜色
    case COLOR_COORD:	col = QColor(180, 0, 0);        break;//浅红色坐标轴颜色
    case COLOR_DISUSE:	col = QColor(50, 50, 50);	    break;//浅灰色不展开单元轴外形
    case COLOR_CIRCLE:	col = QColor(80, 80, 80);       break;//浅灰色单元展开最外圆
    case COLOR_TARGET:	col = QColor(245, 255, 110);	break;//浅黄色目标点
    case COLOR_BORDER:	col = QColor(0, 0, 255);    	break;//蓝色边界
    case COLOR_SHAFT:   col = QColor(235,170,255);      break;//浅紫色运行轴外形

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
        m_pSolidPen[i] = new QPen(col, 1.2, Qt::SolidLine,Qt::SquareCap,Qt::MiterJoin);
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
        emit SendCenValue(QString::number(nID,10),sc);
        emit SendEccValue(QString::number(nID,10),se);
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
    if(!m_bRunStatus)
    {
        for (int i = 1; i <= CELL_NUM;i++)
        {
            CreateCellPath(i);
        }
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

void Pan::SetCellWavePos(int nID, int nPos)
{
    m_pCell[nID-1]->SetWavePos(nPos);
}

void Pan::SetCellRunStatus(int nID, bool bStatus)
{
    m_pCell[nID-1]->SetRunStyle(bStatus);
}

void Pan::SetCellCenSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentCenSlice(nSlice);
}

void Pan::SetCellEccSlice(int nID, int nSlice)
{
    m_pCell[nID-1]->SetCurrentEccSlice(nSlice);
}

void Pan::SetCellEccRadius(int nID, double dRadius)
{
    m_pCell[nID-1]->SetEccRadius(dRadius);
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
    killTimer(m_nRunTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nSliceIndex++;
        m_nPlayIndex++;
        m_bRunEnd=true;//每次都进行假设运行完成
        break;
    case 1:
        SetCellValue();
        m_nPlayIndex++;
        break;
    case 2:
        if(m_bRunEnd)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
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
        m_nPlayIndex=0;
        m_pPaintArea->update();
        break;
    default:
        return;
    }
    m_nRunTimerID=this->startTimer(RUN_TIME);
}

void Pan::PlayDeal()
{
    killTimer(m_nDealTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nPosIndex++;
        m_nPlayIndex++;
    case 1:
        if(m_nPosIndex<=m_nCellWavePos)
        {
            for(int i=1;i<=CELL_NUM;i++)
            {
                SetCellWavePos(i,m_nPosIndex);
                SetCellRunStatus(i,false);
                SetCellEccRadius(i,MIN_RADIUS);
                CreateCellPath(i);
            }
        }
        else
        {
            m_nPosIndex=0;
            m_nPlayIndex=0;
            m_bRunStatus=false;
            emit ShowText("运行结束！");
            return;
        }
        m_nPlayIndex++;
    case 2:
        CheckIntersects();
        m_nPlayIndex++;
    case 3:
        m_pPaintArea->update();
        m_nPlayIndex++;
    case 4:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nDealTimerID=this->startTimer(RUN_TIME);
}

void Pan::SetCellValue()
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
        SetCellRunStatus(i,true);
        SetCellEccRadius(i,MAX_RADIUS);
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
            if(m_nRunID[i]!=j&&nDistance==INT_SPACE)
            {
                QString sTemp;
                QPainterPath path1=m_pCell[m_nRunID[i]-1]->GetCenPath();
                QPainterPath path2=m_pCell[m_nRunID[i]-1]->GetEccPath();
                QPainterPath path3=m_pCell[j-1]->GetCenPath();
                QPainterPath path4=m_pCell[j-1]->GetEccPath();
                bool b1=path1.intersects(path4);
                bool b2=path2.intersects(path3);
                bool b3=path2.intersects(path4);
                if(b1)
                {
                    m_pathInsertects[m_nPathIndex]=path1.intersected(path4);
                    m_nPathIndex++;
                    sTemp.sprintf("单元%02d的中心轴与单元%02d的偏心轴碰撞！",m_nRunID[i],j);
                    emit ShowText(sTemp);
                }
                if(b2)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path3);
                    m_nPathIndex++;
                    sTemp.sprintf("单元%02d的偏心轴与单元%02d的中心轴碰撞！",m_nRunID[i],j);
                    emit ShowText(sTemp);
                }
                if(b3)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path4);
                    m_nPathIndex++;
                    sTemp.sprintf("单元%02d的偏心轴与单元%02d的偏心轴碰撞！",m_nRunID[i],j);
                    emit ShowText(sTemp);
                }
            }
        }
    }
}


void Pan::PlaySolution()
{
    killTimer(m_nSolutionTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nCellWavePos++;
        m_nPlayIndex++;
        m_bDealEnd=true;
    case 1:
        if(m_nCellWavePos>WAVE_NUM)
        {
            emit ShowText("处理出错！");
            return;
        }
        m_nPlayIndex++;
    case 2:
        DealCellPos();
        emit DealProgress();
        m_nPlayIndex++;
    case 3:
        if(m_bDealEnd)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            emit ShowText("处理完成！");
            return;
        }
        else
        {
            DealSolution();
            m_nPlayIndex++;
        }
    case 4:
        CheckIntersects();
        m_pPaintArea->update();
        m_nPlayIndex++;
    case 5:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nSolutionTimerID=this->startTimer(RUN_TIME);
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
        }
        SetCellWavePos(i,m_nCellWavePos);
        SetCellRunStatus(i,false);
        SetCellEccRadius(i,MAX_RADIUS);
        CreateCellPath(i);
    }
}

int Pan::DetectCollision(int nID, int mID)
{
    int nValue=0;
    CreateCellPath(nID);
    CreateCellPath(mID);
    QPainterPath path1=m_pCell[nID-1]->GetCenPath();
    QPainterPath path2=m_pCell[nID-1]->GetEccPath();
    QPainterPath path3=m_pCell[mID-1]->GetCenPath();
    QPainterPath path4=m_pCell[mID-1]->GetEccPath();
    bool b1=path1.intersects(path4);//n中心轴与m偏心轴
    bool b2=path2.intersects(path3);//n偏心轴与m中心轴
    bool b3=path2.intersects(path4);//n偏心轴与m偏心轴
    if(b1)
    {
        nValue+=100;
    }
    if(b2)
    {
        nValue+=10;
    }
    if(b3)
    {
        nValue+=1;
    }
    return nValue;
}

void Pan::DealSolution()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= CELL_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(m_nRunID[i]!=j&&nDistance==INT_SPACE)
            {
                int nResult=0;
                bool bDeal=false;
                while(nResult=DetectCollision(m_nRunID[i],j))
                {
                    DealCollision(m_nRunID[i],j,m_nCellWavePos,nResult);
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

void Pan::DealCollision(int nID, int mID, int nPos, int nResult)
{
    int nCE=nResult/100;     //n中心轴与m偏心轴相碰，偏心轴一定是展开的
    int nEC=nResult%100/10;  //n偏心轴与m中心轴相碰，偏心轴一定是展开的
    int nEE=nResult%10;      //n偏心轴与m偏心轴相碰，偏心轴至少有一个是展开的
    if(nCE)
    {
        //nID单元不运动或中心轴转动到目标位置
        if(CenTargetSlice(nID)==CalCenSlice(nID,nPos))
        {
            //当前位置数已经大于mID中心轴目标位置，只能处理中心轴
            if(CenTargetSlice(mID)<nPos)
            {
                DealCen(mID,nPos);
            }
            //当前mID中心轴还在转动，处理偏心轴
            else
            {
                DealEcc(mID,nPos);
            }
        }
        else
        {
            DealEcc(mID,nPos);
            DealEcc(mID,nPos);
        }
    }
    if(nEC)
    {
        if(CenTargetSlice(mID)==CalCenSlice(mID,nPos))
        {
            if(CenTargetSlice(nID)<nPos)
            {
                DealCen(nID,nPos);
            }
            else
            {
                DealEcc(nID,nPos);
            }
        }
        else
        {
            DealEcc(nID,nPos);
            DealEcc(nID,nPos);
        }
    }
    if(nEE)
    {
        //nID单元不运动或单元回到原位，mID单元处理
        if(CalCenSlice(nID,nPos)==0&&CalEccSlice(nID,nPos)==0)
        {
            //mID的中心轴还在转动
            if(CalCenSlice(mID,nPos)<CenTargetSlice(mID))
            {
                DealEcc(mID,nPos);
            }
            //mID中心轴已到达目标位置
            else
            {
                DealCen(mID,nPos);
            }
        }

        else if(CalCenSlice(mID,nPos)==0&&CalEccSlice(mID,nPos)==0)
        {
            //nID的中心轴还在转动
            if(CalCenSlice(nID,nPos)<CenTargetSlice(nID))
            {
                DealEcc(nID,nPos);
            }
            //nID中心轴已到达目标位置
            else
            {
                DealCen(nID,nPos);
            }
        }
        else if(CalEccSlice(nID,nPos)==0&&CellRunStatus(nID))
        {
            DealCen(nID,nPos);
        }
        else if(CalEccSlice(mID,nPos)==0&&CellRunStatus(mID))
        {
            DealCen(mID,nPos);
        }
        else if(m_bCellDeal[nID])
        {
            if(CalCenSlice(mID,nPos)==nPos)
            {
                DealEcc(mID,nPos);
            }
            else
            DealEcc(nID,nPos);
        }
        else if(m_bCellDeal[mID])
        {
            if(CalCenSlice(nID,nPos)==nPos)
            {
                DealEcc(nID,nPos);
            }
            else
            DealEcc(mID,nPos);
        }
        //两个偏心轴都运动，偏心轴展开需要少的处理
        else if(EccTargetSlice(nID)-CalEccSlice(nID,nPos)<=EccTargetSlice(mID)-CalEccSlice(mID,nPos))
        {
            m_bCellDeal[nID]=true;
            DealEcc(nID,nPos);
            if(CalCenSlice(mID,nPos)==nPos)
            {
                DealEcc(mID,nPos);
            }
        }
        else if(EccTargetSlice(nID)-CalEccSlice(nID,nPos)>EccTargetSlice(mID)-CalEccSlice(mID,nPos))
        {
            m_bCellDeal[mID]=true;
            DealEcc(mID,nPos);
            if(CalCenSlice(nID,nPos)==nPos)
            {
                DealEcc(nID,nPos);
            }
        }
    }
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
        int nResult=0;
        bool bDeal=false;
        while(nResult=DetectCollision(nID,mID))
        {
            DealCollision(nID,mID,i,nResult);
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
    for(int i=0;i<DEAL_SLICE;i++)
    {
        while(CellCenPosValue(nID,nPos)!=BYTE_RUN
              &&nPos>1)
        {
            if(CellCenPosValue(nID,nPos)==BYTE_NULL)
            {
                SetCenPosValue(nID,nPos,BYTE_STOP);
            }
            nPos--;
        }
        if(CellCenPosValue(nID,nPos)==BYTE_RUN)
        {
            SetCenPosValue(nID,nPos,BYTE_STOP);
            int nEnd=CellCenFinalPos(nID);
            SetCenPosValue(nID,nEnd,BYTE_RUN);
        }
    }
}

void Pan::DealEcc(int nID, int nPos)
{
    for(int i=0;i<DEAL_SLICE;i++)
    {
        while(CellEccPosValue(nID,nPos)!=BYTE_RUN
              &&nPos>1)
        {
            if(CellEccPosValue(nID,nPos)==BYTE_NULL)
            {
                SetEccPosValue(nID,nPos,BYTE_STOP);
            }
            nPos--;
        }
        if(CellEccPosValue(nID,nPos)==BYTE_RUN)
        {
            SetEccPosValue(nID,nPos,BYTE_STOP);
            int nEnd=CellEccFinalPos(nID);
            SetEccPosValue(nID,nEnd,BYTE_RUN);
        }
    }
}



void Pan::ShowRun()
{
    m_nPlayIndex=0;
    m_nSliceIndex=0;
    m_bRunStatus=true;
    m_nRunTimerID=this->startTimer(RUN_TIME);
}

void Pan::PauseRun()
{
    if(m_bPause)
    {
        m_bRunStatus=true;
        m_bPause=false;
        m_nPlayIndex=4;
        m_nRunTimerID=this->startTimer(RUN_TIME);
    }
    else
    {
        m_bRunStatus=false;
        m_bPause=true;
    }
}

void Pan::ResetRun()
{
    killTimer(m_nRunTimerID);
    m_nSliceIndex=0;//转过度数设为0
    m_nPlayIndex=0;
    m_bRunStatus=false;
    for (int i = 0; i < m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],0);
        SetCellEccSlice(m_nRunID[i],0);
        SetCellRunStatus(m_nRunID[i],true);
    }
    m_pPaintArea->update();
}

void Pan::CheckEndStatus()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],CenTargetSlice(m_nRunID[i]));
        SetCellEccSlice(m_nRunID[i],EccTargetSlice(m_nRunID[i]));
        SetCellRunStatus(m_nRunID[i],true);
        SetCellEccRadius(m_nRunID[i],MAX_RADIUS);
    }
    CheckIntersects();
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

void Pan::PlanSolution()
{
    m_nPlayIndex=0;
    m_nCellWavePos=0;
    m_bRunStatus=true;
    m_nSolutionTimerID=this->startTimer(RUN_TIME);
}

void Pan::RunShape()
{
    m_nPlayIndex=0;
    m_nPosIndex=0;
    m_bRunStatus=true;
    m_nDealTimerID=this->startTimer(RUN_TIME);
}

void Pan::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_nRunTimerID)
    {
        PlayRun();
    }
    else if(event->timerId()==m_nDealTimerID)
    {
        PlayDeal();
    }
    else if(event->timerId()==m_nSolutionTimerID)
    {
        PlaySolution();
    }
}
