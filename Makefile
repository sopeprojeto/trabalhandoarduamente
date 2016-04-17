all: createdir lsdir rmdup

createdir: 
	mkdir -p bin/
	
lsdir: lsdir.c
	gcc -Wall -o bin/lsdir lsdir.c

rmdup: rmdup.c
	gcc -Wall -o bin/rmdup rmdup.c

trash:
	rm -rf *.o *.d

clean:
	rm -rf *.o *.d bin/lsdir bin/rmdup