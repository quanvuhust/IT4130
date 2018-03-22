#ifndef DES_H
#define DES_H 0

void PC1(int64_t key, int32_t &left, int32_t &right);
void PC2(int64_t subkey_array[], int32_t left, int32_t right, int index);
void rotate_left_bit32(int32_t &x, int i);
void key_schedule(int64_t subkey_array[], int64_t key, bool is_encrypt, int n_subkey = 16);
void IP(int64_t plaintext, int32_t &left, int32_t &right);
int32_t f(int32_t right, int64_t subkey);
void feistel_scheme(int64_t input, int64_t &output, int64_t subkey_array[]);
void FP(int64_t ciphertext, int32_t &left, int32_t &right);
void des(int64_t plaintext, int64_t &ciphertext, int64_t subkey_array[]);
void inv_des(int64_t ciphertext, int64_t &plaintext, int64_t rev_subkey_array[]);
int PKCS7_padding(int64_t *input, int n);
int PKCS7_truncate(int64_t *output, int n);

#endif