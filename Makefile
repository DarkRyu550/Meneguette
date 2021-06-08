
CC=cc
CXX=c++
CXXFLAGS=-Wall -Wextra -Werror -pedantic -fmodules-ts -std=c++20 -g

PROJECTS=Socket
all: $(PROJECTS)

Socket: Socket/Makefile
	make -C Socket/ CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)" all

clean:
	rm -rf gcm.cache
	for i in $(PROJECTS); do make -C $$i clean; done

.PHONY: $(PROJECTS) all clean
