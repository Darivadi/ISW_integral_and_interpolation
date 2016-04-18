CC = gcc
CFLAGSDEBUG = -g -Wall -c -I/home/$(USER)/local/include/ -I/usr/include/ -DBINARYDATA -DSIMPSON
CFLAGSSIMP = -g -Wall -c -I/home/$(USER)/local/include/ -I/usr/include/ -DBINARYDATA -DSIMPSON
CFLAGSASCII = -c -O3 -Wall -I/home/$(USER)/local/include/ -I/usr/include/ -DASCIIDATA
CFLAGS = -c -O3 -I$(HOME)/local/include/ -I/usr/include/ -DBINARYDATA -DGSLINTERPINTEG
LFLAGS = -lm -L$(HOME)/local/lib -Wl,"-R /export/$(USER)/local/lib"


PROGRAM = main_interp_SW_integral

$(PROGRAM):
	$(CC) $(CFLAGS) $@.c -o $@.o
	$(CC) $@.o $(LFLAGS) -lgsl -lgslcblas -lm -o $@
	mv main_interp_SW_integral Interp_testing.x

debug:
	echo Compiling for debug $(PROGRAM).c
	$(CC) $(CFLAGSDEBUG) $(PROGRAM).c -o $(PROGRAM).o
	$(CC) $(PROGRAM).o $(LFLAGS) -lgsl -lgslcblas -lm -o $(PROGRAM).x

asciidata:
	echo Compiling for ascii $(PROGRAM).c
	$(CC) $(CFLAGSASCII) $(PROGRAM).c -o $(PROGRAM).o
	$(CC) $(PROGRAM).o $(LFLAGS) -lgsl -lgslcblas -lm -o $(PROGRAM).x

simpsonint:
	$(CC) $(CFLAGSSIMP) $(PROGRAM).c -o $(PROGRAM).o
	$(CC) $(PROGRAM).o $(LFLAGS) -lgsl -lgslcblas -lm -o $(PROGRAM)
	mv main_interp_SW_integral Interp_testing_simp.x

clean:
	rm -rf $(PROGRAM)
	rm -rf *~
	rm -rf *.out
	rm -rf *#
	rm -rf *.o
	rm -rf *.a      
	rm -rf *.so
	rm *.x
