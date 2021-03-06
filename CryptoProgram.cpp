#include <iostream>
#include <fstream>
#include <regex>
#include <cstdio>
#include "CryptoProgram.h"
#include "Mode.h"

using namespace std;

int CryptoProgram::input() {
    string option;
    regex e("^0*[12]$");
    while (1) {
        cout << "Mode crypto:" << endl;
        cout << "1. Encrypt." << endl;
        cout << "2. Decrypt." << endl;
        getline(cin, option);
        if (regex_match(option, e)) {
            break;
        } else {
            cout << "No option." << endl;
        }
    }
    is_encrypt = (stoi(option) == 1);
    string list_file;

    switch (is_encrypt) {
    case 0:
        list_file = "ListDecrypt.txt";
        break;
    default:
        while (1) {
            cout << "Mode encrypt:" << endl;
            cout << "1. ECB." << endl;
            cout << "2. CTR." << endl;
            getline(cin, option);
            if (regex_match(option, e)) {
                mode = stoi(option);
                break;
            } else {
                cout << "No option." << endl;
            }
        }
        list_file = "ListEncrypt.txt";
        break;
    }
    cout << list_file << endl;
    fstream inp(list_file.c_str(), ios::in);
    session sess;
    if (inp.is_open()) {
	int n = 0;
	inp >> n; 
        for(int i = 0; i < n; i++) {
            inp >> sess.in_file;
            inp >> sess.ou_file;
            inp >> sess.nproc;
            lst.emplace_back(sess);
        }
    } else {
        cerr << "Error opening file " + list_file << endl;
        return 1; // Loi xay ra
    }
    inp.close();
    return 0; // Khong co loi xay ra
}


void CryptoProgram::execute() {
    int n = lst.size();
    string key("0x133457799BBCDFF1");
    for (int i = 0; i < n; i++) {
        string command("mpirun -np ");

        const string host_file = "machinefile";
        fstream machine_file(host_file.c_str(), ios::out);
        machine_file << "master:" << lst[i].nproc / 2 << endl;
        machine_file << "slave:" << lst[i].nproc - lst[i].nproc / 2;
        machine_file.close();
        command.append(to_string(lst[i].nproc));
        if(lst[i].nproc == 1) {
            command.append(" ./Execute ");
        } else {
            command.append(" -f " + host_file + " ./Execute ");
        }
        
        command.append("-k " + key + " ");
        if (is_encrypt) {
            command.append("-e" + to_string(mode));
            command.append(" " + lst[i].in_file);
            switch (mode) {
            case ECB:
                command.append(" " + lst[i].ou_file + ".ECB");
                break;
            case CTR:
                command.append(" " + lst[i].ou_file + ".CTR");
                break;
            }
        } else {
            string extension = lst[i].in_file.substr(lst[i].in_file.find_last_of('.'));
            if (extension.compare(".ECB") == 0) {
                command.append("-d1");
            } else if (extension.compare(".CTR") == 0) {
                command.append("-d2");
            }
            command.append(" " + lst[i].in_file);
            command.append(" " + lst[i].ou_file);
        }

        /* Doi lenh system() thuc hien xong */
        cout << command << endl;
        system(command.c_str());
        //system("rm -rf ./Test/Ciphertext/*");
    }
}

int main() {
    CryptoProgram program;
    if (program.input()) {
        return 1;
    }
    program.execute();

    return 0;
}
