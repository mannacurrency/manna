// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2009-2012 The *coin developers
// where * = (Bit, Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
// Previously distributed under the MIT/X11 software license, see the
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
// Distributed under the Affero GNU General public license version 3
// file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
// Copyright (c) 2015 The Manna Foundation
#ifndef CODECOIN_MAIN_H
#define CODECOIN_MAIN_H

#include "codecoin.h"
#include "bignum.h"
#include "sync.h"
#include "net.h"
#include "script.h"

#include <list>

class CWallet;
class CBlock;
class CBlockIndex;
class CKeyItem;
class CReserveKey;

class CAddress;
class CInv;
class CNode;

struct CBlockIndexWorkComparator;

/** The maximum allowed size for a serialized block, in bytes (network rule) */
static const unsigned int MAX_BLOCK_SIZE = 1000000;						 // 1000KB block hard limit
/** Obsolete: maximum size for mined blocks */
static const unsigned int MAX_BLOCK_SIZE_GEN = MAX_BLOCK_SIZE/4;		 // 250KB  block soft limit
/** Default for -blockmaxsize, maximum size for mined blocks **/
static const unsigned int DEFAULT_BLOCK_MAX_SIZE = 250000;
/** Default for -blockprioritysize, maximum space for zero/low-fee transactions **/
static const unsigned int DEFAULT_BLOCK_PRIORITY_SIZE = 17000;
/** The maximum size for transactions we're willing to relay/mine */
static const unsigned int MAX_STANDARD_TX_SIZE = 100000;
/** The maximum allowed number of signature check operations in a block (network rule) */
static const unsigned int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE/50;
/** The maximum number of orphan transactions kept in memory */
static const unsigned int MAX_ORPHAN_TRANSACTIONS = MAX_BLOCK_SIZE/100;
/** The maximum number of entries in an 'inv' protocol message */
static const unsigned int MAX_INV_SZ = 50000;
/** The maximum size of a blk?????.dat file (since 0.8) */
static const unsigned int MAX_BLOCKFILE_SIZE = 0x8000000; // 128 MiB
/** The pre-allocation chunk size for blk?????.dat files (since 0.8) */
static const unsigned int BLOCKFILE_CHUNK_SIZE = 0x1000000; // 16 MiB
/** The pre-allocation chunk size for rev?????.dat files (since 0.8) */
static const unsigned int UNDOFILE_CHUNK_SIZE = 0x100000; // 1 MiB
/** Fake height value used in CCoins to signify they are only in the memory pool (since 0.8) */
static const unsigned int MEMPOOL_HEIGHT = 0x7FFFFFFF;
/** Threshold for nLockTime: below this value it is interpreted as block number, otherwise as UNIX timestamp. */
static const unsigned int LOCKTIME_THRESHOLD = 500000000; // Tue Nov  5 00:53:20 1985 UTC
/** Maximum number of script-checking threads allowed */
static const int MAX_SCRIPTCHECK_THREADS = 16;
#ifdef USE_UPNP
static const int fHaveUPnP = true;
#else
static const int fHaveUPnP = false;
#endif

extern CScript COINBASE_FLAGS;

extern CCriticalSection cs_main;
extern std::map<uint256, CBlockIndex*> mapBlockIndex;
extern std::set<CBlockIndex*, CBlockIndexWorkComparator> setBlockIndexValid;
#if defined(PPCOINSTAKE)
extern std::set<std::pair<COutPoint, unsigned int> > setStakeSeen;
extern uint64_t nLastCoinStakeSearchInterval;
#endif
extern uint256 hashGenesisBlock;
extern CBlockIndex* pindexGenesisBlock;
extern int nBestHeight;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;
extern unsigned int nTransactionsUpdated;
extern uint64_t nLastBlockTx;
extern uint64_t nLastBlockSize;
extern const std::string strMessageMagic;
extern double dHashesPerSec;
extern int64_t nHPSTimerStart;
extern int64_t nTimeBestReceived;
extern CCriticalSection cs_setpwalletRegistered;
extern std::set<CWallet*> setpwalletRegistered;
extern unsigned char pchMessageStart[4];
extern bool fImporting;
extern bool fReindex;
extern bool fBenchmark;
extern int nScriptCheckThreads;
extern bool fTxIndex;
extern unsigned int nCoinCacheSize;

// Settings
extern int64_t nTransactionFee;
extern int64_t nMinimumInputValue;

#if defined(ENABLE_MAXFUTURE)
// Maximum future timestamp we will accept in ProcessBlock
extern int64_t nMaxFutureTime;
#endif

// Minimum disk space required - used in CheckDiskSpace()
static const uint64_t nMinDiskSpace = 52428800;


class CReserveKey;
class CCoinsDB;
class CBlockTreeDB;
struct CDiskBlockPos;
class CCoins;
class CTxUndo;
class CCoinsView;
class CCoinsViewCache;
class CScriptCheck;
class CValidationState;

struct CBlockTemplate;

/** Register a wallet to receive updates from core */
void RegisterWallet(CWallet* pwalletIn);
/** Unregister a wallet from core */
void UnregisterWallet(CWallet* pwalletIn);
/** Push an updated transaction to all registered wallets */
void SyncWithWallets(const uint256 &hash, const CTransaction& tx, const CBlock* pblock = NULL, bool fUpdate = false, bool fConnect = true);
/** Process an incoming block */
bool ProcessBlock(CValidationState &state, CNode* pfrom, CBlock* pblock, CDiskBlockPos *dbp = NULL);
/** Check whether enough disk space is available for an incoming block */
bool CheckDiskSpace(uint64_t nAdditionalBytes = 0);
/** Open a block file (blk?????.dat) */
FILE* OpenBlockFile(const CDiskBlockPos &pos, bool fReadOnly = false);
/** Open an undo file (rev?????.dat) */
FILE* OpenUndoFile(const CDiskBlockPos &pos, bool fReadOnly = false);
/** Import blocks from an external file */
bool LoadExternalBlockFile(FILE* fileIn, CDiskBlockPos *dbp = NULL);
/** Initialize a new block tree database + block data on disk */
bool InitBlockIndex();
/** Load the block tree and coins database from disk */
bool LoadBlockIndex(); /* wrapper, sets pch */
bool LoadBlockIndexDB(); /* does the real work */
/** Unload database information */
void UnloadBlockIndex();
/** Verify consistency of the block and coin databases */
bool VerifyDB(int nCheckLevel, int nCheckDepth);
/** Print the loaded block tree */
void PrintBlockTree();
/** Find a block by height in the currently-connected chain */
CBlockIndex* FindBlockByHeight(int nHeight);
/** Process protocol messages received from a given node */
bool ProcessMessages(CNode* pfrom);
/** Send queued protocol messages to be sent to a give node */
bool SendMessages(CNode* pto, bool fSendTrickle);
/** Run an instance of the script checking thread */
void ThreadScriptCheck();
/** Run the miner threads */
void GenerateBitcoins(bool fGenerate, CWallet* pwallet);
/** Generate a new block, without valid proof-of-work */
/** If called with CWallet argument, assume fProofOfStake=true */
CBlockTemplate* CreateNewBlock(const CScript& scriptPubKeyIn, CWallet* pwallet=NULL);
CBlockTemplate* CreateNewBlockWithKey(CReserveKey& reservekey, CWallet* pwallet=NULL);
/** Modify the extranonce in a block */
void IncrementExtraNonce(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce);
/** Do mining precalculation */
void FormatHashBuffers(CBlock* pblock, char* pmidstate, char* pdata, char* phash1);
/** Check mined block */
bool CheckWork(CBlock* pblock, CWallet& wallet, CReserveKey& reservekey);
/** Check whether a block hash satisfies the proof-of-work requirement specified by nBits */
bool CheckProofOfWork(uint256 hash, unsigned int nBits);
/** Get the number of active peers */
int GetNumBlocksOfPeers();
/** Check whether we are doing an initial block download (synchronizing from disk or network) */
bool IsInitialBlockDownload();
/** Format a string that describes several potential problems detected by the core */
std::string GetWarnings(std::string strFor);
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
uint256 WantedByOrphan(const CBlock* pblockOrphan);
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);
#ifdef TESTING
void CodecoinMiner(CWallet *pwallet, bool fProofOfStake, bool fGenerateSingleBlock = false, CBlockIndex* parent = NULL);
#else
void CodecoinMiner(CWallet *pwallet, bool fProofOfStake);
#endif
#endif
/** Retrieve a transaction (from memory pool, or from disk, if possible) */
bool GetTransaction(const uint256 &hash, CTransaction &tx, uint256 &hashBlock, bool fAllowSlow = false);
/** Connect/disconnect blocks until pindexNew is the new tip of the active block chain */
bool SetBestChain(CValidationState &state, CBlockIndex* pindexNew);
/** Find the best known block, and make it the tip of the block chain */
bool ConnectBestBlock(CValidationState &state);
/** Create a new block index entry for a given block hash */
CBlockIndex * InsertBlockIndex(uint256 hash);
/** Verify a signature */
bool VerifySignature(const CCoins& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType);
/** Abort with a message */
bool AbortNode(const std::string &msg);



bool GetWalletFile(CWallet* pwallet, std::string &strWalletFileOut);

struct CDiskBlockPos
{
	int nFile;
	unsigned int nPos;

	IMPLEMENT_SERIALIZE(
		READWRITE(VARINT(nFile));
		READWRITE(VARINT(nPos));
	)

	CDiskBlockPos() {
		SetNull();
	}

	CDiskBlockPos(int nFileIn, unsigned int nPosIn) {
		nFile = nFileIn;
		nPos = nPosIn;
	}

	friend bool operator==(const CDiskBlockPos &a, const CDiskBlockPos &b) {
		return (a.nFile == b.nFile && a.nPos == b.nPos);
	}

	friend bool operator!=(const CDiskBlockPos &a, const CDiskBlockPos &b) {
		return !(a == b);
	}

	void SetNull() { nFile = -1; nPos = 0; }
	bool IsNull() const { return (nFile == -1); }
};

struct CDiskTxPos : public CDiskBlockPos
{
	unsigned int nTxOffset; // after header

	IMPLEMENT_SERIALIZE(
		READWRITE(*(CDiskBlockPos*)this);
		READWRITE(VARINT(nTxOffset));
	)

	CDiskTxPos(const CDiskBlockPos &blockIn, unsigned int nTxOffsetIn) : CDiskBlockPos(blockIn.nFile, blockIn.nPos), nTxOffset(nTxOffsetIn) {
	}

	CDiskTxPos() {
		SetNull();
	}

	void SetNull() {
		CDiskBlockPos::SetNull();
		nTxOffset = 0;
	}
};


/** An inpoint - a combination of a transaction and an index n into its vin */
class CInPoint
{
public:
	CTransaction* ptx;
	unsigned int n;

	CInPoint() { SetNull(); }
	CInPoint(CTransaction* ptxIn, unsigned int nIn) { ptx = ptxIn; n = nIn; }
	void SetNull() { ptx = NULL; n = (unsigned int) -1; }
	bool IsNull() const { return (ptx == NULL && n == (unsigned int) -1); }
};



