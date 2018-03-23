#ifndef DES_H
#define DES_H 0

void PC1(uint64_t key, uint32_t &left, uint32_t &right);
void PC2(uint64_t subkey_array[], uint32_t left, uint32_t right, int index);
void rotate_left_bit32(uint32_t &x, int i);
void key_schedule(uint64_t subkey_array[], uint64_t key, bool is_encrypt, int n_subkey = 16);
void IP(uint64_t plaintext, uint32_t &left, uint32_t &right);
uint32_t f(uint32_t right, uint64_t subkey);
void feistel_scheme(uint64_t input, uint64_t &output, uint64_t subkey_array[]);
void FP(uint64_t &ciphertext, uint32_t left, uint32_t right);
void des(uint64_t plaintext, uint64_t &ciphertext, uint64_t subkey_array[]);
void inv_des(uint64_t ciphertext, uint64_t &plaintext, uint64_t rev_subkey_array[]);
int PKCS7_padding(uint64_t *input, int n);
int PKCS7_truncate(uint64_t *output, int n);

#endif