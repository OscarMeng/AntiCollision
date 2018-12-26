#include "controlwidget.h"

ControlWidget::ControlWidget(Pan *pPan) : m_pPan(pPan)
{
    setPalette(QPalette(Qt::white));
    setAutoFillBackground(true);
    setMinimumSize(400,400);

    m_pFilePath=new QLabel(tr("文件路径:"));
    m_pLineText=new QLineEdit;
    m_sFilePath="E:/QAntiCollision/File/CheckCenter.txt";
    m_pLineText->setText(m_sFilePath);
    m_pOpen=new QPushButton(tr("打开"));
    connect(m_pOpen,SIGNAL(clicked()),this,SLOT(OpenFile()));
    m_pRead=new QPushButton(tr("读取"));
    connect(m_pRead,SIGNAL(clicked()),this,SLOT(ReadPath()));
    connect(this,SIGNAL(Path(QString)),m_pPan,SLOT(OverInitPan(QString)));

    m_pRunBtn=new QPushButton(tr("运行"));
    connect(m_pRunBtn,SIGNAL(clicked()),m_pPan,SLOT(ShowRun()));
    m_pPause=new QPushButton(tr("暂停"));
    connect(m_pPause,SIGNAL(clicked()),this,SLOT(ChangeText()));
    connect(m_pPause,SIGNAL(clicked()),m_pPan,SLOT(PauseRun()));
    m_pResetBtn=new QPushButton(tr("重置"));
    connect(m_pResetBtn,SIGNAL(clicked()),m_pPan,SLOT(ResetRun()));
    m_pCheckBtn=new QPushButton(tr("检测"));//检测最终位置点是否碰撞，如果碰撞不能运行
    connect(m_pCheckBtn,SIGNAL(clicked()),m_pPan,SLOT(CheckEndStatus()));

    m_pCalculateBtn=new QPushButton(tr("处理碰撞"));
    connect(m_pCalculateBtn,SIGNAL(clicked()),m_pPan,SLOT(PlanSolution()));
    m_pProgressBar=new QProgressBar;
    m_pProgressBar->setRange(0,999);
    m_pProgressBar->setValue(0);
    connect(m_pPan,SIGNAL(DealProgress()),this,SLOT(SetProgressBar()));

    m_pRunShapeBtn=new QPushButton(tr("结果运行"));
    connect(m_pRunShapeBtn,SIGNAL(clicked()),m_pPan,SLOT(RunShape()));
    m_pShowText=new QTextEdit;

    m_pLabel=new QLabel(tr("信息如下:"));
    connect(m_pPan,SIGNAL(ShowText(QString)),this,SLOT(SetTextEdit(QString)));
    m_pClearBtn=new QPushButton(tr("清空"));
    connect(m_pClearBtn,SIGNAL(clicked()),this,SLOT(ClearText()));
    m_pFileLayout=new QHBoxLayout();
    m_pFileLayout->setSpacing(10);
    m_pFileLayout->addWidget(m_pFilePath);
    m_pFileLayout->addWidget(m_pLineText);
    m_pFileLayout->addWidget(m_pOpen);
    m_pFileLayout->addWidget(m_pRead);

    m_pBtnLayout=new QGridLayout();
    m_pBtnLayout->setSpacing(10);
    m_pBtnLayout->addWidget(m_pCheckBtn,0,0,1,1);
    m_pBtnLayout->addWidget(m_pRunBtn,0,1,1,1);
    m_pBtnLayout->addWidget(m_pPause,0,2,1,1);
    m_pBtnLayout->addWidget(m_pResetBtn,0,3,1,1);
    m_pBtnLayout->addWidget(m_pCalculateBtn,1,0,1,1);
    m_pBtnLayout->addWidget(m_pProgressBar,1,1,1,2);
    m_pBtnLayout->addWidget(m_pRunShapeBtn,1,3,1,1);

    m_pInfoLayout =new QGridLayout();
    m_pInfoLayout->setSpacing(10);
    m_pInfoLayout->addWidget(m_pLabel,0,0,1,1,Qt::AlignLeft);
    m_pInfoLayout->addWidget(m_pClearBtn,0,3,1,1);
    m_pInfoLayout->addWidget(m_pShowText,1,0,5,4);

    m_pMainLayout=new QGridLayout(this);
    m_pMainLayout->setMargin(10);
    m_pMainLayout->setSpacing(10);
    m_pMainLayout->addLayout(m_pFileLayout,0,0,1,1);
    m_pMainLayout->addLayout(m_pBtnLayout,1,0,1,1);
    m_pMainLayout->addLayout(m_pInfoLayout,2,0,1,1);
}

void ControlWidget::OpenFile()
{
    m_sFilePath=QFileDialog::getOpenFileName(
                this,
                tr("选择文件"),
                QDir::currentPath(),
                tr("All Files(*);;Text Files(*.txt)"));
    if(!m_sFilePath.isNull())
    {
        m_pLineText->setText(m_sFilePath);
    }
}

void ControlWidget::ReadPath()
{
    QString s=m_pLineText->text();
    emit Path(s);
}

void ControlWidget::ChangeText()
{
    QString s=m_pPause->text();
    if(s=="暂停")
    {
        m_pPause->setText("继续");
    }
    else
    {
        m_pPause->setText("暂停");
    }
}

void ControlWidget::SetTextEdit(QString s)
{
    if(!m_sText.contains(s))
    {
        m_sText+=s;
        m_sText+="\n";
        m_pShowText->setText(m_sText);
    }
    m_pShowText->moveCursor(QTextCursor::End);//最下行显示
}

void ControlWidget::ClearText()
{
    m_sText="";
    m_pShowText->setText(m_sText);
}

void ControlWidget::SetProgressBar()
{
    int nCenPos=0;
    int nEccPos=0;
    for(int i=0;i<m_pPan->m_nRunNum;i++)
    {
        if(nCenPos<m_pPan->CellCenFinalPos(m_pPan->m_nRunID[i]))
        {
            nCenPos=m_pPan->CellCenFinalPos(m_pPan->m_nRunID[i]);
        }
        if(nEccPos<m_pPan->CellEccFinalPos(m_pPan->m_nRunID[i]))
        {
            nEccPos=m_pPan->CellEccFinalPos(m_pPan->m_nRunID[i]);
        }
    }
    int nPos=m_pPan->GetWavePos();
    if(nCenPos>=nEccPos)
    {
        m_pProgressBar->setValue(double(nPos+1)/nCenPos*999);
    }
    else
    {
        m_pProgressBar->setValue(double(nPos+1)/nEccPos*999);
    }
}


