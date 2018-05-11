// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2013-2014 The Catcoin developers
// Copyright (c) 2014 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
// Copyright (c) 2015 The Manna Foundation

#include "codecoin.h"
#if defined(BRAND_mannastake)
#include "mannastake.h"
#else
#include "manna.h"
#endif
#include "alert.h"
#include "checkpoints.h"
#include "db.h"
#include "txdb.h"
#include "net.h"
#include "init.h"
#include "ui_interface.h"
#include "checkqueue.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std;
using namespace boost;

uint256 hashGenesisBlock = 0;			// TODO: objectize this for multicoin support

const string strMessageMagic = "Manna Signed Message:\n";

const CBigNum bnProofOfWorkLimit(~uint256(0) >> 28);  // Reduced initial difficulty from Peercoin's 32
const CBigNum bnInitialHashTarget(~uint256(0) >> 28);  // Reduced from Peercoin's 40

int nCoinbaseMaturity = COINBASE_MATURITY;

/** Fees smaller than this (in satoshi) are considered zero fee (for transaction creation) */
int64_t CTransaction::nMinTxFee = CENT;
/** Fees smaller than this (in satoshi) are considered zero fee (for relaying) */
int64_t CTransaction::nMinRelayTxFee = CENT;

// TODO: separate max clock drift from tx timestamp limits?
const unsigned int nMaxClockDrift = 2*60*60;   // this is WAY to big..



/* value, in percent of what difficulty value we'll accept for orphans */
const int ORPHAN_WORK_THRESHOLD = 1; // FIXME WAY TOO WIDE right now

// DNS seeds
// Each pair gives a source name and a seed name.
// The first name is used as information source for addrman.
// The second name should resolve to a list of seed addresses.
// FIXME use a single string and/or objectify this
const char *strMainNetDNSSeed[][2] = {
	{"mannacurrency.net", "seed1.mannacurrency.net"},
	{"mannacurrency.net", "seed2.mannacurrency.net"},
	{"mannacurrency.net", "seed3.mannacurrency.net"},
	
	{NULL, NULL}
};

const char *strTestNetDNSSeed[][2] = {
	{"seed", "159.89.160.125"},
	{NULL, NULL}
};

int64_t GetProofOfWorkRewardTestNet(int nHeight)
{
    int64_t nSubsidy = COIN;
    if (nHeight == 1)
        nSubsidy = 10000000000 * COIN;
    else if (nHeight < 500)
        nSubsidy = CENT;
    else if (nHeight < 510)
        nSubsidy = 6.25 * COIN;
    else if (nHeight < 520)
        nSubsidy = 12.5 * COIN;
    else if (nHeight < 530)
        nSubsidy = 25 * COIN;
    else if (nHeight < 540)
        nSubsidy = 50 * COIN;
    else if (nHeight < 550)
        nSubsidy = 100 * COIN;
    else if (nHeight < 560)
        nSubsidy = 200 * COIN;
    else if (nHeight < 1000)
        nSubsidy = 400 * COIN;  // Maximum reward reached
    else if (nHeight < 1500)
        nSubsidy = 200 * COIN;  // Reward starts to decline
    else if (nHeight < 2000)
        nSubsidy = 100 * COIN;
    else if (nHeight < 2500)
        nSubsidy = 50 * COIN;
    else if (nHeight < 3000)
        nSubsidy = 25 * COIN;
    else if (nHeight < 3500)
        nSubsidy = 12.5 * COIN;
    else if (nHeight < 4000)
        nSubsidy = 6.25 * COIN;
    else if (nHeight < 4500)
        nSubsidy = 3.13 * COIN;
    else if (nHeight < 5000)
        nSubsidy = 1.57 * COIN;
    else if (nHeight >= 5000)
        nSubsidy = COIN;  // PoW reward reaches final minimum value

    // if (fDebug && GetBoolArg("-printcreation"))
    //     printf("GetProofOfWorkReward() : create=%s nBits=0x%08x nSubsidy=%"PRI64d"\n", FormatMoney(nSubsidy).c_str(), nBits, nSubsidy);

    return nSubsidy;
}

