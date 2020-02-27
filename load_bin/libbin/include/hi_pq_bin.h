/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

******************************************************************************/


#ifndef __HI_PQ_BIN_H__
#define __HI_PQ_BIN_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_BIN_NULL_POINT    0xCB000001
#define HI_BIN_REG_ATTR_ERR  0xCB000002
#define HI_BIN_MALLOC_ERR    0xCB000003
#define HI_BIN_CHIP_ERR      0xCB000004
#define HI_BIN_CRC_ERR       0xCB000005
#define HI_BIN_SIZE_ERR      0xCB000006
#define HI_BIN_LEBLE_ERR     0xCB000007
#define HI_BIN_DATA_ERR      0xCB000008
#define HI_BIN_SECURITY_SOLUTION_FAILED 0xCB00000A

/*****************************************************************************
*   Prototype    : HI_PQ_GetBinTotalLen
*   Description: : Get length of bin data
*   Input        : void
*   Output       : void
*   Return Value : length of bin data
*****************************************************************************/

unsigned int HI_PQ_GetBinTotalLen(void);

/*****************************************************************************
*   Prototype    : HI_PQ_BIN_ExportBinData
*   Description: : Get bin data from buffer
*   Input        : pu8Buffer:save bin data
*                  u32DataLength:length of bin data
*   Output       : 
*   Return Value : 0: Success;
*                  Error codes: 0xCB000001:input para is null
*                               0xCB000003:malloc error
*                               0xCB000004:chipid not match
*                               0xCB000005:crc error
*                               0xCB000008:data error
*                               0xCB00000A:security solution failed
*****************************************************************************/

int HI_PQ_BIN_ExportBinData(unsigned char* pu8Buffer, unsigned int u32DataLength);



/*****************************************************************************
*   Prototype    : HI_PQ_BIN_ImportBinData
*   Description: : set bin data from buffer
*   Input        : pu8Buffer:save bin data
                   u32DataLength:length of bin data
*   Output       : 
*   Return Value : 0: Success;
*                  Error codes: 0xCB000001:input para is null
*                               0xCB000003:malloc error
*                               0xCB000008:data erro
*                               0xCB00000A:security solution failed
*****************************************************************************/

int HI_PQ_BIN_ImportBinData(unsigned char* pu8Buffer, unsigned int u32DataLength);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef __HI_PQ_BIN_H__ */

