# Copyright (c) 2009-2010 Satoshi Nakamoto
# Copyright (c) 2011-201? The *Coin developers
# where * = (Lite, PP, Peerunity, Blu, Cat, Solar, URO, ...)
# Previously distributed under the MIT/X11 software license, see the
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
# Copyright (c) 2014-2015 Troy Benjegerdes, under AGPLv3
# Distributed under the Affero GNU General public license version 3
# file COPYING or http://www.gnu.org/licenses/agpl-3.0.html
# Copyright (c) 2015 The Manna Foundation


# :=0 --> UPnP support turned off by default at runtime
# :=1 --> UPnP support turned on by default at runtime
# :=- --> No UPnP support - miniupnp not required
USE_UPNP:=0

# :=1 --> Enable IPv6 support
# :=0 --> Disable IPv6 support
USE_IPV6:=1

# :=1 --> enable SSE
# :=0 --> disable SSE
USE_SSE2:=1

LINK:=$(CXX)

COIN_BRAND:=manna

DEFS=-DBOOST_SPIRIT_THREADSAFE -D_FILE_OFFSET_BITS=64 -DBRAND_$(COIN_BRAND)

OBJDIR = obj/$(COIN_BRAND)
TESTDIR = $(OBJDIR)/test

DEFS += $(addprefix -I,$(CURDIR) $(CURDIR)/$(OBJDIR) $(BOOST_INCLUDE_PATH) $(BDB_INCLUDE_PATH) $(OPENSSL_INCLUDE_PATH))
LIBS = $(addprefix -L,$(BOOST_LIB_PATH) $(BDB_LIB_PATH) $(OPENSSL_LIB_PATH))

TESTDEFS = -DTEST_DATA_DIR=$(abspath test/data)


LMODE = dynamic
LMODE2 = dynamic
ifdef STATIC
	LMODE = static
	ifeq (${STATIC}, all)
		LMODE2 = static
	endif
else
	TESTDEFS += -DBOOST_TEST_DYN_LINK
endif

# for boost 1.37, add -mt to the boost libraries
LIBS += \
 -Wl,-B$(LMODE) \
   -l boost_system$(BOOST_LIB_SUFFIX) \
   -l boost_filesystem$(BOOST_LIB_SUFFIX) \
   -l boost_program_options$(BOOST_LIB_SUFFIX) \
   -l boost_thread$(BOOST_LIB_SUFFIX) \
   -l db_cxx$(BDB_LIB_SUFFIX) \
   -l ssl \
   -l crypto

TESTLIBS += \
 -Wl,-B$(LMODE) \
   -l boost_unit_test_framework$(BOOST_LIB_SUFFIX)

ifndef USE_UPNP
	override USE_UPNP = -
endif
ifneq (${USE_UPNP}, -)
	LIBS += -l miniupnpc
	DEFS += -DUSE_UPNP=$(USE_UPNP)
endif

ifneq (${USE_IPV6}, -)
	DEFS += -DUSE_IPV6=$(USE_IPV6)
endif

LIBS+= \
 -Wl,-B$(LMODE2) \
   -l z \
   -l dl \
   -l pthread


# Hardening
# Make some classes of vulnerabilities unexploitable in case one is discovered.
#
    # This is a workaround for Ubuntu bug #691722, the default -fstack-protector causes
    # -fstack-protector-all to be ignored unless -fno-stack-protector is used first.
    # see: https://bugs.launchpad.net/ubuntu/+source/gcc-4.5/+bug/691722
    HARDENING=-fno-stack-protector

    # Stack Canaries
    # Put numbers at the beginning of each stack frame and check that they are the same.
    # If a stack buffer if overflowed, it writes over the canary number and then on return
    # when that number is checked, it won't be the same and the program will exit with
    # a "Stack smashing detected" error instead of being exploited.
    HARDENING+=-fstack-protector-all -Wstack-protector

    # Make some important things such as the global offset table read only as soon as
    # the dynamic linker is finished building it. This will prevent overwriting of addresses
    # which would later be jumped to.
    LDHARDENING+=-Wl,-z,relro -Wl,-z,now

    # Build position independent code to take advantage of Address Space Layout Randomization
    # offered by some kernels.
    # see doc/build-unix.txt for more information.
    PIE=1
    ifdef PIE
        HARDENING+=-fPIE
        LDHARDENING+=-pie
    endif

    # -D_FORTIFY_SOURCE=2 does some checking for potentially exploitable code patterns in
    # the source such overflowing a statically defined buffer.
    HARDENING+=-D_FORTIFY_SOURCE=2
#


DEBUGFLAGS=-g

# CXXFLAGS can be specified on the make command line, so we use xCXXFLAGS that only
# adds some defaults in front. Unfortunately, CXXFLAGS=... $(CXXFLAGS) does not work.
xCXXFLAGS=-O2 -pthread -Wall -Wextra -Wformat -Wformat-security -Wno-unused-parameter \
    $(DEBUGFLAGS) $(DEFS) $(HARDENING) $(CXXFLAGS)

# LDFLAGS can be specified on the make command line, so we use xLDFLAGS that only
# adds some defaults in front. Unfortunately, LDFLAGS=... $(LDFLAGS) does not work.
xLDFLAGS=$(LDHARDENING) $(LDFLAGS)

STAKE_OBJS = \
    $(OBJDIR)/kernel.o \
    $(OBJDIR)/kernelrecord.o

