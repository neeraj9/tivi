LIBS = -lm -lcurses
CPP = g++
CPPFLAGS = -g

HEADERS = eutil.h editor9.h

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: tivi

tivi: $(OBJECTS)
	$(CPP) $(LIBS) $(CPPFLAGS) $(OBJECTS) -o $@

%.o: %.c $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $@ -c $<

clean:
	rm -f *.o tivi

.PHONY: all clean
