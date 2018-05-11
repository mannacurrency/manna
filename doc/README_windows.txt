Grantcoin 0.2.0 BETA

Copyright (c) 2015 Grantcoin Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com).


Intro
-----
Grantcoin is a free open source project derived from Peercoin (PPCoin),
with the goal of providing a long-term energy-efficient cryptocurrency
which is primarily distributed by the Grantcoin Foundation, a legally
incorporated nonprofit organization, as Proof-of-Participation grants to
support and incentivize sustainable business around the world. Built on
the technological foundations of Bitcoin and Peercoin, Grantcoin adds
organizational innovation to make digital currency serve the cause of
social and environmental responsibility and a fair distribution of wealth.


Setup
-----
After completing windows setup then run grantcoin-qt.
Alternatively you can run windows command line (cmd) in grantcoin program dir.
  cd daemon
  grantcoind
You would need to create a configuration file grantcoin.conf in the default
wallet directory. Grant access to grantcoind/grantcoin-qt in anti-virus and
firewall applications if necessary.

The software automatically finds other nodes to connect to.  You can
enable Universal Plug and Play (UPnP) with your router/firewall
or forward port 9982 (TCP) to your computer so you can receive
incoming connections.  Grantcoin works without incoming connections,
but allowing incoming connections helps the Grantcoin network.


See the documentation at the Grantcoin website and GitHub:
  http://www.grantcoin.org
  http://github.com/grantcoin/grantcoin
for help and more information.


----------------------------
Peercoin (PPCoin) 0.4.0 BETA

Copyright (c) 2011-2014 Peercoin (PPCoin) Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com).


Intro
-----
PPCoin is a free open source project derived from Bitcoin, with
the goal of providing a long-term energy-efficient crypto-currency.
Built on the foundation of Bitcoin, innovations such as proof-of-stake
help further advance the field of crypto-currency.


Upgrade
-------
All your existing coins/transactions should be intact with the upgrade.
To upgrade from 0.3, first backup wallet in qt menu or by command line
ppcoind backupwallet <destination_backup_file>
Then shutdown ppcoind by
ppcoind stop
Uninstall v0.3 client, download and install v0.4 client.
If upgrade is performed after upgrade deadline, blockchain re-download is
required.


See the documentation/wiki at github:
  http://github.com/ppcoin/ppcoin
for help and more information.


------------------
Bitcoin 0.6.3 BETA

Copyright (c) 2009-2012 Bitcoin Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com).


Intro
-----
Bitcoin is a free open source peer-to-peer electronic cash system that is
completely decentralized, without the need for a central server or trusted
parties.  Users hold the crypto keys to their own money and transact directly
with each other, with the help of a P2P network to check for double-spending.


See the bitcoin wiki at:
  https://en.bitcoin.it/wiki/Main_Page
for more help and information.
