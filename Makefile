.PHONY: run build run_build

run: build run_build

build:
	@cmake -B build -DCMAKE_BUILD_TYPE=Release > /dev/null
	@cmake --build build > /dev/null

run_build: build
	@./build/main
