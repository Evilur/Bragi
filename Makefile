.PHONY: all run configure build clean

all: build

run: build
	@if [ -f .env ]; then \
		set -a;\
		. ./.env;\
	fi; \
	exec build/Bragi

configure:
	cmake -B build \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cmake --build build -j$(nproc)

clean:
	rm -rf build
