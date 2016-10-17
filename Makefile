CC = gcc
CFLAGSDEBUG = -g -Wall -c -I/home/$(USER)/local/include/ -I/usr/include/ -DCIC_400
CFLAGS = -c -O3 -I$(HOME)/local/include/ -I/usr/include/ -DCIC_400 -DDTDR -DZLOS 
#ZLOS is along z-axis as Line Oof Sight. The same applies for XLOS and YLOS to integrate alont the respective axis
CFLAGSSUPERCIC = -c -O3 -I$(HOME)/local/include/ -I/usr/include/ -DSUPERCIC -DDTDR -DZLOS
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

SUPERCIC:
	$(CC) $(CFLAGSSUPERCIC) $(PROGRAM).c -o $(PROGRAM).o
	$(CC) $(PROGRAM).o $(LFLAGS) -lgsl -lgslcblas -lm -o $(PROGRAM)
	mv main_interp_SW_integral Interp_superCIC.x

clean:
	rm -rf $(PROGRAM)
	rm -rf *~
	rm -rf *.out
	rm -rf *#
	rm -rf *.o
	rm -rf *.a      
	rm -rf *.so
	rm *.x
