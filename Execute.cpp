#include "DES.h"
#include <mpi.h>

int ask_yesno(std::string &option)
{
    std::getline(std::cin, option);
    std::regex e ("y(es){0,1}|no{0,1}", std::regex_constants::icase);
    if (std::regex_match(option, e)) {
        return 0;
    } else {
        std::cout << "No option." << std::endl;
    }
    return 1;
}

int open_file(FILE* &in, FILE* &out) {
    /* Doc file that bai thi tra ve 0, nguoc lai tra ve 1 */
    in = fopen(infile_name.c_str(), "rb");
    if (in == nullptr) {
        std::cout << "Cannot open file " + infile_name << std::endl;
        return 0;
    }

    out = fopen(oufile_name.c_str(), "rb");
    if (out != nullptr) {
        std::string option;
        std::cout << oufile_name + " is existed." << std::endl;
        do {
            std::cout << "Are you want overwrite? (Y/N)" << std::endl;
        } while(ask_yesno(option));
        fclose(out);
        std::regex yes("y(es){0,1}", std::regex_constants::icase);
        if (std::regex_match(option, yes)) {
            out = fopen(oufile_name.c_str(), "wb");
        } else {
            return 0;
        }
    }
    return 1;
}

int read_file(FILE *in) {

}

int reallocate(int64_t* &p, int new_size)
{
    delete[] p;
    p = nullptr;
    p  = new int64_t[new_size]();
    if (p == nullptr) {
        std::cout << "Not enough memory." << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int flag_kill = 1;
    int rank = 0, nproc = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    FILE *in = nullptr, *out = nullptr;
    int64_t subkey_array[16];

    /* Chi process 0 duoc doc file */
    if(rank == 0) {
        if(!open_file(in, out)) {
            flag = 0;
            MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
        } else {
            
            key_schedule(subkey_array, key, is_encrypt, 16);
            MPI_Bcast(subkey_array, 16, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
        }
        
    }
    /* Tat ca process phai doi process 0 doc file xong moi duoc thuc hien tiep */
    MPI_Barrier(MPI_COMM_WORLD);

    /* Kiem tra xem chuong trinh co ket thuc chua */
    if(flag_kill) {

    }

    /* Process 0 dong file */
    if(rank == 0) {
        if(in != nullptr) fclose(in);
        if(out != nullptr) fclose(out);
    }
    MPI_Finalize();
    return 0;
}