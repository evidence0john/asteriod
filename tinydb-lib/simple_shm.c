#ifdef __cplusplus
extern "C" {
#endif

#include"stdlib.h"
#ifdef _FCGI_
#include "fcgi_stdio.h"
#else
#include"stdio.h"
#endif
#include"string.h"

#include"simple_shm.h"


#ifdef LINUX

    static unsigned int DJBHashKey(char *key, unsigned int seed)
    {
        unsigned int nHash = seed;
        while (*key)
            nHash = (nHash << 5) + nHash + *key++;
        return nHash;
    }
    void *create_shm_linux(unsigned int LEN, unsigned int ID)
    {
        void *shm = NULL;
        int shmid;
        shmid = shmget((key_t)ID, LEN, _ACCESS_);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            return NULL;
        }
        shm = shmat(shmid, (void *)0, 0);
        if(shm == (void *) - 1)
        {
            fprintf(stderr, "shmat failed\n");
            return NULL;
        }
        return shm;
    }
    int free_shm_linux(unsigned int LEN, unsigned int ID)
    {
        int shmid;
        shmid = shmget((key_t)ID, LEN, _ACCESS_);
        if(shmid == -1)
        {
            fprintf(stderr, "shmget failed\n");
            return SHM_GETID_FAILED;
        }
        if(shmctl(shmid, IPC_RMID, 0) == -1)
        {
            fprintf(stderr, "shmctl(IPC_RMID) failed\n");
            return SHMCTL_IPC_RMID_FAILED;
        }
        return 0;
    }
#endif

#ifdef WIN_NT

    void *create_file_mapping_win_nt(unsigned int LEN, char *name)
    {
        HANDLE hMapFile;
        void *pBuf;
        hMapFile = CreateFileMapping(MAPPING_MODE, NULL, _ACCESS_PAGE_, 0, LEN, name);
        if (hMapFile == NULL)
        {
            fprintf(stderr, "Could not create file mapping object.\n");
            return NULL;
        }
        pBuf = (void *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, LEN);
        if (pBuf == NULL)
        {
            fprintf(stderr, "Could not map view of file .\n");
            CloseHandle(hMapFile);
            return NULL;
        }
        return pBuf;
    }
    int close_file_mapping_win_nt(unsigned int LEN, char *name)
    {
        HANDLE hMapFile;
        void *pBuf;
        hMapFile = CreateFileMapping(MAPPING_MODE, NULL, _ACCESS_PAGE_, 0, LEN, name);
        if (hMapFile == NULL)
        {
            fprintf(stderr, "Could not create file mapping object.\n");
            return CREATE_FILE_MAPPING_FAILED;
        }
        pBuf = (void *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, LEN);
        if (pBuf == NULL)fprintf(stderr, "Could not map view of file .\n");
        CloseHandle(hMapFile);
        return 0;
    }
#endif


    void *create_shm(unsigned int LEN, char *name)
    {
        void *shm;
#ifdef LINUX
        shm = create_shm_linux(LEN, DJBHashKey(name, STR2ID_SEED));
#endif
#ifdef WIN_NT
        shm = create_file_mapping_win_nt(LEN, name);
#endif
        return shm;
    }
    int free_shm(unsigned int LEN, char *name)
    {
#ifdef LINUX
        return free_shm_linux(LEN, DJBHashKey(name, STR2ID_SEED));
#endif
#ifdef WIN_NT
        return close_file_mapping_win_nt(LEN, name);
#endif
    }

    int mem_dump(char *mem, unsigned int length, char *filename)
    {
        FILE *fp;
        fp = fopen(filename, "wb+");
        if(fp == NULL)return -1;
        fwrite(mem, length, 1, fp);
        fclose(fp);
        return 0;
    }
    int mem_load(char *mem, char *filename)
    {
        FILE *fp;
        int length;
        fp = fopen(filename, "rb");
        if(fp == NULL)return 0;
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        fread(mem, 1, length, fp);
        fclose(fp);
        return length;
    }
    unsigned int _get_file_length(char *filename)
    {
        unsigned int length;
        FILE *fp;
        fp = fopen(filename, "rb");
        if(fp == NULL)return 0;
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fclose(fp);
        return length;
    }
#ifdef __cplusplus
}
#endif

