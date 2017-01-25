#include "string.h"
#include "stdlib.h"
#include "tinylist.h"
#include "swaplist.h"

unsigned int calc_list_swap_length(LIST * list)
{
	unsigned int i, length = 0;
	length += LIST_SINFO_BUFFER;
	length += LIST_HEAD_LEN;
	length += list->length;
	length += list->record * list->aux_blk;
	if (!list->mode_flag & LIST_pool_blklist_mode) {
		i = list->record * sizeof(int);
		length += i;
	}
	if (list->mode_flag & LIST_Hash_table_DI32) {
		i = list->record * LIST_HASH_DI32;
		length += i;
	}
	return length;
}

int upload_list_info(LIST * list, char *str)
{
	char list_info_uchar[LIST_SINFO_BUFFER];
	unsigned int *list_info_uint = (unsigned int *)list_info_uchar;
	list_info_uchar[LIST_SINFO_MODE_F] = list->mode_flag;
	list_info_uchar[LIST_SINFO_ERRCHK] = list->error_check;
	list_info_uint[LIST_SINFO_RFBLOCK] = list->aux_blk;
	list_info_uint[LIST_SINFO_BLEN] = list->blen;
	list_info_uint[LIST_SINFO_LENGTH] = list->length;
	list_info_uint[LIST_SINFO_RECORD] = list->record;
	list_info_uint[LIST_SINFO_PIDC] = list->pidc;
	list_info_uint[LIST_SINFO_EIDC] = list->eidc;
	memcpy(str, list_info_uchar, LIST_SINFO_BUFFER);
	return 0;
}

int upload_list_heap(LIST * list, char *str)
{
	int i;
	str += LIST_SINFO_BUFFER;
	memcpy(str, list->head, LIST_HEAD_LEN);
	str += LIST_HEAD_LEN;
	memcpy(str, list->mem_pool, list->length);
	str += list->length;
	if (list->aux_blk) {
		i = list->aux_blk * list->record;
		memcpy(str, list->aux, i);
	}
	if (!list->mode_flag & LIST_pool_blklist_mode) {
		str += i;
		i = list->record * sizeof(int);
		memcpy(str, list->map, i);
	}
	if (list->mode_flag & LIST_Hash_table_DI32) {
		str += i;
		i = list->record * LIST_HASH_DI32;
		memcpy(str, list->hash_table, i);
	}
	return 0;
}

int sync_list_info(LIST * list, char *str)
{
	char *list_info_uchar = str;
	unsigned int *list_info_uint = (unsigned int *)list_info_uchar;
	list->mode_flag = list_info_uchar[LIST_SINFO_MODE_F];
	list->error_check = list_info_uchar[LIST_SINFO_ERRCHK];
	list->aux_blk = list_info_uint[LIST_SINFO_RFBLOCK];
	list->blen = list_info_uint[LIST_SINFO_BLEN];
	list->length = list_info_uint[LIST_SINFO_LENGTH];
	list->record = list_info_uint[LIST_SINFO_RECORD];
	list->pidc = list_info_uint[LIST_SINFO_PIDC];
	list->eidc = list_info_uint[LIST_SINFO_EIDC];
	return 0;
}

int link_list_heap(LIST * list, char *str)
{
	int i;
	str += LIST_SINFO_BUFFER;
	memcpy(list->head, str, LIST_HEAD_LEN);
	str += LIST_HEAD_LEN;
	list->mem_pool = str;
	str += list->length;
	if (list->aux_blk) {
		i = list->aux_blk * list->record;
		list->aux = str;
	}
	if (!list->mode_flag & LIST_pool_blklist_mode) {
		str += i;
		i = list->record * sizeof(int);
		list->map = (unsigned int *)str;
	}
	if (list->mode_flag & LIST_Hash_table_DI32) {
		str += i;
		list->hash_table = str;
	}
	return 0;
}

char *create_list(char *name, unsigned int blen, unsigned int aux_blk,
		  unsigned int record)
{
	LIST list;
	char *pool;
	list = LIST_new_list_blklist(blen, aux_blk, record);
	if (list.error_check)
		return NULL;
	pool = (char *)malloc(calc_list_swap_length(&list));
	if (pool == NULL)
		return NULL;
	LIST_set_head(&list, name);
	upload_list_heap(&list, pool);
	upload_list_info(&list, pool);
	LIST_delete(list);
	return pool;
}

int list_write_binary(char *slist, unsigned int id, char *str, unsigned int len)
{
	LIST trans;
	sync_list_info(&trans, slist);
	link_list_heap(&trans, slist);
	memcpy(LIST_get_mem_record(&trans, id), str, len);
	return 0;
}

int list_write_string(char *slist, unsigned int id, char *str)
{
	LIST trans;
	sync_list_info(&trans, slist);
	link_list_heap(&trans, slist);
	strcpy(LIST_get_mem_record(&trans, id), str);
	return 0;
}

void list_delete_record(char *slist, unsigned int id)
{
	LIST trans;
	sync_list_info(&trans, slist);
	link_list_heap(&trans, slist);
	remove_LIST_RECORD(&trans, id);
}

char *list_get_record(char *slist, unsigned int id)
{
	LIST trans;
	sync_list_info(&trans, slist);
	link_list_heap(&trans, slist);
	return LIST_get_mem_record(&trans, id);
}

char *list_get_aux(char *slist, unsigned int id)
{
	LIST trans;
	sync_list_info(&trans, slist);
	link_list_heap(&trans, slist);
	return LIST_get_aux(&trans, id);
}