/** An outpoint - a combination of a transaction hash and an index n into its vout */
class COutPoint
{
public:
	uint256 hash;
	unsigned int n;

	COutPoint() { SetNull(); }
	COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }
	IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
	void SetNull() { hash = 0; n = (unsigned int) -1; }
	bool IsNull() const { return (hash == 0 && n == (unsigned int) -1); }

	friend bool operator<(const COutPoint& a, const COutPoint& b)
	{
		return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
	}

	friend bool operator==(const COutPoint& a, const COutPoint& b)
	{
		return (a.hash == b.hash && a.n == b.n);
	}

	friend bool operator!=(const COutPoint& a, const COutPoint& b)
	{
		return !(a == b);
	}

	std::string ToString() const
	{
		return strprintf("COutPoint(%s, %u)", hash.ToString().c_str(), n);
	}

	void print() const
	{
		printf("%s\n", ToString().c_str());
	}
};




/** An input of a transaction.	It contains the location of the previous
 * transaction's output that it claims and a signature that matches the
 * output's public key.
 */
class CTxIn
{
public:
	COutPoint prevout;
	CScript scriptSig;
	unsigned int nSequence;

	CTxIn()
	{
		nSequence = std::numeric_limits<unsigned int>::max();
	}

	explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
	{
		prevout = prevoutIn;
		scriptSig = scriptSigIn;
		nSequence = nSequenceIn;
	}

	CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
	{
		prevout = COutPoint(hashPrevTx, nOut);
		scriptSig = scriptSigIn;
		nSequence = nSequenceIn;
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(prevout);
		READWRITE(scriptSig);
		READWRITE(nSequence);
	)

	bool IsFinal() const
	{
		return (nSequence == std::numeric_limits<unsigned int>::max());
	}

	friend bool operator==(const CTxIn& a, const CTxIn& b)
	{
		return (a.prevout	== b.prevout &&
				a.scriptSig == b.scriptSig &&
				a.nSequence == b.nSequence);
	}

	friend bool operator!=(const CTxIn& a, const CTxIn& b)
	{
		return !(a == b);
	}

	std::string ToStringShort() const
	{
		return strprintf(" %s %d", prevout.hash.ToString().c_str(), prevout.n);
	}

	std::string ToString() const
	{
		std::string str;
		str += "CTxIn(";
		str += prevout.ToString();
		if (prevout.IsNull())
			str += strprintf(", coinbase %s", HexStr(scriptSig).c_str());
		else
			str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24).c_str());
		if (nSequence != std::numeric_limits<unsigned int>::max())
			str += strprintf(", nSequence=%u", nSequence);
		str += ")";
		return str;
	}

	void print() const
	{
		printf("%s\n", ToString().c_str());
	}
};




/** An output of a transaction.  It contains the public key that the next input
 * must be able to sign with to claim it.
 */
class CTxOut
{
public:
	int64_t nValue;
	CScript scriptPubKey;

	CTxOut()
	{
		SetNull();
	}

	CTxOut(int64_t nValueIn, CScript scriptPubKeyIn)
	{
		nValue = nValueIn;
		scriptPubKey = scriptPubKeyIn;
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(nValue);
		READWRITE(scriptPubKey);
	)

	void SetNull()
	{
		nValue = -1;
		scriptPubKey.clear();
	}

	bool IsNull() const
	{
		return (nValue == -1);
	}

	void SetEmpty()
	{
		nValue = 0;
		scriptPubKey.clear();
	}

	bool IsEmpty() const
	{
		return (nValue == 0 && scriptPubKey.empty());
	}

	uint256 GetHash() const
	{
		return SerializeHash(*this);
	}

	friend bool operator==(const CTxOut& a, const CTxOut& b)
	{
		return (a.nValue	   == b.nValue &&
				a.scriptPubKey == b.scriptPubKey);
	}

	friend bool operator!=(const CTxOut& a, const CTxOut& b)
	{
		return !(a == b);
	}

	bool IsDust() const;

	std::string ToStringShort() const
	{
		return strprintf(" out %s %s", FormatMoney(nValue).c_str(), scriptPubKey.ToString(true).c_str());
	}

	std::string ToString() const
	{
		if (IsEmpty()) return "CTxOut(empty)";
		if (scriptPubKey.size() < 6)
			return "CTxOut(error)";
		return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue).c_str(), scriptPubKey.ToString().c_str());
	}

	void print() const
	{
		printf("%s\n", ToString().c_str());
	}
};



enum GetMinFee_mode
{
	GMF_BLOCK,
	GMF_RELAY,
	GMF_SEND,
};

/** The basic transaction that is broadcasted on the network and contained in
 * blocks. A transaction can contain multiple inputs and outputs.
 */
class CTransaction
{
public:
	static int64_t nMinTxFee;
	static int64_t nMinRelayTxFee;
#if defined(BRAND_manna)
	static const int CURRENT_VERSION = 1;
#else
	static const int CURRENT_VERSION = 1, VERSION_nTime = 1;
#endif
	int nVersion;
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
	unsigned int nTime;
#endif
	std::vector<CTxIn> vin;
	std::vector<CTxOut> vout;
	unsigned int nLockTime;

	CTransaction()
	{
		SetNull();
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(this->nVersion);
		nVersion = this->nVersion;
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		READWRITE(nTime);
#endif
		READWRITE(vin);
		READWRITE(vout);
		READWRITE(nLockTime);
	)

	void SetNull()
	{
		nVersion = CTransaction::CURRENT_VERSION;
		vin.clear();
		vout.clear();
		nLockTime = 0;
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		nTime = GetAdjustedTime();
#endif
	}

	bool IsNull() const
	{
		return (vin.empty() && vout.empty());
	}

	uint256 GetHash() const
	{
		return SerializeHash(*this);
	}

	bool IsFinal(int nBlockHeight=0, int64_t nBlockTime=0) const
	{
		// Time based nLockTime implemented in 0.1.6
		if (nLockTime == 0)
			return true;
		if (nBlockHeight == 0)
			nBlockHeight = nBestHeight;
		if (nBlockTime == 0)
			nBlockTime = GetAdjustedTime();
		if ((int64_t)nLockTime < ((int64_t)nLockTime < LOCKTIME_THRESHOLD ? (int64_t)nBlockHeight : nBlockTime))
			return true;
		BOOST_FOREACH(const CTxIn& txin, vin)
			if (!txin.IsFinal())
				return false;
		return true;
	}

	bool IsNewerThan(const CTransaction& old) const
	{
		if (vin.size() != old.vin.size())
			return false;
		for (unsigned int i = 0; i < vin.size(); i++)
			if (vin[i].prevout != old.vin[i].prevout)
				return false;

		bool fNewer = false;
		unsigned int nLowest = std::numeric_limits<unsigned int>::max();
		for (unsigned int i = 0; i < vin.size(); i++)
		{
			if (vin[i].nSequence != old.vin[i].nSequence)
			{
				if (vin[i].nSequence <= nLowest)
				{
					fNewer = false;
					nLowest = vin[i].nSequence;
				}
				if (old.vin[i].nSequence < nLowest)
				{
					fNewer = true;
					nLowest = old.vin[i].nSequence;
				}
			}
		}
		return fNewer;
	}

	bool IsCoinBase() const
	{
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		return (vin.size() == 1 && vin[0].prevout.IsNull() && vout.size() >= 1);
#else
		return (vin.size() == 1 && vin[0].prevout.IsNull());
#endif
	}

#if defined(PPCOINSTAKE)
	bool IsCoinStake() const
	{
		// manna: the coin stake transaction is marked with the first output empty
		// givecoin: Do we want this, or something else?
		return (vin.size() > 0 && (!vin[0].prevout.IsNull()) && vout.size() >= 2 && vout[0].IsEmpty());
	}
#warning "this is rather a hack, and it still doesn't work yet"
	bool has_nTime() const { return nVersion > LEGACY_VERSION; };
#else
	inline bool IsCoinStake() const { return false; };
#endif

	bool IsSeigniorage() const
	{
		return (IsCoinBase() || IsCoinStake());
	}

	/* aliases for above */
	inline bool IsNewCoin() const { return IsSeigniorage(); };
	inline bool IsGenerated() const { return IsSeigniorage(); };

	/** Check for standard transaction types
		@return True if all outputs (scriptPubKeys) use only standard transaction forms
	*/
	bool IsStandard(std::string& strReason) const;
	bool IsStandard() const
	{
		std::string strReason;
		return IsStandard(strReason);
	}

	/** Check for standard transaction types
		@param[in] mapInputs	Map of previous transactions that have outputs we're spending
		@return True if all inputs (scriptSigs) use only standard transaction forms
	*/
	bool AreInputsStandard(CCoinsViewCache& mapInputs) const;

	/** Count ECDSA signature operations the old-fashioned (pre-0.6) way
		@return number of sigops this transaction's outputs will produce when spent
	*/
	unsigned int GetLegacySigOpCount() const;

	/** Count ECDSA signature operations in pay-to-script-hash inputs.

		@param[in] mapInputs	Map of previous transactions that have outputs we're spending
		@return maximum number of sigops required to validate this transaction's inputs
	 */
	unsigned int GetP2SHSigOpCount(CCoinsViewCache& mapInputs) const;

	/** Amount of bitcoins spent by this transaction.
		@return sum of all outputs (note: does not include fees)
	 */
	int64_t GetValueOut() const
	{
		int64_t nValueOut = 0;
		BOOST_FOREACH(const CTxOut& txout, vout)
		{
			nValueOut += txout.nValue;
			if (!MoneyRange(txout.nValue) || !MoneyRange(nValueOut)){
				printf("Out of range txout.Nvalue 0x%" PRIx64" or nValueOut 0x%" PRIx64, txout.nValue, nValueOut);
				throw std::runtime_error("CTransaction::GetValueOut() : value out of range");
			}
		}
		return nValueOut;
	}

	/** Amount of codecoins coming in to this transaction
		Note that lightweight clients may not know anything besides the hash of previous transactions,
		so may not be able to calculate this.

		@param[in] mapInputs	Map of previous transactions that have outputs we're spending
		@return	Sum of value of all inputs (scriptSigs)
	 */
	int64_t GetValueIn(CCoinsViewCache& mapInputs) const;

	static bool AllowFree(double dPriority)
	{
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		return false;
#else
		// Large (in bytes) low-priority (new, small-coin) transactions
		// need a fee.
		return dPriority > MIN_FREE_PRIORITY;
#endif
	}

	// Apply the effects of this transaction on the UTXO set represented by view
	void UpdateCoins(const CTransaction& tx, CValidationState &state, CCoinsViewCache &inputs, CTxUndo &txundo, int nHeight, const uint256 &txhash);

	int64_t GetMinFee(unsigned int nBlockSize=1, bool fAllowFree=false, enum GetMinFee_mode mode=GMF_BLOCK, unsigned int nBytes=0) const;

