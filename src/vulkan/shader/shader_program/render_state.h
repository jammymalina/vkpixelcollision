#ifndef RENDER_STATE_H
#define RENDER_STATE_H

#include <stdint.h>

#define RST_SRCBLEND_ONE                 (UINT64_C(0) << 0)
#define RST_SRCBLEND_ZERO                (UINT64_C(1) << 0)
#define RST_SRCBLEND_DST_COLOR           (UINT64_C(2) << 0)
#define RST_SRCBLEND_ONE_MINUS_DST_COLOR (UINT64_C(3) << 0)
#define RST_SRCBLEND_SRC_ALPHA           (UINT64_C(4) << 0)
#define RST_SRCBLEND_ONE_MINUS_SRC_ALPHA (UINT64_C(5) << 0)
#define RST_SRCBLEND_DST_ALPHA           (UINT64_C(6) << 0)
#define RST_SRCBLEND_ONE_MINUS_DST_ALPHA (UINT64_C(7) << 0)
#define RST_SRCBLEND_BITS                (UINT64_C(7) << 0)

#define RST_DSTBLEND_ZERO                (UINT64_C(0) << 3)
#define RST_DSTBLEND_ONE                 (UINT64_C(1) << 3)
#define RST_DSTBLEND_SRC_COLOR           (UINT64_C(2) << 3)
#define RST_DSTBLEND_ONE_MINUS_SRC_COLOR (UINT64_C(3) << 3)
#define RST_DSTBLEND_SRC_ALPHA           (UINT64_C(4) << 3)
#define RST_DSTBLEND_ONE_MINUS_SRC_ALPHA (UINT64_C(5) << 3)
#define RST_DSTBLEND_DST_ALPHA           (UINT64_C(6) << 3)
#define RST_DSTBLEND_ONE_MINUS_DST_ALPHA (UINT64_C(7) << 3)
#define RST_DSTBLEND_BITS                (UINT64_C(7) << 3)

// these masks are inversed
#define RST_DEPTHMASK (UINT64_C(1) << 6)
#define RST_REDMASK   (UINT64_C(1) << 7)
#define RST_GREENMASK (UINT64_C(1) << 8)
#define RST_BLUEMASK  (UINT64_C(1) << 9)
#define RST_ALPHAMASK (UINT64_C(1) << 10)
#define RST_COLORMASK (RST_REDMASK | RST_GREENMASK | RST_BLUEMASK)

#define RST_POLYMODE_LINE  (UINT64_C(1) << 11)
#define RST_POLYGON_OFFSET (UINT64_C(1) << 12)

#define RST_DEPTHFUNC_LESS    (UINT64_C(0) << 13)
#define RST_DEPTHFUNC_ALWAYS  (UINT64_C(1) << 13)
#define RST_DEPTHFUNC_GREATER (UINT64_C(2) << 13)
#define RST_DEPTHFUNC_EQUAL   (UINT64_C(3) << 13)
#define RST_DEPTHFUNC_BITS    (UINT64_C(3) << 13)

#define RST_CULL_FRONTSIDED (UINT64_C(0) << 15)
#define RST_CULL_BACKSIDED  (UINT64_C(1) << 15)
#define RST_CULL_TWOSIDED   (UINT64_C(2) << 15)
#define RST_CULL_BITS       (UINT64_C(7) << 15)
#define RST_CULL_MASK       (RST_CULL_FRONTSIDED | RST_CULL_BACKSIDED | \
    RST_CULL_TWOSIDED)

#define RST_BLENDOP_ADD  (UINT64_C(0) << 18)
#define RST_BLENDOP_SUB  (UINT64_C(1) << 18)
#define RST_BLENDOP_MIN  (UINT64_C(2) << 18)
#define RST_BLENDOP_MAX  (UINT64_C(3) << 18)
#define RST_BLENDOP_BITS (UINT64_C(3) << 18)

// STENCIL

#define RST_STENCIL_FUNC_REF_SHIFT  (UINT64_C(20))
#define RST_STENCIL_FUNC_REF_BITS   (UINT64_C(0xFF11) << \
    RST_STENCIL_FUNC_REF_SHIFT)

#define RST_STENCIL_FUNC_MASK_SHIFT (UINT64_C(28))
#define RST_STENCIL_FUNC_MASK_BITS  (UINT64_C(0xFF11) << \
    RST_STENCIL_FUNC_MASK_SHIFT)

#define RST_STENCIL_MAKE_REF(x)     ((UINT64_C(x) << \
    RST_STENCIL_FUNC_REF_SHIFT) & RST_STENCIL_FUNC_REF_BITS)
