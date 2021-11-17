CFLAGS = -O3
CXXFLAGS = -std=c++14 -fopenmp -O3 -mavx2 $(INCLUDES)

INCLUDES := -Ilib -Ilib/yaml-cpp/include
OBJS := build/main.o build/scene.o build/image.o build/libyaml-cpp.a
PCH := build/all.gch
HEADERS := $(wildcard src/*.h)

all: mkdir main

main: $(OBJS)
	g++ -o $@ $(CXXFLAGS) $^

build/main.o: FORCE

build/%.o: src/%.cc $(PCH) $(HEADERS)
	g++ -c -o $@ $(CXXFLAGS) -include $(PCH:.gch=) $<

$(PCH): src/all.h
	g++ -c -o $@ $(CXXFLAGS) $<

build/image.o: lib/stb_image_write.h
	gcc -c -o $@ $(CFLAGS) -DSTB_IMAGE_WRITE_IMPLEMENTATION -x c $<

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