int64_t GetProofOfWorkReward(int nHeight)
{
    int64_t nSubsidy = COIN;
/* kludge */
    if (fTestNet){
	return GetProofOfWorkRewardTestNet(nHeight);
    }

    if (nHeight == 1)
        nSubsidy = 10000000000 * COIN;  // Manna created for planned distribution
    else if (nHeight < 50000)
        nSubsidy = CENT;  // De minimus reward pre-launch and up to 2 weeks post-launch
    else if (nHeight < 51000)
        nSubsidy = 6.25 * COIN;  // Public mining begins
    else if (nHeight < 52000)
        nSubsidy = 12.5 * COIN;  // Reward gradually increases during first few days
    else if (nHeight < 53000)
        nSubsidy = 25 * COIN;
    else if (nHeight < 54000)
        nSubsidy = 50 * COIN;
    else if (nHeight < 55000)
        nSubsidy = 100 * COIN;
    else if (nHeight < 56000)
        nSubsidy = 200 * COIN;
    else if (nHeight < 100000)
        nSubsidy = 400 * COIN;  // Maximum reward reached
    else if (nHeight < 150000)
        nSubsidy = 200 * COIN;  // Reward starts to decline
    else if (nHeight < 200000)
        nSubsidy = 100 * COIN;
    else if (nHeight < 250000)
        nSubsidy = 50 * COIN;
    else if (nHeight < 300000)
        nSubsidy = 25 * COIN;
    else if (nHeight >= 300000)
        nSubsidy = 10 * COIN;  // Final minimum reward for PoW phase

    // if (fDebug && GetBoolArg("-printcreation"))
    //     printf("GetProofOfWorkReward() : create=%s nBits=0x%08x nSubsidy=%"PRI64d"\n", FormatMoney(nSubsidy).c_str(), nBits, nSubsidy);

    return nSubsidy;
}

// peercoin: miner's coin stake is rewarded based on coin age spent (coin-days)
int64_t PPCoin_StakeReward(int64_t nCoinAge)
{
    static int64_t nRewardCoinYear = CENT;  // creation amount per coin-year
    int64_t nSubsidy = nCoinAge * 33 / (365 * 33 + 8) * nRewardCoinYear;
    if (fDebug && GetBoolArg("-printcreation"))
        printf("GetProofOfStakeReward(): create=%s nCoinAge=%" PRId64 "\n", FormatMoney(nSubsidy).c_str(), nCoinAge);
    return nSubsidy;
}

/*
 * Get the allow Seigniorage (money creation, or reward) of the current
 * block. If CoinAge is > 0, this is a proof of stake block.
 */
int64_t GetSeigniorage(const CBlockIndex *block, int64_t nFees, int64_t CoinAge)
{
	if(CoinAge == 0){
		return GetProofOfWorkReward(block->nHeight);
	} else {
		return GetProofOfStakeReward(CoinAge);
	}
}

static const int64_t nTargetTimespan = 24 * 60 * 60;  // 24 hours
static const int64_t nTargetSpacingWorkMax = 12 * STAKE_TARGET_SPACING; // 18 minutes

//
// minimum amount of work that could possibly be required nTime after
// minimum work required was nBase
//
unsigned int ComputeMinWork(unsigned int nBase, int64_t nTime, const CBlockHeader* pblock)
{
    CBigNum bnResult;
    bnResult.SetCompact(nBase);
    bnResult *= 2;
    while (nTime > 0 && bnResult < bnProofOfWorkLimit)
    {
        // Maximum 200% adjustment per 3 hours
        bnResult *= 2;
        nTime -= 3 * 60 * 60;
    }
    if (bnResult > bnProofOfWorkLimit)
        bnResult = bnProofOfWorkLimit;
    return bnResult.GetCompact();
}

unsigned int static GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake)
{
    if (pindexLast == NULL)
        return bnProofOfWorkLimit.GetCompact(); // genesis block

    const CBlockIndex* pindexPrev = GetLastBlockIndex(pindexLast, fProofOfStake);
    if (pindexPrev->pprev == NULL)
        return bnInitialHashTarget.GetCompact(); // first block
    const CBlockIndex* pindexPrevPrev = GetLastBlockIndex(pindexPrev->pprev, fProofOfStake);
    if (pindexPrevPrev->pprev == NULL)
        return bnInitialHashTarget.GetCompact(); // second block

    int64_t nActualSpacing = pindexPrev->GetBlockTime() - pindexPrevPrev->GetBlockTime();

    // peercoin: target change every block
    // peercoin: retarget with exponential moving toward target spacing
    CBigNum bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    int64_t nTargetSpacing = fProofOfStake? STAKE_TARGET_SPACING : min(nTargetSpacingWorkMax, (int64_t) STAKE_TARGET_SPACING * (1 + pindexLast->nHeight - pindexPrev->nHeight));
    int64_t nInterval = nTargetTimespan / nTargetSpacing;
    bnNew *= ((nInterval - 1) * nTargetSpacing + nActualSpacing + nActualSpacing);
    bnNew /= ((nInterval + 1) * nTargetSpacing);

    if (bnNew > bnProofOfWorkLimit)
        bnNew = bnProofOfWorkLimit;

    return bnNew.GetCompact();
}

