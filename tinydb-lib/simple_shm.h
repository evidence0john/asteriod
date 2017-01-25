/*
This is a simple lib to creat shared memory on Linux or Microsoft Windows .

When using Linux IPC , shared memory used a type key_t as ID .
On Windows , shared memory can be creat using the Windows API , the way is to creat
a flie mapping , and Windows using a string as the tag for file mapping , so this lib using
DJB hash function to calculate a hash key of a string as the ID using for Linux IPC method .
By this way both Windows and Linux can using a string name as the tag for shared memory ,
and programmer should take care about the conflict of DJB Hash .

*/
#define LINUX
//#define WIN_NT

#ifdef LINUX

#include"unistd.h"
#include"sys/shm.h"

#define STR2ID_SEED 0 //seed for DJBHash to switch a string as ID

#define _ACCESS_ 0666|IPC_CREAT

#define SHM_GETID_FAILED 		-1
#define SHMCTL_IPC_RMID_FAILED	-2

#define get_shm_linux create_shm_linux

#endif

#ifdef WIN_NT

#include "Windows.h"
#define MAPPING_MODE 	INVALID_HANDLE_VALUE
#define _ACCESS_PAGE_ 	PAGE_READWRITE
#define CREATE_FILE_MAPPING_FAILED 		-1

#endif

void *create_shm(unsigned int LEN, char *name);
int free_shm(unsigned int LEN, char *name);
int mem_dump(char *mem, unsigned int length, char *filename);
int mem_load(char *mem, char *filename);
unsigned int _get_file_length(char *filename);

#ifdef LINUX

void *create_shm_linux(unsigned int LEN, unsigned int ID);
int free_shm_linux(unsigned int LEN, unsigned int ID);

#endif

#ifdef WIN_NT

void *create_file_mapping_win_nt(unsigned int LEN, char *name);
int close_file_mapping_win_nt(unsigned int LEN, char *name);

#endif
