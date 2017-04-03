#ifndef _TINYCHAIN_
	#include "tinychain.hpp"
#endif

/*
Shared data chain
FLAG-LBRDB
|    |__chain's body
|_____datachain info--->
|__________shm flags   |__blen
                       |__total
                       |__in_use
                       |__available
*/
#define SHARED_CHAIN_FLAGS_LEN	128
#define SHARED_CHAIN_LOG_LEN	512
#define SHARED_CHAIN_NAME_LEN	128

#define SHM_ERR_ALLOC	-10
#define SHM_ERR_LINK	-11
#define SC_ERR_EXPORT	-20

using namespace tinychain;

int _share_chain(TINYCHAIN *chain, char *logfile);
int create_shared_chain(int blklen, int max_blkID, char *name, char *logfile);
int dump_shared_chain(int schainlen, char *name);
int load_shared_chain(int schainlen, char *name);
int remove_shared_chain(int schainlen, char *name);
char *link_shared_chain(int schainlen, char *name);
int write_shared_chain(char *shm, int id, char *str);
void fout_shared_chain(char *shm, int id, void *fp);
int read_shared_chain(char *shm, int id, char *buffer);
char *block_shared_chain(char *shm, int id);
int getlen_shared_chain(char *shm, int id);
int status_shared_chain(char *shm, int id);
int available_shared_chain(char *shm);
int calcstr_shared_chain(char *shm, char *str);
int isfstart_shared_chain(char *shm, int id);
int iscend_shared_chain(char *shm, int id);
int loadtext_shared_chain(char *shm, int id, char *filename);
int exporttext_shared_chain(char *shm, int id, char *filename);
int del_shared_chain(char *shm, int id);