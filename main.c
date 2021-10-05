#include <stdio.h>
#include "pipes.h"
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.h"
#include "pa1.h"

#define NOT_CREATED_PROCESS 666
#define UNEXPECTED_MESSAGE 66

int getOpt(int argc, char * argv[]) {
    int quantity = 0;
    if (argc != 3 && argc != 2) {
        printf("option -p is needed and this option quantity of processes");
        return -1;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "-p") == 0) {
            if ((quantity = atoi(argv[2])) != 0) {
                return quantity;
            }
            return -1;
        }
    }else {
        if(argv[1][0] == '-' && argv[1][1] == 'p')
            if ((quantity = atoi(argv[1]+2)) != 0) {
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
    printf("quantity of process: %i\n", quantity_of_processes);
    fflush(stdout);
    parent = getpid();
    current = parent;



    FILE *events_file;



    pipes_file = fopen(pipes_log, "w");
    events_file = fopen(events_log, "w");

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
            break;
        }

        if(pid != 0){
            printf("created child\n");
            fflush(stdout);
        }

    }

    if(pid != 0){
        current = parent;
    }

    close_unused_pipes();
    Message start_template = {.s_header = {.s_magic = MESSAGE_MAGIC, .s_type=STARTED, .s_payload_len = strlen(""),}, .s_payload = ""};
    Message finish_template = {.s_header = {.s_magic = MESSAGE_MAGIC, .s_type=DONE, .s_payload_len = strlen(""),}, .s_payload = ""};
    //init template
    sprintf(start_template.s_payload, log_started_fmt, id, current, parent);
    start_template.s_header.s_payload_len = strlen(start_template.s_payload) + 1;

    sprintf(finish_template.s_payload, log_done_fmt, id);
    finish_template.s_header.s_payload_len = strlen(finish_template.s_payload) + 1;

    if(pid == 0) {
        send_multicast(&id, &start_template);
    }

    Message received;
    for(local_id i = 1; i <= quantity_of_processes; i++){
        if(i != id){
            int error = receive(&id, i, &received);
            if(error == SUCCESS && received.s_header.s_type != STARTED){
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

    if(parent == current){

        int status;
        for(int i = 1; i <= quantity_of_processes; i++ ){
            if(wait(&status) == -1){
                printf("Error while ending child\n");
                exit(UNEXPECTED_MESSAGE);
            }else{
                if(WEXITSTATUS(status) == SUCCESS){
                    printf("SUCCESS WAIT\n");
                }
            }
        }
    }

    return 0;
}

