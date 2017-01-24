/*
Using these functions below to replicate a list to a string .
    int upload_list_info(LIST *list,char *str);
    int upload_list_heap(LIST *list,char *str);
Function " sync_list_info " to get list info from swap string
Function " link_list_heap " to link another list to the swap string
If swap list to a string in shard-mem , list will be global for operation system .

Note:
    Lbrdb in swap string can not be free by " LIST_delete " .
    After you swap a list you may free the list to saving mem .
*/

#ifndef _TINYLIST_
#include "tinylist.h"
#endif
#ifndef LIST_HEAD_LEN
#define LIST_HEAD_LEN  128
#endif
#define LIST_SINFO_FLAG     0

#define LIST_SINFO_MODE_F   0
#define LIST_SINFO_ERRCHK   1

#define LIST_SINFO_RFBLOCK  1
#define LIST_SINFO_BLEN     2
#define LIST_SINFO_LENGTH   3
#define LIST_SINFO_RECORD   4
#define LIST_SINFO_PIDC     5
#define LIST_SINFO_EIDC     6

#define LIST_SINFO_BUFFER   32

/*
string allow binary data
string_length >= calc_list_swap_length(&list)
string:
|uchar mode_flag|uchar error_check|uchar NO-USE|uchar NO-USE    ---> 4 byte
|uint aux_blk|uint blen|uint length|uint record|uint pidc|uint eide
|uint  NO-USE            ----> 4*7 = 28 byte
|head|mem_pool|aux|map|hash_table
*/

unsigned int calc_list_swap_length(LIST * list);
int upload_list_info(LIST * list, char *str);
int upload_list_heap(LIST * list, char *str);
int sync_list_info(LIST * list, char *str);
int link_list_heap(LIST * list, char *str);
char *create_list(char *name, unsigned int blen, unsigned int aux_blk,
		  unsigned int record);
int list_write_binary(char *slist, unsigned int id, char *str,
		      unsigned int len);
int list_write_string(char *slist, unsigned int id, char *str);
void list_delete_record(char *slist, unsigned int id);
char *list_get_record(char *slist, unsigned int id);
char *list_get_aux(char *slist, unsigned int id);
