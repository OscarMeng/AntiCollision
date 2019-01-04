#include "newpan.h"

NewPan::NewPan(NewPaint *pPaint, int nCenterX, int nCenterY):m_pNewPaint(pPaint)
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
    m_pNewControl=m_pNewPaint->m_pNewAnti->m_pNewControl;
    m_pNewShape=m_pNewPaint->m_pNewAnti->m_pNewShape;
    m_sFilePath="/QAntiCollision/File/NewUnit/CheckCenter.txt";
    m_dZoom = ORIGNAL_ZOOM;//2D�Ŵ�
    m_pCoord = new Coord;
    InitPan();
}

NewPan::~NewPan()
{
    delete m_pCoord;
    for (int i = 0; i < NEW_NUM;++i)
    {
        if (m_pNewCell[i]!=NULL)
        {
            delete m_pNewCell[i];
        }
    }
    CloseDrawTools();
}

void NewPan::InitPan()
{
    InitDrawTools();
    InitCheckCell();
}

void NewPan::InitCheckCell()
{
    for (int i = 0; i < NEW_NUM;i++)
    {
        m_nRunID[i] = -1;
        m_dRunRadius[i] = -1;
        m_dRunRadian[i] = -1;
    }
    ReadDataFile();
    for(int i=0;i<NEW_NUM;i++)
    {
        m_pNewCell[i] = new NewCell(i + 1, STOP_STATUS, m_pCoord->m_dNewCoord[i][0], m_pCoord->m_dNewCoord[i][1]);
        m_pNewCell[i]->InitCell(this,STOP_RADIUS,STOP_RADIAN);
        m_bDealRuned[i] = true;
    }
    for(int j=0;j<m_nRunNum;j++)
    {
        ResetCell(m_nRunID[j], RUN_STATUS, m_dRunRadius[j], m_dRunRadian[j]);
    }
    SolutionBasis();
}

void NewPan::ReadDataFile()
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
            QStringList strList=strLine.split(QRegExp("\\s{1,}"));//�����еĶ���Ŀո�Ϊ���,������ʽQRegExp("[\\s]+")
            bool b;
            m_nRunID[i]=strList[0].toInt(&b);
            m_dRunRadius[i]=strList[1].toDouble(&b);
            double dA=strList[2].toDouble(&b);
            m_dRunRadian[i]=dA/180.0*PI;//�Ѷ���ת��Ϊ���ȣ������ɶ��Ժã����ȼ��㷽��
            i++;
        }
        m_nRunNum = i;
    }
    fp.close();
}


QColor NewPan::GetDefinedColor(int n)
{
    QColor col;
    switch (n)
    {
    case COLOR_NUM:	    col = QColor(255, 255, 255);	break;//��ɫ��Ԫ����ɫ
    case COLOR_BREAK:	col = QColor(255, 0, 0);		break;//��ɫ��Ԫ��ײ������ɫ
    case COLOR_RUN:	    col = QColor(0, 225, 0);	    break;//��ɫ��Ԫ���й���·����ɫ
    case COLOR_COORD:	col = QColor(180, 0, 0);        break;//ǳ��ɫ��������ɫ
    case COLOR_DISUSE:	col = QColor(50, 50, 50);	    break;//ǳ��ɫ��չ����Ԫ������
    case COLOR_CIRCLE:	col = QColor(80, 80, 80);       break;//ǳ��ɫ��Ԫչ������Բ
    case COLOR_TARGET:	col = QColor(245, 255, 110);	break;//ǳ��ɫĿ���
    case COLOR_BORDER:	col = QColor(0, 0, 255);    	break;//��ɫ�߽�
    case COLOR_SHAFT:   col = QColor(235,170,255);      break;//ǳ��ɫ����������

    default:			col = QColor(192, 192, 192);
    }
    return col;
}

void NewPan::InitDrawTools()
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

void NewPan::CloseDrawTools()
{
    for (int i = 0; i < COLOR_AMOUNT; i++)
    {
        delete m_pSolidPen[i];
        delete m_pDashPen[i];
        delete m_pDddPen[i];
        delete m_bBrush[i];
    }
}

