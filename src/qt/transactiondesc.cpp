// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2009-2012 *coin developers
// where * = (Bit, Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
// Previously distributed under the MIT/X11 software license, see the
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
#include "transactiondesc.h"

#include "guiutil.h"
#include "codecoinunits.h"
#include "main.h"
#include "wallet.h"
#include "db.h"
#include "ui_interface.h"
#include "base58.h"

#include <QString>

using namespace std;

QString TransactionDesc::FormatTxStatus(const CWalletTx& wtx)
{
    if (!wtx.IsFinal())
    {
        if (wtx.nLockTime < LOCKTIME_THRESHOLD)
            return tr("Open for %1 blocks").arg(nBestHeight - wtx.nLockTime);
        else
            return tr("Open until %1").arg(GUIUtil::dateTimeStr(wtx.nLockTime));
    }
    else
    {
        int nDepth = wtx.GetDepthInMainChain();
        if (GetAdjustedTime() - wtx.nTimeReceived > 2 * 60 && wtx.GetRequestCount() == 0)
            return tr("%1/offline?").arg(nDepth);
        else if (nDepth < 6)
            return tr("%1/unconfirmed").arg(nDepth);
        else
            return tr("%1 confirmations").arg(nDepth);
    }
}

QString TransactionDesc::toHTML(CWallet *wallet, CWalletTx &wtx)
{
    QString strHTML;

    {
        LOCK(wallet->cs_wallet);
        strHTML.reserve(4000);
        strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";

        qint64 nTime = wtx.GetTxTime();
        qint64 nCredit = wtx.GetCredit();
        qint64 nDebit = wtx.GetDebit();
        qint64 nNet = nCredit - nDebit;

        strHTML += tr("<b>Status:</b> ") + FormatTxStatus(wtx);
        int nRequests = wtx.GetRequestCount();
        if (nRequests != -1)
        {
            if (nRequests == 0)
                strHTML += tr(", has not been successfully broadcast yet");
            else if (nRequests == 1)
                strHTML += tr(", broadcast through %1 node").arg(nRequests);
            else
                strHTML += tr(", broadcast through %1 nodes").arg(nRequests);
        }
        strHTML += "<br>";

        strHTML += tr("<b>Date:</b> ") + (nTime ? GUIUtil::dateTimeStr(nTime) : QString("")) + "<br>";

        //
        // From
        //
        if (wtx.IsCoinBase())
        {
            strHTML += tr("<b>Source:</b> Generated<br>");
        }
        else if (!wtx.mapValue["from"].empty())
        {
            // Online transaction
            if (!wtx.mapValue["from"].empty())
                strHTML += tr("<b>From:</b> ") + GUIUtil::HtmlEscape(wtx.mapValue["from"]) + "<br>";
        }
        else
        {
            // Offline transaction
            if (nNet > 0)
            {
                // Credit
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                {
                    if (wallet->IsMine(txout))
                    {
                        CTxDestination address;
                        if (ExtractDestination(txout.scriptPubKey, address) && IsMine(*wallet, address))
                        {
                            if (wallet->mapAddressBook.count(address))
                            {
                                strHTML += tr("<b>From:</b> ") + tr("unknown") + "<br>";
                                strHTML += tr("<b>To:</b> ");
                                strHTML += GUIUtil::HtmlEscape(CBitcoinAddress(address).ToString());
                                if (!wallet->mapAddressBook[address].empty())
                                    strHTML += tr(" (yours, label: ") + GUIUtil::HtmlEscape(wallet->mapAddressBook[address]) + ")";
                                else
                                    strHTML += tr(" (yours)");
                                strHTML += "<br>";
                            }
                        }
                        break;
                    }
                }
            }
        }

        //
        // To
        //
        string strAddress;
        if (!wtx.mapValue["to"].empty())
        {
            // Online transaction
            strAddress = wtx.mapValue["to"];
            strHTML += tr("<b>To:</b> ");
            CTxDestination dest = CBitcoinAddress(strAddress).Get();
            if (wallet->mapAddressBook.count(dest) && !wallet->mapAddressBook[dest].empty())
                strHTML += GUIUtil::HtmlEscape(wallet->mapAddressBook[dest]) + " ";
            strHTML += GUIUtil::HtmlEscape(strAddress) + "<br>";
        }

        //
        // Amount
        //
        if (wtx.IsCoinBase() && nCredit == 0)
        {
            //
            // Coinbase
            //
            qint64 nUnmatured = 0;
            BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                nUnmatured += wallet->GetCredit(txout);
            strHTML += tr("<b>Credit:</b> ");
            if (wtx.IsInMainChain())
                strHTML += tr("(%1 matures in %2 more blocks)")
                        .arg(CodecoinUnits::formatWithUnit(CodecoinUnits::CC, nUnmatured))
                        .arg(wtx.GetBlocksToMaturity());
            else
                strHTML += tr("(not accepted)");
            strHTML += "<br>";
        }
        else if (nNet > 0)
        {
            //
            // Credit
            //
            strHTML += "<b>" + tr("Credit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, nNet) + "<br>";
        }
        else
        {
            bool fAllFromMe = true;
            BOOST_FOREACH(const CTxIn& txin, wtx.vin)
                fAllFromMe = fAllFromMe && wallet->IsMine(txin);

            bool fAllToMe = true;
            BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                fAllToMe = fAllToMe && wallet->IsMine(txout);

            if (fAllFromMe)
            {
                //
                // Debit
                //
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                {
                    if (wallet->IsMine(txout))
                        continue;

                    if (wtx.mapValue["to"].empty())
                    {
                        // Offline transaction
                        CTxDestination address;
                        if (ExtractDestination(txout.scriptPubKey, address))
                        {
                            strHTML += tr("<b>To:</b> ");
                            if (wallet->mapAddressBook.count(address) && !wallet->mapAddressBook[address].empty())
                                strHTML += GUIUtil::HtmlEscape(wallet->mapAddressBook[address]) + " ";
                            strHTML += GUIUtil::HtmlEscape(CBitcoinAddress(address).ToString());
                            strHTML += "<br>";
                        }
                    }

                    strHTML += "<b>" + tr("Debit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, -txout.nValue) + "<br>";
                }

                if (fAllToMe)
                {
                    // Payment to self
                    qint64 nChange = wtx.GetChange();
                    qint64 nValue = nCredit - nChange;
                    strHTML += tr("<b>Debit:</b> ") + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, -nValue) + "<br>";
                    strHTML += tr("<b>Credit:</b> ") + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, nValue) + "<br>";
                }

                qint64 nTxFee = nDebit - wtx.GetValueOut();
                if (nTxFee > 0)
                    strHTML += "<b>" + tr("Transaction fee") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, -nTxFee) + "<br>";
            }
            else
            {
                //
                // Mixed debit transaction
                //
                BOOST_FOREACH(const CTxIn& txin, wtx.vin)
                    if (wallet->IsMine(txin))
                        strHTML += "<b>" + tr("Debit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, -wallet->GetDebit(txin)) + "<br>";
                BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                    if (wallet->IsMine(txout))
                        strHTML += "<b>" + tr("Credit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, wallet->GetCredit(txout)) + "<br>";
            }
        }

        if((!wtx.IsCoinStake() && !wtx.IsCoinBase()) || wtx.GetBlocksToMaturity() == 0)
        {
            strHTML += tr("<b>Net amount:</b> ") + CodecoinUnits::formatWithUnit(CodecoinUnits::CC,nNet, true) + "<br>";
        }else{
            strHTML += tr("<b>Retained amount:</b> %1 until %2 more blocks<br>").arg(CodecoinUnits::formatWithUnit(CodecoinUnits::CC,-nNet)).arg(wtx.GetBlocksToMaturity());
        }

        //
        // Message
        //
        if (!wtx.mapValue["message"].empty())
            strHTML += QString("<br><b>") + tr("Message:") + "</b><br>" + GUIUtil::HtmlEscape(wtx.mapValue["message"], true) + "<br>";
        if (!wtx.mapValue["comment"].empty())
            strHTML += QString("<br><b>") + tr("Comment:") + "</b><br>" + GUIUtil::HtmlEscape(wtx.mapValue["comment"], true) + "<br>";

        strHTML += QString("<b>") + tr("Transaction ID:") + "</b> " + wtx.GetHash().ToString().c_str() + "<br>";

        if (wtx.IsCoinBase())
            strHTML += QString("<br>") + tr("Generated coins must wait 520 blocks before they can be spent.  When you generated this block, it was broadcast to the network to be added to the block chain.  If it fails to get into the chain, it will change to \"not accepted\" and not be spendable.  This may occasionally happen if another node generates a block within a few seconds of yours.") + "<br>";
        if (wtx.IsCoinStake())
            strHTML += QString("<br>") + tr("Staked coins must wait 520 blocks before they can return to balance and be spent.  When you generated this proof-of-stake block, it was broadcast to the network to be added to the block chain.  If it fails to get into the chain, it will change to \"not accepted\" and not be a valid stake.  This may occasionally happen if another node generates a proof-of-stake block within a few seconds of yours.") + "<br>";

        //
        // Debug view
        //
        if (fDebug)
        {
            strHTML += "<hr><br>" + tr("Debug information") + "<br><br>";
            BOOST_FOREACH(const CTxIn& txin, wtx.vin)
                if(wallet->IsMine(txin))
                    strHTML += "<b>" + tr("Debit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, -wallet->GetDebit(txin)) + "<br>";
            BOOST_FOREACH(const CTxOut& txout, wtx.vout)
                if(wallet->IsMine(txout))
                    strHTML += "<b>" + tr("Credit") + ":</b> " + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, wallet->GetCredit(txout)) + "<br>";

            strHTML += "<br><b>Transaction:</b><br>";
            strHTML += GUIUtil::HtmlEscape(wtx.ToString(), true);

            strHTML += "<br><b>" + tr("Inputs") + ":</b>";
            strHTML += "<ul>";

            {
                LOCK(wallet->cs_wallet);
                BOOST_FOREACH(const CTxIn& txin, wtx.vin)
                {
                    COutPoint prevout = txin.prevout;

                    CCoins prev;
                    if(pcoinsTip->GetCoins(prevout.hash, prev))
                    {
                        if (prevout.n < prev.vout.size())
                        {
                            strHTML += "<li>";
                            const CTxOut &vout = prev.vout[prevout.n];
                            CTxDestination address;
                            if (ExtractDestination(vout.scriptPubKey, address))
                            {
                                if (wallet->mapAddressBook.count(address) && !wallet->mapAddressBook[address].empty())
                                    strHTML += GUIUtil::HtmlEscape(wallet->mapAddressBook[address]) + " ";
                                strHTML += QString::fromStdString(CBitcoinAddress(address).ToString());
                            }
                            strHTML = strHTML + " " + tr("Amount") + "=" + CodecoinUnits::formatWithUnit(CodecoinUnits::CC, vout.nValue);
                            strHTML = strHTML + " IsMine=" + (wallet->IsMine(vout) ? tr("true") : tr("false")) + "</li>";
                        }
                    }
                }
            }
            strHTML += "</ul>";
        }

        strHTML += "</font></html>";
    }
    return strHTML;
}
