make:
	make -f Makefile.static
	make -f Makefile.dynamic
	gcc -o graph main.c

clean:
	rm *.o





