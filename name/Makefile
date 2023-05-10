all: clean
	ca65 name.asm -o name.o -t nes
	ld65 name.o -o name.nes -t nes

clean:
	rm -f name.0
	rm -f name.nes
	