	friend bool operator==(const CTransaction& a, const CTransaction& b)
	{
		return (a.nVersion	== b.nVersion &&
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
				a.nTime		== b.nTime &&
#endif
				a.vin		== b.vin &&
				a.vout		== b.vout &&
				a.nLockTime == b.nLockTime);
	}

	friend bool operator!=(const CTransaction& a, const CTransaction& b)
	{
		return !(a == b);
	}


	std::string ToStringShort() const
	{
		std::string str;
		str += strprintf("%s %s", GetHash().ToString().c_str(), IsCoinBase()? "base" : (IsCoinStake()? "stake" : "user"));
		return str;
	}

	std::string ToString() const
	{
		std::string str;
#if defined(PPCOINSTAKE) || defined(BRAND_manna) // todo: cleanup later
		str += IsCoinBase()? "Coinbase" : (IsCoinStake()? "Coinstake" : "CTransaction");
		str += strprintf("(hash=%s, nTime=%d, ver=%d, vin.size=%" PRIszu", vout.size=%" PRIszu", nLockTime=%d)\n",
			GetHash().ToString().substr(0,10).c_str(),
			nTime,
#else
		str += strprintf("CTransaction(hash=%s, ver=%d, vin.size=%" PRIszu", vout.size=%" PRIszu", nLockTime=%u)\n",
			GetHash().ToString().c_str(),
#endif
			nVersion,
			vin.size(),
			vout.size(),
			nLockTime);
		for (unsigned int i = 0; i < vin.size(); i++)
			str += "	" + vin[i].ToString() + "\n";
		for (unsigned int i = 0; i < vout.size(); i++)
			str += "	" + vout[i].ToString() + "\n";
		return str;
	}

	void print() const
	{
		printf("%s", ToString().c_str());
	}


	// Check whether all prevouts of this transaction are present in the UTXO set represented by view
	bool HaveInputs(CCoinsViewCache &view) const;

	// Check whether all inputs of this transaction are valid (no double spends, scripts & sigs, amounts)
	// This does not modify the UTXO set. If pvChecks is not NULL, script checks are pushed onto it
	// instead of being performed inline.
	bool CheckInputs(CValidationState &state, CCoinsViewCache &view, bool fScriptChecks = true,
					 unsigned int flags = SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC,
					 std::vector<CScriptCheck> *pvChecks = NULL) const;

#if defined(PPCOINSTAKE)
	bool GetCoinAge(CValidationState &state, CCoinsViewCache &view, uint64_t& nCoinAge) const;  // manna: get transaction coin age
#endif
	// Apply the effects of this transaction on the UTXO set represented by view
	void UpdateCoins(CValidationState &state, CCoinsViewCache &view, CTxUndo &txundo, int nHeight, const uint256 &txhash) const;

	// Context-independent validity checks
	bool CheckTransaction(CValidationState &state) const;

	// Try to accept this transaction into the memory pool
	bool AcceptToMemoryPool(CValidationState &state, bool fCheckInputs=true, bool fLimitFree = true, bool* pfMissingInputs=NULL);

protected:
	static const CTxOut &GetOutputFor(const CTxIn& input, CCoinsViewCache& mapInputs);
};

/** wrapper for CTxOut that provides a more compact serialization */
class CTxOutCompressor
{
private:
	CTxOut &txout;

public:
	static uint64_t CompressAmount(uint64_t nAmount);
	static uint64_t DecompressAmount(uint64_t nAmount);

	CTxOutCompressor(CTxOut &txoutIn) : txout(txoutIn) { }

	IMPLEMENT_SERIALIZE(({
		if (!fRead) {
			uint64_t nVal = CompressAmount(txout.nValue);
			READWRITE(VARINT(nVal));
		} else {
			uint64_t nVal = 0;
			READWRITE(VARINT(nVal));
			txout.nValue = DecompressAmount(nVal);
		}
		CScriptCompressor cscript(REF(txout.scriptPubKey));
		READWRITE(cscript);
	});)
};

#include "undo.h"

/** pruned version of CTransaction: only retains metadata and unspent transaction outputs
 *
 * Serialized format:
 * - VARINT(nVersion)
 * - VARINT(nCode)
 * - unspentness bitvector, for vout[2] and further; least significant byte first
 * - the non-spent CTxOuts (via CTxOutCompressor)
 * - VARINT(nHeight)
 *
 * The nCode value consists of:
 * - bit 1: IsCoinBase()
 * - bit 2: vout[0] is not spent
 * - bit 4: vout[1] is not spent
 * - The higher bits encode N, the number of non-zero bytes in the following bitvector.
 *	 - In case both bit 2 and bit 4 are unset, they encode N-1, as there must be at
 *	   least one non-spent output).
 *
 * Example: 0104835800816115944e077fe7c803cfa57f29b36bf87c1d358bb85e
 *			<><><--------------------------------------------><---->
 *			|  \				  |								/
 *	  version	code			 vout[1]				  height
 *
 *	  - version = 1
 *	  - code = 4 (vout[1] is not spent, and 0 non-zero bytes of bitvector follow)
 *	  - unspentness bitvector: as 0 non-zero bytes follow, it has length 0
 *	  - vout[1]: 835800816115944e077fe7c803cfa57f29b36bf87c1d35
 *				 * 8358: compact amount representation for 60000000000 (600 BTC)
 *				 * 00: special txout type pay-to-pubkey-hash
 *				 * 816115944e077fe7c803cfa57f29b36bf87c1d35: address uint160
 *	  - height = 203998
 *
 *
 * Example: 0109044086ef97d5790061b01caab50f1b8e9c50a5057eb43c2d9563a4eebbd123008c988f1a4a4de2161e0f50aac7f17e7f9555caa486af3b
 *			<><><--><--------------------------------------------------><----------------------------------------------><---->
 *		   /  \   \						|															|					  /
 *	version  code  unspentness		 vout[4]													 vout[16]			height
 *
 *	- version = 1
 *	- code = 9 (coinbase, neither vout[0] or vout[1] are unspent,
 *				  2 (1, +1 because both bit 2 and bit 4 are unset) non-zero bitvector bytes follow)
 *	- unspentness bitvector: bits 2 (0x04) and 14 (0x4000) are set, so vout[2+2] and vout[14+2] are unspent
 *	- vout[4]: 86ef97d5790061b01caab50f1b8e9c50a5057eb43c2d9563a4ee
 *			   * 86ef97d579: compact amount representation for 234925952 (2.35 BTC)
 *			   * 00: special txout type pay-to-pubkey-hash
 *			   * 61b01caab50f1b8e9c50a5057eb43c2d9563a4ee: address uint160
 *	- vout[16]: bbd123008c988f1a4a4de2161e0f50aac7f17e7f9555caa4
 *				* bbd123: compact amount representation for 110397 (0.001 BTC)
 *				* 00: special txout type pay-to-pubkey-hash
 *				* 8c988f1a4a4de2161e0f50aac7f17e7f9555caa4: address uint160
 *	- height = 120891
 */
class CCoins
{
public:
	// whether transaction is a coinbase
	bool fCoinBase;

	// unspent transaction outputs; spent outputs are .IsNull(); spent outputs at the end of the array are dropped
	std::vector<CTxOut> vout;

	// at which height this transaction was included in the active block chain
	int nHeight;

	// version of the CTransaction; accesses to this value should probably check for nHeight as well,
	// as new tx version will probably only be introduced at certain heights
	int nVersion;
#if defined(PPCOINSTAKE)
	// manna: whether transaction is a coinstake
	bool fCoinStake;

	// manna: transaction timestamp
	unsigned int nTime;

	// construct a CCoins from a CTransaction, at a given height
	CCoins(const CTransaction &tx, int nHeightIn) : fCoinBase(tx.IsCoinBase()), vout(tx.vout), nHeight(nHeightIn), nVersion(tx.nVersion), fCoinStake(tx.IsCoinStake()), nTime(tx.nTime) { }

	// empty constructor
	CCoins() : fCoinBase(false), vout(0), nHeight(0), nVersion(0), fCoinStake(false), nTime(0) { }
#else
	// construct a CCoins from a CTransaction, at a given height
	CCoins(const CTransaction &tx, int nHeightIn) : fCoinBase(tx.IsCoinBase()), vout(tx.vout), nHeight(nHeightIn), nVersion(tx.nVersion) { }

	// empty constructor
	CCoins() : fCoinBase(false), vout(0), nHeight(0), nVersion(0) { }
#endif
	// remove spent outputs at the end of vout
	void Cleanup() {
		while (vout.size() > 0 && vout.back().IsNull())
			vout.pop_back();
		if (vout.empty())
			std::vector<CTxOut>().swap(vout);
	}

	void swap(CCoins &to) {
		std::swap(to.fCoinBase, fCoinBase);
		to.vout.swap(vout);
		std::swap(to.nHeight, nHeight);
		std::swap(to.nVersion, nVersion);
#if defined(PPCOINSTAKE)
		std::swap(to.fCoinStake, fCoinStake);
		std::swap(to.nTime, nTime);
#endif
	}

	// equality test
	friend bool operator==(const CCoins &a, const CCoins &b) {
		 return a.fCoinBase == b.fCoinBase &&
				a.nHeight == b.nHeight &&
				a.nVersion == b.nVersion &&
				a.vout == b.vout
#if defined(PPCOINSTAKE)
				&& a.vout == b.vout &&
				a.fCoinStake == b.fCoinStake &&
				a.nTime == b.nTime
#endif
				;
	}
	friend bool operator!=(const CCoins &a, const CCoins &b) {
		return !(a == b);
	}

	// calculate number of bytes for the bitmask, and its number of non-zero bytes
	// each bit in the bitmask represents the availability of one output, but the
	// availabilities of the first two outputs are encoded separately
	void CalcMaskSize(unsigned int &nBytes, unsigned int &nNonzeroBytes) const {
		unsigned int nLastUsedByte = 0;
		for (unsigned int b = 0; 2+b*8 < vout.size(); b++) {
			bool fZero = true;
			for (unsigned int i = 0; i < 8 && 2+b*8+i < vout.size(); i++) {
				if (!vout[2+b*8+i].IsNull()) {
					fZero = false;
					continue;
				}
			}
			if (!fZero) {
				nLastUsedByte = b + 1;
				nNonzeroBytes++;
			}
		}
		nBytes += nLastUsedByte;
	}

	bool IsCoinBase() const {
		return fCoinBase;
	}

#if defined(PPCOINSTAKE)
    bool IsCoinStake() const { // manna: coinstake
        return fCoinStake;
    }
#endif

