#basic makefile for Planet Wars bot (Antimatroid)
objects=MyBot.o GameState.o Tree.o
cppfiles=MyBot.cc GameState.cc Tree.cc



CXX?=g++
CXXFLAGS+=-std=c++11 -O3
#CXXFLAGS=-std=c++11 -Wall -Wextra -Werror -pedantic -O3
LDFLAGS+=

###

all: MyBot

###

MyBot: $(objects)
	$(CXX) $(CXXFLAGS) $(objects) -o antimatroid $(LDFLAGS)

MyBot.o: MyBot.cc GameState.o Tree.o General.h GameState.h Tree.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

GameState.o: GameState.cc GameState.h Planet.h FutureState.h Fleet.h Move.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

Tree.o: Tree.cc Tree.h General.h GameState.h Node.h Move.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

###

clean:
	rm *.o

clean-all: clean
	rm antimatroid

