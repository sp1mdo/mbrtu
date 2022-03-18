mbrtu: mbrtu.c
	gcc -I/usr/include/modbus/ -o mbrtu mbrtu.c -lmodbus
clean:
	rm -f mbrtu

