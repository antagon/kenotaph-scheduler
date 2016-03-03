.PHONY: clean

all: main.lua.h

main.lua.h: main.lua
	lua bin2h.lua +main.lua > $@

clean:
	rm -f main.lua.h

