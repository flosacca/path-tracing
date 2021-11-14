CFLAGS := -O3
CXXFLAGS := -std=c++17 -fopenmp -O3 -mavx2 -Ilib

OBJS := build/image.o
PCH := build/glm_all.gch

all: mkdir main

main: $(PCH) $(OBJS) FORCE
	g++ -o $@ $(CXXFLAGS) -include $(PCH:.gch=) src/main.cc $(OBJS)

$(PCH): src/glm_all.h
	g++ -c -o $@ $(CXXFLAGS) $<

build/image.o: lib/stb_image_write.h
	gcc -c -o $@ $(CFLAGS) -DSTB_IMAGE_WRITE_IMPLEMENTATION -x c $<

clean:
	rm -rf build

mkdir:
	@mkdir -p build

FORCE:

.PHONY: all clean mkdir FORCE

-include run.mk
