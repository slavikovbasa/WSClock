#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "process.h"

#define ITERATIONS 10
#define MEM_ACCESSES 5

Process processes[PROC_NUM];

int main() {
    int seed;
	FILE *urand = fopen("/dev/urandom", "rb");
	if(1 != fread(&seed, sizeof(seed), 1, urand)){
		return -1;
	}
	srand(seed);
    fclose(urand);

    init_memory();
    printf("Initializing processes...\n");
    for (int i = 0; i < PROC_NUM; i++) {
        int process_page_count = rand() % PAGE_NUM + PAGE_NUM/2;
        if (init_process(&processes[i], i, process_page_count, PROC_FRAMES)) {
            printf("init_process error\n");
            return -1;
        }
    }

    for (int j = 0; j < ITERATIONS; j++) {
        for (int i = 0; i < PROC_NUM; i++) {
            Process *p = &processes[i];
            for(int k = 0; k < MEM_ACCESSES; k++){
                int page_id = 0;
                int local_size = (p->page_count - 1)/10 + 1;
                double r = rand() / (double)RAND_MAX;
                if (r < 0.9)
                    page_id = rand() % local_size;
                else
                    page_id = local_size + rand() % (p->page_count - local_size);

                access_page(p, page_id);
                processes[i].curr_time++;
            }
            for(int k = 0; k < p->page_count; k++)
                p->page_table[k].referenced = 0;
            printf("\n");
        }
    }

    printf("Terminating processes...\n");
    for (int i = 0; i < PROC_NUM; i++)
        end_process(&processes[i]);
    

    return 0;
}
