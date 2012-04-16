#include "addrselectiondlg.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QApplication>

const static QString AVAIL_ADDR = "Available IP Address(es)";
const static QString OK = "Ok";
const static QString CANCEL = "Cancel";

AddrSelectionDlg::AddrSelectionDlg(QWidget *parent) :
    QDialog(parent), mTableAddr(0), mBtnOk(0), mBtnCancel(0)
{
    init();
    listAddresses();
}

void AddrSelectionDlg::init()
{
    createWidgets();
    setupLayout();
    setupEventHandlers();
}

void AddrSelectionDlg::createWidgets()
{
    mTableAddr = new QTableWidget(0, 1, this);
    QTableWidgetItem* item = new QTableWidgetItem(AVAIL_ADDR);
    mTableAddr->setHorizontalHeaderItem(0, item);
    mTableAddr->setShowGrid(true);
    mTableAddr->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    mBtnOk = new QPushButton(OK, this);
    mBtnOk->setDisabled(true);
    mBtnCancel = new QPushButton(CANCEL, this);
}

void AddrSelectionDlg::setupLayout()
{
    QWidget* group = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(mBtnOk);
    hLayout->addWidget(mBtnCancel);
    group->setLayout(hLayout);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(mTableAddr);
    vLayout->addWidget(group);
    this->setLayout(vLayout);
}

void AddrSelectionDlg::setupEventHandlers()
{
    QObject::connect(mBtnOk, SIGNAL(clicked()), this, SLOT(saveAddr()));
    QObject::connect(mBtnCancel, SIGNAL(clicked()), this, SLOT(closeAndExit()));
    QObject::connect(mTableAddr, SIGNAL(cellClicked(int, int)), this, SLOT(selectItem(int, int)));
}

void AddrSelectionDlg::saveAddr()
{
    emit addrSaved(mAddr);
    this->close();
}

void AddrSelectionDlg::selectItem(int row, int column)
{
        mAddr = mTableAddr->item(row, column)->text();
        mBtnOk->setEnabled(true);
}

void AddrSelectionDlg::listAddresses()
{
    QNetworkInterface *qni = new QNetworkInterface();
    QList<QHostAddress> allAddrs = qni->allAddresses();

    foreach (QHostAddress mAddr, allAddrs)
    {
        QTableWidgetItem* item = new QTableWidgetItem(mAddr.toString());
        int curRow = mTableAddr->rowCount();
        mTableAddr->insertRow(curRow);
        mTableAddr->setItem(curRow, 0, item);
    }
    delete qni;
}

void AddrSelectionDlg::closeAndExit()
{
    this->close();
    if (mAddr.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Local address not selected!"));
        QApplication::exit(-1);
    }
}
