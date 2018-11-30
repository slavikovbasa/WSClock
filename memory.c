#include <stdio.h>
#include "memory.h"

int memory[PAGE_NUM];

void init_memory() {
    for (int i = 0; i < PAGE_NUM; i++)
        memory[i] = 0;
    printf("Physical memory initialized with %d pages\n", PAGE_NUM);
}

int request_page_frame(int pid) {
    printf("Process %d requests page frame...\n", pid);
    int start = pid * PROC_FRAMES;
    for (int i = start; i < start + PROC_FRAMES; i++)
        if (memory[i] == 0){
            memory[i] = 1;
            printf("Free page frame with address %d is found\n", i);
            return i;
        }
    return -1;
}
