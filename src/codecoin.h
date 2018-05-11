// Copyright (c) 2014 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
// Copyright (c) 2015 The Manna Foundation
#ifndef CODECOIN_codecoin_H
#define CODECOIN_codecoin_H

#include "bignum.h"
#include <inttypes.h>
#include <string>

/* TODO: make this a class */
extern const std::string strMessageMagic;
extern const CBigNum bnProofOfWorkLimit;
extern const CBigNum bnProofOfStakeLimit;
//extern const unsigned int nStakeMinAge;
//extern const unsigned int nStakeMaxAge;

#ifdef BRAND_manna
#include "manna.h"
#elif BRAND_mannastake
#include "mannastake.h"
#else
#error "No BRAND_*COIN defined!!"
#endif

#ifndef BRAND_SYM
#define BRAND_SYM BRAND_CODE
#endif

#endif
