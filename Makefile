OBJS = src/DARBY_main.o \
       src/DARBY_match.o \
       src/DARBY_ref.o \
       src/DARBY_render.o \
       src/intset.o \
       src/strutil.o \
       data/DARBY_data.o
CFLAGS += -Wall -Isrc/
LDLIBS += -lreadline

DARBY: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LDLIBS)

src/DARBY_main.o: src/DARBY_main.c src/DARBY_config.h src/DARBY_data.h src/DARBY_match.h src/DARBY_ref.h src/DARBY_render.h src/strutil.h

src/DARBY_match.o: src/DARBY_match.h src/DARBY_match.c src/DARBY_config.h src/DARBY_data.h src/DARBY_ref.h

src/DARBY_ref.o: src/DARBY_ref.h src/DARBY_ref.c src/intset.h src/DARBY_data.h

src/DARBY_render.o: src/DARBY_render.h src/DARBY_render.c src/DARBY_config.h src/DARBY_data.h src/DARBY_match.h src/DARBY_ref.h

src/insetset.o: src/intset.h src/insetset.c

src/strutil.o: src/strutil.h src/strutil.c

data/DARBY_data.o: src/DARBY_data.h data/DARBY_data.c

data/DARBY_data.c: data/DARBY.tsv data/generate.awk src/DARBY_data.h
	awk -f data/generate.awk $< > $@

.PHONY: clean
clean:
	rm -rf $(OBJS) DARBY
