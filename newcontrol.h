#ifndef NEWCONTROL_H
#define NEWCONTROL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include "newpan.h"

class NewPan;
class NewControl : public QWidget
{
    Q_OBJECT
public:
    explicit NewControl(NewPan *pNewPan);

signals:
    void Path(QString);
    void Progress();
public slots:
    void OpenFile();
    void ReadPath();
    void ChangeText();
    void ReceiveCollision(int nID, int mID);
    void SetTextEdit(QString sTemp);
    void ClearText();
    void SetProgressBar();

private:
    QLabel      *m_pFilePath;
    QLineEdit   *m_pLineText;
    QPushButton *m_pOpen;
    QPushButton *m_pRead;
    QPushButton *m_pCheckBtn;
    QPushButton *m_pRandomBtn;
    QPushButton *m_pRunBtn;
    QPushButton *m_pPause;
    QPushButton *m_pStop;
    QPushButton *m_pResetBtn;
    QTextEdit   *m_pShowText;
    QPushButton *m_pCalculateBtn;
    QProgressBar*m_pProgressBar;
    QPushButton *m_pRunShapeBtn;
    QLabel      *m_pLabel;
    QPushButton *m_pClearBtn;

    QHBoxLayout *m_pFileLayout;
    QGridLayout *m_pBtnLayout;
    QGridLayout *m_pInfoLayout;
    QGridLayout *m_pMainLayout;

private:
    QString   m_sFilePath;
    QString   m_sText;
    NewPan*   m_pNewPan;
};

#endif // NEWCONTROL_H
