#define _TINYLIST_
//#define  _LOW_LEVEL_CPU
#define _32BIT_CPU
//#define _64BIT_CPU
#define IA32 4
#define ARM 4

#define PLATFORM_TYPE IA32
//#define PLATFORM_TYPE ARM
//#define WIN_NT
#define LINUX

#define HAVE_FILESYSTEM

#define UNIX_FILE

//#define UNKNOWN_OPT

#ifndef __cplusplus
	#define bool    _Bool
	#define true    1
	#define false   0
#endif

/*****************Mode_flag*****************/
/*
unsigned char mode_flag :	0 1 2 3 4 5 6 7
							0 0 0 0 0 0 0 0
-------------------------------------------
0:-system kept this bit
1:-hash_table di32
2:-hash_table qhash
3:-aux type
4:-system kept this bit
5:-system kept this bit
6:-system kept this bit
7:-mem_pool , mode compact or block
*/
/*******************************************/
#define LIST_NO_Hash_table		0x00
#define LIST_Hash_table_DI32	0x40
#define LIST_Hash_table_QHASH	0x20
#define LIST_pool_default_mode 0x00
#define LIST_pool_compact_mode 0x00
#define LIST_pool_blklist_mode 0x01
/****************error_check****************/
/*
unsigned char error_check :	0 1 2 3 4 5 6 7
							0 0 0 0 0 0 0 0
-------------------------------------------
0:-mem_pool flag
1:-aux flag
2:-init failed
3:-system kept this bit
4:-file_ops flag
5:-system kept this bit
6:-system kept this bit
7:-quick hash error
*/
/*******************************************/
#define LIST_NO_ERROR			0x00
#define LIST_MEM_POOL_FULL	0x80
#define LIST_REFLECTION_FULL	0x40
#define LIST_FILE_OPS_ERROR	0x08
#define LIST_QHASH_ERROR		0x01
#define LIST_INIT_FAILED		0x20


typedef struct {
	char *record;
	char *aux;
	char *hash;
	int rrhl[3];		//0:length of record 1:length of aux 2:length of hash
} LIST_RECORD;

typedef struct {
	char *mem_pool;	//mem_pool
	char head[128];		//head
	char *aux;	//heap for aux
	unsigned int aux_blk;	//length of aux
	unsigned int blen;	//length of block
	unsigned int length;	//length of mem_pool
	unsigned int record;	//max record number
	unsigned int *map;	//mapping the records
	unsigned int pidc;	//address counter
	unsigned int eidc;	//counter of records
	char *hash_table;	//hash table
	unsigned char mode_flag;	//Set the list mode
	unsigned char error_check;	//Error check flag
} LIST;

#define LIST_default_item_length		1024
#define LIST_default_aux_length	512
#define LIST_default_record			4096
#define LIST_min_item_length			5

#define LIST_clear_mode_flag(list) list->mode_flag=0;
void LIST_set_mode_flag(LIST *list, unsigned char mode_flag);
void LIST_output_error_log(LIST *list);
void LIST_print_debug_info(LIST list);

//common//

#define DJBHash_default_seed 0

//list//

#define LIST_INFO_BUFFER 512
void LIST_set_aux_blk(LIST *list, unsigned int aux_blk);

void LIST_set_head(LIST *list, char *str);
LIST LIST_new_list(unsigned int length, unsigned int nrecord,
		   unsigned int aux_blk);
LIST LIST_new_list_compact(unsigned int length, unsigned int aux_blk,
				 unsigned int nrecord);
LIST LIST_new_list_blklist(unsigned int blen, unsigned int aux_blk,
				 unsigned int nrecord);

LIST LIST_clone(LIST *list);
void LIST_delete(LIST list);

#ifdef HAVE_FILESYSTEM
	void LIST_export(LIST list);
	unsigned char LIST_import(LIST *list, char *filename);
#endif

//record//

LIST_RECORD new_LIST_RECORD(unsigned int length, unsigned int aux_blk);
LIST_RECORD get_LIST_RECORD(LIST *list, unsigned int eidc);
/*write LIST_RECORD to a compact list may cause error*/
void write_LIST_RECORD(LIST *list, LIST_RECORD *record,
			unsigned int eidc);
