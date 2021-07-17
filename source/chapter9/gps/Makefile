all:
	#gcc -std=c99 -DX86_PC -g -o test_gps test_gps.c gps.c -lpthread -lm
	arm-linux-gcc -std=c99 -DPXA_XSCALE -g -o test_gps test_gps.c gps.c -lpthread -lm

clean:
	rm -rf ./*.o
