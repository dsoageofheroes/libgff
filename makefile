.PHONY: clean test all buildit test generate install

all: buildit

build:
	mkdir -p build

buildit: build
	cd build ; cmake -DCMAKE_BUILD_TYPE=Debug ../ ; make

install:
	cd build ; cmake -DCMAKE_INSTALL_PREFIX=`pwd`/../release/linux .
	cd build ; make install

clean:
	rm -rf build

test: buildit
	cd build ; make test
