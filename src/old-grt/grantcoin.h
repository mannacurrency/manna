// Copyright (c) 2014 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
// Copyright (c) 2015 The Manna Foundation
#ifndef CODECOIN_grantcoin_H
#define CODECOIN_grantcoin_H

static const int RPC_PORT = 9983;
static const int RPC_PORT_TESTNET = 9985;
static const int P2P_PORT = 9982;
static const int P2P_PORT_TESTNET = 9984;

static const int64_t COIN = 1000000;
static const int64_t CENT = 10000;
static const int COIN_DECIMALS = 6; /* decimal places for coin */
#define COIN_DECIMALS_FMT "06"
/** No amount larger than this (in satoshi) is valid */
static const int64_t MAX_MONEY = 50000000000 * COIN;
inline bool MoneyRange(int64_t nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

static const float MIN_TX_FEE = 0.01;
static const float MIN_TXOUT_AMOUNT = MIN_TX_FEE;
static const int64_t MIN_TXOUT = CENT; // minimum transaction value is 0.01 MANNA, which is 10000 MANNA-satoshis

/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
static const int COINBASE_MATURITY = 500;

static const int STAKE_TARGET_SPACING = 1.5 * 60; // 90-second block spacing 
static const unsigned int nStakeMinAge = 60 * 60 * 24; // minimum age for coin age (24 hours)
static const unsigned int nStakeMaxAge = 60 * 60 * 24 * 90; // stake age of full weight
//static const int64 START_BLOCK_PROOF_OF_STAKE = 250000; // PoS allowed starting at this block

extern const unsigned int nMaxClockDrift;


/** Dust Soft Limit, allowed with additional fee per output */
static const int64_t DUST_SOFT_LIMIT = CENT; // 0.01 MANNA
/** Dust Hard Limit, ignored as wallet inputs (mininput default) */
static const int64_t DUST_HARD_LIMIT = 100;   // 0.0001 MANNA mininput

/** main.h CTransaction:AllowFree  This doesn't get used for Manna. **/
static const int MIN_FREE_PRIORITY = COIN * 960 / 250;

/** The interval over which we look to calculate the next difficulty **/
// TODO: not needed for MANNA???
static const int RETARGET_INTERVAL = 36;

/** Minimum block time spacing (hard limit) **/
static const int64_t MINIMUM_BLOCK_SPACING = 60;	// Absolute minimum spacing

#define BRAND "Manna"
#define BRAND_upper "Manna"
#define BRAND_lower "grantcoin"
#define BRAND_domain "grantcoin.org"
#define BRAND_CODE "MANNA"

//required for grantcoin since it started with stake data structures
// except not for granttest...
//#define PPCOINSTAKE

// Faster bootstraps with xz. Probably only works on mac/linux
#define USE_BOOTSTRAP_XZ
#define USE_BOOTSTRAP_GZ	// might work on android too

#endif
