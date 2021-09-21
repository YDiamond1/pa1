//
// Created by diy on 20.09.2021.
//
#include <malloc.h>
#include "ipc.h"
#include "pipes.h"

int send(void * self, local_id dst, const Message * msg){
    local_id arg = *(local_id*)self;
    size_t size_of_message = sizeof(MessageHeader) + msg->s_header.s_payload_len;

    ssize_t bytes = write(pipes[arg][dst].fd[WRITE], msg, size_of_message);
    if(bytes == -1 || bytes < size_of_message)
        return FAIL;
    return SUCCESS;
}

int send_multicast(void * self, const Message * msg){
    local_id arg = *(local_id*)self;
    
    for(local_id i = 0; i <= quantity_of_processes; i++){
        if(i != arg){
            int error = send(self, i, msg);
            if(error == FAIL)
                return FAIL;
        }
    }
    return SUCCESS;
}

int receive(void * self, local_id from, Message * msg){
    local_id arg = *(local_id*)self;

    long error = read(pipes[from][arg].fd[READ], &(msg->s_header), sizeof(MessageHeader));
    if(error < sizeof (MessageHeader)){
        return FAIL;
    }

    size_t nbytes = sizeof(char) * msg->s_header.s_payload_len;
    error = read(pipes[from][arg].fd[READ], &(msg->s_payload), nbytes);

    if(error < nbytes)
        return FAIL;
    return SUCCESS;
}

int receive_any(void * self, Message * msg){
    local_id arg = *(local_id*)self;

    for(local_id i = 0; i < quantity_of_processes; i++){
        if(i != arg){
            int error = receive(self, i, msg);
            if(error == SUCCESS) return SUCCESS;
        }
    }
}