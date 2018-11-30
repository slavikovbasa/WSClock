#define PAGE_NUM 100
#define PROC_NUM 10
#define PROC_FRAMES PAGE_NUM/PROC_NUM

void init_memory();
int request_page_frame(int pid);

