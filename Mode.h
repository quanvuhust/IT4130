#ifndef MODE_H
#define MODE_H 1

enum MODE {ECB = 1, CTR = 2};
void ECB(uint64_t *text, uint64_t n, int is_encrypt, uint64_t subkey_array[]);
void CTR(uint64_t *text, int n, int is_encrypt, uint64_t subkey_array[]);

class CTR_IV {
private:
    const uint64_t init_vector = 0x0101010101010101;
    uint64_t counter = 0;
public:
    void setCounter(uint64_t counter) {
        this->counter = counter;
    }
    uint64_t increment_IV() {
        return init_vector + counter++;
    }
};

#endif