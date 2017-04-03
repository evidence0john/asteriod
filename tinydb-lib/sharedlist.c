#ifdef __cplusplus
	extern "C" {
#endif
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "simple_shm.h"
#include "tinylist.h"
#include "swaplist.h"

#include "sharedlist.h"

int create_shared_list(unsigned int blen, unsigned int aux_blk,
		       unsigned int record, char *name, char *logfile)
{
	char *pool, *shm;
	LIST trans;
	char log[SHARED_LIST_LOGFILE_LEN];
	int length;
	FILE *logfp;
	pool = create_list(name, blen, aux_blk, record);
	if (pool == NULL)
		return SHARED_LIST_ERR_ALLOC;
	sync_list_info(&trans, pool);
	length = calc_list_swap_length(&trans);
	length += SHARED_LIST_FLAGS;
	shm = create_shm(length, name);
	if (shm == NULL) {
		free(pool);
		return SHARED_LIST_ERR_SHM;
	}
	memcpy(shm + SHARED_LIST_FLAGS, pool, length - SHARED_LIST_FLAGS);
	sprintf(log, "%s = %d\n", name, length);
	logfp = fopen(logfile, "wb+");
	if (logfp == NULL) {
		free_shm(length, name);
		return SHARED_LIST_ERR_LOGFILE;
	}
	fwrite(log, strlen(log), 1, logfp);
	fclose(logfp);
	return length;
}

char *link_shared_list(int len, char *name)
{
	return create_shm(len, name) + SHARED_LIST_FLAGS;
}

char *link_shared_list_shm(int len, char *name)
{
	return create_shm(len, name);
}

int remove_shared_list(int len, char *name)
{
	free_shm(len, name);
	return 0;
}

int dump_shared_list(char *mem, unsigned int length, char *filename)
{
	return mem_dump(mem, length, filename);
}

int load_shared_list(char *mem, char *filename)
{
	return mem_load(mem, filename);
}
#ifdef __cplusplus
	}
#endif