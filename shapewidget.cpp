#include "shapewidget.h"

ShapeWidget::ShapeWidget(PaintArea *pPaintArea,Pan *pPan) : m_pPaintArea(pPaintArea),m_pPan(pPan)
{
    setPalette(QPalette(Qt:: white));
    setAutoFillBackground(true);
    setMinimumSize(400,300);

    m_bCen=true;
    m_bEcc=false;

    m_pCellLabel=new QLabel(tr("单元号:"));
    m_pCellIDEdit=new QLineEdit;
    m_pBtn=new QPushButton(tr("坐标轴"));
    m_pAxis1=new AxisWidget(m_bCen);
    m_pAxis2=new AxisWidget(m_bEcc);

    m_pCellLayout=new QHBoxLayout();
    m_pCellLayout->addWidget(m_pCellLabel);
    m_pCellLayout->addWidget(m_pCellIDEdit);
    m_pCellLayout->addWidget(m_pBtn);

    m_pMainLayout=new QGridLayout(this);
    m_pMainLayout->addLayout(m_pCellLayout,0,0);
    m_pMainLayout->addWidget(m_pAxis1);
    m_pMainLayout->addWidget(m_pAxis2);
    m_pMainLayout->setRowStretch(0,1);
    m_pMainLayout->setRowStretch(1,3);
    m_pMainLayout->setRowStretch(2,3);

    connect(m_pBtn,SIGNAL(clicked(bool)),this,SLOT(PressBtn()));
    connect(m_pPan,SIGNAL(SendShapeValue(QString,QString,QString)),
            this,SLOT(ReceiveShapeValue(QString,QString,QString)));
    m_pCellIDEdit->setText("1");
}

ShapeWidget::~ShapeWidget()
{
    delete m_pAxis1;
    delete m_pAxis2;
}

void ShapeWidget::ReceiveShapeValue(QString sID, QString sCenData,QString sEccData)
{
    m_pCellIDEdit->setText(sID);
    m_pAxis1->ReceiveValue(sID,sCenData);
    m_pAxis2->ReceiveValue(sID,sEccData);
    m_pAxis1->update();
    m_pAxis2->update();
}

void ShapeWidget::PressBtn()
{
    QString sID=m_pCellIDEdit->text().trimmed();
    bool b;
    int nID=sID.toInt(&b);
    if(nID>0&&nID<=CELL_NUM)
    {
        QString sc="";
        QString se="";
        for(int i=1;i<=WAVE_NUM;i++)
        {
            if(m_pPan->CellCenPosValue(nID,i)!=BYTE_NULL)
            {
                sc+=QString::number(m_pPan->CellCenPosValue(nID,i));
            }
            if(m_pPan->CellEccPosValue(nID,i)!=BYTE_NULL)
            {
                se+=QString::number(m_pPan->CellEccPosValue(nID,i));
            }
        }
        m_pAxis1->ReceiveValue(sID,sc);
        m_pAxis2->ReceiveValue(sID,se);
        m_pAxis1->update();
        m_pAxis2->update();
    }
}


