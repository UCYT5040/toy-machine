.PHONY: all run clean

all:
	@cmake -B build -G Ninja
	@cmake --build build

run: all
	./build/toy_machine

clean:
	rm -rf build