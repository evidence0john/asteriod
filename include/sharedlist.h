#define SHARED_LIST_LOGFILE_LEN  512
#define SHARED_LIST_FLAGS 128

#define SHARED_LIST_ERR_LOGPATH	-10
#define SHARED_LIST_ERR_ALLOC	 	-11
#define SHARED_LIST_ERR_SHM	 	-12
#define SHARED_LIST_ERR_LOGFILE	 	-13

/*
shared list---
		   |___flags
		   |___list(swap_lbrdb)
*/

void set_shared_list_logfile_path(char *path);
int create_shared_list(unsigned int blen, unsigned int aux_blk,
		       unsigned int record, char *name, char *logfile);
char *link_shared_list(int len, char *name);
char *link_shared_list_shm(int len, char *name);
int remove_shared_list(int len, char *name);
int dump_shared_list(char *mem, unsigned int length, char *filename);
int load_shared_list(char *mem, char *filename);
