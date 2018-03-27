#include <iostream>
#include <fstream>
#include <regex>

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

    fstream inp(list_file.c_str(), ios::in);
    if (inp.is_open()) {
        while (!inp.eof()) {
            inp >> lst.in_file;
            inp >> lst.ou_file;
            inp >> lst.nproc;
        }
    } else {
        cerr << "Error opening file " + list_file << endl;
        return 1; // Loi xay ra
    }
    list_file.close();
    return 0; // Khong co loi xay ra
}


void CryptoProgram::execute() {
    int n = lst.size();
    for (int i = 0; i < n; i++) {
        string command("mpirun -np ");

        const string host_file = "machinefile";
        fstream machine_file(hostfile.c_str(), ios::out);
        machinefile << "master:" << lst[i].nproc / 2 << endl;
        machinefile << "slave:" << lst[i].nproc - lst[i].nproc / 2;
        machine_file.close();
        command.append(to_string(lst[i].nproc));
        command.append(" -f " + hostfile + " ./CryptoProgram ");

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
        system(command.c_str());
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