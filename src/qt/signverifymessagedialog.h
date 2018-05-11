// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2009-2012 *coin developers
// where * = (Bit, Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
// Previously distributed under the MIT/X11 software license, see the
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
#ifndef SIGNVERIFYMESSAGEDIALOG_H
#define SIGNVERIFYMESSAGEDIALOG_H

#include <QDialog>

class WalletModel;

namespace Ui {
    class SignVerifyMessageDialog;
}

class SignVerifyMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SignVerifyMessageDialog(QWidget *parent);
    ~SignVerifyMessageDialog();

    void setModel(WalletModel *model);
    void setAddress_SM(const QString &address);
    void setAddress_VM(const QString &address);

    void showTab_SM(bool fShow);
    void showTab_VM(bool fShow);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::SignVerifyMessageDialog *ui;
    WalletModel *model;

private slots:
    /* sign message */
    void on_addressBookButton_SM_clicked();
    void on_pasteButton_SM_clicked();
    void on_signMessageButton_SM_clicked();
    void on_copySignatureButton_SM_clicked();
    void on_clearButton_SM_clicked();
    /* verify message */
    void on_addressBookButton_VM_clicked();
    void on_verifyMessageButton_VM_clicked();
    void on_clearButton_VM_clicked();
};

#endif // SIGNVERIFYMESSAGEDIALOG_H
