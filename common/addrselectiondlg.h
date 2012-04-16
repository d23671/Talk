#ifndef ADDRSELECTIONDLG_H
#define ADDRSELECTIONDLG_H

#include <QDialog>

class QTableWidget;
class QPushButton;
class QLabel;
class QLineEdit;

/*
 * To select local address of network interface card.
 * Loopback can be selected as server's address but clients that don't use
 * loopback addr will not be able to connect to server.
 */
class AddrSelectionDlg : public QDialog
{
    Q_OBJECT
public:
    explicit AddrSelectionDlg(QWidget *parent = 0);

private:
    void init();
    void createWidgets();
    void setupLayout();
    void setupEventHandlers();
    void listAddresses();

signals:
    // Local address saved to server/client.
    void addrSaved(QString);

private slots:
    // On OK button clicked.
    void saveAddr();

    // On one of listed addresses selected.
    void selectItem(int row, int column);

    // On close button clicked.
    void closeAndExit();

private:
    QTableWidget* mTableAddr;
    QPushButton* mBtnOk;
    QPushButton* mBtnCancel;

    QString mAddr;
};

#endif // ADDRSELECTIONDLG_H
