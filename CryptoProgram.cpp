#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void execute_crypto_program(int nproc, ) {
    string command("mpirun -np ");
    const string host_file = "machinefile";
    const string exec_file = "./CryptoProgram";
    command += to_string(nproc) + " -f " + hostfile + " " + exec_file;
    system(command.c_str());
}

int main() {
    const string list_file = "List.txt";
    ifstream inp(list_file.c_str(), ios::in);
    if (inp.is_open()) {
        while () {

        }
    } else {
        cerr << "Error opening file " + list_file << endl;
        return 1;
    }
    list_file.close();


    return 0;
}