	unsigned int GetSerializeSize(int nType, int nVersion) const {
		unsigned int nSize = 0;
		unsigned int nMaskSize = 0, nMaskCode = 0;
		CalcMaskSize(nMaskSize, nMaskCode);
		bool fFirst = vout.size() > 0 && !vout[0].IsNull();
		bool fSecond = vout.size() > 1 && !vout[1].IsNull();
		assert(fFirst || fSecond || nMaskCode);
		unsigned int nCode = 8*(nMaskCode - (fFirst || fSecond ? 0 : 1)) + (fCoinBase ? 1 : 0) + (fFirst ? 2 : 0) + (fSecond ? 4 : 0);
		// version
		nSize += ::GetSerializeSize(VARINT(this->nVersion), nType, nVersion);
		// size of header code
		nSize += ::GetSerializeSize(VARINT(nCode), nType, nVersion);
		// spentness bitmask
		nSize += nMaskSize;
		// txouts themself
		for (unsigned int i = 0; i < vout.size(); i++)
			if (!vout[i].IsNull())
				nSize += ::GetSerializeSize(CTxOutCompressor(REF(vout[i])), nType, nVersion);
		// height
		nSize += ::GetSerializeSize(VARINT(nHeight), nType, nVersion);
#if defined(PPCOINSTAKE)
		// manna flags
		unsigned int nFlag = fCoinStake? 1 : 0;
		nSize += ::GetSerializeSize(VARINT(nFlag), nType, nVersion);
		// manna transaction timestamp
		nSize += ::GetSerializeSize(VARINT(nTime), nType, nVersion);
#endif
		return nSize;
	}

	template<typename Stream>
	void Serialize(Stream &s, int nType, int nVersion) const {
		unsigned int nMaskSize = 0, nMaskCode = 0;
		CalcMaskSize(nMaskSize, nMaskCode);
		bool fFirst = vout.size() > 0 && !vout[0].IsNull();
		bool fSecond = vout.size() > 1 && !vout[1].IsNull();
		assert(fFirst || fSecond || nMaskCode);
		unsigned int nCode = 8*(nMaskCode - (fFirst || fSecond ? 0 : 1)) + (fCoinBase ? 1 : 0) + (fFirst ? 2 : 0) + (fSecond ? 4 : 0);
		// version
		::Serialize(s, VARINT(this->nVersion), nType, nVersion);
		// header code
		::Serialize(s, VARINT(nCode), nType, nVersion);
		// spentness bitmask
		for (unsigned int b = 0; b<nMaskSize; b++) {
			unsigned char chAvail = 0;
			for (unsigned int i = 0; i < 8 && 2+b*8+i < vout.size(); i++)
				if (!vout[2+b*8+i].IsNull())
					chAvail |= (1 << i);
			::Serialize(s, chAvail, nType, nVersion);
		}
		// txouts themself
		for (unsigned int i = 0; i < vout.size(); i++) {
			if (!vout[i].IsNull())
				::Serialize(s, CTxOutCompressor(REF(vout[i])), nType, nVersion);
		}
		// coinbase height
		::Serialize(s, VARINT(nHeight), nType, nVersion);
#if defined(PPCOINSTAKE)
		// manna flags
		unsigned int nFlag = fCoinStake? 1 : 0;
		::Serialize(s, VARINT(nFlag), nType, nVersion);
		// manna transaction timestamp
		::Serialize(s, VARINT(nTime), nType, nVersion);
#endif
	}

	template<typename Stream>
	void Unserialize(Stream 
&s, int nType, int nVersion) {
		unsigned int nCode = 0;
		// version
		::Unserialize(s, VARINT(this->nVersion), nType, nVersion);
		// header code
		::Unserialize(s, VARINT(nCode), nType, nVersion);
		fCoinBase = nCode & 1;
		std::vector<bool> vAvail(2, false);
		vAvail[0] = nCode & 2;
		vAvail[1] = nCode & 4;
		unsigned int nMaskCode = (nCode / 8) + ((nCode & 6) != 0 ? 0 : 1);
		// spentness bitmask
		while (nMaskCode > 0) {
			unsigned char chAvail = 0;
			::Unserialize(s, chAvail, nType, nVersion);
			for (unsigned int p = 0; p < 8; p++) {
				bool f = (chAvail & (1 << p)) != 0;
				vAvail.push_back(f);
			}
			if (chAvail != 0)
				nMaskCode--;
		}
		// txouts themself
		vout.assign(vAvail.size(), CTxOut());
		for (unsigned int i = 0; i < vAvail.size(); i++) {
			if (vAvail[i])
				::Unserialize(s, REF(CTxOutCompressor(vout[i])), nType, nVersion);
		}
		// coinbase height
		::Unserialize(s, VARINT(nHeight), nType, nVersion);
#if defined(PPCOINSTAKE)
		// manna flags
		unsigned int nFlag = 0;
		::Unserialize(s, VARINT(nFlag), nType, nVersion);
		fCoinStake = nFlag & 1;
		// manna transaction timestamp
		::Unserialize(s, VARINT(nTime), nType, nVersion);
#endif
		Cleanup();
	}

	// mark an outpoint spent, and construct undo information
	bool Spend(const COutPoint &out, CTxInUndo &undo) {
		if (out.n >= vout.size())
			return false;
		if (vout[out.n].IsNull())
			return false;
		undo = CTxInUndo(vout[out.n]);
		vout[out.n].SetNull();
		Cleanup();
		if (vout.size() == 0) {
			undo.nHeight = nHeight;
			undo.fCoinBase = fCoinBase;
			undo.nVersion = this->nVersion;
#if defined(PPCOINSTAKE)
			undo.fCoinStake = fCoinStake;  // manna
			undo.nTime = nTime;            // manna
#endif
		}
		return true;
	}

	// mark a vout spent
	bool Spend(int nPos) {
		CTxInUndo undo;
		COutPoint out(0, nPos);
		return Spend(out, undo);
	}

	// check whether a particular output is still available
	bool IsAvailable(unsigned int nPos) const {
		return (nPos < vout.size() && !vout[nPos].IsNull());
	}

	// check whether the entire CCoins is spent
	// note that only !IsPruned() CCoins can be serialized
	bool IsPruned() const {
		BOOST_FOREACH(const CTxOut &out, vout)
			if (!out.IsNull())
				return false;
		return true;
	}
};

/** Closure representing one script verification
 *	Note that this stores references to the spending transaction */
class CScriptCheck
{
private:
	CScript scriptPubKey;
	const CTransaction *ptxTo;
	unsigned int nIn;
	unsigned int nFlags;
	int nHashType;

public:
	CScriptCheck() {}
	CScriptCheck(const CCoins& txFromIn, const CTransaction& txToIn, unsigned int nInIn, unsigned int nFlagsIn, int nHashTypeIn) :
		scriptPubKey(txFromIn.vout[txToIn.vin[nInIn].prevout.n].scriptPubKey),
		ptxTo(&txToIn), nIn(nInIn), nFlags(nFlagsIn), nHashType(nHashTypeIn) { }

	bool operator()() const;

	void swap(CScriptCheck &check) {
		scriptPubKey.swap(check.scriptPubKey);
		std::swap(ptxTo, check.ptxTo);
		std::swap(nIn, check.nIn);
		std::swap(nFlags, check.nFlags);
		std::swap(nHashType, check.nHashType);
	}
};

/** A transaction with a merkle branch linking it to the block chain. */
class CMerkleTx : public CTransaction
{
public:
	uint256 hashBlock;
	std::vector<uint256> vMerkleBranch;
	int nIndex;

	// memory only
	mutable bool fMerkleVerified;


	CMerkleTx()
	{
		Init();
	}

	CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
	{
		Init();
	}

	void Init()
	{
		hashBlock = 0;
		nIndex = -1;
		fMerkleVerified = false;
	}


	IMPLEMENT_SERIALIZE
	(
		nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, nVersion, ser_action);
		nVersion = this->nVersion;
		READWRITE(hashBlock);
		READWRITE(vMerkleBranch);
		READWRITE(nIndex);
	)


	int SetMerkleBranch(const CBlock* pblock=NULL);
	int GetDepthInMainChain(CBlockIndex* &pindexRet) const;
	int GetDepthInMainChain() const { CBlockIndex *pindexRet; return GetDepthInMainChain(pindexRet); }
	bool IsInMainChain() const { return GetDepthInMainChain() > 0; }
	int GetBlocksToMaturity() const;
	bool AcceptToMemoryPool(bool fCheckInputs=true, bool fLimitFree=true);
};





/** Data structure that represents a partial merkle tree.
 *
 * It respresents a subset of the txid's of a known block, in a way that
 * allows recovery of the list of txid's and the merkle root, in an
 * authenticated way.
 *
 * The encoding works as follows: we traverse the tree in depth-first order,
 * storing a bit for each traversed node, signifying whether the node is the
 * parent of at least one matched leaf txid (or a matched txid itself). In
 * case we are at the leaf level, or this bit is 0, its merkle node hash is
 * stored, and its children are not explorer further. Otherwise, no hash is
 * stored, but we recurse into both (or the only) child branch. During
 * decoding, the same depth-first traversal is performed, consuming bits and
 * hashes as they written during encoding.
 *
 * The serialization is fixed and provides a hard guarantee about the
 * encoded size:
 *
 *	 SIZE <= 10 + ceil(32.25*N)
 *
 * Where N represents the number of leaf nodes of the partial tree. N itself
 * is bounded by:
 *
 *	 N <= total_transactions
 *	 N <= 1 + matched_transactions*tree_height
 *
 * The serialization format:
 *	- uint32	 total_transactions (4 bytes)
 *	- varint	 number of hashes	(1-3 bytes)
 *	- uint256[]  hashes in depth-first order (<= 32*N bytes)
 *	- varint	 number of bytes of flag bits (1-3 bytes)
 *	- byte[]	 flag bits, packed per 8 in a byte, least significant bit first (<= 2*N-1 bits)
 * The size constraints follow from this.
 */
class CPartialMerkleTree
{
protected:
	// the total number of transactions in the block
	unsigned int nTransactions;

	// node-is-parent-of-matched-txid bits
	std::vector<bool> vBits;

	// txids and internal hashes
	std::vector<uint256> vHash;

	// flag set when encountering invalid data
	bool fBad;

	// helper function to efficiently calculate the number of nodes at given height in the merkle tree
	unsigned int CalcTreeWidth(int height) {
		return (nTransactions+(1 << height)-1) >> height;
	}

	// calculate the hash of a node in the merkle tree (at leaf level: the txid's themself)
	uint256 CalcHash(int height, unsigned int pos, const std::vector<uint256> &vTxid);

	// recursive function that traverses tree nodes, storing the data as bits and hashes
	void TraverseAndBuild(int height, unsigned int pos, const std::vector<uint256> &vTxid, const std::vector<bool> &vMatch);

	// recursive function that traverses tree nodes, consuming the bits and hashes produced by TraverseAndBuild.
	// it returns the hash of the respective node.
	uint256 TraverseAndExtract(int height, unsigned int pos, unsigned int &nBitsUsed, unsigned int &nHashUsed, std::vector<uint256> &vMatch);

public:

	// serialization implementation
	IMPLEMENT_SERIALIZE(
		READWRITE(nTransactions);
		READWRITE(vHash);
		std::vector<unsigned char> vBytes;
		if (fRead) {
			READWRITE(vBytes);
			CPartialMerkleTree &us = *(const_cast<CPartialMerkleTree*>(this));
			us.vBits.resize(vBytes.size() * 8);
			for (unsigned int p = 0; p < us.vBits.size(); p++)
				us.vBits[p] = (vBytes[p / 8] & (1 << (p % 8))) != 0;
			us.fBad = false;
		} else {
			vBytes.resize((vBits.size()+7)/8);
			for (unsigned int p = 0; p < vBits.size(); p++)
				vBytes[p / 8] |= vBits[p] << (p % 8);
			READWRITE(vBytes);
		}
	)

