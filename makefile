.PHONY: clean test all buildit test generate install

all: buildit

build:
	mkdir -p build

buildit: build
	cd build ; cmake -DCMAKE_BUILD_TYPE=Debug ../ ; make
	#cd build ; cmake -DCMAKE_BUILD_TYPE=MinSizeRel ../ ; make

install:
	cd build ; cmake -DCMAKE_INSTALL_PREFIX=`pwd`/../release/linux .
	cd build ; make install

clean:
	rm -rf build bin obj

test: buildit
	cd build ; env CTEST_OUTPUT_ON_FAILURE=1 make test
