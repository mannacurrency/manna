// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2009-2012 *coin developers
// where * = (Bit, Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
// Previously distributed under the MIT/X11 software license, see the
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
#ifndef TRANSACTIONTABLEMODEL_H
#define TRANSACTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class CWallet;
class TransactionTablePriv;
class TransactionRecord;
class WalletModel;

/** UI model for the transaction table of a wallet.
 */
class TransactionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionTableModel(CWallet* wallet, WalletModel *parent = 0);
    ~TransactionTableModel();

    enum ColumnIndex {
        Status = 0,
        Date = 1,
        Type = 2,
        ToAddress = 3,
        Amount = 4
    };

    /** Roles to get specific information from a transaction row.
        These are independent of column.
    */
    enum RoleIndex {
        /** Type of transaction */
        TypeRole = Qt::UserRole,
        /** Date and time this transaction was created */
        DateRole,
        /** Long description (HTML format) */
        LongDescriptionRole,
        /** Address of transaction */
        AddressRole,
        /** Label of address related to transaction */
        LabelRole,
        /** Net amount of transaction */
        AmountRole,
        /** Unique identifier */
        TxIDRole,
        /** Is transaction confirmed? */
        ConfirmedRole,
        /** Formatted amount, without brackets when unconfirmed */
        FormattedAmountRole
    };

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    void refresh();
private:
    CWallet* wallet;
    WalletModel *walletModel;
    QStringList columns;
    TransactionTablePriv *priv;
    int cachedNumBlocks;

    QString lookupAddress(const std::string &address, bool tooltip) const;
    QVariant addressColor(const TransactionRecord *wtx) const;
    QString formatTxStatus(const TransactionRecord *wtx) const;
    QString formatTxDate(const TransactionRecord *wtx) const;
    QString formatTxType(const TransactionRecord *wtx) const;
    QString formatTxToAddress(const TransactionRecord *wtx, bool tooltip) const;
    QString formatTxAmount(const TransactionRecord *wtx, bool showUnconfirmed=true) const;
    QString formatTooltip(const TransactionRecord *rec) const;
    QVariant txStatusDecoration(const TransactionRecord *wtx) const;
    QVariant txAddressDecoration(const TransactionRecord *wtx) const;

public slots:
    void updateTransaction(const QString &hash, int status);
    void updateConfirmations();
    void updateDisplayUnit();

    friend class TransactionTablePriv;
};

#endif // TRANSACTIONTABLEMODEL_H
