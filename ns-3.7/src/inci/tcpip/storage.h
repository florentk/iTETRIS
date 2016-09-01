/************************************************************************
 ** This file is part of the network simulator Shawn.                  **
 ** Copyright (C) 2004-2007 by the SwarmNet (www.swarmnet.de) project  **
 ** Shawn is free software; you can redistribute it and/or modify it   **
 ** under the terms of the BSD License. Refer to the shawn-licence.txt **
 ** file in the root of the Shawn source tree for further details.     **
 ************************************************************************
 **                                                                    **
 ** \author Axel Wegener <wegener@itm.uni-luebeck.de>                  **
 **                                                                    **
 ************************************************************************/
#ifndef __SHAWN_APPS_TCPIP_STORAGE_H
#define __SHAWN_APPS_TCPIP_STORAGE_H

#ifdef SHAWN
     #include <shawn_config.h>
     #include "_apps_enable_cmake.h"
     #ifdef ENABLE_TCPIP
            #define BUILD_TCPIP
     #endif
#else
     #define BUILD_TCPIP
#endif


#ifdef BUILD_TCPIP

#include <vector>
#include <string>
#include <stdexcept>

namespace tcpip
{

class Storage
{

public:
	typedef std::vector<unsigned char> StorageType;

private:
	StorageType store;

	unsigned int pos_;
	bool iterValid_;
	bool iterEndValid_;

	StorageType::const_iterator iter_;
	StorageType::const_iterator iterEnd_;

	// sortation of bytes forwards or backwards?
	bool bigEndian_;
	
	/// Used in constructors to initialize local variables
	void init();

public:

	/// Standard Constructor
	Storage();

	/// Constructor, that fills the storage with an char array. If length is -1, the whole array is handed over
	Storage(unsigned char[], int length=-1);

	// Destructor
	virtual ~Storage();

	virtual bool valid_pos();
	virtual unsigned int position() const;

	void reset();

	virtual unsigned char readChar() throw(std::invalid_argument);
	virtual void writeChar(unsigned char) throw();

	virtual int readByte() throw(std::invalid_argument);
	virtual void writeByte(int) throw(std::invalid_argument);

	virtual int readUnsignedByte() throw(std::invalid_argument);
	virtual void writeUnsignedByte(int) throw(std::invalid_argument);

	virtual std::string readString() throw(std::invalid_argument);
	virtual void writeString(std::string s) throw();

	virtual std::vector<std::string> readStringList() throw(std::invalid_argument);
	virtual void writeStringList(const std::vector<std::string> &s) throw();

	virtual std::vector<int> readIntList() throw(std::invalid_argument);
	virtual void writeIntList(const std::vector<int> &s) throw();

	virtual std::vector<float> readFloatList() throw(std::invalid_argument);
	virtual void writeFloatList(const std::vector<float> &s) throw();

	virtual int readShort() throw(std::invalid_argument);
	virtual void writeShort(int) throw(std::invalid_argument);

	virtual int readInt() throw(std::invalid_argument);
	virtual void writeInt(int) throw();

	virtual float readFloat() throw(std::invalid_argument);
	virtual void writeFloat( float ) throw();

	virtual double readDouble() throw(std::invalid_argument);
	virtual void writeDouble( double ) throw();

	virtual void writePacket(unsigned char* packet, int length);

	virtual void writeStorage(tcpip::Storage& store);

	// Some enabled functions of the underlying std::list
	size_t size() const { return store.size(); }

	StorageType::const_iterator begin() const { return store.begin(); }
	StorageType::const_iterator end() const { return store.end(); }

};

} // namespace tcpip

#endif // BUILD_TCPIP

#endif
/*-----------------------------------------------------------------------
 * Source  $Source: $
 * Version $Revision: 342 $
 * Date    $Date: 2009-04-14 13:56:09 +0200 (mar, 14 abr 2009) $
 *-----------------------------------------------------------------------
 * $Log: $
 *-----------------------------------------------------------------------*/
