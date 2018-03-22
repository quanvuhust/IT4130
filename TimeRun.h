#ifndef TIMERUN_H
#define TIMERUN_H 1
#include <mpi.h>

class TimeRun {
private:
    double startTime = 0;
public:
    void initTime() {
        startTime = MPI_Wtime();
    }

    double getTime() {
        return MPI_Wtime() - startTime;
    }
};

#endif