	// Construct a partial merkle tree from a list of transaction id's, and a mask that selects a subset of them
	CPartialMerkleTree(const std::vector<uint256> &vTxid, const std::vector<bool> &vMatch);

	CPartialMerkleTree();

	// extract the matching txid's represented by this partial merkle tree.
	// returns the merkle root, or 0 in case of failure
	uint256 ExtractMatches(std::vector<uint256> &vMatch);
};


/** Nodes collect new transactions into a block, hash them into a hash tree,
 * and scan through nonce values to make the block's hash satisfy proof-of-work
 * requirements.  When they solve the proof-of-work, they broadcast the block
 * to everyone and the block is added to the block chain.  The first transaction
 * in the block is a special one that creates a new coin owned by the creator
 * of the block.
 */
/** codecoin: TODO: first class validating mobile-friendly wallet that can pull
 * 'historical' blocks that have been checkpoint validated from the p2p network
 * or from trusted servers. Ideally clients that do not keep full blockchains 
 * but are staking would pay fees to retrieve blocks from nodes that have a full
 * copy. Call this proof-of-data, or proof-of-history
 */
class CBlockHeader
{
public:
	// header
#if defined (BRAND_mannastake)
/* Optionally overload lowest bit of nVersion as the 'proof-of-work' indicator,
   instead of overloading the vtx'es */
	static const int VERSION_STAKE_START=2;
	static const int CURRENT_VERSION=2;
	static const int CURRENT_VERSION_PoW = CURRENT_VERSION;
	static const int CURRENT_VERSION_PoS = CURRENT_VERSION | 1;
#elif defined(BRAND_manna)
	// TODO: re-endianize when we go to version 2/4 for MANNA
	static const int VERSION_STAKE_START=2;
	static const int CURRENT_VERSION=1;
#else
	static const int CURRENT_VERSION=2;
#endif

	int32_t nVersion;
	uint256 hashPrevBlock;
	uint256 hashMerkleRoot;
	uint32_t nTime;
	uint32_t nBits;
	uint32_t nNonce;

	CBlockHeader()
	{
		SetNull();
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(this->nVersion);
		nVersion = this->nVersion;
		READWRITE(hashPrevBlock);
		READWRITE(hashMerkleRoot);
		READWRITE(nTime);
		READWRITE(nBits);
		READWRITE(nNonce);
	)

	void SetNull()
	{
		nVersion = CBlockHeader::CURRENT_VERSION;
		hashPrevBlock = 0;
		hashMerkleRoot = 0;
		nTime = 0;
		nBits = 0;
		nNonce = 0;
	}

	bool IsNull() const
	{
		return (nBits == 0);
	}

	uint256 GetHash() const
	{
/* Generic 'Hash' function, which should be fast */
		return Hash(BEGIN(nVersion), END(nNonce));
	}

	uint256 GetPoWHash() const
	{
#if !defined(LITECOIN_SCRYPT_POWHASH)
		return GetHash();
#else
		uint256 thash;
		scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
		return thash;
#endif
	}

	int64_t GetBlockTime() const
	{
		return (int64_t)nTime;
	}

	void UpdateTime(const CBlockIndex* pindexPrev);
};

class CBlock : public CBlockHeader
{
public:
	// network and disk
	std::vector<CTransaction> vtx;
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
	// manna: block signature - signed by coin base txout[0]'s owner
	std::vector<unsigned char> vchBlockSig;
#endif

	// memory only
	mutable std::vector<uint256> vMerkleTree;

	CBlock()
	{
		SetNull();
	}

	CBlock(const CBlockHeader &header)
	{
		SetNull();
		*((CBlockHeader*)this) = header;
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(*(CBlockHeader*)this);
// TODO: determine correct way to manage vchBlockSig 
//		READWRITE(vtx);
//  
//#if defined(PPCOINSTAKE) || defined(BRAND_manna)
//#if defined(BRAND_givecoin) || defined(BRAND_hamburger)
//		if ((this->nVersion | 0x1) == 1) /* overload bit 0 of version as PoS indicator */
//#endif
//		READWRITE(vchBlockSig);
//#endif
//
#if defined(PPCOINSTAKE) || defined(BRAND_manna) // Was moved from CBlockHeader
		// ConnectBlock depends on vtx following header to generate CDiskTxPos
		if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
		{
			READWRITE(vtx);
			READWRITE(vchBlockSig);
		}
		else if (fRead)
		{
			const_cast<CBlock*>(this)->vtx.clear();
			const_cast<CBlock*>(this)->vchBlockSig.clear();
		}
#endif
	)

	void SetNull()
	{
		CBlockHeader::SetNull();
		vtx.clear();
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		vchBlockSig.clear();
#endif
		vMerkleTree.clear();
	}

#if defined(PPCOINSTAKE)
	// This is the newer manna 0.4 version.
#if !defined(PPCOINSTAKE_SLOW)
	// ?coin: entropy bit for stake modifier if chosen by modifier
	//unsigned int GetStakeEntropyBit(unsigned int nHeight) const
	unsigned int GetStakeEntropyBit() const
	{
		// Take last bit of block hash as entropy bit
		unsigned int nEntropyBit = ((GetHash().Get64()) & 1llu);
		if (fDebug && GetBoolArg("-printstakemodifier"))
			//printf("GetStakeEntropyBit: nHeight=%u hashBlock=%s nEntropyBit=%u\n", nHeight, GetHash().ToString().c_str(), nEntropyBit);
			printf("GetStakeEntropyBit: hashBlock=%s nEntropyBit=%u\n", GetHash().ToString().c_str(), nEntropyBit);
		return nEntropyBit;
	}
#endif /* ! PPCOINSTAKE_SLOW */

	// manna: two types of block: proof-of-work or proof-of-stake
	bool IsProofOfStake() const
	{
#if defined(BRAND_mannastake)
		/* Before you try to be clever here, read the assembly code, because
		   the compiler is smarter and more deterministic than you are */

		if (nVersion < VERSION_STAKE_START)
			return false;

		if ((nVersion & 0x1L) == 1)
			return true;	/* bit 0==1 is proof-of-stake */
		else
			return false;	/* bit 0==0 is proof-of-work */
#elif defined(BRAND_manna)
		return false;		// no stake for now
#else
#error "Your chosen coin brand needs IsProofOfStake implemented"
		//manna did it this way:
		return (vtx.size() > 1 && vtx[1].IsCoinStake());
#endif
	}

	bool IsProofOfWork() const
	{
		return !IsProofOfStake();
	}

	std::pair<COutPoint, unsigned int> GetProofOfStake() const
	{
		return IsProofOfStake()? std::make_pair(vtx[1].vin[0].prevout, vtx[1].nTime) : std::make_pair(COutPoint(), (unsigned int)0);
	}

#else /* !PPCOINSTAKE */

	inline bool IsProofOfStake() const { return false;};
	inline bool IsProofOfWork() const { return true;};

#endif /* PPCOINSTAKE */

#if defined(PPCOINSTAKE) || defined(BRAND_manna)
	// manna: get max transaction timestamp
	// should be #if FEATURE_TXNTIME?
	int64_t GetMaxTransactionTime() const
	{
		int64_t maxTransactionTime = 0;
		BOOST_FOREACH(const CTransaction& tx, vtx)
			maxTransactionTime = std::max(maxTransactionTime, (int64_t)tx.nTime);
		return maxTransactionTime;
	}
#endif /* FEATURE_TXNTIME */

	CBlockHeader GetBlockHeader() const
	{
		CBlockHeader block;
		block.nVersion		 = nVersion;
		block.hashPrevBlock  = hashPrevBlock;
		block.hashMerkleRoot = hashMerkleRoot;
		block.nTime			 = nTime;
		block.nBits			 = nBits;
		block.nNonce		 = nNonce;
		return block;
	}

	uint256 BuildMerkleTree() const
	{
		vMerkleTree.clear();
		BOOST_FOREACH(const CTransaction& tx, vtx)
			vMerkleTree.push_back(tx.GetHash());
		int j = 0;
		for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
		{
			for (int i = 0; i < nSize; i += 2)
			{
				int i2 = std::min(i+1, nSize-1);
				vMerkleTree.push_back(Hash(BEGIN(vMerkleTree[j+i]),  END(vMerkleTree[j+i]),
										   BEGIN(vMerkleTree[j+i2]), END(vMerkleTree[j+i2])));
			}
			j += nSize;
		}
		return (vMerkleTree.empty() ? 0 : vMerkleTree.back());
	}

	const uint256 &GetTxHash(unsigned int nIndex) const {
		assert(vMerkleTree.size() > 0); // BuildMerkleTree must have been called first
		assert(nIndex < vtx.size());
		return vMerkleTree[nIndex];
	}

	std::vector<uint256> GetMerkleBranch(int nIndex) const
	{
		if (vMerkleTree.empty())
			BuildMerkleTree();
		std::vector<uint256> vMerkleBranch;
		int j = 0;
		for (int nSize = vtx.size(); nSize > 1; nSize = (nSize + 1) / 2)
		{
			int i = std::min(nIndex^1, nSize-1);
			vMerkleBranch.push_back(vMerkleTree[j+i]);
			nIndex >>= 1;
			j += nSize;
		}
		return vMerkleBranch;
	}

