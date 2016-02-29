.PHONY: clean

all: main.lua.bin

main.lua.bin: main.lua
	luac -s -o $@ $< 

clean:
	rm -f main.lua.bin

