all:
	g++-6 -Wall -g -O2 -ffast-math -msse -msse2 -msse3 -mssse3 -msse4.1 -mavx -mavx2 -mfma -mlzcnt -mbmi -mbmi2 -mf16c -mfpmath=sse -o triangle_intersect -I./util -I./kernel main.cc
