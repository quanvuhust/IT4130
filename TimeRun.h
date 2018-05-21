#ifndef TIMERUN_H
#define TIMERUN_H 1
#include <mpi.h>

class TimeRun {
private:
    double startTime = 0;
    const char *log_file_name = "result.txt";
public:
    void initTime() {
        startTime = MPI_Wtime();
    }

    double getTime() {
        return MPI_Wtime() - startTime;
    }

    void writeLog(unsigned long long file_size, int nproc) {
        FILE *log = nullptr;
        log = fopen(log_file_name, "at");
        fprintf(log, "%llu %d %lf\n", file_size, nproc, getTime());
        printf("%llu %d %lf\n", file_size, nproc, getTime());
        if (log != nullptr) fclose(log);
    }
};

#endif
