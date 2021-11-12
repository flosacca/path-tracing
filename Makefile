CFLAGS := -O3
CXXFLAGS := -fopenmp -O3 -std=c++17 -Ilib

OBJS := build/image.o
PCH := build/glm_all.gch

all: mkdir main

main: $(PCH) $(OBJS) FORCE
	g++ -o $@ $(CXXFLAGS) -include $(PCH:.gch=) main.cc $(OBJS)

$(PCH): glm_all.h
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
