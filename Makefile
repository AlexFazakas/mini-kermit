all: build 

build: ksender kreceiver

ksender: ksender.o utilities.o link_emulator/lib.o
	gcc -g ksender.o utilities.o link_emulator/lib.o -o ksender

kreceiver: kreceiver.o utilities.o link_emulator/lib.o
	gcc -g kreceiver.o utilities.o link_emulator/lib.o -o kreceiver

.c.o: 
	gcc -Wall -g -c $? 

clean:
	-rm -f *.o ksender kreceiver 
