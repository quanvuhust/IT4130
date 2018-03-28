#ifndef DES_H
#define DES_H 0
#include <iostream>

/* Lấy gía trị bit tại vị trí thứ i tính từ vị trí số 0 từ phải sang của số x */
#define get_bit(x, i) (((x) >> (i)) & (uint64_t)1)

/* Đặt gía trị bit tại vị trí thứ i tính từ vị trí số 0 từ phải sang của số x bằng
   0 nếu set == 0
   1 nếu set != 0
*/
#define set_bit(x, i, set) ((set) != 0 ? (x) |= ((uint64_t)1 << (i)) : (x) &= ~((uint64_t)1 << (i)))

void PC1(uint64_t key, uint32_t &left, uint32_t &right);
void PC2(uint64_t subkey_array[], uint32_t left, uint32_t right, int index);
void rotate_left_bit32(uint32_t &x, int i);
int check_key_weak(uint64_t key);
int key_schedule(uint64_t subkey_array[], uint64_t key, int is_encrypt, int n_subkey = 16);
void IP(uint64_t plaintext, uint32_t &left, uint32_t &right);
uint32_t f(uint32_t right, uint64_t subkey);
void feistel_scheme(uint64_t input, uint64_t &output, uint64_t subkey_array[]);
void FP(uint64_t &ciphertext, uint32_t left, uint32_t right);
void des(uint64_t plaintext, uint64_t &ciphertext, uint64_t subkey_array[]);
void inv_des(uint64_t ciphertext, uint64_t &plaintext, uint64_t rev_subkey_array[]);
uint64_t PKCS7_padding(uint64_t *input, uint64_t n);
uint64_t PKCS7_truncate(unsigned char *output, uint64_t n);

#endif