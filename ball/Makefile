all: clean
	ca65 ball.asm -o ball.o -t nes
	ld65 ball.o -o ball.nes -t nes

clean:
	rm -f ball.o
	rm -f ball.nes
	
