#ifdef __cplusplus
	extern "C" {
#endif
#include "tinylist.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef LINUX
    #include "sys/stat.h"
    #include "sys/types.h"
    #include "unistd.h"
#endif

#ifdef WIN_NT
    #include "conio.h"
    #include "direct.h"
    #include "windows.h"
#endif

//common//

#ifndef UNKNOWN_OPT
	#define str_copy strcpy
	#define str_length strlen
#endif

#ifdef WIN_NT
	#define _creat_directory mkdir
	#define _remove_directory rmdir
#endif

#ifdef LINUX
	#define _creat_directory(name) mkdir(name,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH)
	#define _remove_directory rmdir
	#define getch getchar
#endif

#ifdef UNKNOWN_OPT
	static int str_length(char *str)
	{
		int l = 0;
		while (*str++)
			l++;
		return l;
	}

	/*
	static int str_length(char *str){
		char *str0=str;
		while(*++str);
		return str-str0;
	}
	*/
	static void str_copy(char *str0, char *str1)
	{
		while ((*str0++ = *str1++) != '\0') ;
	}

	/*
	static inline void str_copyi(char *str0,char *str1){
		int *dest=str0,*src=str1;
		while(*src&0xff)*dest++=*src++;
	}
	*/
#endif

#define SWAP(x,y) {int t;t=x;x=y;y=t;}
#define mem_copy memcpy

static void array_init(char *str, int length)
{
	while (length--) {
		str[length] = 0;
	}
}

static int str_over_write(char *str, char *key, int point, int length)
{
	int i, l;
	if (length == 0)
		l = str_length(key);
	else
		l = length;
	for (i = 0; i < l; i++) {
		str[point + i] = key[i];
	}
	return l;
}

static int block_key_compare(char *key1, char *key2, int block)
{
	int i = 0;
	int ex_num = 0;
	for (block; block > 0; block--) {
		if (key1[i] != key2[i])
			ex_num++;
		i++;
	}
	return ex_num;
}

#ifndef _LOW_LEVEL_CPU
	static void int_to_char(int num, char *key)
	{
		key[0] = num;
		key[1] = num >> 8;
		key[2] = num >> 16;
		key[3] = num >> 24;
		//key[4]=0;
	}

	static int char_to_int(char *key)
	{
		return *(int *)key;
	}
#endif

#ifdef HAVE_FILESYSTEM
	static int load_file(char *file, char *filename)
	{
		FILE *fp;
		int length;
		fp = fopen(filename, "rb");
		if (fp == NULL)
			return 0;
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(file, 1, length, fp);
		fclose(fp);
		return length;
	}

	static unsigned int get_file_length(char *filename)
	{
		unsigned int length;
		FILE *fp;
		fp = fopen(filename, "rb");
		if (fp == NULL)
			return 0;
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		return length;
	}

	static char *_load_file(char *filename, char end)
	{
		FILE *fp;
		char *file;
		int length;
		fp = fopen(filename, "rb");
		if (fp == NULL)
			return NULL;
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		file = (char *)malloc(length);
		fseek(fp, 0, SEEK_SET);
		fread(file, 1, length, fp);
		if (end == 0)
			file[length] = 0;
		fclose(fp);
		return file;
	}
#endif

static char which_word_is_bigger(char *word0, char *word1)
{
	int i = 0;
	while (word0[i] == word1[i]) {
		i++;
		if (word0[i] == 0)
			return 2;	//word0==word1
	}
	if (word0[i] > word1[i])
		return 0;	//word0>word1
	else
		return 1;	//word0<word1
}

static char which_word_is_smaller(char *word0, char *word1)
{
	int i = 0;
	while (word0[i] == word1[i]) {
		i++;
		if (word0[i] == 0)
			return 2;	//word0==word1
	}
	if (word0[i] > word1[i])
		return 1;	//word0<word1
	else
		return 0;	//word0>word1
}

static char which_word_is_longer(char *word0, char *word1)
{
	int l0 = str_length(word0);
	int l1 = str_length(word1);
	if (l0 > l1)
		return 0;	//word0>word1
	else if (l0 == l1)
		return 2;	//word0==word1
	else
		return 1;	//word0<word1
}

static char which_word_is_shorter(char *word0, char *word1)
{
	int l0 = str_length(word0);
	int l1 = str_length(word1);
	if (l0 > l1)
		return 1;	//word0<word1
	else if (l0 == l1)
		return 2;	//word0==word1
	else
		return 0;	//word0>word1
}

static unsigned int DJBHashKey(char *key, unsigned int seed)
{
	unsigned int nHash = seed;
	while (*key)
		nHash = (nHash << 5) + nHash + *key++;
	return nHash;
}

static unsigned int DJBHashKey_bin(char *str, unsigned int seed, int length)
{
	unsigned int hash = seed;
	int i;
	for (i = 0; i < length; i++) {
		hash = (hash << 5) + hash + str[i];
	}
	return hash;
}

typedef struct {
	int point;
	char *str;
} HEAP;

static void heap_free(HEAP heap)
{
	free(heap.str);
}

static void heap_reset(HEAP *heap)
{
	heap->point = 0;
	heap->str[0] = 0;
}

#ifdef HAVE_FILESYSTEM
	static void heap_output_file(HEAP *heap, char *filename)
	{
		int length;
		FILE *fp;
		length = str_length(heap->str);
		fp = fopen(filename, "w+");
		fwrite(heap->str, length, 1, fp);
		fclose(fp);
	}

	static void heap_output_file_bin(HEAP *heap, char *filename)
	{
		int length;
		FILE *fp;
		length = heap->point;
		fp = fopen(filename, "wb+");
		fwrite(heap->str, length, 1, fp);
		fclose(fp);
	}

	#ifdef UNIX_FILE
		static unsigned char file_to_heap(HEAP *heap, char *filename)
		{
			heap->point = load_file(heap->str, filename);
			return 0;
		}
	#endif
#endif

static HEAP build_heap(int length)
{
	HEAP heap;
	heap.point = 0;
	heap.str = (char *)malloc(length);
	heap.str[0] = 0;
	return heap;
}

static unsigned char heap_line_store(char *str, HEAP *heap)
{
	int i, l;
	l = str_length(str);
	for (i = 0; i < l; i++) {
		heap->str[heap->point + i] = str[i];
	}
	heap->str[heap->point + i] = 0;
	heap->point += l;
	return 0;
}

//list//

void LIST_set_aux_blk(LIST *list, unsigned int aux_blk)
{
	list->aux_blk = aux_blk;
}

void LIST_set_head(LIST *list, char *str)
{
	str_copy(list->head, str);
}

LIST LIST_new_list(unsigned int length, unsigned int nrecord,
		   unsigned int aux_blk)
{
	LIST list;
	list.error_check = 0;
	list.aux_blk = aux_blk;
	list.mem_pool = (char *)malloc(length);
	if (list.mem_pool == NULL) {
		list.error_check |= LIST_INIT_FAILED;
		return list;
	}
	list.length = length;
	list.record = nrecord;
	list.aux = (char *)malloc(list.aux_blk * nrecord);
	if (list.aux == NULL) {
		free(list.mem_pool);
		list.error_check |= LIST_INIT_FAILED;
		return list;
	}
	list.map = (unsigned int *)malloc(nrecord * sizeof(int));
	if (list.map == NULL) {
		free(list.mem_pool);
		free(list.aux);
		list.error_check |= LIST_INIT_FAILED;
		return list;
	}
	array_init(list.mem_pool, list.length);
	array_init(list.aux, list.aux_blk * nrecord);
	array_init((char *)list.map, nrecord * sizeof(int));
	list.pidc = 0;
	list.eidc = 0;
	list.mode_flag = 0;
	list.error_check = 0;
	return list;
}

LIST LIST_new_list_compact(unsigned int length, unsigned int aux_blk,
				 unsigned int nrecord)
{
	LIST list;
	list = LIST_new_list(length, nrecord, aux_blk);
	return list;
}

LIST LIST_new_list_blklist(unsigned int blen, unsigned int aux_blk,
				 unsigned int nrecord)
{
	LIST list;
	unsigned int length = blen * nrecord;
	list = LIST_new_list(length, nrecord, aux_blk);
	if (list.error_check != LIST_NO_ERROR)
		return list;
	list.mode_flag |= LIST_pool_blklist_mode;
	list.blen = blen;
	free(list.map);
	return list;
}

void LIST_delete(LIST list)
{
	list.head[0] = '\0';
	free(list.mem_pool);
	if (list.aux)
		free(list.aux);
	if (!(list.mode_flag & LIST_pool_blklist_mode))
		free(list.map);
	if (list.mode_flag & LIST_Hash_table_DI32)
		free(list.hash_table);
}

LIST LIST_clone(LIST *list)
{
	LIST clist;
	clist.aux_blk = list->aux_blk;
	clist.mem_pool = (char *)malloc(list->length);
	if (clist.mem_pool == NULL) {
		clist.error_check |= LIST_INIT_FAILED;
		return clist;
	}
	mem_copy(clist.mem_pool, list->mem_pool, list->length);
	clist.length = list->length;
	clist.record = list->record;
	clist.aux = (char *)malloc(list->aux_blk * list->record);
	if (clist.aux == NULL) {
		free(clist.mem_pool);
		clist.error_check |= LIST_INIT_FAILED;
		return clist;
	}
	mem_copy(clist.aux, list->aux,
		 list->aux_blk * list->record);
	if (!(list->mode_flag & LIST_pool_blklist_mode)) {
		clist.map =
		    (unsigned int *)malloc(clist.record * sizeof(int));
		if (clist.map == NULL) {
			free(clist.mem_pool);
			free(clist.aux);
			clist.error_check |= LIST_INIT_FAILED;
			return clist;
		}
		mem_copy((char *)clist.map, (char *)list->map,
			 clist.record * sizeof(int));
	}
	clist.pidc = list->pidc;
	clist.eidc = list->eidc;
	clist.blen = list->blen;
	clist.mode_flag = list->mode_flag;
	clist.error_check = list->error_check;
	str_copy(clist.head, list->head);
	if (list->mode_flag & LIST_Hash_table_DI32) {
		clist.hash_table = (char *)malloc(clist.record * 4);
		if (clist.hash_table == NULL) {
			free(clist.mem_pool);
			free(clist.aux);
			if (!(list->mode_flag & LIST_pool_blklist_mode))
				free(clist.map);
			clist.error_check |= LIST_INIT_FAILED;
			return clist;
		}
		mem_copy(clist.hash_table, list->hash_table,
			 clist.record * 4);
	}
	return clist;
}

#ifdef HAVE_FILESYSTEM
	unsigned char LIST_import(LIST *list, char *filename)
	{
		HEAP heap;
		chdir(filename);
		str_copy(list->head, filename);
		heap = build_heap(LIST_INFO_BUFFER);
		file_to_heap(&heap, "info");
		sscanf(heap.str, "\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d", &list->aux_blk,
			   &list->blen, &list->length, &list->record, &list->pidc,
			   &list->eidc, &list->mode_flag, &list->error_check);
		list->mem_pool = (char *)malloc(list->length);
		load_file(list->mem_pool, "mem_pool");
		list->aux = (char *)malloc(list->aux_blk * list->record);
		load_file(list->aux, "aux");
		if (!(list->mode_flag & LIST_pool_blklist_mode)) {
			list->map =
				(unsigned int *)malloc(list->record * sizeof(int));
			load_file((char *)list->map, "map");
		}
		if (list->mode_flag & LIST_Hash_table_DI32) {
			list->hash_table = (char *)malloc(list->record * 4);
			load_file(list->hash_table, "di32");
		}
		chdir("..");
		heap_free(heap);
		return list->error_check;
	}

	void LIST_export(LIST list)
	{
		HEAP heap, temp;
		_creat_directory(list.head);
		chdir(list.head);
		temp = build_heap(LIST_INFO_BUFFER);
		heap.str = list.mem_pool;
		heap.point = list.length;
		heap_output_file_bin(&heap, "mem_pool");
		heap.str = list.aux;
		heap.point = list.record * list.aux_blk;
		heap_output_file_bin(&heap, "aux");
		if (!(list.mode_flag & LIST_pool_blklist_mode)) {
			heap.str = (char *)list.map;
			heap.point = list.record * sizeof(int);
			heap_output_file_bin(&heap, "map");
		}
		if (list.mode_flag & LIST_Hash_table_DI32) {
			heap.str = (char *)list.hash_table;
			heap.point = list.record * 4;
			heap_output_file_bin(&heap, "di32");
		}
		heap_reset(&temp);
		sprintf(temp.str, "\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d", list.aux_blk,
			list.blen, list.length, list.record, list.pidc, list.eidc,
			list.mode_flag, list.error_check);
		temp.point = str_length(temp.str);
		heap_output_file_bin(&temp, "info");
		chdir("..");
		heap_free(temp);
	}
#endif

//record//

LIST_RECORD new_LIST_RECORD(unsigned int length, unsigned int aux_blk)
{
	LIST_RECORD record;
	record.record = (char *)malloc(length);
	record.aux = (char *)malloc(aux_blk);
	array_init(record.record, length);
	array_init(record.aux, aux_blk);
	record.rrhl[0] = length;
	record.rrhl[1] = aux_blk;
	record.rrhl[2] = 0;
	return record;
}

void delete_LIST_RECORD(LIST_RECORD *record)
{
	free(record->record);
	if (record->rrhl[1] != 0)
		free(record->aux);
	if (record->rrhl[2] != 0)
		free(record->hash);
}

void item_to_LIST_RECORD(LIST_RECORD *record, char *item,
			  unsigned int length)
{
	length -= record->rrhl[1];
	str_over_write(record->record, item, 0, length);
	str_over_write(record->aux, item + length, 0, record->rrhl[1]);
}

LIST_RECORD get_LIST_RECORD(LIST *list, unsigned int eidc)
{
	LIST_RECORD record;
	unsigned int length;
	if (list->mode_flag & LIST_pool_blklist_mode) {
		length = list->blen;
	} else {
		length = list->map[eidc + 1];
		if (length == 0)
			length = list->pidc;
		length -= list->map[eidc];
	}
	record = new_LIST_RECORD(length, list->aux_blk);
	LIST_copy_mem_record_bin(list, eidc, record.record, length);
	LIST_copy_aux(list, eidc, record.aux);
	if (list->mode_flag & LIST_Hash_table_DI32) {
		record.hash = (char *)malloc(4);
		*(unsigned int *)record.hash = LIST_get_hash_di32(list, eidc);
		record.rrhl[2] = 4;
	}
	return record;
}

void write_LIST_RECORD(LIST *list, LIST_RECORD *record, unsigned int eidc)
{
	unsigned int linker;
	if (list->mode_flag & LIST_pool_blklist_mode)
		linker = list->blen * eidc;
	else
		linker = list->map[eidc];
	str_over_write(list->mem_pool, record->record, linker,
		       record->rrhl[0]);
	str_over_write(list->aux, record->aux,
		       eidc * list->aux_blk, list->aux_blk);
	if (list->mode_flag & LIST_Hash_table_DI32) {
		str_over_write(list->hash_table, record->hash, eidc * 4, 4);
	}
}

unsigned char add_LIST_RECORD_auto(LIST *list, LIST_RECORD *record)
{
	if (list->eidc + 1 > list->record) {
		list->error_check |= LIST_REFLECTION_FULL;
		return list->error_check;
	}
	write_LIST_RECORD(list, record, list->eidc);
	list->eidc++;
	return list->error_check;
}

unsigned char LIST_swap_blklist_record(LIST *list, unsigned int eidc0,
					unsigned int eidc1)
{
	LIST_RECORD r0;
	if (!(list->mode_flag & LIST_pool_blklist_mode))
		return -1;
	//r0=new_LIST_RECORD(list->blen,list->aux_blk);
	r0 = get_LIST_RECORD(list, eidc0);
	str_over_write(list->mem_pool,
		       list->mem_pool + list->blen * eidc1,
		       list->blen * eidc0, list->blen);
	str_over_write(list->aux,
		       list->aux + eidc1 * list->aux_blk,
		       eidc0 * list->aux_blk, list->aux_blk);
	if (list->mode_flag & LIST_Hash_table_DI32) {
		str_over_write(list->hash_table, list->hash_table + eidc1 * 4,
			       eidc0 * 4, 4);
	}
	write_LIST_RECORD(list, &r0, eidc1);
	delete_LIST_RECORD(&r0);
	return 0;
}

char remove_LIST_RECORD(LIST *list, unsigned int eidc)
{
	if (!(list->mode_flag & LIST_pool_blklist_mode))
		return -1;
	array_init(LIST_get_mem_record(list, eidc), list->blen);
	array_init(LIST_get_aux(list, eidc), list->aux_blk);
	if (list->mode_flag & LIST_Hash_table_DI32)
		array_init(list->hash_table + eidc * 4, 4);
	return 0;
}

char *LIST_get_mem_record(LIST *list, unsigned int eidc)
{
	char *linker;
	if (list->mode_flag & LIST_pool_blklist_mode)
		linker = list->mem_pool + list->blen * eidc;
	else
		linker = list->mem_pool + list->map[eidc];
	return linker;
}

char *LIST_get_aux(LIST *list, unsigned int eidc)
{
	char *linker;
	linker = list->aux + list->aux_blk * eidc;
	return linker;
}

void LIST_copy_aux(LIST *list, unsigned int eidc, char *str)
{
	char *linker;
	linker = list->aux + list->aux_blk * eidc;
	str_over_write(str, linker, 0, list->aux_blk);
}

void LIST_copy_mem_record(LIST *list, unsigned int eidc, char *str)
{
	char *linker;
	if (list->mode_flag & LIST_pool_blklist_mode)
		linker = list->mem_pool + list->blen * eidc;
	else
		linker = list->mem_pool + list->map[eidc];
	str_copy(str, linker);
}

void LIST_copy_mem_record_bin(LIST *list, unsigned int eidc, char *str,
			       unsigned int length)
{
	char *linker;
	if (list->mode_flag & LIST_pool_blklist_mode)
		linker = list->mem_pool + list->blen * eidc;
	else
		linker = list->mem_pool + list->map[eidc];
	str_over_write(str, linker, 0, length);
}

unsigned char LIST_write_mem(LIST *list, unsigned int eidc, char *str,
			      unsigned int length)
{
	str_over_write(LIST_get_mem_record(list, eidc), str, 0, length);
	return list->error_check;
}

unsigned int LIST_search_by_aux(LIST *list, char *key)
{
	char e = 1;
	unsigned int i;
	for (i = 0; i < list->eidc; i++) {
		if (!block_key_compare
		    (list->aux + i * list->aux_blk, key,
		     list->aux_blk)) {
			e = 0;
			break;
		}
	}
	if (e)
		i = -1;
	return i;
}

//hash di32//

static unsigned int LIST_hash_seed_di32 = default_seed_di32;

void LIST_set_hash_seed_di32(unsigned int seed)
{
	LIST_hash_seed_di32 = seed;
}

unsigned char LIST_hash_table_init(LIST *list, unsigned int length)
{
	list->hash_table = (char *)malloc(length);
	if (list->hash_table == NULL) {
		list->error_check |= LIST_INIT_FAILED;
		return list->error_check;
	}
	array_init(list->hash_table, length);
	list->mode_flag |= LIST_Hash_table_DI32;
	return list->error_check;
}

unsigned char LIST_creat_hash_table_di32(LIST *list)
{
	return LIST_hash_table_init(list, list->record * 4);
}

unsigned int LIST_record_create_hash_di32_bin(LIST *list, unsigned int eidc,
					       unsigned int length)
{
	unsigned int *hash_table = (unsigned int *)list->hash_table;
	hash_table[eidc] =
	    DJBHashKey_bin(LIST_get_mem_record(list, eidc),
			   LIST_hash_seed_di32, length);
	return hash_table[eidc];
}

unsigned int LIST_record_create_hash_di32_str(LIST *list, unsigned int eidc)
{
	unsigned int *hash_table = (unsigned int *)list->hash_table;
	hash_table[eidc] =
	    DJBHashKey_bin(LIST_get_mem_record(list, eidc),
			   LIST_hash_seed_di32,
			   str_length(LIST_get_mem_record(list, eidc)));
	return hash_table[eidc];
}

unsigned int LIST_record_auto_create_hash_di32(LIST *list,
						unsigned int eidc)
{
	unsigned int *hash_table = (unsigned int *)list->hash_table;
	int hm;
	if (list->mode_flag & LIST_pool_blklist_mode) {
		hash_table[eidc] =
		    DJBHashKey_bin(LIST_get_mem_record(list, eidc),
				   LIST_hash_seed_di32, list->blen);
	} else {
		hm = list->map[eidc + 1];
		if (hm == 0)
			hm = list->pidc;
		hash_table[eidc] =
		    DJBHashKey_bin(LIST_get_mem_record(list, eidc),
				   LIST_hash_seed_di32, hm - list->map[eidc]);
	}
	return hash_table[eidc];
}

unsigned char LIST_auto_create_hash_di32(LIST *list)
{
	int i;
	for (i = 0; i <= list->eidc; i++)
		LIST_record_auto_create_hash_di32(list, i);
	return list->error_check;
}

unsigned char LIST_create_hash_di32_str(LIST *list)
{
	int i;
	for (i = 0; i <= list->eidc; i++)
		LIST_record_create_hash_di32_str(list, i);
	return list->error_check;
}

unsigned char LIST_create_hash_di32_bin(LIST *list, unsigned int length)
{
	int i;
	for (i = 0; i <= list->eidc; i++)
		LIST_record_create_hash_di32_bin(list, i, length);
	return list->error_check;
}

unsigned int LIST_get_hash_di32(LIST *list, unsigned int eidc)
{
	int *hash;
	hash = (int *)list->hash_table;
	return hash[eidc];
}

unsigned int LIST_search_hast_table_di32(LIST *list, unsigned int nhash,
					  unsigned int point)
{
	int *pool = (int *)list->hash_table;
	int eidc = 0;
	pool += point;
	eidc += point;
	while (*pool++ != nhash) {
		eidc++;
		if (eidc > list->eidc)
			return -1;
	}
	return eidc;
}

unsigned int LIST_search_str_di32(LIST *list, char *str, unsigned int point)
{
	return LIST_search_hast_table_di32(list,
					    DJBHashKey(str,
						       LIST_hash_seed_di32),
					    point);
}

unsigned int LIST_search_bin_di32(LIST *list, char *str, unsigned int point,
				   unsigned int length)
{
	return LIST_search_hast_table_di32(list,
					    DJBHashKey_bin(str,
							   LIST_hash_seed_di32,
							   length), point);
}

//quick hash//

static unsigned int qhash_seed = LIST_QHASH_SEED;
static unsigned int max_seed = LIST_qhash_seed_max;

static unsigned char LIST_QHASH_ERROR_FLAG = 0;

void LIST_qhash_error_log(unsigned char error)
{
	LIST_QHASH_ERROR_FLAG = error;
}

unsigned char LIST_qhash_get_error()
{
	return LIST_QHASH_ERROR_FLAG;
}

unsigned int LIST_qhash_get_seed()
{
	return qhash_seed;
}

unsigned int LIST_qhash_get_max_seed()
{
	return max_seed;
}

void LIST_qhash_set_seed(unsigned int seed)
{
	qhash_seed = seed;
}

void LIST_qhash_set_max_seed(unsigned int seed)
{
	max_seed = seed;
}

int LIST_qhash_check_crash(char *str)
{
	return (int)*str;
}

int LIST_qhash_calc_offset(unsigned int max)
{
	int i = 32;
	while (max) {
		max = max >> 1;
		i--;
	}
	return i;
}

unsigned int LIST_qhash(char *str, unsigned int seed, unsigned int length,
			 unsigned int max, unsigned char offset)
{
	unsigned int nhash;
 HASH_CALC:
	nhash = DJBHashKey_bin(str, seed, length);
	nhash = nhash << offset;
	nhash = nhash >> offset;
	if (!max)
		return nhash;
	else if (nhash > max) {
		seed++;
		goto HASH_CALC;
	}
	return nhash;
}

unsigned int LIST_qhash_test(LIST *list, char *str, unsigned int length)
{
	unsigned int qhash, seed;
	int offset = LIST_qhash_calc_offset(list->record);
	seed = qhash_seed;
 HASH_CALC:
	qhash = LIST_qhash(str, seed, length, list->record, offset);
	if (LIST_qhash_check_crash(LIST_get_mem_record(list, qhash))) {
		seed++;
		if (seed > max_seed)
			return -1;
		goto HASH_CALC;
	}
	return qhash;
}

unsigned int LIST_qhash_search_str(LIST *list, char *str)
{
	return LIST_qhash_search_bin(list, str, str_length(str));
}

unsigned int LIST_qhash_search_bin(LIST *list, char *str,
				    unsigned int length)
{
	int qhash, seed;
	int offset = LIST_qhash_calc_offset(list->record);
	seed = qhash_seed;
 HASH_CALC:
	qhash = LIST_qhash(str, seed, length, list->record, offset);
	if (!LIST_qhash_check_crash(LIST_get_mem_record(list, qhash)))
		return -1;
	if (which_word_is_bigger(str, LIST_get_mem_record(list, qhash)) != 2) {
		seed++;
		if (seed > LIST_qhash_seed_max)
			return -1;
		goto HASH_CALC;
	}
	return qhash;
}

//sort//

static unsigned char LIST_SORT_MODE = LIST_SORT_MODE_DEFAULT;

void LIST_set_sort_mode(char mode)
{
	LIST_SORT_MODE = mode;
}

char LIST_word_compare(char *word0, char *word1)
{
	switch (LIST_SORT_MODE) {
	case LIST_SORT_BY_ASCII_ASC:
		return which_word_is_bigger(word0, word1);
	case LIST_SORT_BY_ASCII_DESC:
		return which_word_is_smaller(word0, word1);
	case LIST_SORT_BY_LENGTH_ASC:
		return which_word_is_longer(word0, word1);
	case LIST_SORT_BY_LENGTH_DESC:
		return which_word_is_shorter(word0, word1);
	default:
		return -1;
	}
}

void LIST_auto_sort(LIST *list)
{
	int i, j, k, gap;
	gap = list->eidc / 2;
	while (gap > 0) {
		for (k = 0; k < gap; k++) {
			for (i = k + gap; i < list->eidc; i += gap) {
				for (j = i - gap; j >= k; j -= gap) {
					if (LIST_word_compare
					    (LIST_get_mem_record(list, j),
					     LIST_get_mem_record(list,
								  j + gap)) ==
					    0) {
						LIST_swap_blklist_record(list,
									  j,
									  j +
									  gap);
					} else
						break;
				}
			}
		}
		gap /= 2;
	}
}

//debug//

void LIST_set_mode_flag(LIST *list, unsigned char mode_flag)
{
	list->mode_flag |= mode_flag;
}

#ifdef HAVE_FILESYSTEM
	void LIST_output_error_log(LIST *list)
	{
		HEAP heap = build_heap(2048);
		char str[256];
		sprintf(str,
			"LIST INFO:HEAD:%s\nLENGTH:	%d	RECORD:	%d	AUX_BLOCK:	%d\n",
			list->head, list->length, list->record, list->aux_blk);
		heap_line_store(str, &heap);
		sprintf(str, "PIDC:	%d	EIDC:	%d	MODE_FLAG: %x\n",
			list->pidc, list->eidc, list->mode_flag);
		heap_line_store(str, &heap);
		heap_line_store("ERROR_CHECK:", &heap);
		sprintf(str, "%d", list->error_check);
		heap_line_store(str, &heap);
		heap_line_store("\nERROR_QHASH:", &heap);
		sprintf(str, "%d", LIST_qhash_get_error());
		heap_line_store(str, &heap);
		heap_output_file(&heap, "list_error.log");
		heap_free(heap);
	}
#endif

void LIST_print_debug_info(LIST list)
{
	int i, hm;
	char key[LIST_default_item_length];
	char aux[LIST_default_aux_length];
	int *hash_table = (int *)list.hash_table;
	printf
	    ("LIST info:HEAD_>%sEOS\nLENGTH:	%d	RECORD:	%d	AUX_BLOCK:	%d\n",
	     list.head, list.length, list.record, list.aux_blk);
	printf("PIDC:	%d	EIDC:	%d	MODE_FLAG: %x\n", list.pidc,
	       list.eidc, list.mode_flag);
	for (i = 0; i < list.eidc; i++) {
		if (list.mode_flag & LIST_pool_blklist_mode) {
			hm = list.blen;
		} else {
			hm = list.map[i + 1];
			if (hm == 0)
				hm = list.pidc;
			hm -= list.map[i];
		}
		array_init(aux, 10);
		array_init(key, 100);
		LIST_copy_aux(&list, i, aux);
		LIST_copy_mem_record_bin(&list, i, key, hm);
		if (list.mode_flag & LIST_Hash_table_DI32) {
			printf("ID:%d	OUTPUT_>%s	AUX:%s	HASH:%d\n", i,
			       key, aux, hash_table[i]);
		} else {
			if (list.aux_blk == 0)
				aux[0] = 0;
			printf("ID:%d	OUTPUT_>%s	AUX:%s\n", i, key,
			       aux);
		}
	}
}
#ifdef __cplusplus
	}
#endif