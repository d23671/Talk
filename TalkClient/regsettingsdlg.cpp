#include "regsettingsdlg.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

const static QString NAME = "Nick name";
const static QString ADDR = "Serv Addr";
const static QString PORT = "Serv Port";
const static QString OK = "Ok";
const static QString CANCEL = "Cancel";

RegSettingsDlg::RegSettingsDlg(QWidget *parent) :
        QDialog(parent),
        mLabelName(0),
        mLabelSrvAddr(0),
        mLabelSrvPort(0),
        mLeName(0),
        mLeSrvAddr(0),
        mLeSrvPort(0),
        mBtnOk(0),
        mBtnCancel(0),
        mName(),
        mAddr(),
        mPort(0xFFFF)
{
    init();
}

void RegSettingsDlg::init()
{
    createWidgets();
    setupLayout();
    setupEventHandlers();
}

void RegSettingsDlg::createWidgets()
{
    mLabelName = new QLabel(NAME, this);
    mLabelSrvAddr = new QLabel(ADDR, this);
    mLabelSrvPort = new QLabel(PORT, this);
    mLeName = new QLineEdit(this);
    mLeSrvAddr = new QLineEdit(this);
    mLeSrvPort = new QLineEdit(this);
    mBtnOk = new QPushButton(OK, this);
    mBtnOk->setEnabled(false);
    mBtnCancel = new QPushButton(CANCEL, this);
}

void RegSettingsDlg::setupLayout()
{
    QWidget* group1 = new QWidget(this);
    QHBoxLayout* hLayout1 = new QHBoxLayout(this);
    hLayout1->addWidget(mLabelName);
    hLayout1->addWidget(mLeName);
    group1->setLayout(hLayout1);

    QWidget* group2 = new QWidget(this);
    QHBoxLayout* hLayout2 = new QHBoxLayout(this);
    hLayout2->addWidget(mLabelSrvAddr);
    hLayout2->addWidget(mLeSrvAddr);
    group2->setLayout(hLayout2);

    QWidget* group3 = new QWidget(this);
    QHBoxLayout* hLayout3 = new QHBoxLayout(this);
    hLayout3->addWidget(mLabelSrvPort);
    hLayout3->addWidget(mLeSrvPort);
    group3->setLayout(hLayout3);

    QWidget* group4 = new QWidget(this);
    QHBoxLayout* hLayout4 = new QHBoxLayout(this);
    hLayout4->addWidget(mBtnOk);
    hLayout4->addWidget(mBtnCancel);
    group4->setLayout(hLayout4);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(group1);
    vLayout->addWidget(group2);
    vLayout->addWidget(group3);
    vLayout->addWidget(group4);
    this->setLayout(vLayout);
}

void RegSettingsDlg::setupEventHandlers()
{
    QObject::connect(mLeName, SIGNAL(textChanged(QString)), this, SLOT(getName(QString)));
    QObject::connect(mLeSrvAddr, SIGNAL(textChanged(QString)), this, SLOT(getAddr(QString)));
    QObject::connect(mLeSrvPort, SIGNAL(textChanged(QString)), this, SLOT(getPort(QString)));
    QObject::connect(mBtnOk, SIGNAL(clicked()), this, SLOT(saveSettings()));
    QObject::connect(mBtnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void RegSettingsDlg::setBtnOkEnabled()
{
    mBtnOk->setEnabled(!mName.isEmpty() && !mAddr.isEmpty() && mPort != 0xFFFF);
}

void RegSettingsDlg::saveSettings()
{
    emit settingsSaved(mName, mAddr, mPort);
    this->close();
}

void RegSettingsDlg::getAddr(QString addr)
{
    mAddr = addr;
    setBtnOkEnabled();
}

void RegSettingsDlg::getPort(QString port)
{
    bool ok = false;
    mPort = port.toUShort(&ok);

    if (!ok)
        mPort = 0xFFFF;

    setBtnOkEnabled();
}

void RegSettingsDlg::getName(QString name)
{
    mName = name;
    setBtnOkEnabled();
}