#define RST_STENCIL_MAKE_MASK(x)    ((UINT64_C(x) << \
    RST_STENCIL_FUNC_MASK_SHIFT) & RST_STENCIL_FUNC_MASK_BITS)

// Next 12 bits act as front+back unless GLS_SEPARATE_STENCIL is set, in which case it acts as front
#define RST_STENCIL_FUNC_ALWAYS             (UINT64_C(0) << 36)
#define RST_STENCIL_FUNC_LESS               (UINT64_C(1) << 36)
#define RST_STENCIL_FUNC_LEQUAL             (UINT64_C(2) << 36)
#define RST_STENCIL_FUNC_GREATER            (UINT64_C(3) << 36)
#define RST_STENCIL_FUNC_GEQUAL             (UINT64_C(4) << 36)
#define RST_STENCIL_FUNC_EQUAL              (UINT64_C(5) << 36)
#define RST_STENCIL_FUNC_NOTEQUAL           (UINT64_C(6) << 36)
#define RST_STENCIL_FUNC_NEVER              (UINT64_C(7) << 36)
#define RST_STENCIL_FUNC_BITS               (UINT64_C(7) << 36)

#define RST_STENCIL_OP_FAIL_KEEP            (UINT64_C(0) << 39)
#define RST_STENCIL_OP_FAIL_ZERO            (UINT64_C(1) << 39)
#define RST_STENCIL_OP_FAIL_REPLACE         (UINT64_C(2) << 39)
#define RST_STENCIL_OP_FAIL_INCR            (UINT64_C(3) << 39)
#define RST_STENCIL_OP_FAIL_DECR            (UINT64_C(4) << 39)
#define RST_STENCIL_OP_FAIL_INVERT          (UINT64_C(5) << 39)
#define RST_STENCIL_OP_FAIL_INCR_WRAP       (UINT64_C(6) << 39)
#define RST_STENCIL_OP_FAIL_DECR_WRAP       (UINT64_C(7) << 39)
#define RST_STENCIL_OP_FAIL_BITS            (UINT64_C(7) << 39)

#define RST_STENCIL_OP_ZFAIL_KEEP           (UINT64_C(0) << 42)
#define RST_STENCIL_OP_ZFAIL_ZERO           (UINT64_C(1) << 42)
#define RST_STENCIL_OP_ZFAIL_REPLACE        (UINT64_C(2) << 42)
#define RST_STENCIL_OP_ZFAIL_INCR           (UINT64_C(3) << 42)
#define RST_STENCIL_OP_ZFAIL_DECR           (UINT64_C(4) << 42)
#define RST_STENCIL_OP_ZFAIL_INVERT         (UINT64_C(5) << 42)
#define RST_STENCIL_OP_ZFAIL_INCR_WRAP      (UINT64_C(6) << 42)
#define RST_STENCIL_OP_ZFAIL_DECR_WRAP      (UINT64_C(7) << 42)
#define RST_STENCIL_OP_ZFAIL_BITS           (UINT64_C(7) << 42)

#define RST_STENCIL_OP_PASS_KEEP            (UINT64_C(0) << 45)
#define RST_STENCIL_OP_PASS_ZERO            (UINT64_C(1) << 45)
#define RST_STENCIL_OP_PASS_REPLACE         (UINT64_C(2) << 45)
#define RST_STENCIL_OP_PASS_INCR            (UINT64_C(3) << 45)
#define RST_STENCIL_OP_PASS_DECR            (UINT64_C(4) << 45)
#define RST_STENCIL_OP_PASS_INVERT          (UINT64_C(5) << 45)
#define RST_STENCIL_OP_PASS_INCR_WRAP       (UINT64_C(6) << 45)
#define RST_STENCIL_OP_PASS_DECR_WRAP       (UINT64_C(7) << 45)
#define RST_STENCIL_OP_PASS_BITS            (UINT64_C(7) << 45)

// Next 12 bits act as back and are only active when GLS_SEPARATE_STENCIL is set
#define RST_BACK_STENCIL_FUNC_ALWAYS        (UINT64_C(0) << 48)
#define RST_BACK_STENCIL_FUNC_LESS          (UINT64_C(1) << 48)
#define RST_BACK_STENCIL_FUNC_LEQUAL        (UINT64_C(2) << 48)
#define RST_BACK_STENCIL_FUNC_GREATER       (UINT64_C(3) << 48)
#define RST_BACK_STENCIL_FUNC_GEQUAL        (UINT64_C(4) << 48)
#define RST_BACK_STENCIL_FUNC_EQUAL         (UINT64_C(5) << 48)
#define RST_BACK_STENCIL_FUNC_NOTEQUAL      (UINT64_C(6) << 48)
#define RST_BACK_STENCIL_FUNC_NEVER         (UINT64_C(7) << 48)
#define RST_BACK_STENCIL_FUNC_BITS          (UINT64_C(7) << 48)

