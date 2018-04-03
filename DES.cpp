#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstdio>
#include <regex>
#include <cassert>
#include <algorithm>
#include "DES.h"

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
        }
        else {
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
    for (int i = 0; i < 28; i++)
    {
        set_bit(left, 31 - i, get_bit(key, 64 - MAP_PC1[i]));
    }

    for (int i = 28; i < 56; i++)
    {
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
    for (int i = 0; i < 48; i++)
    {
        if (MAP_PC2[i] <= 28) {
            set_bit(subkey_array[index], 63 - i, get_bit(left, 32 - MAP_PC2[i]));
        }
        else {
            set_bit(subkey_array[index], 63 - i, get_bit(right, 60 - MAP_PC2[i]));
        }
    }

}

void rotate_left_28bit(uint32_t &x, int i)
{
    /* Quay trái i bit */
    x = (x << i) | (x >> (28 - i));
}


const uint64_t WEAK_KEY_TABLE[16] =
{
    0x0101010101010101, 0xFEFEFEFEFEFEFEFE,
    0x1F1F1F1F0E0E0E0E, 0xE0E0E0E0F1F1F1F1,
    0x011F011F010E010E, 0x1F011F010E010E01,
    0x01E001E001F101F1, 0xE001E001F101F101,
    0x01FE01FE01FE01FE, 0xFE01FE01FE01FE01,
    0x1FE01FE00EF10EF1, 0xE01FE01FF10EF10E,
    0x1FFE1FFE0EFE0EFE, 0xFE1FFE1FFE0EFE0E,
    0xE0FEE0FEF1FEF1FE, 0xFEE0FEE0FEF1FEF1
};

int check_key_weak(uint64_t key) {
    return std::find(WEAK_KEY_TABLE, WEAK_KEY_TABLE + 16, key) != (WEAK_KEY_TABLE + 16);
}


int key_schedule(uint64_t subkey_array[], uint64_t key, int is_encrypt, int n_subkey)
{
    if(check_key_weak(key)) {
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
        for (int i = 0; i < n_subkey; i++) index[i] = i;
    } else {
        for (int i = 0; i < n_subkey; i++) index[i] = n_subkey - 1 - i;
    }

    for (int i = 0; i < 16; i++) {
        rotate_left_28bit(left, BIT_ROTATIONS[i]);
        rotate_left_28bit(right, BIT_ROTATIONS[i]);

        PC2(subkey_array, left, right, index[i]);
    }
    return 1;
}

const int SBox[8][4][16] = {
    {
        14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
        0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
        4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
        15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
    },
    {
        15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
        3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
        0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
        13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
    },
    {
        10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
        13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
        1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
    },
    {
        7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
        13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
        10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
        3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
    },
    {
        2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
        14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
        4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
        11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
    },
    {
        12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
        10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
        9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
        4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
    },
    {
        4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
        13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
        1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
        6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
    },

    {
        13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
        1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
        7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
        2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
    }
};

uint32_t f(uint32_t right, uint64_t subkey)
{
    /*  The Feistel (F) function
        right: 32 bit
        subkey: 48 bit
    */

    /* Expansion function */
    const int E[] = {
        32,  1, 2,  3,  4, 5,
        4,  5, 6,  7,  8, 9,
        8,  9, 10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1
    };

    uint64_t ex_right = 0;

    for (int i = 0; i < 48; i++) {
        set_bit(ex_right, 63 - i, get_bit(right, 32 - E[i]));
    }

    ex_right ^= subkey;
    int outers_bit = 0, group_bit = 0;
    uint32_t tmp = 0;

    /* Chuyển đổi S-Box */
    for (int i = 0; i < 8; i++) {
        outers_bit = (get_bit(ex_right, 63) << 1) + get_bit(ex_right, 58); // Lấy 2 bit ngoài cùng bit 1, 6
        ex_right <<= 1;
        group_bit = ex_right >> 60; // Lấy 4 bit chính
        ex_right <<= 5;

        tmp |= SBox[i][outers_bit][group_bit] << (28 - 4 * i);
    }

    /* Permutation */
    const int P[] = {
        16, 7, 20, 21,
        29, 12, 28, 17,
        1, 15, 23, 26,
        5, 18, 31, 10,
        2, 8, 24, 14,
        32, 27, 3, 9,
        19, 13, 30, 6,
        22, 11, 4, 25
    };

    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        set_bit(result, 31 - i, get_bit(tmp, 32 - P[i]));
    }

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
    return n-num_byte_padding;
}
