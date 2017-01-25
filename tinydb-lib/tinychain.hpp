/*
This is a simple datachain based on list_model.

Class a datachain object and using TINYCHAIN::init(unsigned int blen,unsigned int blocks,char *name) to initial.
blen to set the length of a block in list
blocks to set the number of blocks in list
name to set the name of list

Refer to the structure LIST , using LIST::memory to store data and LIST::reflection for pointers.
For the class datachain LIST will initialed as block mode

ID  memory  reflection
0   block0  pointer0
1   block1  pointer1
....
N   blockN  pointerN

At default mode pointer takes 4 byte , it always be switched as an unsigned integer and store the id pointer in it.
Pointer's value is the ID of the next record(block) of the chain.
If pointer's value is 0 , it means the record(block) of the pointer is not in used.
If pointer's value is the same as the record's(block's) ID ,it means this record(block) it the end record(block) of the chain.
So , the first record(block) of a chain is always kept , case it's ID is 0 .

*/
#define _TINYCHAIN_

#define TINYCHAIN_DEFAULT_POINTER  4

#define TINYCHAIN_START_ID         1

#define TINYCHAIN_INFO_LEN			64

#define TINYCHAIN_OVER

#define TINYCHAIN_ERR_
#define TINYCHAIN_ERR_OVERFLOW     -01
#define TINYCHAIN_ERR_ID_IN_USE    -02

#define TINYCHAIN_NO_AVAILABLE_ID  -11

#define FILE_LOAD_ERR_
#define FILE_LOAD_ERR_OPENFAILED	-21
#define FILE_LOAD_ERR_NOSPACE		-22
#define FILE_LOAD_ERR_OVERLEN		-23

#define FILE_BUFFER_LEN   			1048576

extern "C" {
	#ifndef _TINYLIST_
	#include "tinylist.h"
	#endif
}

namespace tinychain {
	int write_data(LIST * list, unsigned int id, int length, char *str);
	int write_data(LIST * list, unsigned int id, char *str);
	int read_data(LIST * list, unsigned int id, char *buffer);
	int output_string(LIST * list, unsigned int id);
	int delete_data(LIST * list, unsigned int id);
	int find_available_id(LIST * list);

	typedef class {
 public:
		LIST list, reflect;
		int init(unsigned int sblen, unsigned int blocks, char *name);
		int write(unsigned int id, int length, char *str);
		int write(unsigned int id, char *str);
		int read(unsigned int id, char *buffer);
		char *block(unsigned int id);
		char *getend(unsigned int id);
		int stroutput(void *fp, unsigned int id);
		int del(unsigned int id);
		int get_len(unsigned int id);
		int status(unsigned int id);
		int is_field_start(unsigned int id);
		int is_chain_end(unsigned int id);
		int load_text_file(unsigned int id, char *filename);
		int load_text_file(char *filename);
		int chain_export();
		int chain_import(char *filename);
		int info(unsigned int *para);
		int sync(unsigned int *para);
		int available_id();
		int calcstr(char *str);
		void cfree();
 private:
		unsigned int blen;
		unsigned int total;
		unsigned int in_use;
		unsigned int available;
	} TINYCHAIN;
}