unsigned int GetNextTrustRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
	return GetNextTargetRequired(pindexLast, false);	
}

static int minimum_time_fork = 250000;	// minimum time fork
static int minimum_time_fork_2 = 250000;	// minimum time fork

//Checks for 'hardcoded' block timestamps
bool AcceptBlockTimestamp(CValidationState &state, CBlockIndex* pindexPrev, const CBlockHeader *pblock)
{
	int64_t time_allow = -30;
	int64_t time_warn = MINIMUM_BLOCK_SPACING;
	int64_t delta = pblock->GetBlockTime() - pindexPrev->GetBlockTime();
	int nHeight = pindexPrev->nHeight + 1;

	if (nHeight > minimum_time_fork_2){
		time_allow = 30;
	}
	
	if (delta < time_warn){
		printf("WARNING blocktime nHeight %d time_allow %" PRId64" time_warn %" PRId64" time delta %" PRId64"\n", nHeight, time_allow, time_warn, delta);
	}

	if (nHeight >= minimum_time_fork_2) {
		if (delta <= time_allow) // see above, from first hard limit
			return state.Invalid(error("AcceptBlock(height=%d) : block time delta %" PRId64" too short", nHeight, delta));
	}
	if (nHeight >= minimum_time_fork) { /* don't forward these */
		if (delta <= MINIMUM_BLOCK_SPACING)
			return state.DoS(10, (error("AcceptBlock(height=%d) : block time delta %" PRId64" too short", nHeight, delta)));
	}
	return true;	
}

