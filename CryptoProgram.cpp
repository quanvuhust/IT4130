#include <iostream>
#include <string>

using namespace std;


int main() {
    int nproc = 0;
    string command("mpirun -np ");
    const string host_file = "machinefile";
    const string exec_file = "./CryptoProgram";
    command += to_string(nproc) + " -f " + hostfile + " " + exec_file;
    system(command.c_str());
    return 0;
}