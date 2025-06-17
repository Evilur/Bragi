.PHONY: all configure build clean

all: build

configure:
	cmake -B build

build: configure
	cmake --build build -j12

clean:
	rm -rf build