	static uint256 CheckMerkleBranch(uint256 hash, const std::vector<uint256>& vMerkleBranch, int nIndex)
	{
		if (nIndex == -1)
			return 0;
		BOOST_FOREACH(const uint256& otherside, vMerkleBranch)
		{
			if (nIndex & 1)
				hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
			else
				hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));
			nIndex >>= 1;
		}
		return hash;
	}

	bool WriteToDisk(CDiskBlockPos &pos)
	{
		// Open history file to append
		CAutoFile fileout = CAutoFile(OpenBlockFile(pos), SER_DISK, CLIENT_VERSION);
		if (!fileout)
			return error("CBlock::WriteToDisk() : OpenBlockFile failed");

		// Write index header
		unsigned int nSize = fileout.GetSerializeSize(*this);
		fileout << FLATDATA(pchMessageStart) << nSize;

		// Write block
		long fileOutPos = ftell(fileout);
		if (fileOutPos < 0)
			return error("CBlock::WriteToDisk() : ftell failed");
		pos.nPos = (unsigned int)fileOutPos;
		fileout << *this;

		// Flush stdio buffers and commit to disk before returning
		fflush(fileout);
		if (!IsInitialBlockDownload())
			FileCommit(fileout);

		return true;
	}

	/* TODO: extend to dynamically read historical blocks from network */ 
	bool ReadFromDisk(const CDiskBlockPos &pos)
	{
		SetNull();

		// Open history file to read
		CAutoFile filein = CAutoFile(OpenBlockFile(pos, true), SER_DISK, CLIENT_VERSION);
		if (!filein)
			return error("CBlock::ReadFromDisk() : OpenBlockFile failed");

		// Read block
		try {
			filein >> *this;
		}
		catch (std::exception &e) {
			return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
		}

		// Check the header  TODO: *this is slow*
		if (IsProofOfWork() && !CheckProofOfWork(GetPoWHash(), nBits))
			return error("CBlock::ReadFromDisk() : errors in block header");

		return true;
	}



	void print() const
	{
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
		printf("CBlock(hash=%s, vchBlockSig=%s, PoW=%s, input=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%" PRIszu")\n",
			GetHash().ToString().c_str(),
			HexStr(vchBlockSig.begin(), vchBlockSig.end()).c_str(),
#else
		printf("CBlock(hash=%s, PoW=%s, input=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%" PRIszu")\n",
			GetHash().ToString().c_str(),
#endif
			GetPoWHash().ToString().c_str(),
			HexStr(BEGIN(nVersion),BEGIN(nVersion)+80,false).c_str(),	
			nVersion,
			hashPrevBlock.ToString().c_str(),
			hashMerkleRoot.ToString().c_str(),
			nTime, nBits, nNonce,
			vtx.size());
		for (unsigned int i = 0; i < vtx.size(); i++)
		{
			printf("  ");
			vtx[i].print();
		}
		printf("  vMerkleTree: ");
		for (unsigned int i = 0; i < vMerkleTree.size(); i++)
			printf("%s ", vMerkleTree[i].ToString().c_str());
		printf("\n");
	}


	/** Undo the effects of this block (with given index) on the UTXO set represented by coins.
	 *	In case pfClean is provided, operation will try to be tolerant about errors, and *pfClean
	 *	will be true if no problems were found. Otherwise, the return value will be false in case
	 *	of problems. Note that in any case, coins may be modified. */
	bool DisconnectBlock(CValidationState &state, CBlockIndex *pindex, CCoinsViewCache &coins, bool *pfClean = NULL);

	// Apply the effects of this block (with given index) on the UTXO set represented by coins
	bool ConnectBlock(CValidationState &state, CBlockIndex *pindex, CCoinsViewCache &coins, bool fJustCheck=false);

	// Read a block from disk
	bool ReadFromDisk(const CBlockIndex* pindex);

	// Add this block to the block index, and if necessary, switch the active block chain to this
	bool AddToBlockIndex(CValidationState &state, const CDiskBlockPos &pos);

	// Context-independent validity checks
	bool CheckBlock(CValidationState &state, bool fCheckPOW=true, bool fCheckMerkleRoot=true) const;

	// Store block on disk
	// if dbp is provided, the file is known to already reside on disk
	bool AcceptBlock(CValidationState &state, CDiskBlockPos *dbp = NULL);
#if defined(PPCOINSTAKE) || defined(BRAND_manna)
	bool GetCoinAge(uint64_t& nCoinAge) const; // manna: calculate total coin age spent in block
	bool SignBlock(const CKeyStore& keystore);
	bool CheckBlockSignature() const;
#if defined(PPCOINSTAKE_SLOW)
	unsigned int GetStakeEntropyBit() const; // manna: entropy bit for stake modifier if chosen by modifier
#endif
#endif
};





class CBlockFileInfo
{
public:
	unsigned int nBlocks;	   // number of blocks stored in file
	unsigned int nSize;		   // number of used bytes of block file
	unsigned int nUndoSize;    // number of used bytes in the undo file
	unsigned int nHeightFirst; // lowest height of block in file
	unsigned int nHeightLast;  // highest height of block in file
	uint64_t nTimeFirst;		   // earliest time of block in file
	uint64_t nTimeLast;		   // latest time of block in file

	IMPLEMENT_SERIALIZE(
		READWRITE(VARINT(nBlocks));
		READWRITE(VARINT(nSize));
		READWRITE(VARINT(nUndoSize));
		READWRITE(VARINT(nHeightFirst));
		READWRITE(VARINT(nHeightLast));
		READWRITE(VARINT(nTimeFirst));
		READWRITE(VARINT(nTimeLast));
	 )

	 void SetNull() {
		 nBlocks = 0;
		 nSize = 0;
		 nUndoSize = 0;
		 nHeightFirst = 0;
		 nHeightLast = 0;
		 nTimeFirst = 0;
		 nTimeLast = 0;
	 }

	 CBlockFileInfo() {
		 SetNull();
	 }

	 std::string ToString() const {
		 return strprintf("CBlockFileInfo(blocks=%u, size=%u, heights=%u...%u, time=%s...%s)", nBlocks, nSize, nHeightFirst, nHeightLast, DateTimeStrFormat("%Y-%m-%d", nTimeFirst).c_str(), DateTimeStrFormat("%Y-%m-%d", nTimeLast).c_str());
	 }

	 // update statistics (does not update nSize)
	 void AddBlock(unsigned int nHeightIn, uint64_t nTimeIn) {
		 if (nBlocks==0 || nHeightFirst > nHeightIn)
			 nHeightFirst = nHeightIn;
		 if (nBlocks==0 || nTimeFirst > nTimeIn)
			 nTimeFirst = nTimeIn;
		 nBlocks++;
		 if (nHeightIn > nHeightFirst)
			 nHeightLast = nHeightIn;
		 if (nTimeIn > nTimeLast)
			 nTimeLast = nTimeIn;
	 }
};

extern CCriticalSection cs_LastBlockFile;
extern CBlockFileInfo infoLastBlockFile;
extern int nLastBlockFile;

enum BlockStatus {
	BLOCK_VALID_UNKNOWN		 =	  0,
	BLOCK_VALID_HEADER		 =	  1, // parsed, version ok, hash satisfies claimed PoW, 1 <= vtx count <= max, timestamp not in future
	BLOCK_VALID_TREE		 =	  2, // parent found, difficulty matches, timestamp >= median previous, checkpoint
	BLOCK_VALID_TRANSACTIONS =	  3, // only first tx is coinbase, 2 <= coinbase input script length <= 100, transactions valid, no duplicate txids, sigops, size, merkle root
	BLOCK_VALID_CHAIN		 =	  4, // outputs do not overspend inputs, no double spends, coinbase output ok, immature coinbase spends, BIP30
	BLOCK_VALID_SCRIPTS		 =	  5, // scripts/signatures ok
	BLOCK_VALID_MASK		 =	  7,

	BLOCK_HAVE_DATA			 =	  8, // full block available in blk*.dat
	BLOCK_HAVE_UNDO			 =	 16, // undo data available in rev*.dat
	BLOCK_HAVE_MASK			 =	 24,

	BLOCK_FAILED_VALID		 =	 32, // stage after last reached validness failed
	BLOCK_FAILED_CHILD		 =	 64, // descends from failed block
	BLOCK_FAILED_MASK		 =	 96
};

/** The block chain is a tree shaped structure starting with the
 * genesis block at the root, with each block potentially having multiple
 * candidates to be the next block.  pprev and pnext link a path through the
 * main/longest chain.	A blockindex may have multiple pprev pointing back
 * to it, but pnext will only point forward to the longest branch, or will
 * be null if the block is not part of the longest chain.
 */
class CBlockIndex
{
public:
	// pointer to the hash of the block, if any. memory is owned by this CBlockIndex
	const uint256* phashBlock;

	// pointer to the index of the predecessor of this block
	CBlockIndex* pprev;

	// (memory only) pointer to the index of the *active* successor of this block
	CBlockIndex* pnext;

	// height of the entry in the chain. The genesis block has height 0
	int nHeight;

	// Which # file this block is stored in (blk?????.dat)
	int nFile;

	// Byte offset within blk?????.dat where this block's data is stored
	unsigned int nDataPos;

	// Byte offset within rev?????.dat where this block's undo data is stored
	unsigned int nUndoPos;

	/* (memory only) Total amount of trust score in the chain up to and including this block
	 * For proof-of-work only coins, this is approximately equivalent to the number of hashes
	 * applied towards finding proof-of-work.
	 * For proof-of-stake only coins, this is (mostly) the coin-age consumed by staking, with
	 * some multipliers to get something that looks like the old Proof-of-Work 'nChainWork'.
	 * For hybrid coins, this is (at least conceptually) PoW-hashes + PoS-CoinAge*multiplier
	 * HERE THERE BE DRAGONS (or at least hackers and stealth-switchpools)
	 * The dragons in the details centers around assumptions about the ratio of PoS CoinAge
	 * with PoS Hashes can go all over the place and change by orders of magnitude when ASIC
	 * hashpower shows up.
	 * There are further issues with the 'nothing-at-stake' attacks, and all of this makes
	 * Proof-of-stake a much more complicated and potentially ambiguous consensus system.
	 * Hybrid systems, with a properly functioning stake/hash trust multiplier can mitigate
	 * some of the ambiguity by requiring a consensus between miners and stakers. */
	uint256 nChainTrust;

	// Number of transactions in this block.
	// Note: in a potential headers-first mode, this number cannot be relied upon
	unsigned int nTx;

	// (memory only) Number of transactions in the chain up to and including this block
	unsigned int nChainTx; // change to 64-bit type when necessary; won't happen before 2030

	// Verification status of this block. See enum BlockStatus
	unsigned int nStatus;

#if defined(PPCOINSTAKE)
	int64_t nMint;

	// manna: proof-of-stake related block index fields
	unsigned int nFlags;  // manna: block index flags
	enum  
	{
		BLOCK_PROOF_OF_STAKE = (1 << 0), // is proof-of-stake block
		BLOCK_STAKE_ENTROPY  = (1 << 1), // entropy bit for stake modifier
		BLOCK_STAKE_MODIFIER = (1 << 2), // regenerated stake modifier
	};

	uint64_t nStakeModifier; // hash modifier for proof-of-stake
	unsigned int nStakeModifierChecksum; // checksum of index; in-memeory only
	COutPoint prevoutStake;
	unsigned int nStakeTime;
	uint256 hashProofOfStake;
#endif
	int64_t nMoneySupply;

	// block header
	int nVersion;
	uint256 hashMerkleRoot;
	unsigned int nTime;
	unsigned int nBits;
	unsigned int nNonce;


	CBlockIndex()
	{
		phashBlock = NULL;
		pprev = NULL;
		pnext = NULL;
		nHeight = 0;
		nFile = 0;
		nDataPos = 0;
		nUndoPos = 0;
		nChainTrust = 0;
#if defined(PPCOINSTAKE)
		nMint = 0;
		nFlags = 0;
		nStakeModifier = 0;
		nStakeModifierChecksum = 0;
		hashProofOfStake = 0;
		prevoutStake.SetNull();
		nStakeTime = 0;
#endif
		nMoneySupply = 0;
		nTx = 0;
		nChainTx = 0;
		nStatus = 0;

		nVersion	= 0;
		hashMerkleRoot = 0;
		nTime		= 0;
		nBits		= 0;
		nNonce		= 0;
	}

