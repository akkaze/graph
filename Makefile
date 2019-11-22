ifndef CXX
CXX = clang++
endif

INCPATH = -I./graph
CFLAGS = -std=c++11 -fPIC -Os -finline-functions $(INCPATH)

all: test

clean:
	rm -rf build $(TEST) test/*.d

include test/test.mk
test: $(TEST)
