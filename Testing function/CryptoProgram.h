#ifndef CRYPTOPROGRAM_H
#define CRYPTOPROGRAM_H
#include <vector>
#include <string>

using namespace std;
enum MODE{ECB = 1, CTR = 2};
class CryptoProgram {
private:
    struct session{
        string in_file;
        string ou_file;
        int nproc;
    };
    int is_encrypt = 1;
    int mode;
    vector<session> lst;
public:
    int input();
    void execute();
};

#endif
