/* Can sua */
void ECB(int64_t *input, int64_t *output, int n, bool is_encrypt, int64_t subkey_array[])
{
    int rank, size;
    

    int64_t counter = 0;
    int64_t inputBlock;
    MPI_Scatter(input, 1, MPI_LONG_LONG_INT, inputBlock, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(subkey_array, 16, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    int64_t result;
    if (is_encrypt) {
        encrypt(inputBlock, result, subkey_array);
    }
    else {
        decrypt(inputBlock, result, subkey_array);
    }
    MPI_Gather(result, 1, MPI_LONG_LONG_INT, output, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (!is_encrypt) {
            for (int i = 0; i < 7; i++) {
                if (output[length - 1] == 0) {
                    length--;
                }
            }
        }
        string filename;
        string extension = inputFile.substr(inputFile.find_last_of('.'));
        if (encryption) {
            filename = "files/encryption_result" + extension;
        }
        else {
            filename = "files/decryption_result" + extension;
        }
    }
}
