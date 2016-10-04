CXX?=g++
CXXFLAGS=-Wall
# -Wall: turn on almost all warnings
# -c: compile only. Produces .o (object) files. No linking
# -o: name the output file
# $^: variable representing the full list of the dependencies in the target
#      in which it is mentioned
# $@: variable representing the name of the target in which it is mentioned

all: sodaCommand sodaDaemon

sodaCommand: sodaCommand.cpp sodaMachine.o
	$(CXX) $(CXXFLAGS) $^ -o $@

sodaDaemon: sodaDaemon.cpp sodaMachine.o
	$(CXX) $(CXXFLAGS) $^ -o $@

sodaMachine.o: sodaMachine.h

sodaMCU: soda8951.h, reg89C51.h, sodaMCU.c
	gcc $^ -S -o $@

# make already knows that file.h depends on file.cpp

clean:
	rm -rf *.o *.exe sodaTest sodaCommand sodaDaemon log pipes