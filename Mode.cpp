#include "Mode.h"
#include "DES.h"
#include <mpi.h>

/* Can sua */
void ecb(uint64_t *text, uint64_t n, int is_encrypt, uint64_t subkey_array[])
{
    //MPI_Barrier(MPI_COMM_WORLD);
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

    PRINT_TEXT

    MPI_Datatype stype;
    MPI_Type_vector(rcounts[rank], 1, 1, MPI_UNSIGNED_LONG_LONG, &stype);
    MPI_Type_commit(&stype);

    /*  Chia du lieu cho cac process:
        Chi co con tro text cua process 0 la tro toi vung co du lieu */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatterv(text, rcounts, displs, MPI_UNSIGNED_LONG_LONG, block,
                 1, stype, 0, MPI_COMM_WORLD);
    
    /* Phan ma hoa du lieu */
    if (is_encrypt) {
        for (int i = 0; i < rcounts[rank]; i++) {
            des(block[i], block[i], subkey_array);
        }
    }
    else {
        for (int i = 0; i < rcounts[rank]; i++) {
            inv_des(block[i], block[i], subkey_array);
        }
    }

    /* Gop du lieu tu cac process */
    MPI_Gatherv(block, 1, stype, text, rcounts, displs,
                MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    
    PRINT_TEXT

    delete[] block;
    delete[] rcounts;
    delete[] displs;
}

void ctr(uint64_t *text, int n, int is_encrypt, uint64_t subkey_array[]) {
    //MPI_Barrier(MPI_COMM_WORLD);
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
    MPI_Datatype stype;
    MPI_Type_vector(rcounts[rank], 1, 1, MPI_UNSIGNED_LONG_LONG, &stype);
    MPI_Type_commit(&stype);
    MPI_Barrier(MPI_COMM_WORLD);
    /*  Chia du lieu cho cac process:
        Chi co con tro text cua process 0 la tro toi vung co du lieu */
    MPI_Scatterv(text, rcounts, displs, MPI_UNSIGNED_LONG_LONG, block,
                 1, stype, 0, MPI_COMM_WORLD);

    /* Phan ma hoa du lieu */
    CTR_IV IV;
    IV.setCounter(displs[rank]);
    uint64_t tmp = 0;
    for (int i = 0; i < rcounts[rank]; i++) {
        des(IV.increment_IV(), tmp, subkey_array);
        block[i] ^= tmp;
    }

    /* Gop du lieu tu cac process */
    MPI_Gatherv(block, 1, stype, text, rcounts, displs,
                MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    delete[] block;
    delete[] rcounts;
    delete[] displs;
}