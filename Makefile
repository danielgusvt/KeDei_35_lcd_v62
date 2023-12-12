all:
	gcc -Wall -O3 -o kedei_lcd_v62_opi kedei_lcd_v62_opi.c -lwiringPi
clean:
	rm -f kedei_lcd_v62_opi