unsigned char add_LIST_RECORD_auto(LIST *list, LIST_RECORD *record);
unsigned char LIST_swap_blklist_record(LIST *list, unsigned int eidc0,
					unsigned int eidc1);
void delete_LIST_RECORD(LIST_RECORD *record);
char remove_LIST_RECORD(LIST *list, unsigned int eidc);	//remove record only for blklist
char *LIST_get_mem_record(LIST *list, unsigned int eidc);
char *LIST_get_aux(LIST *list, unsigned int eidc);
void LIST_copy_aux(LIST *list, unsigned int eidc, char *str);
void LIST_copy_mem_record(LIST *list, unsigned int eidc, char *str);
void LIST_copy_mem_record_bin(LIST *list, unsigned int eidc, char *str,
			       unsigned int length);
unsigned char LIST_write_mem(LIST *list, unsigned int eidc, char *str,
			      unsigned int length);
unsigned int LIST_search_by_aux(LIST *list, char *key);

//hash di32//

#define default_seed_di32 0
#define LIST_HASH_DI32   4

void LIST_set_hash_seed_di32(unsigned int seed);
unsigned char LIST_hash_table_init(LIST *list, unsigned int length);
unsigned char LIST_creat_hash_table_di32(LIST *list);
unsigned char LIST_auto_create_hash_di32(LIST *list);
unsigned char LIST_create_hash_di32_str(LIST *list);
unsigned char LIST_create_hash_di32_bin(LIST *list, unsigned int length);
unsigned int LIST_get_hash_di32(LIST *list, unsigned int eidc);
unsigned int LIST_record_create_hash_di32_bin(LIST *list, unsigned int eidc,
					       unsigned int length);
unsigned int LIST_record_create_hash_di32_str(LIST *list,
					       unsigned int eidc);
unsigned int LIST_record_auto_create_hash_di32(LIST *list,
						unsigned int eidc);
unsigned int LIST_search_hast_table_di32(LIST *list, unsigned int nhash,
					  unsigned int point);
unsigned int LIST_search_str_di32(LIST *list, char *str,
				   unsigned int point);
unsigned int LIST_search_bin_di32(LIST *list, char *str, unsigned int point,
				   unsigned int length);
				   
//quick hash//

#define LIST_QHASH_SEED		0
#define LIST_qhash_seed_max	254

#define LIST_QHASH_ERROR_NO_ERROR 		0
#define LIST_QHASH_ERROR_SEED_OVERFLOW 1

#ifdef HAVE_FILESYSTEM
	void LIST_qhash_error_log(unsigned char error);
#endif
unsigned char LIST_qhash_get_error();
unsigned int LIST_qhash_get_seed();
unsigned int LIST_qhash_get_max_seed();
void LIST_qhash_set_seed(unsigned int seed);
void LIST_qhash_set_max_seed(unsigned int seed);
int LIST_qhash_check_crash(char *str);
int LIST_qhash_calc_offset(unsigned int max);
unsigned int LIST_qhash(char *str, unsigned int seed, unsigned int length,
			 unsigned int max, unsigned char offset);
unsigned int LIST_qhash_test(LIST *list, char *str, unsigned int length);
unsigned int LIST_qhash_search_str(LIST *list, char *str);
unsigned int LIST_qhash_search_bin(LIST *list, char *str,
				    unsigned int length);

//sort //

/*Cation : Sorting is only for BLOCK MODE*/
#define LIST_SORT_MODE_DEFAULT		0
#define LIST_SORT_BY_ASCII			0
#define LIST_SORT_BY_ASCII_ASC		0
#define LIST_SORT_BY_ASCII_DESC	1
#define LIST_SORT_BY_LENGTH		2
#define LIST_SORT_BY_LENGTH_ASC	2
#define LIST_SORT_BY_LENGTH_DESC	3

/*Before sorting ,you must set sort mode first ,using LIST_set_sort_mode*/
void LIST_set_sort_mode(char mode);
/*LIST_set_sort_mode will set a mode flag to switch the work mode of LIST_word_compare*/
char LIST_word_compare(char *word0, char *word1);
/*This function will sorting your list at the mode set by LIST_set_sort_mode
This function using Shellsort algorithm ,you may using faster algorithm*/
void LIST_auto_sort(LIST *list);
