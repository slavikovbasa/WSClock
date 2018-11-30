#include "memory.h"
#define TAU 10

typedef struct {
    int present;
    int referenced;
    int modified;
    int swapped;
    int page_frame;
    long access_time;
} PageTabEntry;

struct PageList {
    struct PageListNode *curr;
    struct PageListNode *tail;
};

struct PageListNode {
    struct PageListNode *next;
    PageTabEntry *entry;
    int address;
};

typedef struct {
    int pid;
    int page_count;
    int frame_count;
    PageTabEntry *page_table;
    struct PageList page_list;
    long curr_time;
} Process;

int init_process(Process *p, int pid, int page_count, int frames_count);
void access_page(Process *p, int page_id);
void end_process(Process *p);
