CXXFLAGS = -std=c++14 -fopenmp -O3 -mavx2 $(INCLUDES)

INCLUDES := -Ilib -Ilib/yaml-cpp/include
OBJS := build/main.o build/scene.o build/image.o build/libyaml-cpp.a
PCH := build/all.gch
HEADERS := $(wildcard src/*.h)

all: mkdir main

main: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^

build/%.o: src/%.cc $(PCH) $(HEADERS)
	$(CXX) -c -o $@ $(CXXFLAGS) -include $(PCH:.gch=) $<

$(PCH): src/all.h
	$(CXX) -c -o $@ $(CXXFLAGS) -x c++-header $<

build/libyaml-cpp.a:
	cd build/yaml-cpp \
		&& cmake ../../lib/yaml-cpp \
		&& cmake --build . \
		&& mv libyaml-cpp.a ..

clean:
	rm -f build/*.o build/*.gch

mkdir:
	@mkdir -p build/yaml-cpp

FORCE:

.PHONY: all clean mkdir FORCE

-include run.mk
