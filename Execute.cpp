#include "DES.h"
#include "Mode.h"
#include "TimeRun.h"
#include <mpi.h>
#include <regex>
#include <string>

#define BLOCK_SIZE (16 * 1024 * 1024)

struct info {
    std::string infile_name;
    std::string oufile_name;
    uint64_t key;
    int is_encrypt = 1;
    int mode;
};

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

int open_file(FILE* &in, FILE* &out, const info &inf) {
    /* Doc file that bai thi tra ve 0, nguoc lai tra ve 1 */
    in = fopen(inf.infile_name.c_str(), "rb");
    if (in == nullptr) {
        std::cout << "Cannot open file " + inf.infile_name << std::endl;
        return 0;
    }

    out = fopen(inf.oufile_name.c_str(), "rb");
    if (out != nullptr) {
        std::string option;
        std::cout << inf.oufile_name + " is existed." << std::endl;
        do {
            std::cout << "Are you want overwrite? (Y/N)" << std::endl;
        } while (ask_yesno(option));
        fclose(out);
        std::regex yes("y(es){0,1}", std::regex_constants::icase);
        if (std::regex_match(option, yes)) {
            out = fopen(inf.oufile_name.c_str(), "wb");
        } else {
            return 0;
        }
    } else {
        out = fopen(inf.oufile_name.c_str(), "wb");
    }
    return 1;
}

int read_file(FILE *in, uint64_t* &text, int is_encrypt, uint64_t &n) {
    n = fread(text, sizeof(unsigned char), 8 * BLOCK_SIZE, in);
    std::cout << "n= " << n << std::endl;
    if (n <= 0) {
        return 1;
    }
    if (is_encrypt) {
        n = PKCS7_padding(text, n);
    }

    if (feof(in)) {
        return 1;
    }
    return 0;
}

int get_parameter(char *argv[], info &inf, int &i) {
    std::regex encrypt_pattern("^-e\\d$", std::regex_constants::icase);
    std::regex decrypt_pattern("^-d\\d$", std::regex_constants::icase);
    std::regex key_pattern("^-k$", std::regex_constants::icase);
    if (regex_match(argv[i], encrypt_pattern)) {
        inf.is_encrypt = 1;
        inf.mode = atoi(&argv[i][2]);
        return 0;
    } else if (regex_match(argv[i], decrypt_pattern)) {
        inf.is_encrypt = 0;
        inf.mode = atoi(&argv[i][2]);
        return 0;
    }  else if (regex_match(argv[i], key_pattern)) {
        i++;
        inf.key = std::stoull(argv[i], nullptr, 16);
    }
    return 1;
}

int get_path(char *s, info &inf) {
    static int state = 0;
    if (state == 0) {
        inf.infile_name = s;
        state++;
    } else {
        inf.oufile_name = s;
    }
    return 1;
}

int sparse_arg(int argc, char* argv[], info &inf) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            get_parameter(argv, inf, i);
        } else {
            get_path(argv[i], inf);
        }
    }
}

int main(int argc, char* argv[]) {
    TimeRun timer;
    int flag_kill = 1;
    int rank = 0, nproc = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    int is_encrypt;
    int mode;
    uint64_t key = 0;
    info inf;
    if (rank == 0) {
        timer.initTime();
        sparse_arg(argc, argv, inf);
        is_encrypt = inf.is_encrypt;
        mode = inf.mode;
        key = inf.key;
    }

    MPI_Bcast(&is_encrypt, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&mode, 1, MPI_INT, 0, MPI_COMM_WORLD);

    FILE *in = nullptr, *out = nullptr;
    uint64_t subkey_array[16];
    uint64_t n = 0;
    unsigned long long file_size = 0;

    /* Chi process 0 duoc mo file va tao khoa con*/
    if (rank == 0) {
        if (!open_file(in, out, inf)) {
            flag_kill = 0;
        } else {
            if (!key_schedule(subkey_array, key, is_encrypt, 16)) {
                flag_kill = 0;
            }
        }
    }
    MPI_Bcast(&flag_kill, 1, MPI_INT, 0, MPI_COMM_WORLD);
    /* Tat ca process phai doi process 0 doc file xong moi duoc thuc hien tiep */

    uint64_t *text = nullptr;

    /* Kiem tra xem chuong trinh co ket thuc chua */
    if (flag_kill) {
        MPI_Bcast(subkey_array, 16, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            text = new uint64_t[BLOCK_SIZE + 1]; // +1 to padding
            if (text == nullptr) {
                std::cerr << "Not enough memory." << std::endl;
                flag_kill = 0;
            }
        }
        MPI_Bcast(&flag_kill, 1, MPI_INT, 0, MPI_COMM_WORLD);
        int count = 0;
        while (flag_kill) {
            /* Chi process 0 duoc doc file */
            if (rank == 0) {
                if (read_file(in, text, is_encrypt, n)) {
                    flag_kill = 0;
                }
                file_size += n;
            }
            MPI_Bcast(&flag_kill, 1, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Bcast(&n, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
            //MPI_Barrier(MPI_COMM_WORLD);

            if (flag_kill) {
                /* Tien hanh ma hoa hoac giai ma khoi du lieu */
                switch (mode) {
                case ECB:
                    ecb(text, n / 8, is_encrypt, subkey_array);
                    break;
                case CTR:
                    ctr(text, n / 8, is_encrypt, subkey_array);
                    break;
                }
                /* Doi tat ca cac processor hoan thanh xong nhiem vu */
                MPI_Barrier(MPI_COMM_WORLD);
                /* Processor 0 thuc hien ghi ket qua xuong file */
                if (rank == 0) {
                    if (!is_encrypt) {
                        n = PKCS7_truncate((unsigned char*)text, n);
                    }
                    fwrite(text, sizeof(unsigned  char), n, out);
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    /* Process 0 dong file */
    if (rank == 0) {
        timer.writeLog(file_size, nproc);
        if (in != nullptr) fclose(in);
        if (out != nullptr) fclose(out);
        if(text != nullptr) delete[] text;
    }
    MPI_Finalize();
    return 0;
}