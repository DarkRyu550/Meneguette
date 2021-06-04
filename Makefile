
CXX=g++
CXXFLAGS=-Wall -Wextra -Werror -pedantic -fmodules-ts -std=c++20 -g

all: Socket

Socket: Socket/Makefile
	make -C Socket/ CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" all

.PHONY: Socket
