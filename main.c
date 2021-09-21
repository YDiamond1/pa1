#include <stdio.h>
#include "pipes.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "pa1.h"

#define NOT_CREATED_PROCESS 666
#define UNEXPECTED_MESSAGE 66
pid_t 
int getOpt(int argc, char * argv[]){
    int quantity = 0;
    if(argc != 3){
        printf("option -p is needed and this option quantity of processes");
        return -1;
    }
    if(strcmp(argv[1], "-p") == 0){
        if((quantity = atoi(argv[2])) != 0){
            return quantity;
        }
        return -1;
    }
    return -1;
}

int main(int argc, char *argv[]) {

    quantity_of_processes = 0;
    id = 0;
    quantity_of_processes = (local_id) getOpt(argc, argv);
    parent = getpid();
    current = parent;
    Message start_template = {.s_header = {.s_magic = MESSAGE_MAGIC, .s_type=STARTED, .s_payload_len = strlen(""),}, .s_payload = ""};
    Message finish_template = {.s_header = {.s_magic = MESSAGE_MAGIC, .s_type=DONE, .s_payload_len = strlen(""),}, .s_payload = ""};


    FILE *pipes_file, *events_file;

    pipes_file = fopen (pipes_log, "a");
    events_file = fopen(events_log, "a");

    create_pipes(pipes_file);
    pid_t pid;
    for(int i = 1; i <= quantity_of_processes; i++){
        pid = fork();
        if(pid == -1){
            printf("ERROR ON CREATING CHILD PROCESS: %i\n" , i);
            exit(NOT_CREATED_PROCESS);
        }

        if(pid == 0){
            id = (int8_t)i;
            current = getpid();
            fprintf(events_file, log_started_fmt, i, current, parent);
        }
    }

    close_unused_pipes();

    if(pid == 0) {
        send_multicast(&id, &start_template);
    }
    
    Message received;
    for(local_id i = 1; i <= quantity_of_processes; i++){
        if(i != id){
            int error = receive(&id, i, &received);
            if(received.s_header.s_type != STARTED){
                fprintf(events_file, "PROCESS %d: UNEXPECTED MESSAGE\n", id);
                if(parent != current)
                    send_multicast(&id, &finish_template);
                exit(UNEXPECTED_MESSAGE);
            }

            if(error == FAIL){
                fprintf(events_file, "PROCESS %d: MESSAGE HAD A WEIRD SIZE\n", id);
            }
        }
    }

    fprintf(events_file, log_received_all_started_fmt, current);


    fprintf(events_file, log_done_fmt, id);

    if(parent != current){
        send_multicast(&id, &finish_template);
    }

    for(local_id i = 1; i <= quantity_of_processes; i++){
        if(i != id){
            int error = receive(&id, i, &received);
            if(received.s_header.s_type != DONE){
                fprintf(events_file, "PROCESS %d: UNEXPECTED MESSAGE\n", id);
                exit(UNEXPECTED_MESSAGE);
            }

            if(error == FAIL){
                fprintf(events_file, "PROCESS %d: MESSAGE HAD A WEIRD SIZE\n", id);
            }
        }
    }

    close_remain_pipes();

}