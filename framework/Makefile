all: clean
	ca65 framework.asm -o framework.o -t nes
	ld65 framework.o -o framework.nes -t nes

clean:
	rm -f framework.0
	rm -f framework.nes
	
