extern "C" {
	#include "string.h"
	#include "stdlib.h"
	#include "tinylist.h"
	#include "stdio.h"
}
#include "tinychain.hpp"
namespace tinychain {
	int write_data(LIST * list, unsigned int id, int length, char *str)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list->aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		if (!length)
			 return 0;
		if (id_pointer[id])
			 return TINYCHAIN_ERR_ID_IN_USE;
		unsigned int counter = list->blen;
		unsigned int block_counter = 1;
		unsigned int bid;
		char *pointer = LIST_get_mem_record(list, id);
		while (length--) {
			*pointer++ = *str++;
			counter--;
			if (!counter && length) {
				if (id == list->record - 1) {
					id_pointer[id] = id;
					return TINYCHAIN_ERR_OVERFLOW;
				}
				bid = id;
				while (id_pointer[++id]) ;
				id_pointer[bid] = id;
				pointer = LIST_get_mem_record(list, id);
				counter = list->blen;
				block_counter++;
			}
		}
		id_pointer[id] = id;
		return block_counter;
	}
	
	int write_data(LIST * list, unsigned int id, char *str)
	{
		return write_data(list, id, strlen(str) + 1, str);
	}
	
	int read_data(LIST * list, unsigned int id, char *buffer)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list->aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		if (!id_pointer[id])
			return 0;
		int counter = 0;
		unsigned int bid;
 loop:
		bid = id;
		memcpy(buffer, LIST_get_mem_record(list, id), list->blen);
		buffer += list->blen;
		counter++;
		if (id_pointer[bid] != id) {
			id = id_pointer[bid];
			goto loop;
		}
		return counter;
	}
	
	int output_string(LIST * list, void *fp, unsigned int id)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list->aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		char *str = LIST_get_mem_record(list, id);
		char ch = str[list->blen];
		str[list->blen] = 0;
		fprintf((FILE *) fp, "%s", str);
		str[list->blen] = ch;
		if (id_pointer[id] != id) {
			output_string(list, fp, id_pointer[id]);
		}
		return 0;
	}
	
	int delete_data(LIST * list, unsigned int id)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list->aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		if (!id_pointer[id])
			return 0;
		int counter = 0;
		unsigned int bid;
 loop:
		bid = id;
		memset(LIST_get_mem_record(list, id), 0, list->blen);
		counter++;
		if (id_pointer[bid] != id) {
			id = id_pointer[bid];
			id_pointer[bid] = 0;
			goto loop;
		}
		id_pointer[id] = 0;
		return counter;
	}
	/*
	   int delete_data_nonreport(LIST *list,unsigned int id)
	   {
	   #if TINYCHAIN_DEFAULT_POINTER == 4
	   unsigned int *id_pointer = (unsigned int*)list->aux;
	   #endif // TINYCHAIN_DEFAULT_POINTER
	   memset(LIST_get_mem_record(list,id),0,list->blen);
	   if(id_pointer[id]!=id){
	   delete_data(list,id_pointer[id]);
	   }
	   id_pointer[id] = 0;
	   return 0;
	   }
	 */
	int find_available_id(LIST * list)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list->aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		int id = TINYCHAIN_START_ID;
		while (id_pointer[id++])
			if (id == list->record)
				return TINYCHAIN_NO_AVAILABLE_ID;
		return id - 1;
	}

	int TINYCHAIN::init(unsigned int sblen, unsigned int blocks,
			     char *name)
	{
		list =
		    LIST_new_list_blklist(sblen,
					       TINYCHAIN_DEFAULT_POINTER,
					       blocks);
		LIST_set_head(&list, name);
		blen = sblen;
		total = blocks;
		in_use = 0;
		available = blocks;
		return 0;
	}
	
	int TINYCHAIN::write(unsigned int id, int length, char *str)
	{
		int i = write_data(&list, id, length, str);
		if (i >= 0) {
			in_use += i;
			available -= i;
		}
		return i;
	}
	
	int TINYCHAIN::write(unsigned int id, char *str)
	{
		int i = write_data(&list, id, str);
		if (i >= 0) {
			in_use += i;
			available -= i;
		}
		return i;
	}
	
	int TINYCHAIN::read(unsigned int id, char *buffer)
	{
		return read_data(&list, id, buffer);
	}
	
	int TINYCHAIN::stroutput(void *fp, unsigned int id)
	{
		return output_string(&list, fp, id);
	}
	
	int TINYCHAIN::get_len(unsigned int id)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list.aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		if (!id_pointer[id])
			return 0;
		int counter = 0;
		unsigned int bid;
 loop:
		bid = id;
		counter++;
		if (id_pointer[bid] != id) {
			id = id_pointer[bid];
			goto loop;
		}
		return counter;
	}
	
	char *TINYCHAIN::getend(unsigned int id)
	{
#if TINYCHAIN_DEFAULT_POINTER == 4
		unsigned int *id_pointer = (unsigned int *)list.aux;
#endif				// TINYCHAIN_DEFAULT_POINTER
		if (!id_pointer[id])
			return 0;
		int counter = 0;
		unsigned int bid;
 loop:
		bid = id;
		counter++;
		if (id_pointer[bid] != id) {
			id = id_pointer[bid];
			goto loop;
		}
		return block(id - 1);
	}
	
	int TINYCHAIN::load_text_file(unsigned int id, char *filename)
	{
		FILE *fp;
		char *file;
		int length;
		fp = fopen(filename, "rb");
		if (fp == NULL)
			return FILE_LOAD_ERR_OPENFAILED;
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		if (FILE_BUFFER_LEN <= length)
			return FILE_LOAD_ERR_OVERLEN;
		if ((available * blen) <= length)
			return FILE_LOAD_ERR_NOSPACE;
		file = (char *)malloc(FILE_BUFFER_LEN);
		memset(file, 0, FILE_BUFFER_LEN);
		fseek(fp, 0, SEEK_SET);
		fread(file, 1, length, fp);
		fclose(fp);
		length = write(id, length, file);
		free(file);
		return length;
	}
	
	int TINYCHAIN::load_text_file(char *filename)
	{
		int id, status;
		id = find_available_id(&list);
		if (id == TINYCHAIN_NO_AVAILABLE_ID)
			return FILE_LOAD_ERR_NOSPACE;
		status = load_text_file(id, filename);
		if (status < 0)
			return status;
		return id;
	}
	
	int TINYCHAIN::del(unsigned int id)
	{
		int i = delete_data(&list, id);
		in_use -= i;
		available += i;
		return i;
	}
	
	char *TINYCHAIN::block(unsigned int id)
	{
		return LIST_get_mem_record(&list, id);
	}
	
	int TINYCHAIN::chain_export()
	{
		char buffer[TINYCHAIN_INFO_LEN];
		FILE *fp;
		LIST_export(list);
		sprintf(buffer, "%s.cinf", list.head);
		fp = fopen(buffer, "wb+");
		if (fp == NULL)
			return FILE_LOAD_ERR_OPENFAILED;
		sprintf(buffer, "%d\n%d\n%d\n%d\n", blen, total, in_use,
			available);
		fwrite(buffer, 1, strlen(buffer), fp);
		fclose(fp);
		return 0;
	}
	
	int TINYCHAIN::chain_import(char *filename)
	{
		char buffer[TINYCHAIN_INFO_LEN];
		int length;
		FILE *fp;
		sprintf(buffer, "%s.cinf", filename);
		fp = fopen(buffer, "r");
		if (fp == NULL)
			return FILE_LOAD_ERR_OPENFAILED;
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		fread(buffer, 1, length, fp);
		sscanf(buffer, "%d\n%d\n%d\n%d\n", &blen, &total, &in_use,
		       &available);
		fclose(fp);
		LIST_import(&list, filename);
		return 0;
	}

	int TINYCHAIN::status(unsigned int id)
	{
		unsigned int *id_pointer = (unsigned int *)list.aux;
		return id_pointer[id];
	}

	int TINYCHAIN::is_field_start(unsigned int id)
	{
		if (id == 0)
			return 0;
		unsigned int *id_pointer = (unsigned int *)list.aux;
		if (id != id_pointer[id - 1])
			return 0;
		return -1;
	}
	
	int TINYCHAIN::is_chain_end(unsigned int id)
	{
		unsigned int *id_pointer = (unsigned int *)list.aux;
		if (id == id_pointer[id])
			return 0;
		return -1;
	}
	
	int TINYCHAIN::available_id()
	{
		if (available == 0)
			return TINYCHAIN_NO_AVAILABLE_ID;
		return find_available_id(&list);
	}
	
	int TINYCHAIN::calcstr(char *str)
	{
		int blks, len;
		len = strlen(str);
		if(!len)
			return 0;
		blks = ++len;
		return (blks % blen)?(blks / blen) + 1 : blks / blen;
	}
	
	int TINYCHAIN::info(unsigned int *para)
	{
		*para++ = blen;
		*para++ = total;
		*para++ = in_use;
		*para = available;
		return 0;
	}
	
	int TINYCHAIN::sync(unsigned int *para)
	{
		blen = *para++;
		total = *para++;
		in_use = *para++;
		available = *para;
		return 0;
	}
	
	void TINYCHAIN::cfree()
	{
		LIST_delete(list);
		blen = 0;
		total = 0;
		in_use = 0;
		available = 0;
	}
}