#define RST_BACK_STENCIL_OP_FAIL_KEEP       (UINT64_C(0) << 51)
#define RST_BACK_STENCIL_OP_FAIL_ZERO       (UINT64_C(1) << 51)
#define RST_BACK_STENCIL_OP_FAIL_REPLACE    (UINT64_C(2) << 51)
#define RST_BACK_STENCIL_OP_FAIL_INCR       (UINT64_C(3) << 51)
#define RST_BACK_STENCIL_OP_FAIL_DECR       (UINT64_C(4) << 51)
#define RST_BACK_STENCIL_OP_FAIL_INVERT     (UINT64_C(5) << 51)
#define RST_BACK_STENCIL_OP_FAIL_INCR_WRAP  (UINT64_C(6) << 51)
#define RST_BACK_STENCIL_OP_FAIL_DECR_WRAP  (UINT64_C(7) << 51)
#define RST_BACK_STENCIL_OP_FAIL_BITS       (UINT64_C(7) << 51)

#define RST_BACK_STENCIL_OP_ZFAIL_KEEP      (UINT64_C(0) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_ZERO      (UINT64_C(1) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_REPLACE   (UINT64_C(2) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_INCR      (UINT64_C(3) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_DECR      (UINT64_C(4) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_INVERT    (UINT64_C(5) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_INCR_WRAP (UINT64_C(6) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_DECR_WRAP (UINT64_C(7) << 54)
#define RST_BACK_STENCIL_OP_ZFAIL_BITS      (UINT64_C(7) << 54)

#define RST_BACK_STENCIL_OP_PASS_KEEP       (UINT64_C(0) << 57)
#define RST_BACK_STENCIL_OP_PASS_ZERO       (UINT64_C(1) << 57)
#define RST_BACK_STENCIL_OP_PASS_REPLACE    (UINT64_C(2) << 57)
#define RST_BACK_STENCIL_OP_PASS_INCR       (UINT64_C(3) << 57)
#define RST_BACK_STENCIL_OP_PASS_DECR       (UINT64_C(4) << 57)
#define RST_BACK_STENCIL_OP_PASS_INVERT     (UINT64_C(5) << 57)
#define RST_BACK_STENCIL_OP_PASS_INCR_WRAP  (UINT64_C(6) << 57)
#define RST_BACK_STENCIL_OP_PASS_DECR_WRAP  (UINT64_C(7) << 57)
#define RST_BACK_STENCIL_OP_PASS_BITS       (UINT64_C(7) << 57)

#define RST_SEPARATE_STENCIL  (RST_BACK_STENCIL_OP_FAIL_BITS | \
    RST_BACK_STENCIL_OP_ZFAIL_BITS | RST_BACK_STENCIL_OP_PASS_BITS)
#define RST_STENCIL_OP_BITS   (RST_STENCIL_OP_FAIL_BITS | \
    RST_STENCIL_OP_ZFAIL_BITS | RST_STENCIL_OP_PASS_BITS | RST_SEPARATE_STENCIL)
#define RST_STENCIL_FRONT_OPS (RST_STENCIL_OP_FAIL_BITS | \
    RST_STENCIL_OP_ZFAIL_BITS | RST_STENCIL_OP_PASS_BITS)
#define RST_STENCIL_BACK_OPS  (RST_SEPARATE_STENCIL)

// RST_OVERRIDE => override the render prog state
#define RST_DEPTH_TEST_MASK (UINT64_C(1) << 60)
#define RST_CLOCKWISE       (UINT64_C(1) << 61)
#define RST_MIRROR_VIEW     (UINT64_C(1) << 62)
#define RST_OVERRIDE        (UINT64_C(1) << 63)

#define RST_KEEP    RST_DEPTH_TEST_MASK
#define RST_DEFAULT UINT64_C(0)

// predefined states

#define RST_BASIC_2D (RST_CULL_TWOSIDED | RST_DEPTHFUNC_ALWAYS)
#define RST_BASIC_3D (RST_CULL_BACKSIDED | RST_DEPTHFUNC_LESS)

#endif // RENDER_STATE_H
