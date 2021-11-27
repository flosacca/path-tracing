CXXFLAGS = -std=c++14 -O3 -mavx2 -fopenmp $(INCLUDES)

INCLUDES := -Ilib -Ilib/yaml-cpp/include
OBJS := build/main.o \
	build/scene.o \
	build/ext/ply.o \
	build/ext/image.o \
	build/libyaml-cpp.a
PCH := build/all.gch
HEADERS := $(wildcard src/*.h)

all: mkdir main

main: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^

build/%.o: src/%.cc $(PCH) $(HEADERS)
	$(CXX) -c -o $@ $(CXXFLAGS) -include $(PCH:.gch=) $<

build/ext/%.o: src/ext/%.cc src/ext/%.h
	$(CXX) -c -o $@ $(CXXFLAGS) $<

$(PCH): src/all.h Makefile
	$(CXX) -c -o $@ $(CXXFLAGS) -x c++-header $<

build/libyaml-cpp.a:
	cd build/yaml-cpp \
		&& cmake ../../lib/yaml-cpp \
		&& cmake --build . \
		&& mv libyaml-cpp.a ..

clean:
	rm -f $(PCH) build/*.o

mkdir:
	@mkdir -p build/ext build/yaml-cpp

FORCE:

.PHONY: all clean mkdir FORCE

-include run.mk
