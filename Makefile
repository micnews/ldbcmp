
CXX ?= g++
BIN ?= ldbcmp
SRC = ldbcmp.cc
PREFIX ?= /usr/local
LEVELDBPATH ?= ./deps/leveldb
CXXFLAGS += -I$(LEVELDBPATH)/include

# Ubuntu uses GNU C.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -std=gnu++11 -stdlib=libc++
endif

export CXXFLAGS
all: $(LEVELDBPATH)/libleveldb.a $(BIN)

$(LEVELDBPATH)/libleveldb.a:
	$(MAKE) -C $(LEVELDBPATH)

$(BIN):
	$(CXX) -o $(BIN) ./deps/cityhash/src/city.o $(LEVELDBPATH)/libleveldb.a $(SRC) $(CXXFLAGS) -lpthread

clean:
	rm -f $(BIN)
	$(MAKE) clean -C $(LEVELDBPATH)

install: all
	install $(BIN) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

