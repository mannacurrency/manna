// Copyright (c) 2009-2012 *coin developers
// where * = (Bit, Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
// Previously distributed under the MIT/X11 software license, see the
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include "codecoinunits.h"
#include "bitcoinamountfield.h"
#include "qvaluecombobox.h"

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QListWidget;
class QListWidgetItem;
class QPushButton;
QT_END_NAMESPACE
class OptionsModel;
class MainOptionsPage;
class DisplayOptionsPage;
class MonitoredDataMapper;

/** Preferences dialog. */
class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OptionsDialog(QWidget *parent=0);

    void setModel(OptionsModel *model);

signals:

public slots:
    /** Change the current page to \a index. */
    void changePage(int index);

private slots:
    void okClicked();
    void cancelClicked();
    void applyClicked();
    void enableApply();
    void disableApply();

private:
    QListWidget *contents_widget;
    QStackedWidget *pages_widget;
    OptionsModel *model;
    MonitoredDataMapper *mapper;
    QPushButton *apply_button;

    // Pages
    MainOptionsPage *main_page;
    DisplayOptionsPage *display_page;

    void setupMainPage();
};

/* First page of options */
class MainOptionsPage : public QWidget
{
	Q_OBJECT
public:
	explicit MainOptionsPage(QWidget *parent=0);

	void setMapper(MonitoredDataMapper *mapper);
private:
	QCheckBox *bitcoin_at_startup;
#ifndef Q_WS_MAC
	QCheckBox *minimize_to_tray;
#endif
	QCheckBox *map_port_upnp;
#ifndef Q_WS_MAC
	QCheckBox *minimize_on_close;
#endif
	QCheckBox *connect_socks4;
	QCheckBox *detach_database;
	QLineEdit *proxy_ip;
	QLineEdit *proxy_port;
	BitcoinAmountField *fee_edit;

signals:

public slots:

};

class DisplayOptionsPage : public QWidget
{
	Q_OBJECT
public:
	explicit DisplayOptionsPage(QWidget *parent=0);

	void setMapper(MonitoredDataMapper *mapper);
private:
	QValueComboBox *unit;
	QCheckBox *display_addresses;
	QCheckBox *coin_control_features;
signals:

public slots:

};

#endif // OPTIONSDIALOG_H
