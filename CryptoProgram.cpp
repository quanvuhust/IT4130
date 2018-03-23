#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

void execute_crypto_program(int nproc, ) {
    string command("mpirun -np ");
    const string host_file = "machinefile";
    const string exec_file = "./CryptoProgram";
    command += to_string(nproc) + " -f " + hostfile + " " + exec_file;
    system(command.c_str());
}

int main() {
    string option;
    regex e("^0+[12]$");
    while(1) {
        cout << "Mode crypto:" << endl;
        cout << "1. Encrypt." << endl;
        cout << "2. Decrypt." << endl;
        getline(cin, option);
        if(regex_match(option, e)) {
            break;
        } else {
            cout << "No option." << endl;
        }
    }
    int op = stoi(option);
    switch(op) {
        case 1:
            cout << "Mode encrypt:" << endl;
            cout << "1. ECB." << endl;
            cout << "2. CTR." << endl;
            break;
        case 2:
    }

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