build: alt64
	gcc -o alt64 alt64.c

clean:
	rm -f alt64

install: build
	install ./hello $(DESTDIR)/usr/bin