QPoint NewPan::Op2Vp(const double dx, const double dy)
{
    double x = (dx * NEW_RATIO + m_nCellsX)* m_dZoom;
    double y = (dy * NEW_RATIO + m_nCellsY)* m_dZoom;
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

QPointF NewPan::Op2Vp(const QPointF pointF)
{
    double x = (pointF.x() * NEW_RATIO + m_nCellsX)* m_dZoom;
    double y = (pointF.y() * NEW_RATIO + m_nCellsY)* m_dZoom;
    return QPointF(x, y);
}

QPoint NewPan::Vp2Op(const QPoint &point)
{
    double x = (point.x() / m_dZoom - m_nCellsX) / NEW_RATIO;
    double y = (point.y() / m_dZoom - m_nCellsY) / NEW_RATIO;

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

void NewPan::GetCellID(const QPoint &point, int &nID)
{
    QPoint p=Vp2Op(point);
    for(int i=0;i<NEW_NUM;i++)
    {
        double dR=sqrt((m_pNewCell[i]->GetCenterX()-p.x())*(m_pNewCell[i]->GetCenterX()-p.x())
                      +(m_pNewCell[i]->GetCenterY()-p.y())*(m_pNewCell[i]->GetCenterY()-p.y()));
        if(dR<=m_pNewCell[i]->GetRadius())
        {
            nID=i+1;
            return;
        }
    }
}

void NewPan::CellCenEccValue(QPoint &point)
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

QRect NewPan::Radius2Rect(double dx, double dy, double dR)
{
    double left = dx - dR;
    double top = dy - dR;
    double right = dx + dR;
    double bottom = dy + dR;

    QPoint leftTop = Op2Vp(left, top);
    QPoint rightBottom = Op2Vp(right, bottom);
    //���Σ���ʼ�㣬��͸�
    return QRect(leftTop.x(),leftTop.y(),
                 rightBottom.x()-leftTop.x(),rightBottom.y()-leftTop.y());
}

void NewPan::Draw()
{
    //�����У�����������Ŵ��ƶ�ʱ��������Ԫ
    if(!m_bRunStatus)
    {
        for (int i = 1; i <= NEW_NUM;i++)
        {
            CreateCellPath(i);
        }
        CheckIntersects();
    }
    //����Ԫ,����ʱ�Ѿ����ɵ�Ԫ�����켣
    for(int i=0;i<NEW_NUM;i++)
    {
        m_pNewCell[i]->SetPainter(m_pPainter);//ֻ���ڻ�֮ǰ����
        m_pNewCell[i]->Draw();
    }
    //������Բ����Բ
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    m_pPainter->drawEllipse(Radius2Rect(0,0,m_dRPan));
    //����������
    m_pPainter->setPen(*m_pDddPen[COLOR_COORD]);
    m_pPainter->drawLine(Op2Vp(0,m_dRPan+20.0),Op2Vp(0,-m_dRPan-20.0));
    m_pPainter->drawLine(Op2Vp(m_dRPan+20.0,0),Op2Vp(-m_dRPan-20.0,0));
    //����ײ����
    m_pPainter->setPen(*m_pSolidPen[COLOR_BORDER]);
    for(int i=0;i<m_nPathIndex;i++)
    {
        m_pPainter->drawPath(m_pathInsertects[i]);
        m_pPainter->fillPath(m_pathInsertects[i],*m_bBrush[COLOR_BREAK]);
    }
}

int NewPan::CellDistance(int nID, int mID)
{
    int nDistance=int(sqrt((m_pNewCell[nID-1]->GetCenterX()-m_pNewCell[mID-1]->GetCenterX())
                          *(m_pNewCell[nID-1]->GetCenterX()-m_pNewCell[mID-1]->GetCenterX())
                          +(m_pNewCell[nID-1]->GetCenterY()-m_pNewCell[mID-1]->GetCenterY())
                          *(m_pNewCell[nID-1]->GetCenterY()-m_pNewCell[mID-1]->GetCenterY())));
    return nDistance;
}

int NewPan::CenTargetSlice(int nID)
{
    return m_pNewCell[nID-1]->GetCenRadSlice();
}

int NewPan::EccTargetSlice(int nID)
{
    return m_pNewCell[nID-1]->GetEccRadSlice();
}

int NewPan::CalCenSlice(int nID,int nPos)
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

int NewPan::CalEccSlice(int nID, int nPos)
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

void NewPan::SetCenPosValue(int nID, int nPos, int nValue)
{
    m_pNewCell[nID-1]->m_nCenWave[nPos-1]=nValue;
}

void NewPan::SetEccPosValue(int nID, int nPos, int nValue)
{
    m_pNewCell[nID-1]->m_nEccWave[nPos-1]=nValue;
}

int NewPan::CellCenFinalPos(int nID)
{
    int nPos=0;
    for(int i=0;i<WAVE_NUM;i++)
    {
        nPos=i+1;
        if(m_pNewCell[nID-1]->m_nCenWave[i]==BYTE_NULL)
            break;
    }
    return nPos;
}

int NewPan::CellEccFinalPos(int nID)
{
    int nPos=0;
    for(int i=0;i<WAVE_NUM;i++)
    {
        nPos=i+1;
        if(m_pNewCell[nID-1]->m_nEccWave[i]==BYTE_NULL)
            break;
    }
    return nPos;
}

QPainterPath NewPan::CellCenPath(int nID)
{
    return m_pNewCell[nID-1]->GetCenPath();
}

QPainterPath NewPan::CellEccPath(int nID)
{
    return m_pNewCell[nID-1]->GetEccPath();
}

void NewPan::SetCellWavePos(int nID, int nPos)
{
    m_pNewCell[nID-1]->SetWavePos(nPos);
}

void NewPan::SetCellRunStyle(int nID, int nStyle)
{
    m_pNewCell[nID-1]->SetRunStyle(nStyle);
}

void NewPan::SetCellCenSlice(int nID, int nSlice)
{
    m_pNewCell[nID-1]->SetCurrentCenSlice(nSlice);
}

void NewPan::SetCellEccSlice(int nID, int nSlice)
{
    m_pNewCell[nID-1]->SetCurrentEccSlice(nSlice);
}

void NewPan::SetCellEccRadius(int nID, double dRadius)
{
    m_pNewCell[nID-1]->SetEccRadius(dRadius);
}

void NewPan::SetCellFinalValue(int nID,int nStyle,double dEccR)
{
    SetCellCenSlice(nID,CenTargetSlice(nID));
    SetCellEccSlice(nID,EccTargetSlice(nID));
    SetCellRunStyle(nID,nStyle);
    SetCellEccRadius(nID,dEccR);
}

int NewPan::CellCenPosValue(int nID, int nPos)
{
    return m_pNewCell[nID-1]->m_nCenWave[nPos-1];
}

int NewPan::CellEccPosValue(int nID, int nPos)
{
    return m_pNewCell[nID-1]->m_nEccWave[nPos-1];
}

bool NewPan::CellRunStatus(int nID)
{
    return m_pNewCell[nID-1]->GetRunStatus();
}

void NewPan::CreateCellPath(int nID)
{
     m_pNewCell[nID-1]->CreatePath();
}

void NewPan::PlayRun()
{
    killTimer(m_nComTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nSliceIndex++;
        m_nPlayIndex++;
        m_bRunEnd=true;//ÿ�ζ����м����������
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
            emit ShowText("������ɣ�");
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
        m_pNewPaint->update();
        break;
    default:
        return;
    }
    m_nComTimerID=this->startTimer(RUN_TIME);
}

void NewPan::PlayDeal()
{
    killTimer(m_nLastTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nPosIndex++;
        m_nPlayIndex++;
    case 1:
        if(m_nPosIndex<=m_nCellWavePos)
        {
            for(int i=1;i<=NEW_NUM;i++)
            {
                SetCellWavePos(i,m_nPosIndex);
                SetCellRunStyle(i,RUN_LAST);
                SetCellEccRadius(i,MIN_RADIUS);
                CreateCellPath(i);
            }
        }
        else
        {
            m_nPosIndex=0;
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            emit ShowText("���н�����");
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
        m_pNewPaint->update();
        m_nPlayIndex++;
    case 6:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nLastTimerID=this->startTimer(RUN_TIME);
}

void NewPan::RunCellValue()
{
    for (int i = 1; i <= NEW_NUM;i++)
    {
        //���еĵ�Ԫ���и�ֵ
        if(CellRunStatus(i))
        {
            //��������չ��
            if(m_nSliceIndex <= CenTargetSlice(i))
            {
                SetCellCenSlice(i,m_nSliceIndex);
                m_bRunEnd=false;
            }
            //������ʣ��չ��������ƫ���������ͬ��С��ʱ��ƫ���Ὺʼչ��
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
        SetCellEccRadius(i,MAX_RADIUS);
        CreateCellPath(i);
    }
}

void NewPan::CheckIntersects()
{
    //�����ܷ�����ײ��������Ԫ����Ԫ������Χ�ĵ�Ԫ���м��
    m_nPathIndex=0;
    for (int i = 0; i < m_nRunNum;i++)
    {
        for(int j = 1;j <= NEW_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(nDistance==INT_SPACE)
            {
                QPainterPath path1=m_pNewCell[m_nRunID[i]-1]->GetCenPath();
                QPainterPath path2=m_pNewCell[m_nRunID[i]-1]->GetEccPath();
                QPainterPath path3=m_pNewCell[j-1]->GetCenPath();
                QPainterPath path4=m_pNewCell[j-1]->GetEccPath();
                bool b1=path1.intersects(path4);
                bool b2=path2.intersects(path3);
                bool b3=path2.intersects(path4);
                if(b1)
                {
                    m_pathInsertects[m_nPathIndex]=path1.intersected(path4);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j);
                }
                if(b2)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path3);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j);
                }
                if(b3)
                {
                    m_pathInsertects[m_nPathIndex]=path2.intersected(path4);
                    m_nPathIndex++;
                    emit SendCollision(m_nRunID[i],j);
                }
            }
        }
    }
}


void NewPan::PlaySolution()
{
    killTimer(m_nPlanTimerID);
    switch(m_nPlayIndex)
    {
    case 0:
        m_nCellWavePos++;
        m_nPlayIndex++;
        m_bDealEnd=true;
    case 1:
        if(m_nCellWavePos>WAVE_NUM)
        {
            m_nPlayIndex=0;
            m_bRunStatus=false;
            m_nRunMode=RUN_NULL;
            emit ShowText("�������");
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
            m_nRunMode=RUN_NULL;
            emit ShowText("������ɣ�");
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
        m_pNewPaint->update();
        m_nPlayIndex++;
    case 7:
        m_nPlayIndex=0;
        break;
    default:
        return;
    }
    m_nPlanTimerID=this->startTimer(RUN_TIME);
}

void NewPan::DealCellPos()
{
    //λ����ǰ1��λ����ͣ0��Ȼ�������ײ�ж�
    for(int i=1;i<=NEW_NUM;i++)
    {
        //���еĵ�Ԫ,���в���λ��ֵ����
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
        SetCellEccRadius(i,MAX_RADIUS);
        CreateCellPath(i);
    }
}

int NewPan::DetectCollision(int nID, int mID)
{
    int nValue=0;
    CreateCellPath(nID);
    CreateCellPath(mID);
    QPainterPath path1=m_pNewCell[nID-1]->GetCenPath();
    QPainterPath path2=m_pNewCell[nID-1]->GetEccPath();
    QPainterPath path3=m_pNewCell[mID-1]->GetCenPath();
    QPainterPath path4=m_pNewCell[mID-1]->GetEccPath();
    bool b1=path1.intersects(path4);//n��������mƫ����
    bool b2=path2.intersects(path3);//nƫ������m������
    bool b3=path2.intersects(path4);//nƫ������mƫ����
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

void NewPan::DealSolution()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= NEW_NUM;j++)
        {
            int nDistance=CellDistance(m_nRunID[i],j);
            if(nDistance==INT_SPACE)
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

void NewPan::DealCollision(int nID, int mID, int nPos, int nResult)
{
    int nCE=nResult/100;     //n��������mƫ����������ƫ����һ����չ����
    int nEC=nResult%100/10;  //nƫ������m������������ƫ����һ����չ����
    int nEE=nResult%10;      //nƫ������mƫ����������ƫ����������һ����չ����
    if(nCE)
    {
        DealCenMethod(nID,mID,nPos);
    }
    if(nEC)
    {
        DealCenMethod(mID,nID,nPos);
    }
    if(nEE)
    {
        int nBasis=m_mapResult[QString::number(nID,10)+QString::number(mID,10)];
        int mBasis=m_mapResult[QString::number(mID,10)+QString::number(nID,10)];
        //nID�ѵ����Ŀ��λ�ã�mIDδ�����Ŀ��λ��
        if(m_bDealRuned[nID-1]&&!m_bDealRuned[mID-1])
        {
            DealEccMethod(nID,mID,nPos,nBasis);
        }
        //mID�ѵ����Ŀ��λ�ã�nIDδ�����Ŀ��λ��
        else if(m_bDealRuned[mID-1]&&!m_bDealRuned[nID-1])
        {
            DealEccMethod(mID,nID,nPos,mBasis);
        }
        //nID�ѵ����Ŀ��λ�ã�mID�ѵ����Ŀ��λ��
        else if(m_bDealRuned[nID-1]&&m_bDealRuned[mID-1])
        {
            if(CalCenSlice(nID,nPos)==CenTargetSlice(nID))
            {
                DealEccMethod(nID,mID,nPos,nBasis);
            }
            else
            {
                DealEccMethod(mID,nID,nPos,mBasis);
            }
        }
        //nIDδ�����Ŀ��λ�ã�mIDδ�����Ŀ��λ��
        else
        {
            if(EccTargetSlice(nID)-CalEccSlice(nID,nPos)<=EccTargetSlice(mID)-CalEccSlice(mID,nPos))
            {
                DealEccMethod(nID,mID,nPos,nBasis);
            }
            else
            {
                DealEccMethod(mID,nID,nPos,mBasis);
            }
        }
    }
}

void NewPan::DealCenMethod(int nID, int mID, int nPos)
{
    /// nID��Ԫ��������mID��Ԫƫ������ײ����nID��Ԫ������Ϊ���ݣ�����mID��Ԫ�����ᡢƫ����
    //nID��Ԫ���˶���������ת����Ŀ��λ��
    if(CenTargetSlice(nID)==CalCenSlice(nID,nPos))
    {
        //��ǰλ�����Ѿ�����mID������Ŀ��λ�ã�ֻ�ܴ���������
        if(CenTargetSlice(mID)<nPos)
        {
            DealCen(mID,nPos);
        }
        //��ǰmID�����ỹ��ת��������ƫ����
        else
        {
            DealEcc(mID,nPos);
        }
    }
    //nID��Ԫδת����Ŀ��λ��
    else
    {
        //ƫ����ת����0ʱ������������������
        DealEcc(mID,nPos);
    }
}

void NewPan::DealEccMethod(int nID, int mID, int nPos, int nBasis)
{
    /// nID��Ԫƫ������mID��Ԫƫ������������nID��Ԫ��������Ŀ��λ��ʱ��ƫ�����0��Ŀ��λ��ת��
    /// ��mID��ԪĿ��λ�õ���ײ���Ϊ���ݣ�����nID��mID��Ԫ���
    switch(nBasis)
    {
    //nID��Ԫƫ����ɨ����������mID��ԪĿ��λ������ײ
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
    //nID��Ԫƫ����ɨ����������mID��Ԫ������Ŀ��λ����ײ
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
    //nID��Ԫƫ����ɨ����������mID��Ԫƫ����Ŀ��λ����ײ
    case BASIS_ECC:
        DealCen(nID,nPos);
        break;
    //nID��Ԫƫ����ɨ����������mID��ԪĿ��λ�ö���ײ
    case BASIS_CEC:
        DealCen(nID,nPos);
        break;
    default:
        break;
    }
}

void NewPan::SolutionBasis()
{
    for(int i=0;i<m_nRunNum;i++)
    {
        for(int j = 1;j <= NEW_NUM;j++)
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

int NewPan::DealBasis(int nID, int mID)
{
    //nID��Ԫ��������Ŀ��λ�ã�ƫ�����0��Ŀ��λ���Ƿ���mID��������/ƫ����Ŀ��λ����ײ
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

void NewPan::DealNearCell(int nID, int mID, int nPos)
{
    for(int i=1;i<=NEW_NUM;i++)
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

void NewPan::DealEachPos(int nID, int mID, int nPos)
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

void NewPan::DealCen(int nID, int nPos)
{
    //����֮ǰ�Ѳ��ηֶδ���2����Ϊ3�Σ����ֻ��3�Σ�������2��3������������
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
    //����֮��Ϊ3�εĲ�����Ϊ4�Σ�������2��3������������
    //��4�Σ������ѵ���Ŀ��λ�ã�1�Σ��������ͣDEAL_SLICE��λ�ã�2�Σ�����Ŀ��λ�����е���DEAL_SLICE����λ�ã�
    // 3�Σ�����Ŀ��λ�ú���ͣ��DEAL_SLICE��λ�ã�4�Σ������DEAL_SLICE��λ�ú󵽴�Ŀ��λ�ã�
    DealCenWave(nID,3);
}

void NewPan::DealEcc(int nID, int nPos)
{
    //����֮ǰ�Ѳ��ηֶδ���2����Ϊ3�Σ����ֻ��3�Σ�������2��3������������
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
    //����֮��Ϊ3�εĲ�����Ϊ4�Σ�������2��3������������
    //��4�Σ������ѵ���Ŀ��λ�ã�1�Σ��������ͣDEAL_SLICE��λ�ã�2�Σ�����Ŀ��λ�����е���DEAL_SLICE����λ�ã�
    // 3�Σ�����Ŀ��λ�ú���ͣ��DEAL_SLICE��λ�ã�4�Σ������DEAL_SLICE��λ�ú󵽴�Ŀ��λ�ã�
    DealEccWave(nID,3);
}

void NewPan::DealCenWave(int nID, int nPart)
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

void NewPan::DealEccWave(int nID, int nPart)
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

void NewPan::ResetCell(int nID, bool bStatus,double dRadius,double dRadian)
{
    delete m_pNewCell[nID-1];
    m_pNewCell[nID-1] = new NewCell(nID, bStatus, m_pCoord->m_dNewCoord[nID-1][0], m_pCoord->m_dNewCoord[nID-1][1]);
    m_pNewCell[nID-1]->InitCell(this,dRadius,dRadian);
    m_bDealRuned[nID-1] = !bStatus;
}


void NewPan::CommonRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nSliceIndex=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_COM;
    m_nComTimerID=this->startTimer(RUN_TIME);
}

void NewPan::PauseRun()
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

void NewPan::StopRun()
{
    m_bStop=true;
}

void NewPan::ResetRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nSliceIndex=0;//ת��������Ϊ0
    m_nPlayIndex=0;
    m_bRunStatus=false;
    for (int i = 0; i < m_nRunNum;i++)
    {
        SetCellCenSlice(m_nRunID[i],0);
        SetCellEccSlice(m_nRunID[i],0);
        SetCellRunStyle(m_nRunID[i],RUN_COM);
        SetCellEccRadius(m_nRunID[i],MAX_RADIUS);
    }
    m_pNewPaint->update();
}

void NewPan::CheckEndStatus()
{
    for(int i=1;i<=CELL_NUM;i++)
    {
        SetCellFinalValue(i);
    }
    m_pNewPaint->update();
}

void NewPan::DealRandom()
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
                int nResult=DetectCollision(m_nRunID[i],j);
                int nCE=nResult/100;                       //n��������mƫ����������ƫ����һ����չ����
                int nEC=nResult%100/10;                    //nƫ������m������������ƫ����һ����չ����
                int nEE=nResult%10;                        //nƫ������mƫ����������ƫ����������һ����չ����
                if(nCE)
                {
                    ResetCell(j);
                }
                else if(nEC)
                {
                    ResetCell(m_nRunID[i]);
                }
                else if(nEE)
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
                        double iRadius=m_pNewCell[m_nRunID[i]-1]->GetRadius();
                        double iRadian=m_pNewCell[m_nRunID[i]-1]->GetRunRadian();
                        double jRadius=m_pNewCell[j-1]->GetRadius();
                        double jRadian=m_pNewCell[j-1]->GetRunRadian();
                        double icx=m_pNewCell[m_nRunID[i]-1]->GetCenterX();
                        double icy=m_pNewCell[m_nRunID[i]-1]->GetCenterY();
                        double jcx=m_pNewCell[j-1]->GetCenterX();
                        double jcy=m_pNewCell[j-1]->GetCenterY();
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
                        if(DetectCollision(m_nRunID[i],j))
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
    m_pNewPaint->update();
}

void NewPan::OverInitPan(QString sPath)
{
    m_sFilePath=sPath;
    for (int i = 0; i < CELL_NUM;++i)
    {
        if (m_pNewCell[i]!=NULL)
        {
            delete m_pNewCell[i];
        }
    }
    InitCheckCell();
    m_pNewPaint->update();
}

void NewPan::PlanRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nCellWavePos=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_PLAN;
    for(int j=0;j<m_nRunNum;j++)
    {
        m_bDealRuned[m_nRunID[j]-1] = false;
    }
    m_nPlanTimerID=this->startTimer(RUN_TIME);
}

void NewPan::LastRun()
{
    killTimer(m_nComTimerID);
    killTimer(m_nPlanTimerID);
    killTimer(m_nLastTimerID);
    m_nPlayIndex=0;
    m_nPosIndex=0;
    m_bRunStatus=true;
    m_nRunMode=RUN_LAST;
    m_nLastTimerID=this->startTimer(RUN_TIME);
}

void NewPan::timerEvent(QTimerEvent *event)
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
