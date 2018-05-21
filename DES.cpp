#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstdio>
#include <regex>
#include <cassert>
#include <algorithm>
#include "DES.h"

using namespace std;

const int MAP_IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52,
    44, 36, 28, 20, 12, 4, 62, 54, 46, 38,
    30, 22, 14, 6, 64, 56, 48, 40, 32, 24,
    16, 8, 57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3, 61, 53,
    45, 37, 29, 21, 13, 5, 63, 55, 47, 39,
    31, 23, 15, 7
};

void IP(uint64_t plaintext, uint32_t &left, uint32_t &right)
{
    /* Initial permutation */
    for (int i = 0; i < 32; i++) {
        set_bit(left, 31 - i, get_bit(plaintext, 64 - MAP_IP[i]));
    }

    for (int i = 32; i < 64; i++) {
        set_bit(right, 63 - i, get_bit(plaintext, 64 - MAP_IP[i]));
    }
}

const int MAP_FP[] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7,
    47, 15, 55, 23, 63, 31, 38, 6, 46, 14,
    54, 22, 62, 30, 37, 5, 45, 13, 53, 21,
    61, 29, 36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27, 34, 2,
    42, 10, 50, 18, 58, 26, 33, 1, 41, 9,
    49, 17, 57, 25
};

void FP(uint64_t &ciphertext, uint32_t left, uint32_t right)
{
    /* Final permutation */
    for (int i = 0; i < 64; i++) {
        if (MAP_FP[i] <= 32) {
            set_bit(ciphertext, 63 - i, get_bit(left, 32 - MAP_FP[i]));
        } else {
            set_bit(ciphertext, 63 - i, get_bit(right, 64 - MAP_FP[i]));
        }
    }
}

const int MAP_PC1[] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12, 4
};

void PC1(uint64_t key, uint32_t &left, uint32_t &right)
{
    for (int i = 0; i < 28; i++) {
        set_bit(left, 31 - i, get_bit(key, 64 - MAP_PC1[i]));
    }

    for (int i = 28; i < 56; i++) {
        set_bit(right, 59 - i, get_bit(key, 64 - MAP_PC1[i]));
    }
}

