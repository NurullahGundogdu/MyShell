all: gtushell

gtushell:
	gcc -c -std=gnu99 gtushell.c -Wall
	gcc -c -std=gnu99 lsf.c -Wall
	gcc -c -std=gnu99 cat.c -Wall
	gcc -c -std=gnu99 bunedu.c -Wall
	gcc -c -std=gnu99 pwd.c -Wall
	gcc -c -std=gnu99 wc.c -Wall

	gcc -std=gnu99 gtushell.c -o gtushell
	gcc lsf.c -o lsf
	gcc cat.c -o cat
	gcc bunedu.c -o bunedu
	gcc pwd.c -o pwd
	gcc wc.c -o wc

clean:
	rm -f *.o gtushell pwd cat bunedu lsf wc


