#include "Mode.h"

/* Can sua */
void ECB(uint64_t *input, uint64_t *output, int n, int is_encrypt, uint64_t subkey_array[])
{
    int rank, size;
    
    uint64_t counter = 0;
    uint64_t inputBlock;
    MPI_Scatter(input, 1, MPI_LONG_LONG_INT, inputBlock, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);


    uint64_t result;
    if (is_encrypt) {
        des(inputBlock, result, subkey_array);
    }
    else {
        inv_des(inputBlock, result, subkey_array);
    }
    MPI_Gather(result, 1, MPI_LONG_LONG_INT, output, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
}

void CTR(uint64_t *input, uint64_t *output, int n, int is_encrypt, uint64_t subkey_array[]) {

}