const int MAP_PC2[] {
    14, 17, 11, 24,  1, 5,
    3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26, 8,
    16,  7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

void PC2(uint64_t subkey_array[], uint32_t left, uint32_t right, int index)
{
    for (int i = 0; i < 48; i++) {
        if (MAP_PC2[i] <= 28) {
            set_bit(subkey_array[index], 63 - i, get_bit(left, 32 - MAP_PC2[i]));
        } else {
            set_bit(subkey_array[index], 63 - i, get_bit(right, 60 - MAP_PC2[i]));
        }
    }

}

void rotate_left_28bit(uint32_t &x, int i)
{
    /* Quay trái i bit */
    x = (x << i) | (x >> (28 - i));
}


const uint64_t WEAK_KEY_TABLE[16] = {
    0x0101010101010101, 0xFEFEFEFEFEFEFEFE,
    0x1F1F1F1F0E0E0E0E, 0xE0E0E0E0F1F1F1F1,
    0x011F011F010E010E, 0x1F011F010E010E01,
    0x01E001E001F101F1, 0xE001E001F101F101,
    0x01FE01FE01FE01FE, 0xFE01FE01FE01FE01,
    0x1FE01FE00EF10EF1, 0xE01FE01FF10EF10E,
    0x1FFE1FFE0EFE0EFE, 0xFE1FFE1FFE0EFE0E,
    0xE0FEE0FEF1FEF1FE, 0xFEE0FEE0FEF1FEF1
};

int check_key_weak(uint64_t key)
{
    return std::find(WEAK_KEY_TABLE, WEAK_KEY_TABLE + 16, key) != (WEAK_KEY_TABLE + 16);
}


int key_schedule(uint64_t subkey_array[], uint64_t key, int is_encrypt, int n_subkey)
{
    if (check_key_weak(key)) {
        return 0;
    }

    uint32_t left = 0, right = 0;
    PC1(key, left, right);

    const int BIT_ROTATIONS[] = {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
    };

    int index[n_subkey];

    /* Nếu gỉai mã thì dùng key theo thứ tự ngược lại */
    if (is_encrypt) {
        for (int i = 0; i < n_subkey; i++)
            index[i] = i;
    } else {
        for (int i = 0; i < n_subkey; i++)
            index[i] = n_subkey - 1 - i;
    }

    for (int i = 0; i < 16; i++) {
        rotate_left_28bit(left, BIT_ROTATIONS[i]);
        rotate_left_28bit(right, BIT_ROTATIONS[i]);

        PC2(subkey_array, left, right, index[i]);
    }
    return 1;
}

const uint64_t SBox[8][4][16] = {
    {
        0xe0000000, 0x40000000, 0xd0000000, 0x10000000, 0x20000000, 0xf0000000, 0xb0000000, 0x80000000, 0x30000000, 0xa0000000, 0x60000000, 0xc0000000, 0x50000000, 0x90000000, 0, 0x70000000,
        0, 0xf0000000, 0x70000000, 0x40000000, 0xe0000000, 0x20000000, 0xd0000000, 0x10000000, 0xa0000000, 0x60000000, 0xc0000000, 0xb0000000, 0x90000000, 0x50000000, 0x30000000, 0x80000000,
        0x40000000, 0x10000000, 0xe0000000, 0x80000000, 0xd0000000, 0x60000000, 0x20000000, 0xb0000000, 0xf0000000, 0xc0000000, 0x90000000, 0x70000000, 0x30000000, 0xa0000000, 0x50000000, 0,
        0xf0000000, 0xc0000000, 0x80000000, 0x20000000, 0x40000000, 0x90000000, 0x10000000, 0x70000000, 0x50000000, 0xb0000000, 0x30000000, 0xe0000000, 0xa0000000, 0, 0x60000000, 0xd0000000
    },
    {
        0xf000000, 0x1000000, 0x8000000, 0xe000000, 0x6000000, 0xb000000, 0x3000000, 0x4000000, 0x9000000, 0x7000000, 0x2000000, 0xd000000, 0xc000000, 0, 0x5000000, 0xa000000,
        0x3000000, 0xd000000, 0x4000000, 0x7000000, 0xf000000, 0x2000000, 0x8000000, 0xe000000, 0xc000000, 0, 0x1000000, 0xa000000, 0x6000000, 0x9000000, 0xb000000, 0x5000000,
        0, 0xe000000, 0x7000000, 0xb000000, 0xa000000, 0x4000000, 0xd000000, 0x1000000, 0x5000000, 0x8000000, 0xc000000, 0x6000000, 0x9000000, 0x3000000, 0x2000000, 0xf000000,
        0xd000000, 0x8000000, 0xa000000, 0x1000000, 0x3000000, 0xf000000, 0x4000000, 0x2000000, 0xb000000, 0x6000000, 0x7000000, 0xc000000, 0, 0x5000000, 0xe000000, 0x9000000
    },
    {
        0xa00000, 0, 0x900000, 0xe00000, 0x600000, 0x300000, 0xf00000, 0x500000, 0x100000, 0xd00000, 0xc00000, 0x700000, 0xb00000, 0x400000, 0x200000, 0x800000,
        0xd00000, 0x700000, 0, 0x900000, 0x300000, 0x400000, 0x600000, 0xa00000, 0x200000, 0x800000, 0x500000, 0xe00000, 0xc00000, 0xb00000, 0xf00000, 0x100000,
        0xd00000, 0x600000, 0x400000, 0x900000, 0x800000, 0xf00000, 0x300000, 0, 0xb00000, 0x100000, 0x200000, 0xc00000, 0x500000, 0xa00000, 0xe00000, 0x700000,
        0x100000, 0xa00000, 0xd00000, 0, 0x600000, 0x900000, 0x800000, 0x700000, 0x400000, 0xf00000, 0xe00000, 0x300000, 0xb00000, 0x500000, 0x200000, 0xc00000
    },
    {
        0x70000, 0xd0000, 0xe0000, 0x30000, 0, 0x60000, 0x90000, 0xa0000, 0x10000, 0x20000, 0x80000, 0x50000, 0xb0000, 0xc0000, 0x40000, 0xf0000,
        0xd0000, 0x80000, 0xb0000, 0x50000, 0x60000, 0xf0000, 0, 0x30000, 0x40000, 0x70000, 0x20000, 0xc0000, 0x10000, 0xa0000, 0xe0000, 0x90000,
        0xa0000, 0x60000, 0x90000, 0, 0xc0000, 0xb0000, 0x70000, 0xd0000, 0xf0000, 0x10000, 0x30000, 0xe0000, 0x50000, 0x20000, 0x80000, 0x40000,
        0x30000, 0xf0000, 0, 0x60000, 0xa0000, 0x10000, 0xd0000, 0x80000, 0x90000, 0x40000, 0x50000, 0xb0000, 0xc0000, 0x70000, 0x20000, 0xe0000
    },
    {
        0x2000, 0xc000, 0x4000, 0x1000, 0x7000, 0xa000, 0xb000, 0x6000, 0x8000, 0x5000, 0x3000, 0xf000, 0xd000, 0, 0xe000, 0x9000,
        0xe000, 0xb000, 0x2000, 0xc000, 0x4000, 0x7000, 0xd000, 0x1000, 0x5000, 0, 0xf000, 0xa000, 0x3000, 0x9000, 0x8000, 0x6000,
        0x4000, 0x2000, 0x1000, 0xb000, 0xa000, 0xd000, 0x7000, 0x8000, 0xf000, 0x9000, 0xc000, 0x5000, 0x6000, 0x3000, 0, 0xe000,
        0xb000, 0x8000, 0xc000, 0x7000, 0x1000, 0xe000, 0x2000, 0xd000, 0x6000, 0xf000, 0, 0x9000, 0xa000, 0x4000, 0x5000, 0x3000
    },
    {
        0xc00, 0x100, 0xa00, 0xf00, 0x900, 0x200, 0x600, 0x800, 0, 0xd00, 0x300, 0x400, 0xe00, 0x700, 0x500, 0xb00,
        0xa00, 0xf00, 0x400, 0x200, 0x700, 0xc00, 0x900, 0x500, 0x600, 0x100, 0xd00, 0xe00, 0, 0xb00, 0x300, 0x800,
        0x900, 0xe00, 0xf00, 0x500, 0x200, 0x800, 0xc00, 0x300, 0x700, 0, 0x400, 0xa00, 0x100, 0xd00, 0xb00, 0x600,
        0x400, 0x300, 0x200, 0xc00, 0x900, 0x500, 0xf00, 0xa00, 0xb00, 0xe00, 0x100, 0x700, 0x600, 0, 0x800, 0xd00
    },
    {
        0x40, 0xb0, 0x20, 0xe0, 0xf0, 0, 0x80, 0xd0, 0x30, 0xc0, 0x90, 0x70, 0x50, 0xa0, 0x60, 0x10,
        0xd0, 0, 0xb0, 0x70, 0x40, 0x90, 0x10, 0xa0, 0xe0, 0x30, 0x50, 0xc0, 0x20, 0xf0, 0x80, 0x60,
        0x10, 0x40, 0xb0, 0xd0, 0xc0, 0x30, 0x70, 0xe0, 0xa0, 0xf0, 0x60, 0x80, 0, 0x50, 0x90, 0x20,
        0x60, 0xb0, 0xd0, 0x80, 0x10, 0x40, 0xa0, 0x70, 0x90, 0x50, 0, 0xf0, 0xe0, 0x20, 0x30, 0xc0
    },

    {
        0xd, 0x2, 0x8, 0x4, 0x6, 0xf, 0xb, 0x1, 0xa, 0x9, 0x3, 0xe, 0x5, 0, 0xc, 0x7,
        0x1, 0xf, 0xd, 0x8, 0xa, 0x3, 0x7, 0x4, 0xc, 0x5, 0x6, 0xb, 0, 0xe, 0x9, 0x2,
        0x7, 0xb, 0x4, 0x1, 0x9, 0xc, 0xe, 0x2, 0, 0x6, 0xa, 0xd, 0xf, 0x3, 0x5, 0x8,
        0x2, 0x1, 0xe, 0x7, 0x4, 0xa, 0x8, 0xd, 0xf, 0xc, 0x9, 0, 0x3, 0x5, 0x6, 0xb
    }
};

uint32_t f(uint32_t right, uint64_t subkey)
{
    /*  The Feistel (F) function
        right: 32 bit
        subkey: 48 bit
    */

    /* Expansion function */

    uint64_t ex_right = 0;
    ex_right =  (uint64_t)(right >> 27) << 58 |
                (uint64_t)(right << 3>>26) << 52 |
                (uint64_t)(right << 7>>26) << 46 |
                (uint64_t)(right << 11>>26) << 40 |
                (uint64_t)(right << 15>>26) << 34 |
                (uint64_t)(right << 19>>26) << 28 |
                (uint64_t)(right << 23>>26) << 22 |
                (uint64_t)(right << 27>>27) << 17 |
                (uint64_t)(right >> 31) << 16 |
                (uint64_t) right << 63;


    ex_right ^= subkey;

    /* Chuyển đổi S-Box */
    ex_right = SBox[0][(ex_right & 0x8000000000000000) >> 62 | (ex_right & 0x0400000000000000) >> 58][(ex_right & 0x7800000000000000) >> 59] |
               SBox[1][(ex_right & 0x0200000000000000) >> 56 | (ex_right & 0x0010000000000000) >> 52][(ex_right & 0x01e0000000000000) >> 53] |
               SBox[2][(ex_right & 0x0008000000000000) >> 50 | (ex_right & 0x0000400000000000) >> 46][(ex_right & 0x0007800000000000) >> 47] |
               SBox[3][(ex_right & 0x0000200000000000) >> 44 | (ex_right & 0x0000010000000000) >> 40][(ex_right & 0x00001e0000000000) >> 41] |
               SBox[4][(ex_right & 0x0000008000000000) >> 38 | (ex_right & 0x0000000400000000) >> 34][(ex_right & 0x0000007800000000) >> 35] |
               SBox[5][(ex_right & 0x0000000200000000) >> 32 | (ex_right & 0x0000000010000000) >> 28][(ex_right & 0x00000001e0000000) >> 29] |
               SBox[6][(ex_right & 0x0000000008000000) >> 26 | (ex_right & 0x0000000000400000) >> 22][(ex_right & 0x0000000007800000) >> 23] |
               SBox[7][(ex_right & 0x0000000000200000) >> 20 | (ex_right & 0x0000000000010000) >> 16][(ex_right & 0x00000000001e0000) >> 17];

    /* Permutation */
    uint32_t result = ex_right;
    result =    ((result & 1 << 16) << 15) | ((result & 1 << 25) << 5) |
                ((result & 1 << 12) << 17) | ((result & 1 << 11) << 17) |
                ((result & 1 << 3) << 24) | ((result & 1 << 20) << 6) |
                ((result & 1 << 4) << 21) | ((result & 1 << 15) << 9) |
                ((result & 1 << 31) >> 8) | ((result & 1 << 17) << 5) |
                ((result & 1 << 9) << 12) | ((result & 1 << 6) << 14) |
                ((result & 1 << 27) >> 8) | ((result & 1 << 14) << 4) |
                ((result & 1 << 1) << 16) | ((result & 1 << 22) >> 6) |
                ((result & 1 << 30) >> 15) | ((result & 1 << 24) >> 10) |
                ((result & 1 << 8) << 5) | ((result & 1 << 18) >> 6) |
                ((result & 1) << 11) | ((result & 1 << 5) << 5) |
                ((result & 1 << 29) >> 20) | ((result & 1 << 23) >> 15) |
                ((result & 1 << 13) >> 6) | ((result & 1 << 19) >> 13) |
                ((result & 1 << 2) << 3) | ((result & 1 << 26) >> 22) |
                ((result & 1 << 10) >> 7) | ((result & 1 << 21) >> 19) |
                ((result & 1 << 28) >> 27) | ((result & 1 << 7) >> 7);

    return result;
}

void feistel_scheme(uint64_t input, uint64_t &output, uint64_t subkey_array[])
{
    uint32_t left = 0, right = 0, tmp = 0;
    IP(input, left, right);

    for (int i = 0; i < 15; i++) {
        tmp = right;
        right = left ^ f(right, subkey_array[i]);
        left = tmp;
    }
    left = left ^ f(right, subkey_array[15]);
    FP(output, left, right);
}

void des(uint64_t plaintext, uint64_t &ciphertext, uint64_t subkey_array[])
{
    feistel_scheme(plaintext, ciphertext, subkey_array);
}

void inv_des(uint64_t ciphertext, uint64_t &plaintext, uint64_t rev_subkey_array[])
{
    feistel_scheme(ciphertext, plaintext, rev_subkey_array);
}

uint64_t PKCS7_padding(uint64_t *input, uint64_t n)
{
    uint32_t num_byte_padding =  8 - (n % 8);
    uint64_t new_size = (n / 8 + 1) * 8;
    unsigned char *p = (unsigned char*)input + n;

    for (uint32_t i = 0; i < num_byte_padding; i++) {
        p[i] = (unsigned char)num_byte_padding;
    }

    return new_size;
}

uint64_t PKCS7_truncate(unsigned char *output, uint64_t n)
{
    uint32_t num_byte_padding = output[n - 1];
    uint64_t i = 0;
    assert(num_byte_padding > 0 || num_byte_padding < 9);
    for (i = n - 1; (output[i] == num_byte_padding) && (n - i) <= num_byte_padding; i--);
    if (n - 1 - i != num_byte_padding) {
        std::cerr << "Cipher text has been changed." << std::endl;
    }
    return n - num_byte_padding;
}