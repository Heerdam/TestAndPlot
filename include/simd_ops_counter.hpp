#ifndef SIMD_OPS_COUNTER_HPP
#define SIMD_OPS_COUNTER_HPP

#include "ops_counter.hpp"
#include <iostream>

#include <immintrin.h>

template<class BYTE, bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_set1_epi8 (BYTE __A) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::ASSIGN);
        return _mm256_set1_epi8((unsigned char)__A);
    }
    return _mm256_set1_epi8((unsigned char)__A);
};

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_loadu_si256 (__m256i_u const *__P) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::ASSIGN);
    }
    return _mm256_loadu_si256(__P);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_subs_epu8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::SUB);
    }
    return _mm256_subs_epu8(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_adds_epu8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::ADD);
    }
    return _mm256_adds_epu8(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_xor_si256(__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::BYTEOPS);
    }
    return _mm256_xor_si256(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_sub_epi8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::SUB);;
    }
    return _mm256_sub_epi8(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_or_si256 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::BYTEOPS);
    }
    return _mm256_or_si256(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_cmpgt_epi8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::COMP);
    }
    return _mm256_cmpgt_epi8(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_cmpeq_epi8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::COMP);
    }
    return _mm256_cmpeq_epi8(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_blendv_epi8 (__m256i __X, __m256i __Y, __m256i __M) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::BYTEOPS);
    }
    return _mm256_blendv_epi8(__X, __Y, __M);
}

template<class INT, bool COUNT>
__inline INT  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_movemask_epi8 (__m256i __A){
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::BYTEOPS);
        return { _mm256_movemask_epi8(__A) };
    }
    return _mm256_movemask_epi8(__A);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_and_si256 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::BYTEOPS);
    }
    return _mm256_and_si256(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_andnot_si256 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::BYTEOPS);
    }
    return _mm256_andnot_si256(__A, __B);
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_setzero_si256 (void) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::M256i, Counter::ASSIGN);
    }
    return _mm256_setzero_si256();
}

template<bool COUNT>
__inline __m256i  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _mm256_max_epu8 (__m256i __A, __m256i __B) {
    if constexpr (COUNT){
        Counter counter;
        for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::BYTE, Counter::COMP);
    }
    return _mm256_max_epu8(__A, __B);
}

template<class INT, bool COUNT>
__inline INT  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _tzcnt_u32 (INT __X) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::INT, Counter::BYTEOPS);
        return _tzcnt_u32((int)__X);
    }
    return _tzcnt_u32((int)__X);
}

template<class INT, bool COUNT>
__inline INT  __attribute__((__gnu_inline__, __always_inline__, __artificial__)) _blsr_u32 (INT __X) {
    if constexpr (COUNT){
        Counter counter;
        //for(size_t i = 0; i < 32; ++i)
            counter.addCount(Counter::INT, Counter::BYTEOPS);
        return _blsr_u32((int)__X);
    }
    return _blsr_u32((int)__X);
}

#endif //SIMD_OPS_COUNTER_HPP
