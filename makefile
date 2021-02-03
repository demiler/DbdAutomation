all: build

hooks.dll: hooks.c hooks.def
	gcc -c hooks.c -o hooks.o
	gcc -shared -o hooks.dll hooks.o hooks.def '-Wl,--enable-stdcall-fixup'

build/logs.o: logs.c
	 gcc -c -o $@ $<

build/utils.o: utils.c
	 gcc -c -o $@ $<

build/handlers.o: handlers.c
	 gcc -c -o $@ $<

build/emulator.o: emulator.c
	 gcc -c -o $@ $<

build: main.c\
	build/logs.o\
	build/handlers.o\
	build/emulator.o\
	build/utils.o	
		gcc -o run\
			build/logs.o\
			build/handlers.o\
			build/emulator.o\
			build/utils.o\
			main.c\
			-lwinmm

clear:
	rm hooks.o
	rm hooks.dll
	rm run.exe
