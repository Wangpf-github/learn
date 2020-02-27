#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "hi_pq_bin.h"
#include "mpi_isp.h"
#include "mpi_sys.h"
#include "hi_defines.h"
#include <limits.h>

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

#define BIN_NUM (4)

int load_pq_bin(const char *file_path)
{
    int i;
    int s32Ret = -1;
    char path_buffer[100] = {0};
    unsigned int DataLen = 0;
    unsigned int size, TempLen;
    unsigned char *pBuffer;
    FILE *pFile = NULL;

    DataLen = HI_PQ_GetBinTotalLen();
    pBuffer = (unsigned char *)malloc(DataLen);
    if (pBuffer == NULL)
    {
        printf("malloc err!\n");
        return -1;
    }

    for(i = 0; i < BIN_NUM; i++)
    {
        memset(pBuffer, 0, DataLen);
        
        sprintf(path_buffer, "%s/pq_binary_data_%d.bin", file_path, i);
        pFile = fopen(path_buffer, "r");
        if (NULL == pFile)
        {
            printf("fopen error.\n");
            s32Ret = -1;
            break;
        }
        fseek(pFile, 0, SEEK_END);
        size = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        
        if (DataLen != size)
        {
            printf("data length err!\n");
            s32Ret = -1;
            break;
        }
        TempLen = fread(pBuffer, sizeof(unsigned char), size, pFile);
        if(0 >= TempLen)
        {
            printf("read erro\n");
            s32Ret = -1;
            break;
        }
        s32Ret = HI_PQ_BIN_ImportBinData(pBuffer,size);
        if(0 != s32Ret)
        {
            printf("HI_PQ_BIN_ImportBinData error! errno(0x%x)\n",s32Ret);
            break;
        }
        printf("HI_PQ_BIN_ParseBinData_%d success!\n", i);
    }

    free(pBuffer);
    pBuffer = NULL;

    if (pFile != NULL)
    {
        fclose(pFile);
    }

    return s32Ret;
}

int main()
{
    int ret;
    ret = load_pq_bin("/media/storage/PQ_test");

    return ret;
}