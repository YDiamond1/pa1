//
// Created by diy on 20.09.2021.
//

#ifndef PA1_1_PIPES_H
#define PA1_1_PIPES_H

#include "ipc.h"
#include <stdio.h>
#include <unistd.h>


#define max_processes 10
enum IO{
    READ = 0,
    WRITE
};

enum RESULTS{
    SUCCESS = 0,
    FAIL = -666
};

struct pipe{
    int fd[2];
};



local_id id;
struct pipe pipes[max_processes][max_processes];
local_id quantity_of_processes;
pid_t parent;
pid_t current;

void create_pipes(FILE* pipe_log);

void close_unused_pipes();

void close_remain_pipes();

#endif //PA1_1_PIPES_H
