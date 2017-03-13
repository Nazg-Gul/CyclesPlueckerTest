#include <cstdlib>

#define USE_PLUECKER

#define CCL_NAMESPACE_BEGIN namespace ccl {
#define CCL_NAMESPACE_END }

#if !(defined(__GNUC__) && (defined(i386) || defined(_M_IX86)))
#  define __KERNEL_SSE__
#  define __KERNEL_SSE2__
#  define __KERNEL_SSE3__
#  define __KERNEL_SSSE3__
#  define __KERNEL_SSE41__
#  define __KERNEL_AVX__
#  define __KERNEL_AVX2__
#endif

#include "util_optimization.h"
#include "util_types.h"

#include "kernel_compat_cpu.h"
#include "kernel_math.h"
#include "kernel_types.h"
#include "kernel/split/kernel_split_data_types.h"
#include "kernel_globals.h"
#include "kernel_random.h"
#include "kernel/geom/geom_triangle_intersect.h"

// Fake data.
#include "data/prim_tri_index.h"
#include "data/prim_tri_verts.h"

// Assuming a static array.
#if defined(__GNUC__) && !defined(__cplusplus) && !defined(__clang__)
#  define ARRAY_SIZE(arr) \
    ((sizeof(struct {int isnt_array : ((const void *)&(arr) == &(arr)[0]);}) * 0) + \
     (sizeof(arr) / sizeof(*(arr))))
#else
#  define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof(*(arr)))
#endif

CCL_NAMESPACE_BEGIN

void main() {
  KernelGlobals kg = {};

  // Assign all fake data.
  kg.__prim_tri_index.data = prim_tri_index;
  kg.__prim_tri_index.width = ARRAY_SIZE(prim_tri_index);

  float4 prim_tri_verts[ARRAY_SIZE(prim_tri_verts_raw) / 4];
  for (size_t i = 0; i < ARRAY_SIZE(prim_tri_verts); ++i) {
    prim_tri_verts[i] = make_float4(prim_tri_verts_raw[i * 4 + 0],
                                    prim_tri_verts_raw[i * 4 + 1],
                                    prim_tri_verts_raw[i * 4 + 2],
                                    prim_tri_verts_raw[i * 4 + 3]);
  }
  kg.__prim_tri_verts.data = prim_tri_verts;
  kg.__prim_tri_verts.width = ARRAY_SIZE(prim_tri_verts);

  uint prim_visibility[ARRAY_SIZE(prim_tri_index)];
  for (size_t i = 0; i < ARRAY_SIZE(prim_tri_index); ++i) {
    prim_visibility[i] = PATH_RAY_ALL_VISIBILITY;
  }
  kg.__prim_visibility.data = prim_visibility;
  kg.__prim_visibility.width = ARRAY_SIZE(prim_visibility);

  // Ray origin and direction.
  float3 P = make_float3(0.0f, 0.0f, 0.0f);
  float3 dir = make_float3(0.0f, 0.0f, 1.0f);

#ifndef USE_PLUECKER
  IsectPrecalc isect_precalc;
  triangle_intersect_precalc(dir, &isect_precalc);
#endif

  for (int i = 0; i < 10000; ++i) {
    for (int prim = 0; prim < ARRAY_SIZE(prim_tri_index); ++prim) {
      Intersection isect;
      isect.t = FLT_MAX;
      isect.u = 0.0f;
      isect.v = 0.0f;
      isect.prim = PRIM_NONE;
      isect.object = OBJECT_NONE;
#ifdef USE_PLUECKER
      bool hit = triangle_intersect_pluecker(&kg,
                                             &isect,
                                             P,
                                             dir,
                                             PATH_RAY_ALL_VISIBILITY,
                                             OBJECT_NONE,
                                             prim);
#else
      bool hit = triangle_intersect(&kg,
                                    &isect_precalc,
                                    &isect,
                                    P,
                                    PATH_RAY_ALL_VISIBILITY,
                                    OBJECT_NONE,
                                    prim);
#endif
      // Fake usage of the result, so compiler does not optimize anything.
      if (hit && isect.u > 2.0f) {
        printf("ABORT!\n");
      }
    }
  }
}

CCL_NAMESPACE_END

int main(int /*argc*/, char** /*argv*/) {
  ccl::main();
  return EXIT_SUCCESS;
}