#ifdef CATBOX
unsigned int GetNextWorkRequired_Catcoin(const CBlockIndex* pindexLast, const CBlockHeader *pblock)
{
	int64_t nTargetTimespanLocal = 0;
	int64_t nIntervalLocal = 0;
	int forkBlock = 20290 - 1;
	int fork2Block = 21346;

	 // moved variable inits to the top where they belong
	 
	unsigned int nProofOfWorkLimit = bnProofOfWorkLimit.GetCompact();
	int64_t nActualTimespan;
	const CBlockIndex* pindexFirst = pindexLast;

	 int64_t error;	 
	 //int64_t diffcalc;
	double pGainUp=-0.005125;	// Theses values can be changed to tune the PID formula
	double iGainUp=-0.0225;	// Theses values can be changed to tune the PID formula
	double dGainUp=-0.0075;		// Theses values can be changed to tune the PID formula

	double pGainDn=-0.005125;	// Theses values can be changed to tune the PID formula
	double iGainDn=-0.0525;	// Theses values can be changed to tune the PID formula
	double dGainDn=-0.0075;		// Theses values can be changed to tune the PID formula

	double pCalc;
	double iCalc;
	double dCalc;
	double dResult;
	int64_t result;
	CBigNum bResult;
	CBigNum bnNew;
	int i;
	//CBigNum bLowLimit; // Limit for PID calc to never go below this
	
	if(fTestNet){
		forkBlock = -1;
		fork2Block = 36;
	}

	// Genesis block
	if (pindexLast == NULL)
		return nProofOfWorkLimit;

	// Starting from block 20,290 the network diff was set to 16
	// and the retarget interval was changed to 36
	if(pindexLast->nHeight < forkBlock && !fTestNet) 
	{
		nTargetTimespanLocal = nTargetTimespanOld;
		nIntervalLocal = nIntervalOld;
	} 
	else if(pindexLast->nHeight == forkBlock && !fTestNet) 
	{
		bnNew.SetCompact(0x1c0ffff0); // Difficulty 16
		return bnNew.GetCompact();
	} 
	else // Keep in for a resync
	{
		nTargetTimespanLocal = nTargetTimespan;
		nIntervalLocal = nInterval;
	}

	// after fork2Block we retarget every block   
	if(pindexLast->nHeight < fork2Block && !fTestNet)
	{
		// Only change once per interval
		if ((pindexLast->nHeight+1) % nIntervalLocal != 0 && !fTestNet)
		{
			// Special difficulty rule for testnet:
			if (fTestNet)
			{
				// If the new block's timestamp is more than 2* 10 minutes
				// then allow mining of a min-difficulty block.
				if (pblock->nTime > pindexLast->nTime + nTargetSpacing*2)
					return nProofOfWorkLimit;
				else
				{
					// Return the last non-special-min-difficulty-rules-block
					const CBlockIndex* pindex = pindexLast;
					while (pindex->pprev && pindex->nHeight % nIntervalLocal != 0 && pindex->nBits == nProofOfWorkLimit)
						pindex = pindex->pprev;
					return pindex->nBits;
				}
			}

			return pindexLast->nBits;
		}
	}


	if(pindexLast->nHeight < fork3Block && !fTestNet) // let it walk through 2nd fork stuff if below fork3Block, and ignore if on testnet
	{
	// Catcoin: This fixes an issue where a 51% attack can change difficulty at will.
	// Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
		int blockstogoback = nIntervalLocal-1;
		if ((pindexLast->nHeight+1) != nIntervalLocal)
			blockstogoback = nIntervalLocal;

		// Go back by what we want to be 14 days worth of blocks
		const CBlockIndex* pindexFirst = pindexLast;
		for (i = 0; pindexFirst && i < blockstogoback; i++)
			pindexFirst = pindexFirst->pprev;
		assert(pindexFirst);

		// Limit adjustment step
		int numerator = 4;
		int denominator = 1;
		if(pindexLast->nHeight >= fork2Block){
			numerator = 112;
			denominator = 100;
		}
		int64_t nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
		int64_t lowLimit = nTargetTimespanLocal*denominator/numerator;
		int64_t highLimit = nTargetTimespanLocal*numerator/denominator;
		printf("  nActualTimespan = %" PRId64"  before bounds\n", nActualTimespan);
		if (nActualTimespan < lowLimit)
			nActualTimespan = lowLimit;
		if (nActualTimespan > highLimit)
			nActualTimespan = highLimit;

		// Retarget
		bnNew.SetCompact(pindexLast->nBits);
		bnNew *= nActualTimespan;
		bnNew /= nTargetTimespanLocal;
	
		if (bnNew > bnProofOfWorkLimit)
			bnNew = bnProofOfWorkLimit;

		/// debug print
		if(fTestNet) printf("GetNextWorkRequired RETARGET\n");
		if(fTestNet) printf("nTargetTimespan = %" PRId64"    nActualTimespan = %" PRId64"\n", nTargetTimespanLocal, nActualTimespan);
		if(fTestNet) printf("Before: %08x  %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString().c_str());
		if(fTestNet) printf("After:	%08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());
	}
/*
PID formula
Error = Actual Time - Desired time
P Calc = pGain * Error
I Calc = iGain * Error * (Desired Time / Actual Time) 
D Calc = dGain * (Error / Actual Time) * I Calc

New Diff = (Current Diff + P Calc + I Calc + D Calc)

If New diff < 0, then set static value of 0.0001 or so.
*/	

	int nMinSpacing = 30;
	if(pindexLast->nHeight >= fork1min || fTestNet)
		nMinSpacing = MINIMUM_BLOCK_SPACING;
	
	if(pindexLast->nHeight >= fork3Block || fTestNet)
	// Fork 3 to use a PID routine instead of the other 2 forks 
	{
		pindexFirst = pindexLast->pprev;	// Set previous block
		for(i=0;i<7;i++) pindexFirst = pindexFirst->pprev; // Set 4th previous block for 8 block filtering 
		nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();		// Get last X blocks time
		nActualTimespan = nActualTimespan / 8;	// Calculate average for last 8 blocks
		if(pindexLast->nHeight > fork4Block || fTestNet){
			if (nMinSpacing > nActualTimespan){
				printf("WARNING: SANITY CHECK FAILED: PID nActualTimespan %" PRId64" too small! increased to %d\n",
					nActualTimespan, nMinSpacing );
				nActualTimespan = nMinSpacing;
			}
		}
		bnNew.SetCompact(pindexLast->nBits);	// Get current difficulty
		i=0;					// Zero bit-shift counter
		while(bnNew>0)				// Loop while bnNew > 0
		{
			i++;				// Increment bit-shift counter
			bnNew = bnNew >> 1;		// shift bnNew lower by 1 bit
			if(i>256) bnNew = 0;		// overflow test, just to make sure that it never stays in this loop
		}
		bnNew.SetCompact(pindexLast->nBits);	// Get current difficulty again
		

		error = nActualTimespan - nTargetSpacing;	// Calculate the error to be fed into the PID Calculation
		if(error >= -450 && error <= 450) // Slower gains for when the average time is within 2.5 min and 7.5 min 
		{
			// Calculate P ... pGainUp defined at beginning of routine
			pCalc = pGainUp * (double)error;
			// Calculate I ... iGainUp defined at beginning of routine
			iCalc = iGainUp * (double)error * (double)((double)nTargetSpacing / (double)nActualTimespan);
			// Calculate D ... dGainUp defined at beginning of routine
			dCalc = dGainUp * ((double)error / (double)nActualTimespan) * iCalc;
		}
		else // Faster gains for block averages faster than 2.5 min and greater than 7.5 min 
		{
			// Calculate P ... pGainDn defined at beginning of routine
			pCalc = pGainDn * (double)error;
			// Calculate I ... iGainDn defined at beginning of routine
			iCalc = iGainDn * (double)error * (double)((double)nTargetSpacing / (double)nActualTimespan);
			// Calculate D ... dGainDn defined at beginning of routine
			dCalc = dGainDn * ((double)error / (double)nActualTimespan) * iCalc;
		}

		if(error > -10 && error < 10)
		{
			if(fTestNet) printf("Within dead zone. No change!  error: %" PRId64"\n", error);
			return(bnNew.GetCompact());
		}		
		
		dResult = pCalc + iCalc + dCalc;	// Sum the PID calculations
		
		result = (int64_t)(dResult * 65536);	// Adjust for scrypt calcuation
		// Bring the result within max range to avoid overflow condition 
		while(result >	8388607) result = result / 2; 
		bResult = result;			// Set the bignum value
		if(i>24) bResult = bResult << (i - 24);	// bit-shift integer value of result to be subtracted from current diff

		//if(fTestNet)
		printf("pCalc: %f, iCalc: %f, dCalc: %f, Result: %" PRId64" (%f)\n", pCalc, iCalc, dCalc, result, dResult);
		//if(fTestNet) // TODO: make this key on a 'debugPID' or something
		printf("PID Actual Time: %" PRId64", error: %" PRId64"\n", nActualTimespan, error); 
		if(fTestNet)
			printf("Result: %08x %s\n",bResult.GetCompact(), bResult.getuint256().ToString().c_str()); 
		if(fTestNet)
			printf("Before: %08x %s\n",bnNew.GetCompact(), bnNew.getuint256().ToString().c_str()); 
		bnNew = bnNew - bResult;	// Subtract the result to set the current diff
		
		// Make sure that diff is not set too low, ever
		if (bnNew.GetCompact() > 0x1e0fffff) bnNew.SetCompact(0x1e0fffff);
		if(fTestNet) 
			printf("After:  %08x %s\n",bnNew.GetCompact(), bnNew.getuint256().ToString().c_str()); 
		
	} // End Fork 3 to use a PID routine instead of the other 2 forks routine

	return bnNew.GetCompact();
}
#endif

const char *pchGrantMain = "\xe2\xe7\xe1\xe4";

unsigned char pchMessageStart[4];

bool LoadBlockIndex()
{
	if (fTestNet)
	{	/* err, currently use the same one. TODO: change PCH */
		pchMessageStart[0] = pchGrantMain[0];
		pchMessageStart[1] = pchGrantMain[1];
		pchMessageStart[2] = pchGrantMain[2];
		pchMessageStart[3] = pchGrantMain[3];
		hashGenesisBlock = uint256 ("0x000000075c9bddc6a4638910415b2995febabf9dd8b634f0832da86c5bab2df5");
	} else {
		pchMessageStart[0] = pchGrantMain[0];
		pchMessageStart[1] = pchGrantMain[1];
		pchMessageStart[2] = pchGrantMain[2];
		pchMessageStart[3] = pchGrantMain[3];
		hashGenesisBlock = uint256("0000000f0483c7cc4433d89e321373d82d86ef5ba8157d8f7b9ef3449283421a");
	}

	//
	// Load block index from databases
	//
	if (!fReindex && !LoadBlockIndexDB())
		return false;

	return true;
}


bool InitBlockIndex() {
	// Check whether we're already initialized
	if (pindexGenesisBlock != NULL)
		return true;

	// Use the provided setting for -txindex in the new database
	fTxIndex = GetBoolArg("-txindex", false);
	pblocktree->WriteFlag("txindex", fTxIndex);
	printf("Initializing databases...\n");

	// Only add the genesis block if not reindexing (in which case we reuse the one already on disk)
	if (!fReindex) {
		CBlock block;
        	CTransaction txNew;
		if(fTestNet){ // GrantTestnet, optimize later.
        const char* pszTimestamp = "Reuters 10-OCT-2015 Hundreds of thousands protest in Berlin against EU-US trade deal";
        txNew.nTime = 1444509104;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(9999) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].SetEmpty();
        block.vtx.push_back(txNew);
        block.hashPrevBlock = 0;
        block.hashMerkleRoot = block.BuildMerkleTree();
        block.nVersion = 1;
        block.nTime    = 1444510495;
        block.nBits    = bnProofOfWorkLimit.GetCompact();
        block.nNonce   = 87045764;
		} else {        
		// Genesis block
		const char* pszTimestamp = "The Courier-Journal 21-MAR-2015 Prince Charles calls for a revolution";
        txNew.nTime = 1427081625;
		txNew.vin.resize(1);
		txNew.vout.resize(1);
		txNew.vin[0].scriptSig = CScript() << 486604799 << CBigNum(9999) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
		txNew.vout[0].SetEmpty();
		block.vtx.push_back(txNew);
		block.hashPrevBlock = 0;
		block.hashMerkleRoot = block.BuildMerkleTree();
		block.nVersion = 1;
		block.nTime    = 1427086539;
		block.nBits    = bnProofOfWorkLimit.GetCompact();
		block.nNonce   = 413974755;
		}

		//// debug print
		uint256 hash = block.GetHash();
		printf("%s\n", hash.ToString().c_str());
		printf("%s\n", hashGenesisBlock.ToString().c_str());
		printf("%s\n", block.hashMerkleRoot.ToString().c_str());
		block.print();
		assert(hash == hashGenesisBlock);
		if (fTestNet)
			assert(block.hashMerkleRoot == uint256("0x650de4987865a27a1c248908c6a93b9d55931ee3df0e97a845c0915bb53a362f"));
		else {
			assert(block.hashMerkleRoot == uint256("0xca7e1b14fe8d66d18650db8fa0c1b2787fa48b4a342fff3b00aa1cc9b0ae85f3"));
		}

		//assert(block.CheckBlock)

		// Start new block file
		try {
			unsigned int nBlockSize = ::GetSerializeSize(block, SER_DISK, CLIENT_VERSION);
			CDiskBlockPos blockPos;
			CValidationState state;
			if (!FindBlockPos(state, blockPos, nBlockSize+8, 0, block.nTime))
				return error("LoadBlockIndex() : FindBlockPos failed");
			if (!block.WriteToDisk(blockPos))
				return error("LoadBlockIndex() : writing genesis block to disk failed");
			if (!block.AddToBlockIndex(state, blockPos))
				return error("LoadBlockIndex() : genesis block not accepted");
		} catch(std::runtime_error &e) {
			return error("LoadBlockIndex() : failed to initialize block database: %s", e.what());
		}
	}

	return true;
}

namespace Checkpoints
{
	// What makes a good checkpoint block?
	// + Is surrounded by blocks with reasonable timestamps
	//	 (no blocks before with a timestamp after, none after with
	//	  timestamp before)
	// + Contains no strange transactions
	// TODO put this in manna.cpp|.h
	static MapCheckpoints mapCheckpoints =
		boost::assign::map_list_of
		(     0, uint256("0000000f0483c7cc4433d89e321373d82d86ef5ba8157d8f7b9ef3449283421a"))
//		(33000, uint256("0x"))

		;
	const CCheckpointData data = {
		&mapCheckpoints,
		1434870875, 	// * UNIX timestamp of last checkpoint block
		106400,		// * total number of transactions between genesis and last checkpoint
					//	 (the tx=... number in the SetBestChain debug.log lines)
		1000.0		// * estimated number of transactions per day after checkpoint
	};
}
