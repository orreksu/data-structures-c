CFLAGS := -g
LDLIBS := -lbsd

top-triples: top-triples.o btree.o
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

top-triples.o: top-triples.c btree.h
	gcc $(CFLAGS) -c -o $@ $<

btree.o: btree.c btree.h
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o top-triples valgrind.out *.plist

.PHONY: all clean
