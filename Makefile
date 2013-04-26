CXX      = g++
CXXFLAGS = -Wall -O2 -I.
TARGETS  = tlu-ctl 

all:    $(TARGETS)

clean:
	rm -f $(TARGETS) *.o lib/*.o git.*

tlu-ctl:        tlu-ctl.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -letherbone -Wl,-rpath,/usr/local/lib

%.o:    %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
