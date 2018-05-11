#include "main.h"

/** Undo information for a CBlock */
	bool CBlockUndo::WriteToDisk(CDiskBlockPos &pos, const uint256 &hashBlock)
	{
		// Open history file to append
		CAutoFile fileout = CAutoFile(OpenUndoFile(pos), SER_DISK, CLIENT_VERSION);
		if (!fileout)
			return error("CBlockUndo::WriteToDisk() : OpenUndoFile failed");

		// Write index header
		unsigned int nSize = fileout.GetSerializeSize(*this);
		fileout << FLATDATA(pchMessageStart) << nSize;

		// Write undo data
		long fileOutPos = ftell(fileout);
		if (fileOutPos < 0)
			return error("CBlockUndo::WriteToDisk() : ftell failed");
		pos.nPos = (unsigned int)fileOutPos;
		fileout << *this;

		// calculate & write checksum
		CHashWriter hasher(SER_GETHASH, PROTOCOL_VERSION);
		hasher << hashBlock;
		hasher << *this;
		fileout << hasher.GetHash();

		// Flush stdio buffers and commit to disk before returning
		fflush(fileout);
		if (!IsInitialBlockDownload())
			FileCommit(fileout);

		return true;
	}

	bool CBlockUndo::ReadFromDisk(const CDiskBlockPos &pos, const uint256 &hashBlock)
	{
		// Open history file to read
		CAutoFile filein = CAutoFile(OpenUndoFile(pos, true), SER_DISK, CLIENT_VERSION);
		if (!filein)
			return error("CBlockUndo::ReadFromDisk() : OpenBlockFile failed");

		// Read block
		uint256 hashChecksum;
		try {
			filein >> *this;
			filein >> hashChecksum;
		}
		catch (std::exception &e) {
			return error("%s() : deserialize or I/O error", __PRETTY_FUNCTION__);
		}

		// Verify checksum
		CHashWriter hasher(SER_GETHASH, PROTOCOL_VERSION);
		hasher << hashBlock;
		hasher << *this;
		if (hashChecksum != hasher.GetHash())
			return error("CBlockUndo::ReadFromDisk(): checksum mismatch: File: %d Pos: %u", pos.nFile, pos.nPos);

		return true;
	}

