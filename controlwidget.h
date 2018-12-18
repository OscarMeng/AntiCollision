#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

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
#include "pan.h"

class Pan;
class ControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ControlWidget(Pan *pPan);

signals:
    void Path(QString);
    void Progress();
public slots:
    void OpenFile();
    void ReadPath();
    void ChangeText();
    void SetTextEdit(QString s);
    void ClearText();
    void SetProgressBar();

public:

private:
    QLabel      *m_pFilePath;
    QLineEdit   *m_pLineText;
    QPushButton *m_pOpen;
    QPushButton *m_pRead;
    QPushButton *m_pCheckBtn;
    QPushButton *m_pRunBtn;
    QPushButton *m_pPause;
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
    Pan*      m_pPan;

};

#endif // CONTROLWIDGET_H