	CBlockIndex(CBlock& block)
	{
		phashBlock = NULL;
		pprev = NULL;
		pnext = NULL;
		nHeight = 0;
		nFile = 0;
		nDataPos = 0;
		nUndoPos = 0;
		nChainTrust = 0;
#if defined(PPCOINSTAKE)
		nMint = 0;
		nFlags = 0;
		nStakeModifier = 0;
		nStakeModifierChecksum = 0;
		hashProofOfStake = 0;
		if (block.IsProofOfStake())
		{
			SetProofOfStake();
			prevoutStake = block.vtx[1].vin[0].prevout;
			nStakeTime = block.vtx[1].nTime;
		}
		else
		{
			prevoutStake.SetNull();
			nStakeTime = 0;
		}
#endif
		nMoneySupply = 0;
		nTx = 0;
		nChainTx = 0;
		nStatus = 0;

		nVersion	   = block.nVersion;
		hashMerkleRoot = block.hashMerkleRoot;
		nTime		   = block.nTime;
		nBits		   = block.nBits;
		nNonce		   = block.nNonce;
	}

	CDiskBlockPos GetBlockPos() const {
		CDiskBlockPos ret;
		if (nStatus & BLOCK_HAVE_DATA) {
			ret.nFile = nFile;
			ret.nPos  = nDataPos;
		}
		return ret;
	}

	CDiskBlockPos GetUndoPos() const {
		CDiskBlockPos ret;
		if (nStatus & BLOCK_HAVE_UNDO) {
			ret.nFile = nFile;
			ret.nPos  = nUndoPos;
		}
		return ret;
	}

	CBlockHeader GetBlockHeader() const
	{
		CBlockHeader block;
		block.nVersion		 = nVersion;
		if (pprev)
			block.hashPrevBlock = pprev->GetBlockHash();
		block.hashMerkleRoot = hashMerkleRoot;
		block.nTime			 = nTime;
		block.nBits			 = nBits;
		block.nNonce		 = nNonce;
		return block;
	}

	uint256 GetBlockHash() const
	{
		return *phashBlock;
	}

	int64_t GetBlockTime() const
	{
		return (int64_t)nTime;
	}

	/**
	 * Duplicate from bitcoinrpc that originaly define this method.
	 * May require some cleanup since this method should be available both for rpc
	 * and qt clients.
	 */
	double GetBlockDifficulty() const
	{
		int nShift = (nBits >> 24) & 0xff;

		double dDiff =
			(double)0x0000ffff / (double)(nBits & 0x00ffffff);

		while (nShift < 29)
		{
			dDiff *= 256.0;
			nShift++;
		}
		while (nShift > 29)
		{
			dDiff /= 256.0;
			nShift--;
		}

		return dDiff;
	}

#if defined(PPCOINSTAKE)
	CBigNum GetBlockTrust() const;
#else
	CBigNum GetBlockTrust() const
	{
		CBigNum bnTarget;
		bnTarget.SetCompact(nBits);
		if (bnTarget <= 0)
			return 0;
		return (CBigNum(1)<<256) / (bnTarget+1);
	}
#endif

	bool IsInMainChain() const
	{
		return (pnext || this == pindexBest);
	}

	bool CheckIndex() const
	{
#if defined(CHECKINDEX_SLOW)
		return IsProofOfWork() ? CheckProofOfWork(GetBlockHash(), nBits) : true;
#else /* litecoin scrypt code */
		/** Scrypt is used for block proof-of-work, but for purposes of performance the index internally uses sha256.
		 *	This check was considered unneccessary given the other safeguards like the genesis and checkpoints. */
		return true; // return CheckProofOfWork(GetBlockHash(), nBits);
#endif
	}

	enum { nMedianTimeSpan=11 };

	int64_t GetMedianTimePast() const
	{
		int64_t pmedian[nMedianTimeSpan];
		int64_t* pbegin = &pmedian[nMedianTimeSpan];
		int64_t* pend = &pmedian[nMedianTimeSpan];

		const CBlockIndex* pindex = this;
		for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->pprev)
			*(--pbegin) = pindex->GetBlockTime();

		std::sort(pbegin, pend);
		return pbegin[(pend - pbegin)/2];
	}

	int64_t GetMedianTime() const
	{
		const CBlockIndex* pindex = this;
		for (int i = 0; i < nMedianTimeSpan/2; i++)
		{
			if (!pindex->pnext)
				return GetBlockTime();
			pindex = pindex->pnext;
		}
		return pindex->GetMedianTimePast();
	}

	/**
	 * Returns true if there are nRequired or more blocks of minVersion or above
	 * in the last nToCheck blocks, starting at pstart and going backwards.
	 */
	static bool IsSuperMajority(int minVersion, const CBlockIndex* pstart,
								unsigned int nRequired, unsigned int nToCheck);

#if defined(PPCOINSTAKE)
	bool IsProofOfWork() const
	{
		return !(nFlags & BLOCK_PROOF_OF_STAKE);
	}

	bool IsProofOfStake() const
	{
		return (nFlags & BLOCK_PROOF_OF_STAKE);
	}

	void SetProofOfStake()
	{
		nFlags |= BLOCK_PROOF_OF_STAKE;
	}

	unsigned int GetStakeEntropyBit() const
	{
		return ((nFlags & BLOCK_STAKE_ENTROPY) >> 1);
	}

	bool SetStakeEntropyBit(unsigned int nEntropyBit)
	{
		if (nEntropyBit > 1)
			return false;
		nFlags |= (nEntropyBit? BLOCK_STAKE_ENTROPY : 0);
		return true;
	}

	bool GeneratedStakeModifier() const
	{
		return (nFlags & BLOCK_STAKE_MODIFIER);
	}

	void SetStakeModifier(uint64_t nModifier, bool fGeneratedStakeModifier)
	{
		nStakeModifier = nModifier;
		if (fGeneratedStakeModifier)
			nFlags |= BLOCK_STAKE_MODIFIER;
	}

	std::string ToString() const
	{
		return strprintf("CBlockIndex(nprev=%p, pnext=%p,  nHeight=%d, nMint=%s, nMoneySupply=%s, nFlags=(%s)(%d)(%s), nStakeModifier=%016" PRIx64", nStakeModifierChecksum=%08x, hashProofOfStake=%s, prevoutStake=(%s), nStakeTime=%d merkle=%s, hashBlock=%s)",
			pprev, pnext, nHeight,
			FormatMoney(nMint).c_str(), FormatMoney(nMoneySupply).c_str(),
			GeneratedStakeModifier() ? "MOD" : "-", GetStakeEntropyBit(), IsProofOfStake()? "PoS" : "PoW",
			nStakeModifier, nStakeModifierChecksum, 
			hashProofOfStake.ToString().c_str(),
			prevoutStake.ToString().c_str(), nStakeTime,
			hashMerkleRoot.ToString().substr(0,10).c_str(),
			GetBlockHash().ToString().substr(0,20).c_str());
	}
#else
	inline bool IsProofOfWork() const { return true; };
	inline bool IsProofOfStake() const { return false; };	

	std::string ToString() const
	{
		return strprintf("CBlockIndex(pprev=%p, pnext=%p, nHeight=%d, merkle=%s, hashBlock=%s)",
			pprev, pnext, nHeight,
			hashMerkleRoot.ToString().c_str(),
			GetBlockHash().ToString().c_str());
	}
#endif
	void print() const
	{
		printf("%s\n", ToString().c_str());
	}
};

struct CBlockIndexWorkComparator
{
	bool operator()(CBlockIndex *pa, CBlockIndex *pb) {
		if (pa->nChainTrust > pb->nChainTrust) return false;
		if (pa->nChainTrust < pb->nChainTrust) return true;
		if (pa->GetBlockHash() < pb->GetBlockHash()) return false;
		if (pa->GetBlockHash() > pb->GetBlockHash()) return true;

		return false; // identical blocks
	}
};


/** Used to marshal pointers into hashes for db storage. */
class CDiskBlockIndex : public CBlockIndex
{
public:
	uint256 hashPrev;

	CDiskBlockIndex() {
		hashPrev = 0;
	}

	explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex) {
		hashPrev = (pprev ? pprev->GetBlockHash() : 0);
	}

	IMPLEMENT_SERIALIZE
	(
		if (!(nType & SER_GETHASH)){
			READWRITE(VARINT(nVersion));
		}
		READWRITE(VARINT(nHeight));
		READWRITE(VARINT(nStatus));
		READWRITE(VARINT(nTx));
		if (nStatus & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO))
			READWRITE(VARINT(nFile));
		if (nStatus & BLOCK_HAVE_DATA)
			READWRITE(VARINT(nDataPos));
		if (nStatus & BLOCK_HAVE_UNDO)
			READWRITE(VARINT(nUndoPos));

#if defined(PPCOINSTAKE) // TODO: is needed for manna?
		READWRITE(nMint);
		READWRITE(nFlags);
		READWRITE(nStakeModifier);
		if (IsProofOfStake())
		{
			READWRITE(prevoutStake);
			READWRITE(nStakeTime);
			READWRITE(hashProofOfStake);
		}
		else if (fRead)
		{
			const_cast<CDiskBlockIndex*>(this)->prevoutStake.SetNull();
			const_cast<CDiskBlockIndex*>(this)->nStakeTime = 0;
			const_cast<CDiskBlockIndex*>(this)->hashProofOfStake = 0;
		}
#endif
		READWRITE(nMoneySupply);

		// block header
		READWRITE(this->nVersion);
		READWRITE(hashPrev);
		READWRITE(hashMerkleRoot);
		READWRITE(nTime);
		READWRITE(nBits);
		READWRITE(nNonce);
	)

	uint256 GetBlockHash() const
	{
		CBlockHeader block;
		block.nVersion		  = nVersion;
		block.hashPrevBlock   = hashPrev;
		block.hashMerkleRoot  = hashMerkleRoot;
		block.nTime			  = nTime;
		block.nBits			  = nBits;
		block.nNonce		  = nNonce;
		return block.GetHash();
	}


	std::string ToString() const
	{
		std::string str = "CDiskBlockIndex(";
		str += CBlockIndex::ToString();
		str += strprintf("\n                hashBlock=%s, hashPrev=%s)",
			GetBlockHash().ToString().c_str(),
			hashPrev.ToString().c_str());
		return str;
	}

	void print() const
	{
		printf("%s\n", ToString().c_str());
	}
};

/** Capture information about block/transaction validation */
class CValidationState {
private:
	enum mode_state {
		MODE_VALID,   // everything ok
		MODE_INVALID, // network rule violation (DoS value may be set)
		MODE_ERROR,   // run-time error
	} mode;
	int nDoS;
	bool corruptionPossible;
public:
	CValidationState() : mode(MODE_VALID), nDoS(0), corruptionPossible(false) {}
	bool DoS(int level, bool ret = false, bool corruptionIn = false) {
		if (mode == MODE_ERROR)
			return ret;
		nDoS += level;
		mode = MODE_INVALID;
		corruptionPossible = corruptionIn;
		return ret;
	}
	bool Invalid(bool ret = false) {
		return DoS(0, ret);
	}
	bool Error() {
		mode = MODE_ERROR;
		return false;
	}
	bool Abort(const std::string &msg) {
		AbortNode(msg);
		return Error();
	}
	bool IsValid() {
		return mode == MODE_VALID;
	}
	bool IsInvalid() {
		return mode == MODE_INVALID;
	}
	bool IsError() {
		return mode == MODE_ERROR;
	}
	bool IsInvalid(int &nDoSOut) {
		if (IsInvalid()) {
			nDoSOut = nDoS;
			return true;
		}
		return false;
	}
	bool CorruptionPossible() {
		return corruptionPossible;
	}
};







