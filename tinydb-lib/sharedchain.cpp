extern "C" {
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tinylist.h"
#include "simple_shm.h"
#include "swaplist.h"
}

#include "sharedchain.hpp"

using namespace tinychain;

int _share_chain(TINYCHAIN *chain, char *logfile)
{
	char *shm, *rshm, *name;
	char namebuffer[SHARED_CHAIN_NAME_LEN];
	char *log = (char *)malloc(SHARED_CHAIN_LOG_LEN);
	FILE *logfp;
	int listlen, shmlen = SHARED_CHAIN_FLAGS_LEN;
	listlen = calc_list_swap_length(&chain->list);
	shmlen += listlen;
	name = chain->list.head;
	sprintf(namebuffer, "%s-tinychain", name);
	sprintf(log, "%s = %d", namebuffer, shmlen);
	shm = (char *)create_shm(shmlen, namebuffer);
	if (shm == NULL)
		return SHM_ERR_ALLOC;
	rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	upload_list_info(&chain->list, rshm);
	upload_list_heap(&chain->list, rshm);
	chain->info((unsigned int *)shm);
	logfp = fopen(logfile, "wb+");
	fwrite(log, strlen(log), 1, logfp);
	free(log);
	fclose(logfp);
	return shmlen;
}
int create_shared_chain(int blklen, int max_blkID, char *name, char *logfile)
{
	TINYCHAIN new_data;
	int status;
	new_data.init(blklen, max_blkID, name);
	status = _share_chain(&new_data, logfile);
	new_data.cfree();
	return status;
}

int dump_shared_chain(int schainlen, char *name)
{
	char *shm;
	char namebuffer[SHARED_CHAIN_NAME_LEN];
	sprintf(namebuffer, "%s-tinychain", name);
	shm = (char *)create_shm(schainlen, namebuffer);
	if (shm == NULL)
		return SHM_ERR_LINK;
	sprintf(namebuffer, "%s-tinychain.dmp", name);
	mem_dump(shm, schainlen, namebuffer);
	return 0;
}

int load_shared_chain(int schainlen, char *name)
{
	char *shm;
	char namebuffer[SHARED_CHAIN_NAME_LEN];
	sprintf(namebuffer, "%s-tinychain", name);
	shm = (char *)create_shm(schainlen, namebuffer);
	if (shm == NULL)
		return SHM_ERR_LINK;
	sprintf(namebuffer, "%s-tinychain.dmp", name);
	return mem_load(shm, namebuffer);
}

int remove_shared_chain(int schainlen, char *name)
{
	char namebuffer[SHARED_CHAIN_NAME_LEN];
	sprintf(namebuffer, "%s-tinychain", name);
	free_shm(schainlen, namebuffer);
	return 0;
}

char *link_shared_chain(int schainlen, char *name)
{
	char namebuffer[SHARED_CHAIN_NAME_LEN];
	sprintf(namebuffer, "%s-tinychain", name);
	return (char *)create_shm(schainlen, namebuffer);
}

int write_shared_chain(char *shm, int id, char *str)
{
	TINYCHAIN chain;
	int stat;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	stat = chain.write(id, str);
	chain.info((unsigned int *)shm);
	return stat;
}

void fout_shared_chain(char *shm, int id, void *fp)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	chain.stroutput(fp, id);
}

int read_shared_chain(char *shm, int id, char *buffer)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.read(id, buffer);
}

char *block_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.block(id);
}

int getlen_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.get_len(id);;
}

int status_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.status(id);;
}

int available_shared_chain(char *shm)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.available_id();
}

int calcstr_shared_chain(char *shm, char *str)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.calcstr(str);
}

int isfstart_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.is_field_start(id);;
}

int iscend_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	return chain.is_chain_end(id);;
}

int loadtext_shared_chain(char *shm, int id, char *filename)
{
	TINYCHAIN chain;
	int stat;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	stat = chain.load_text_file(id, filename);
	chain.info((unsigned int *)shm);
	return stat;
}

int exporttext_shared_chain(char *shm, int id, char *filename)
{
	TINYCHAIN chain;
	int stat;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	FILE *fp = fopen(filename, "wb+");
	if (fp == NULL)
		return SC_ERR_EXPORT;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	stat = chain.stroutput((void *)fp, id);
	fclose(fp);
	return stat;
}

int del_shared_chain(char *shm, int id)
{
	TINYCHAIN chain;
	char *rshm = shm + SHARED_CHAIN_FLAGS_LEN;
	chain.sync((unsigned int *)shm);
	sync_list_info(&chain.list, rshm);
	link_list_heap(&chain.list, rshm);
	chain.del(id);
	chain.info((unsigned int *)shm);
	return 0;
}