OBJS= $(OBJDIR)/$(COIN_BRAND).o \
    leveldb/libleveldb.a \
    $(OBJDIR)/alert.o \
    $(OBJDIR)/version.o \
    $(OBJDIR)/checkpoints.o \
    $(OBJDIR)/netbase.o \
    $(OBJDIR)/addrman.o \
    $(OBJDIR)/crypter.o \
    $(OBJDIR)/key.o \
    $(OBJDIR)/db.o \
    $(OBJDIR)/init.o \
    $(OBJDIR)/keystore.o \
    $(OBJDIR)/main.o \
    $(OBJDIR)/undo.o \
    $(OBJDIR)/net.o \
    $(OBJDIR)/protocol.o \
    $(OBJDIR)/codecoinrpc.o \
    $(OBJDIR)/rpcdump.o \
    $(OBJDIR)/rpcnet.o \
    $(OBJDIR)/rpcmining.o \
    $(OBJDIR)/rpcwallet.o \
    $(OBJDIR)/rpcblockchain.o \
    $(OBJDIR)/rpcrawtransaction.o \
    $(OBJDIR)/script.o \
    $(OBJDIR)/sync.o \
    $(OBJDIR)/util.o \
    $(OBJDIR)/wallet.o \
    $(OBJDIR)/walletdb.o \
    $(OBJDIR)/hash.o \
    $(OBJDIR)/bloom.o \
    $(OBJDIR)/noui.o \
    $(OBJDIR)/leveldb.o \
    $(OBJDIR)/txdb.o

X11_OBJS = \
    $(OBJDIR)/cubehash.o \
    $(OBJDIR)/luffa.o \
    $(OBJDIR)/aes_helper.o \
    $(OBJDIR)/echo.o \
    $(OBJDIR)/shavite.o \
    $(OBJDIR)/simd.o \
    $(OBJDIR)/blake.o \
    $(OBJDIR)/bmw.o \
    $(OBJDIR)/groestl.o \
    $(OBJDIR)/jh.o \
    $(OBJDIR)/keccak.o \
    $(OBJDIR)/skein.o

SCRYPT_OBJS = $(OBJDIR)/scrypt.o

ifdef USE_SSE2
DEFS += -DUSE_SSE2
OBJS_SSE2= $(OBJDIR)/scrypt-sse2.o
SCRYPT_OBJS += $(OBJS_SSE2)
endif

#ifeq ($(COIN_BRAND),manna)
#OBJS += $(STAKE_OBJS)
#endif
ifeq ($(COIN_BRAND),mannastake)
OBJS += $(STAKE_OBJS)
endif

all: $(TESTDIR) $(COIN_BRAND)d

test check: $(TESTDIR) test_$(COIN_BRAND) FORCE
	./test_$(COIN_BRAND)

$(TESTDIR):
	mkdir -p $(TESTDIR)

#
# LevelDB support
#
MAKEOVERRIDES =
LIBS += $(CURDIR)/leveldb/libleveldb.a $(CURDIR)/leveldb/libmemenv.a
DEFS += $(addprefix -I,$(CURDIR)/leveldb/include)
DEFS += $(addprefix -I,$(CURDIR)/leveldb/helpers)
leveldb/libleveldb.a:
	@echo "Building LevelDB ..." && cd leveldb && $(MAKE) CC=$(CC) CXX=$(CXX) OPT="$(xCXXFLAGS)" libleveldb.a libmemenv.a && cd ..


# auto-generated dependencies:
-include $(OBJDIR)/*.P
-include $(OBJDIR)/test/*.P

$(OBJDIR)/build.h: FORCE
	/bin/sh ../share/genbuild.sh $(OBJDIR)/build.h
version.cpp: $(OBJDIR)/build.h
DEFS += -DHAVE_BUILD_INFO

$(OBJDIR)/%-sse2.o: %-sse2.cpp
	$(CXX) -c $(xCXXFLAGS) -msse2 -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

$(OBJDIR)/%.o: %.c
	$(CXX) -c $(xCXXFLAGS) -fpermissive -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

$(OBJDIR)/%.o: %.cpp
	$(CXX) -c $(xCXXFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

$(COIN_BRAND)d: $(OBJS:$(OBJDIR)/%=$(OBJDIR)/%)
	$(LINK) $(xCXXFLAGS) -o $@ $^ $(xLDFLAGS) $(LIBS)


TESTOBJS := $(patsubst test/%.cpp,$(OBJDIR)/test/%.o,$(wildcard test/*.cpp))

$(OBJDIR)/test/%.o: test/%.cpp
	$(CXX) -c $(TESTDEFS) $(xCXXFLAGS) -MMD -MF $(@:%.o=%.d) -o $@ $<
	@cp $(@:%.o=%.d) $(@:%.o=%.P); \
	  sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	      -e '/^$$/ d' -e 's/$$/ :/' < $(@:%.o=%.d) >> $(@:%.o=%.P); \
	  rm -f $(@:%.o=%.d)

test_$(COIN_BRAND): $(TESTOBJS) $(filter-out $(OBJDIR)/init.o,$(OBJS:$(OBJDIR)/%=$(OBJDIR)/%))
	$(LINK) $(xCXXFLAGS) -o $@ $(LIBPATHS) $^ $(TESTLIBS) $(xLDFLAGS) $(LIBS)

clean:
	-rm -f $(COIN_BRAND)d test_$(COIN_BRAND)
	-rm -f $(OBJDIR)/*.o
	-rm -f $(OBJDIR)/test/*.o
	-rm -f $(OBJDIR)/*.P
	-rm -f $(OBJDIR)/test/*.P
	-rm -f $(OBJDIR)/build.h
	-cd leveldb && $(MAKE) clean || true

FORCE:
