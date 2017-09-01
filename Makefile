all: avx2

avx2:
	g++-6 -Wall -g -O2 -D"CCL_NAMESPACE_BEGIN=namespace ccl {" -DCCL_NAMESPACE_END="}" -ffast-math -msse -msse2 -msse3 -mssse3 -msse4.1 -mavx -mavx2 -mfma -mlzcnt -mbmi -mbmi2 -mf16c -mfpmath=sse -o triangle_intersect -I. -I./atomic main.cc util/util_time.cpp util/util_guarded_allocator.cpp