/** Describes a place in the block chain to another node such that if the
 * other node doesn't have the same branch, it can find a recent common trunk.
 * The further back it is, the further before the fork it may be.
 */
class CBlockLocator
{
protected:
	std::vector<uint256> vHave;
public:

	CBlockLocator()
	{
	}

	explicit CBlockLocator(const CBlockIndex* pindex)
	{
		Set(pindex);
	}

	explicit CBlockLocator(uint256 hashBlock)
	{
		std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
		if (mi != mapBlockIndex.end())
			Set((*mi).second);
	}

	CBlockLocator(const std::vector<uint256>& vHaveIn)
	{
		vHave = vHaveIn;
	}

	IMPLEMENT_SERIALIZE
	(
		if (!(nType & SER_GETHASH))
			READWRITE(nVersion);
		READWRITE(vHave);
	)

	void SetNull()
	{
		vHave.clear();
	}

	bool IsNull()
	{
		return vHave.empty();
	}

	void Set(const CBlockIndex* pindex)
	{
		vHave.clear();
		int nStep = 1;
		while (pindex)
		{
			vHave.push_back(pindex->GetBlockHash());

			// Exponentially larger steps back
			for (int i = 0; pindex && i < nStep; i++)
				pindex = pindex->pprev;
			if (vHave.size() > 10)
				nStep *= 2;
		}
		vHave.push_back(hashGenesisBlock);
	}

	int GetDistanceBack()
	{
		// Retrace how far back it was in the sender's branch
		int nDistance = 0;
		int nStep = 1;
		BOOST_FOREACH(const uint256& hash, vHave)
		{
			std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
			if (mi != mapBlockIndex.end())
			{
				CBlockIndex* pindex = (*mi).second;
				if (pindex->IsInMainChain())
					return nDistance;
			}
			nDistance += nStep;
			if (nDistance > 10)
				nStep *= 2;
		}
		return nDistance;
	}

	CBlockIndex* GetBlockIndex()
	{
		// Find the first block the caller has in the main chain
		BOOST_FOREACH(const uint256& hash, vHave)
		{
			std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
			if (mi != mapBlockIndex.end())
			{
				CBlockIndex* pindex = (*mi).second;
				if (pindex->IsInMainChain())
					return pindex;
			}
		}
		return pindexGenesisBlock;
	}

	uint256 GetBlockHash()
	{
		// Find the first block the caller has in the main chain
		BOOST_FOREACH(const uint256& hash, vHave)
		{
			std::map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
			if (mi != mapBlockIndex.end())
			{
				CBlockIndex* pindex = (*mi).second;
				if (pindex->IsInMainChain())
					return hash;
			}
		}
		return hashGenesisBlock;
	}

	int GetHeight()
	{
		CBlockIndex* pindex = GetBlockIndex();
		if (!pindex)
			return 0;
		return pindex->nHeight;
	}
};








class CTxMemPool
{
public:
	mutable CCriticalSection cs;
	std::map<uint256, CTransaction> mapTx;
	std::map<COutPoint, CInPoint> mapNextTx;

	bool accept(CValidationState &state, CTransaction &tx, bool fCheckInputs, bool fLimitFree, bool* pfMissingInputs);
	bool addUnchecked(const uint256& hash, const CTransaction &tx);
	bool remove(const CTransaction &tx, bool fRecursive = false);
	bool removeConflicts(const CTransaction &tx);
	void clear();
	void queryHashes(std::vector<uint256>& vtxid);
	void pruneSpent(const uint256& hash, CCoins &coins);

	unsigned long size()
	{
		LOCK(cs);
		return mapTx.size();
	}

	bool exists(uint256 hash)
	{
		return (mapTx.count(hash) != 0);
	}

	CTransaction& lookup(uint256 hash)
	{
		return mapTx[hash];
	}
};

extern CTxMemPool mempool;

struct CCoinsStats
{
	int nHeight;
	uint256 hashBlock;
	uint64_t nTransactions;
	uint64_t nTransactionOutputs;
	uint64_t nSerializedSize;
	uint256 hashSerialized;
	int64_t nTotalAmount;

	CCoinsStats() : nHeight(0), hashBlock(0), nTransactions(0), nTransactionOutputs(0), nSerializedSize(0), hashSerialized(0), nTotalAmount(0) {}
};

/** Abstract view on the open txout dataset. */
class CCoinsView
{
public:
	// Retrieve the CCoins (unspent transaction outputs) for a given txid
	virtual bool GetCoins(const uint256 &txid, CCoins &coins);

	// Modify the CCoins for a given txid
	virtual bool SetCoins(const uint256 &txid, const CCoins &coins);

	// Just check whether we have data for a given txid.
	// This may (but cannot always) return true for fully spent transactions
	virtual bool HaveCoins(const uint256 &txid);

	// Retrieve the block index whose state this CCoinsView currently represents
	virtual CBlockIndex *GetBestBlock();

	// Modify the currently active block index
	virtual bool SetBestBlock(CBlockIndex *pindex);

	// Do a bulk modification (multiple SetCoins + one SetBestBlock)
	virtual bool BatchWrite(const std::map<uint256, CCoins> &mapCoins, CBlockIndex *pindex);

	// Calculate statistics about the unspent transaction output set
	virtual bool GetStats(CCoinsStats &stats);

	// As we use CCoinsViews polymorphically, have a virtual destructor
	virtual ~CCoinsView() {}
};

/** CCoinsView backed by another CCoinsView */
class CCoinsViewBacked : public CCoinsView
{
protected:
	CCoinsView *base;

public:
	CCoinsViewBacked(CCoinsView &viewIn);
	bool GetCoins(const uint256 &txid, CCoins &coins);
	bool SetCoins(const uint256 &txid, const CCoins &coins);
	bool HaveCoins(const uint256 &txid);
	CBlockIndex *GetBestBlock();
	bool SetBestBlock(CBlockIndex *pindex);
	void SetBackend(CCoinsView &viewIn);
	bool BatchWrite(const std::map<uint256, CCoins> &mapCoins, CBlockIndex *pindex);
	bool GetStats(CCoinsStats &stats);
};

/** CCoinsView that adds a memory cache for transactions to another CCoinsView */
class CCoinsViewCache : public CCoinsViewBacked
{
protected:
	CBlockIndex *pindexTip;
	std::map<uint256,CCoins> cacheCoins;

public:
	CCoinsViewCache(CCoinsView &baseIn, bool fDummy = false);

	// Standard CCoinsView methods
	bool GetCoins(const uint256 &txid, CCoins &coins);
	bool SetCoins(const uint256 &txid, const CCoins &coins);
	bool HaveCoins(const uint256 &txid);
	CBlockIndex *GetBestBlock();
	bool SetBestBlock(CBlockIndex *pindex);
	bool BatchWrite(const std::map<uint256, CCoins> &mapCoins, CBlockIndex *pindex);

	// Return a modifiable reference to a CCoins. Check HaveCoins first.
	// Many methods explicitly require a CCoinsViewCache because of this method, to reduce
	// copying.
	CCoins &GetCoins(const uint256 &txid);

	// Push the modifications applied to this cache to its base.
	// Failure to call this method before destruction will cause the changes to be forgotten.
	bool Flush();

	// Calculate the size of the cache (in number of transactions)
	unsigned int GetCacheSize();

private:
	std::map<uint256,CCoins>::iterator FetchCoins(const uint256 &txid);
};

/** CCoinsView that brings transactions from a memorypool into view.
	It does not check for spendings by memory pool transactions. */
class CCoinsViewMemPool : public CCoinsViewBacked
{
protected:
	CTxMemPool &mempool;

public:
	CCoinsViewMemPool(CCoinsView &baseIn, CTxMemPool &mempoolIn);
	bool GetCoins(const uint256 &txid, CCoins &coins);
	bool HaveCoins(const uint256 &txid);
};

/** Global variable that points to the active CCoinsView (protected by cs_main) */
extern CCoinsViewCache *pcoinsTip;

/** Global variable that points to the active block tree (protected by cs_main) */
extern CBlockTreeDB *pblocktree;

struct CBlockTemplate
{
	CBlock block;
	std::vector<int64_t> vTxFees;
	std::vector<int64_t> vTxSigOps;
};

#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(_M_X64) || defined(__x86_64__) || defined(_M_AMD64)
extern unsigned int cpuid_edx;
#endif





/** Used to relay blocks as header + vector<merkle branch>
 * to filtered nodes.
 */
class CMerkleBlock
{
public:
	// Public only for unit testing
	CBlockHeader header;
	CPartialMerkleTree txn;

public:
	// Public only for unit testing and relay testing
	// (not relayed)
	std::vector<std::pair<unsigned int, uint256> > vMatchedTxn;

	// Create from a CBlock, filtering transactions according to filter
	// Note that this will call IsRelevantAndUpdate on the filter for each transaction,
	// thus the filter will likely be modified.
	CMerkleBlock(const CBlock& block, CBloomFilter& filter);

	IMPLEMENT_SERIALIZE
	(
		READWRITE(header);
		READWRITE(txn);
	)
};

/** things in main.cpp that coin.cpp need **/
bool FindBlockPos(CValidationState &state, CDiskBlockPos &pos, unsigned int nAddSize, unsigned int nHeight, uint64_t nTime, bool fKnown = false );


/** *coin.cpp functions **/ 
// FIXME DOXYGEN THIS
extern int64_t GetSeigniorage(const CBlockIndex * pPrev, int64_t nFees, int64_t CoinAge=0);
inline static int64_t GetProofOfWorkReward(const CBlockIndex* pPrev, int64_t nFees){
	return GetSeigniorage(pPrev, nFees, 0);
};
inline static int64_t GetBlockValue(const CBlockIndex * pPrev, int64_t nFees){
	return GetSeigniorage(pPrev, nFees, 0);
}; 
inline static int64_t GetProofOfStakeReward(int64_t nCoinAge, const CBlockIndex* pPrev=NULL){
	assert(nCoinAge > 0);
	return GetSeigniorage(pPrev, 0, nCoinAge);
};
extern int64_t GetPoW_seigniorage(CBlockIndex *block, int64_t nFees);
extern unsigned int ComputeMinWork(unsigned int nBase, int64_t nTime, const CBlockHeader* pblock = NULL);
bool AcceptBlockTimestamp(CValidationState &state, CBlockIndex* pindexPrev, const CBlockHeader *pblock);
/* takes the last block, and the current block header, returns nbits of trust */
extern unsigned int GetNextTrustRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock);
extern bool LoadBlockIndex(); 
extern bool InitBlockIndex();

#endif
