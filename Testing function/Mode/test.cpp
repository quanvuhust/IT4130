#include <cstdio>
#include <iostream>
#include <cmath>
#include <mpi.h>
#define BLOCK_SIZE (16 * 1024 * 1024)
using namespace std;

void ecb(uint64_t *text, uint64_t n, int is_encrypt, uint64_t subkey_array[])
{
    int rank = 0, nproc = 0;
    int *displs = nullptr, *rcounts = nullptr;
    uint64_t *block = nullptr;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    displs = new int[nproc];
    rcounts = new int[nproc];

    uint64_t a = n / nproc, b = n % nproc, delta = 1;
    if (b != 0) {
        block = new uint64_t[a + 1];
    } else {
        block = new uint64_t[a];
    }

    displs[0] = 0;
    for (int i = 0; i < nproc; i++) {
        if (b == 0) delta = 0;
        rcounts[i] = a + delta;
        if (i >= 1) {
            displs[i] = displs[i - 1] + rcounts[i - 1];
        }
        b--;
    }
    if (rank == 0) {
        std::cout << "ECB is called" << std::endl;
        std::cout << text << std::endl;
        std::cout << n << std::endl;
        std::cout << a << std::endl;
        for (int i = 0; i < nproc; i++) {
            std::cout << displs[i] << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < nproc; i++) {
            std::cout << rcounts[i] << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << std::hex << text[i] << " ";
        }
        std::cout << std::endl;
    }

    MPI_Datatype stype;
    MPI_Type_vector(rcounts[rank], 1, 1, MPI_UNSIGNED_LONG_LONG, &stype);
    MPI_Type_commit(&stype);

    /*  Chia du lieu cho cac process:
        Chi co con tro text cua process 0 la tro toi vung co du lieu */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatterv(text, rcounts, displs, MPI_UNSIGNED_LONG_LONG, block,
                 1, stype, 0, MPI_COMM_WORLD);

    /* Phan ma hoa du lieu */


    /* Gop du lieu tu cac process */
    MPI_Gatherv(block, 1, stype, text, rcounts, displs,
                MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    delete[] block;
    delete[] rcounts;
    delete[] displs;
}

int main(int argc, char* argv[]) {
    int rank = 0, nproc = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    uint64_t *text = nullptr;
    uint64_t subkey_array[16] = {};
    if (rank == 0) {
        text = new uint64_t[BLOCK_SIZE + 1]; // +1 to padding
    }
    ecb(text, 4, 1, subkey_array);
    MPI_Finalize();
    return 0;
}