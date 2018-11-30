#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "process.h"


void init_page(PageTabEntry *record) {
    record->present = 0;
    record->referenced = 0;
    record->modified = 0;
    record->swapped = 0;
    record->page_frame = 0;
    record->access_time = 0;
}

int init_process(Process *p, int pid, int page_count, int frame_count) {
    size_t page_table_size = sizeof(PageTabEntry)*page_count;
    p->page_table = malloc(page_table_size);

    if (p->page_table == NULL) 
        return -1;

    for (int i = 0; i < page_count; i++)
        init_page(&p->page_table[i]);

    p->pid = pid;
    p->page_count = page_count;
    p->curr_time = 0;
    p->page_list.curr = NULL;
    p->page_list.tail = NULL;
    p->frame_count = frame_count;

    printf("New process with pid %d initialized with %d pages\n", pid, page_count);
    return 0;        
}

void end_process(Process *p) {
    free(p->page_table);

    printf("Process with pid %d has terminated\n", p->pid);
}

void schedule_write(PageTabEntry *page) {
    page->modified = 0;
}

void page_fault(Process *p, int page_id) {
    int page_frame = 0;
    if ((page_frame=request_page_frame(p->pid)) >= 0) {        
        if (p->page_table[page_id].swapped == 1) {
            printf("Virtual page %d of process %d is swapped back to memory...\n", 
                                                                page_id, p->pid);
            p->page_table[page_id].swapped = 0;
        }
        p->page_table[page_id].page_frame = page_frame;
        printf("Virtual page %d of process %d is assigned to page frame %d...\n", 
                                                    page_id, p->pid, page_frame);
        p->page_table[page_id].present = 1;

        struct PageListNode *node = malloc(sizeof(*node));
        node->entry = &p->page_table[page_id];
        node->address = page_id;
        if(p->page_list.curr == NULL) {
            p->page_list.curr = node;
            p->page_list.tail = node;
            node->next = p->page_list.tail;
        } else {
            node->next = p->page_list.curr;
            p->page_list.curr = node;
            p->page_list.tail->next = node;
        }
        printf("Virtual page %d of process %d is added to linked list\n",
                                                    page_id, p->pid);
        return;
    }
    
    printf("No free page frames found. Starting replacing algorithm\n");
    struct PageListNode *curr = p->page_list.curr;
    int step_counter = 0;
    while (1) {
        if (curr->entry->referenced) {
            printf("Skipping page %d of process %d: referenced...\n",
                                                curr->address, p->pid);
            curr->entry->referenced = 0;
            curr = curr->next;
            step_counter++;
        } else if (step_counter < p->frame_count && 
                    p->curr_time - curr->entry->access_time < TAU) {
            printf("Skipping page %d of process %d: currently in working set...\n",
                                                            curr->address, p->pid);
            curr = curr->next;
            step_counter++;
        } else if (curr->entry->modified) {
            printf("Skipping page %d of process %d: modified. Scheduling it for disk write\n",
                                                curr->address, p->pid);
            schedule_write(curr->entry);
            curr = curr->next;
            step_counter++;
        } else {
            printf("Page %d: R=%d, M=%d, P=%d, T=%ld\n", curr->address, curr->entry->referenced
                                                     , curr->entry->modified, curr->entry->present
                                                     , curr->entry->access_time);
            printf("Swapping page %d of process %d\n", curr->address, p->pid);
            curr->entry->swapped = 1;
            curr->entry->present = 0;

            p->page_table[page_id].page_frame = curr->entry->page_frame;
            printf("Page %d of process %d replaced page %d and recieved page frame %d\n",
                            page_id, p->pid, curr->address, curr->entry->page_frame);
            p->page_table[page_id].present = 1;
            curr->entry = &p->page_table[page_id];
            return;
        }
    }
}

void access_page(Process *p, int page_id) {
    PageTabEntry *page_entry = &p->page_table[page_id];

    if (!page_entry->present) {
        printf("Access to virtual page %d of process %d caused page fault...\n", 
                                                                page_id, p->pid);
        page_fault(p, page_id);
    }

    int seed;
	FILE *urand = fopen("/dev/urandom", "rb");
	fread(&seed, sizeof(seed), 1, urand);
	srand(seed);
    fclose(urand);

    double r = rand() / (double)RAND_MAX;
    if (r < 0.5) {
        printf("Virtual page %d of process %d is read...\n", 
                                                page_id, p->pid);
        page_entry->referenced = 1;
    } else {
        printf("Virtual page %d of process %d is written to...\n", 
                                                page_id, p->pid);
        page_entry->referenced = 1;
        page_entry->modified = 1;
    }
    page_entry->access_time = p->curr_time;
}