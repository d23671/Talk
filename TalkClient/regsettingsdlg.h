#ifndef REGSETTINGSDLG_H
#define REGSETTINGSDLG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

/*
 * The Settings includes client's name, server's address and port.
 */
class RegSettingsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit RegSettingsDlg(QWidget *parent = 0);

private:
    void init();
    void createWidgets();
    void setupLayout();
    void setupEventHandlers();
    void setBtnOkEnabled();

signals:
    // Save settings to ClientWindow object.
    void settingsSaved(QString name, QString addr, quint16 port);

private slots:
    // On "Ok" button clicked.
    void saveSettings();

    // On "srv addr" text box edited.
    void getAddr(QString addr);

    // On "srv port" text box edited.
    void getPort(QString port);

    // On "nick name" text box edited.
    void getName(QString name);

private:
    QLabel* mLabelName;
    QLabel* mLabelSrvAddr;
    QLabel* mLabelSrvPort;
    QLineEdit* mLeName;
    QLineEdit* mLeSrvAddr;
    QLineEdit* mLeSrvPort;
    QPushButton* mBtnOk;
    QPushButton* mBtnCancel;
    QString mName;
    QString mAddr;
    quint16 mPort;
};

#endif // REGSETTINGSDLG_H
