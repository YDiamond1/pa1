//
// Created by diy on 20.09.2021.
//

#include "pipes.h"

void create_pipes(FILE * pipe_log){
    for(int i = 0; i <= quantity_of_processes; i++){
        for(int j = 0; j <= quantity_of_processes; j++){
            if(i != j){
                if(pipe(pipes[i][j].fd) == -1){
                    fprintf(pipe_log, "pipe %i -> %i fail\n", i, j);
                }else{
                    fprintf(pipe_log, "pipe %i -> %i created\n", i, j);
                }
            }
        }
    }
    fflush(pipe_log);
}

void close_unused_pipes(){
    for(int i = 0; i <= quantity_of_processes; i++){
        for(int j = 0; j <= quantity_of_processes; j++){
            if(i != id && i != j){
                close(pipes[i][j].fd[WRITE]);
                fprintf(pipes_file, "CLOSE WRITE %i -> %i process %i\n", i, j, id);
                fflush(pipes_file);
            }
            if(j != id && i != j){
                close(pipes[i][j].fd[READ]);
                fprintf(pipes_file, "CLOSE READ %i -> %i process %i\n", i, j, id);
                fflush(pipes_file);
            }
        }
    }
}

void close_remain_pipes(){
    for(local_id j = 0; j < quantity_of_processes; j++){
        if(j != id) {
            close(pipes[id][j].fd[WRITE]);
            close(pipes[j][id].fd[READ]);
        }
    }
}
