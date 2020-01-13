
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : imx577_cmos.c

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/
#if !defined(__IMX577_CMOS_H_)
#define __IMX577_CMOS_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "hi_comm_sns.h"
#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_sys.h"

#include "imx577_slave_priv.h"
#include "hi_i2c.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IMX577_ID 577
#define IMX577_FULL_LINES_STD_FPGA_COMP 0
#define IMX577_VS_TIME_MAX 0xFFFFFFFF
#define IMX577_FLL_MAX 0xFFFF

#define IMX577_AGAIN_TBL_RANGE  62
#define IMX577_DGAIN_TBL_RANGE  97

//sensor gain
#define IMX577_AGAIN_21X    (21504)
#define IMX577_AGAIN_MAX    (21845)

#define IMX577_DGAIN_1X (1024)
#define IMX577_DGAIN_8X (8192)
#define IMX577_DGAIN_9X (9216)
#define IMX577_DGAIN_10X    (10240)
#define IMX577_DGAIN_11X    (11264)
#define IMX577_DGAIN_12X    (12288)
#define IMX577_DGAIN_13X    (13312)
#define IMX577_DGAIN_14X    (14336)
#define IMX577_DGAIN_15X    (15360)
#define IMX577_DGAIN_16X    (16384-1)

const unsigned char imx577_i2c_addr     =    0x34;        /* I2C Address of IMX577 */
const unsigned int  imx577_addr_byte    =    2;
const unsigned int  imx577_data_byte    =    1;

const IMX577_VIDEO_MODE_TBL_S g_astImx577ModeTbl[IMX577_MODE_BUTT] =
{
    {24000000, 228, 800000, 3500, 30,     "4K3K_4CH_12BIT_30FPS"  }, //MODE0
    {24000000, 352, 800800, 2275, 30,  "4K2K_4CH_12BIT_30FPS"  }, //MODE1
    {24000000, 180, 400000, 2215, 60,     "4K2K_4CH_12BIT_60FPS"  }, //MODE2
    {24000000, 142, 480000, 3360, 50,     "3K3K_4CH_10BIT_50FPS"  }, //MODE3
    {24000000, 88,  100088, 1130, 240, "2K1K_4CH_10BIT_240FPS" }, //real fps: 239.79
    {24000000, 131, 400400, 3052, 60,  "3K3K_4CH_10BIT_60FPS"  },
    {24000000, 86,  100100, 1155, 240, "720P_4CH_10BIT_240FPS" }, //real fps: 239.76
};

ISP_SLAVE_SNS_SYNC_S gstImx577Sync[ISP_MAX_DEV_NUM];


/****************************************************************************
 * extern function reference                                                *
 ****************************************************************************/

ISP_SNS_STATE_S *g_pastImx577[ISP_MAX_DEV_NUM] = {HI_NULL};

#define IMX577_SENSOR_GET_CTX(dev, pstCtx)   (pstCtx = g_pastImx577[dev])
#define IMX577_SENSOR_SET_CTX(dev, pstCtx)   (g_pastImx577[dev] = pstCtx)
#define IMX577_SENSOR_RESET_CTX(dev)         (g_pastImx577[dev] = HI_NULL)

static int g_fd[ISP_MAX_DEV_NUM] = {[0 ... (ISP_MAX_DEV_NUM - 1)] = -1};    // ctl.c
static HI_BOOL g_bStandby[ISP_MAX_DEV_NUM] = {0};     // ctl.c

ISP_SNS_COMMBUS_U g_aunImx577BusInfo[ISP_MAX_DEV_NUM] =
{
    [0] = { .s8I2cDev = 0},
    [1 ... ISP_MAX_DEV_NUM - 1] = { .s8I2cDev = -1}
};

//static HI_U32             gu32_IMX577_DgainVal = 0;
static HI_U32   g_u32Imx577AGain[ISP_MAX_DEV_NUM] = {[0 ... (ISP_MAX_DEV_NUM - 1)] = 1024};
static HI_U32   g_u32Imx577DGain[ISP_MAX_DEV_NUM] = {[0 ... (ISP_MAX_DEV_NUM - 1)] = 1024};

/*Depart different sensor mode to get CCM/AWB param*/
HI_U8 g_u8SensorImageMode = IMX577_12M30FPS_LINER_MODE;
HI_U8 g_u8Imx577LensMode = IMX577_SINGLE_LENS_MODE;

/* Sensor slave mode, default binding setting : Slave[0]->Pipe[0]&[1]; Slave[1]->Pipe[2]&[3]; Slave[2]->Pipe[4]&[5] */
HI_S32 g_Imx577SlaveBindDev[ISP_MAX_DEV_NUM] = {0, 0};
HI_U32 g_Imx577SlaveSensorModeTime[ISP_MAX_DEV_NUM] = {0, 0};

static HI_U32 g_au32InitExposure[ISP_MAX_DEV_NUM]  = {0};
static HI_U32 g_au32LinesPer500ms[ISP_MAX_DEV_NUM] = {0};
static HI_U16 g_au16InitWBGain[ISP_MAX_DEV_NUM][3] = {{0}};
static HI_U16 g_au16SampleRgain[ISP_MAX_DEV_NUM] = {0};
static HI_U16 g_au16SampleBgain[ISP_MAX_DEV_NUM] = {0};

int imx577_i2c_init(ISP_DEV ViPipe)
{
    int ret;
    char acDevFile[16];
    HI_U8 u8DevNum;

    if (g_fd[ViPipe] >= 0)
    {
        return HI_SUCCESS;
    }

    u8DevNum = g_aunImx577BusInfo[ViPipe].s8I2cDev;

    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "Open /dev/hi_i2c_drv-%u error!\n", u8DevNum);
        return HI_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx577_i2c_addr >> 1));
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }

    return HI_SUCCESS;
}

int imx577_i2c_exit(ISP_DEV ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

int imx577_read_register(ISP_DEV ViPipe, int addr)
{
    // TODO:
    ViPipe = ViPipe;    // avoid warning
    addr = addr;      // avoid warning
    return HI_SUCCESS;
}


int imx577_write_register(ISP_DEV ViPipe, int addr, int data)
{
    int ret;
    int idx = 0;
    char buf[8];

    if (0 > g_fd[ViPipe])
    {
        return HI_SUCCESS;
    }

    if (imx577_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = addr & 0xff;
        idx++;
    }

    if (imx577_data_byte == 2)
    {
        buf[idx] = (data >> 8) & 0xff;
        idx++;
        buf[idx] = data & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[ViPipe], buf, (imx577_addr_byte + imx577_data_byte));
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_WRITE DATA error!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000);
}

void imx577_standby(ISP_DEV ViPipe)
{
    // TODO:
    imx577_write_register(ViPipe, 0x100, 0x00);
    g_bStandby[ViPipe] = HI_TRUE;
    return;
}

void imx577_restart(ISP_DEV ViPipe)
{
    // TODO:
    imx577_write_register(ViPipe, 0x100, 0x01);
    g_bStandby[ViPipe] = HI_FALSE;
    return;
}

void imx577_sensor_init(ISP_DEV ViPipe)
{
    HI_S32  SlaveDev;
    HI_U8      u8ImgMode;
//    HI_U32     u32SlaveModeTime;
    SlaveDev  = g_Imx577SlaveBindDev[ViPipe];
    u8ImgMode = g_pastImx577[ViPipe]->u8ImgMode;
//    printf("u8ImgMode is %d\n", u8ImgMode);
//    u32SlaveModeTime = g_Imx577SlaveSensorModeTime[ViPipe];

    HI_U8 u16RegData;
    HI_U16 u16RegAddr;
    unsigned int i;
    HI_U32 u32SeqEntries;

    /* hold sync signal as fixed */
    CHECK_RET(HI_MPI_ISP_GetHvSync(SlaveDev, &gstImx577Sync[ViPipe]));

//    gstImx577Sync[ViPipe].u32SlaveModeTime  = u32SlaveModeTime;
    gstImx577Sync[ViPipe].unCfg.stBits.bitHEnable = 0;
    gstImx577Sync[ViPipe].unCfg.stBits.bitVEnable = 0;

    CHECK_RET(HI_MPI_ISP_SetHvSync(SlaveDev, &gstImx577Sync[ViPipe]));

    /* 2. sensor i2c init */
    imx577_i2c_init(ViPipe);

    if ((!g_bStandby[ViPipe]) || (!g_pastImx577[ViPipe]->bInit))
    {
        delay_ms(10); //first delay 10ms
    }

    /* When sensor first init, config all registers */
    if ((!g_bStandby[ViPipe]) || (!g_pastImx577[ViPipe]->bInit))
    {
        u32SeqEntries = sizeof(gs_au16SensorCfgSeqComm) / sizeof(gs_au16SensorCfgSeqComm[0]);
        for ( i = 0 ; i < u32SeqEntries; i++ )
        {
            u16RegAddr = gs_au16SensorCfgSeqComm[i][1];
            u16RegData = gs_au16SensorCfgSeqComm[i][0];

            if (NA == u16RegData)
            {
                continue;
            }

            imx577_write_register(ViPipe, u16RegAddr, u16RegData);
        }
    }
    u32SeqEntries = sizeof(gs_au16SensorCfgSeq) / sizeof(gs_au16SensorCfgSeq[0]);

    for ( i = 0 ; i < u32SeqEntries; i++ )
    {
        u16RegAddr = gs_au16SensorCfgSeq[i][IMX577_MODE_BUTT];
        u16RegData = gs_au16SensorCfgSeq[i][u8ImgMode];

        if (NA == u16RegData)
        {
            continue;
        }
        imx577_write_register(ViPipe, u16RegAddr, u16RegData);
    }

    CHECK_RET(HI_MPI_ISP_GetHvSync(SlaveDev, &gstImx577Sync[ViPipe]));

    if (g_pastImx577[ViPipe]->astRegsInfo[0].stSlvSync.u32SlaveVsTime == 0)
    {
        gstImx577Sync[ViPipe].u32VsTime = g_astImx577ModeTbl[u8ImgMode].u32InckPerVs;
    }
    else
    {
        gstImx577Sync[ViPipe].u32VsTime = g_pastImx577[ViPipe]->astRegsInfo[0].stSlvSync.u32SlaveVsTime;
    }

    gstImx577Sync[ViPipe].unCfg.u32Bytes = 0xc0030000;
    gstImx577Sync[ViPipe].u32VsCyc = 0x3;

    //slave open,master close
    CHECK_RET(HI_MPI_ISP_SetHvSync(SlaveDev, &gstImx577Sync[ViPipe]));

    for (i = 0; i < g_pastImx577[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        imx577_write_register(ViPipe, g_pastImx577[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx577[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    }
    imx577_restart(ViPipe);

    printf("IMX577 %s init succuss!\n", g_astImx577ModeTbl[u8ImgMode].pszModeName);

    g_pastImx577[ViPipe]->bInit = HI_TRUE;

    return ;
}

void imx577_sensor_exit(ISP_DEV ViPipe)
{
    imx577_i2c_exit(ViPipe);
    g_bStandby[ViPipe] = HI_FALSE;

    return;
}

static HI_VOID cmos_fps_set(ISP_DEV ViPipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

static HI_S32 cmos_get_ae_default(ISP_DEV ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_U32 u32Fll;
    HI_FLOAT F32MaxFps;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstAeSnsDft);
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

//    cmos_fps_set(ViPipe, 0, pstAeSnsDft);

    u32Fll = g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32VertiLines;
    F32MaxFps = g_astImx577ModeTbl[pstSnsState->u8ImgMode].f32MaxFps;
    pstSnsState->u32FLStd = u32Fll;
    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FlickerFreq = 50 * 256;

    pstAeSnsDft->stIntTimeAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stIntTimeAccu.f32Accuracy = 1;

    if (g_au32InitExposure[ViPipe] == 0)
    {
        pstAeSnsDft->u32InitExposure = 1000000;
    }
    else
    {
        pstAeSnsDft->u32InitExposure = g_au32InitExposure[ViPipe];
    }

    if (g_au32LinesPer500ms[ViPipe] == 0)
    {
        pstAeSnsDft->u32LinesPer500ms = ((HI_U64)(u32Fll * F32MaxFps)) >> 1;
    }
    else
    {
        pstAeSnsDft->u32LinesPer500ms = g_au32LinesPer500ms[ViPipe];
    }

    switch (pstSnsState->u8ImgMode)
    {
        case IMX577_12M30FPS_LINER_MODE:
        case IMX577_8M30FPS_LINER_MODE:
        case IMX577_8M60FPS_LINER_MODE:
        case IMX577_9M50FPS_LINER_MODE:
        case IMX577_9M60FPS_LINER_MODE:
        case IMX577_2M240FPS_LINER_MODE:
        case IMX577_1M240FPS_LINER_MODE:
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 8;
            pstAeSnsDft->stIntTimeAccu.f32Offset = 0.221;            //此偏移量关系到 曝光时间和sensor增益是否为线性关系
            break;

        default:
            ISP_TRACE(HI_DBG_ERR, "NOT support this mode!\n");
            break;
    }
#if 1
    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 1.0 / 1024;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_LINEAR;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 1.0 / 1024;
#else
    pstAeSnsDft->stAgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stAgainAccu.f32Accuracy = 0.3;

    pstAeSnsDft->stDgainAccu.enAccuType = AE_ACCURACY_TABLE;
    pstAeSnsDft->stDgainAccu.f32Accuracy = 0.3;
#endif
    pstAeSnsDft->u32ISPDgainShift = 8;
    pstAeSnsDft->u32MinISPDgainTarget = 1 << pstAeSnsDft->u32ISPDgainShift;
    pstAeSnsDft->u32MaxISPDgainTarget = 6 << pstAeSnsDft->u32ISPDgainShift;

    memcpy(&pstAeSnsDft->stPirisAttr, &gstPirisAttr, sizeof(ISP_PIRIS_ATTR_S));   
    pstAeSnsDft->enMaxIrisFNO = ISP_IRIS_F_NO_1_4;
    pstAeSnsDft->enMinIrisFNO = ISP_IRIS_F_NO_5_6;

    pstAeSnsDft->bAERouteExValid = HI_FALSE;
    pstAeSnsDft->stAERouteAttr.u32TotalNum = 0;
    pstAeSnsDft->stAERouteAttrEx.u32TotalNum = 0;

    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
        {
            pstAeSnsDft->au8HistThresh[0] = 0xd;
            pstAeSnsDft->au8HistThresh[1] = 0x28;
            pstAeSnsDft->au8HistThresh[2] = 0x60;
            pstAeSnsDft->au8HistThresh[3] = 0x80;

            pstAeSnsDft->u8AeCompensation = 0x38;

            pstAeSnsDft->u32MinIntTime = 8;
            pstAeSnsDft->u32MaxIntTimeTarget = 65515;
            pstAeSnsDft->u32MinIntTimeTarget = 8;

            pstAeSnsDft->u32MaxAgain = 21845;//21845;
            pstAeSnsDft->u32MinAgain = 1024;
            pstAeSnsDft->u32MaxAgainTarget = pstAeSnsDft->u32MaxAgain;
            pstAeSnsDft->u32MinAgainTarget = pstAeSnsDft->u32MinAgain;

            pstAeSnsDft->u32MaxDgain = 16384 - 1;//16384 - 1; /* if Dgain enable,please set ispdgain bigger than 1*/
            pstAeSnsDft->u32MinDgain = 1024;
            pstAeSnsDft->u32MaxDgainTarget = pstAeSnsDft->u32MaxDgain;
            pstAeSnsDft->u32MinDgainTarget = pstAeSnsDft->u32MinDgain;

            break;
        }
    }
//	printf("%s\n", __FUNCTION__);
    return HI_SUCCESS;
}


static HI_VOID cmos_fps_set(ISP_DEV ViPipe, HI_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    HI_FLOAT f32maxFps;
    HI_U32 u32Lines;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER_VOID(pstAeSnsDft);
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    f32maxFps = g_astImx577ModeTbl[pstSnsState->u8ImgMode].f32MaxFps;
    u32Lines = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32VertiLines) * f32maxFps / (f32Fps);

    /* SHR was 16bit, so limit full_lines as 0xFFFF */
    if (f32Fps > f32maxFps || u32Lines > FULL_LINES_MAX)
    {
        ISP_TRACE(HI_DBG_ERR, "Not support Fps: %f\n", f32Fps);
        return;
    }

    gstImx577Sync[ViPipe].u32VsTime = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32InckPerVs) * (f32maxFps / (f32Fps));
    pstSnsState->u32FLStd = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32VertiLines) * (f32maxFps / (f32Fps));

    pstSnsState->astRegsInfo[0].stSlvSync.u32SlaveVsTime = gstImx577Sync[ViPipe].u32VsTime;
    pstSnsState->u32FLStd = (pstSnsState->u32FLStd > FULL_LINES_MAX) ? FULL_LINES_MAX : pstSnsState->u32FLStd;

    pstSnsState->astRegsInfo[0].astI2cData[0].u32Data =  (pstSnsState->u32FLStd & 0xFF);
    pstSnsState->astRegsInfo[0].astI2cData[1].u32Data =  (pstSnsState->u32FLStd & 0xFF00) >> 8;

    pstSnsState->astRegsInfo[0].astI2cData[9].u32Data =  (pstSnsState->u32FLStd & 0xFF);
    pstSnsState->astRegsInfo[0].astI2cData[10].u32Data =  (pstSnsState->u32FLStd & 0xFF00) >> 8;

    pstAeSnsDft->f32Fps = f32Fps;
    pstAeSnsDft->u32LinesPer500ms = pstSnsState->u32FLStd * f32Fps / 2;

    switch (pstSnsState->u8ImgMode)
    {
        case IMX577_12M30FPS_LINER_MODE:
        case IMX577_8M30FPS_LINER_MODE:
        case IMX577_8M60FPS_LINER_MODE:
        case IMX577_9M50FPS_LINER_MODE:
        case IMX577_9M60FPS_LINER_MODE:
        case IMX577_2M240FPS_LINER_MODE:
        case IMX577_1M240FPS_LINER_MODE:
            pstAeSnsDft->u32MaxIntTime = pstSnsState->u32FLStd - 22;
            break;

        default:
            ISP_TRACE(HI_DBG_ERR, "NOT support this mode!\n");
            return;
    }

    pstAeSnsDft->u32FullLinesStd = pstSnsState->u32FLStd;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstAeSnsDft->u32FullLines = pstSnsState->au32FL[0];

//	printf("%s\n", __FUNCTION__);
    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static HI_VOID cmos_inttime_update(ISP_DEV ViPipe, HI_U32 u32IntTime)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);

    pstSnsState->au32FL[0] = u32IntTime;
   
    // SET CORASE_INTEG_TIME
    pstSnsState->astRegsInfo[0].astI2cData[7].u32Data = LOW_8BITS(pstSnsState->au32FL[0]);
    pstSnsState->astRegsInfo[0].astI2cData[8].u32Data = HIGH_8BITS(pstSnsState->au32FL[0]);
 
    return;
}
/* Again segmentation = 62 */
static HI_U32 ad_gain_table[IMX577_AGAIN_TBL_RANGE] =
{
    1024, 1040, 1057, 1074, 1092, 1111, 1130, 1150, 1170, 1192,
    1214, 1237, 1260, 1285, 1311, 1337, 1365, 1394, 1425, 1456,
    1489, 1524, 1560, 1598, 1638, 1680, 1725, 1771, 1820, 1872,
    1928, 1986, 2048, 2114, 2185, 2260, 2341, 2427, 2521, 2621,
    2731, 2849, 2979, 3121, 3277, 3449, 3641, 3855, 4096, 4369,
    4681, 5041, 5461, 5958, 6554, 7282, 8192, 9362, 10923, 13107,
    16384, 21845
};

static HI_VOID cmos_again_calc_table(ISP_DEV ViPipe, HI_U32 *pu32AgainLin, HI_U32 *pu32AgainDb)
{
    int i;

//    CMOS_CHECK_POINTER_VOID(pu32AgainLin);
//    CMOS_CHECK_POINTER_VOID(pu32AgainDb);

    if (*pu32AgainLin >= ad_gain_table[IMX577_AGAIN_TBL_RANGE - 1])
    {
        *pu32AgainLin = ad_gain_table[IMX577_AGAIN_TBL_RANGE - 1];
        *pu32AgainDb = IMX577_AGAIN_TBL_RANGE - 1;
        goto calc_table_end;
    }

    for (i = 1; i < IMX577_AGAIN_TBL_RANGE; i++)
    {
        if (*pu32AgainLin < ad_gain_table[i])
        {
            *pu32AgainLin = ad_gain_table[i - 1];
            *pu32AgainDb = i - 1;
            goto calc_table_end;;
        }
    }


calc_table_end:

    g_u32Imx577AGain[ViPipe] = *pu32AgainLin;

    *pu32AgainDb <<= 4;
//	printf("%s\n", __FUNCTION__);
    return;
}

/* dgain segmentation = 97 */
static HI_U32 dgain_table[IMX577_DGAIN_TBL_RANGE] =
{
    1024, 1034, 1044, 1055, 1066, 1077, 1088, 1099, 1111, 1123,
    1135, 1147, 1160, 1173, 1186, 1200, 1214, 1228, 1242, 1257,
    1273, 1288, 1304, 1321, 1337, 1355, 1372, 1391, 1409, 1429,
    1448, 1469, 1489, 1511, 1533, 1556, 1579, 1603, 1628, 1654,
    1680, 1708, 1736, 1765, 1796, 1827, 1859, 1893, 1928, 1964,
    2001, 2040, 2081, 2123, 2166, 2212, 2260, 2310, 2362, 2416,
    2473, 2533, 2595, 2661, 2731, 2804, 2881, 2962, 3048, 3139,
    3236, 3339, 3449, 3567, 3692, 3827, 3972, 4128, 4297, 4481,
    4681, 4900, 5140, 5405, 5699, 6026, 6394, 6809, 7282, 7825,
    8456, 9198, 10082, 11155, 12483, 14170, (16384 - 1)
};

#if 0
/* dgain segmentation = 61 */
static HI_U32 dgain_table[61] =
{
    1024, 1040, 1057, 1074, 1092, 1111, 1130, 1150, 1170, 1192, 1214, 1237, 1260, 1285, 1311, 1337, 1365, 1394, 1425, 1456, 1489, 1524, 1560, 1598, 1638 ,
    1680, 1725, 1771, 1820, 1872, 1928, 1986, 2048, 2114, 2185, 2260, 2341, 2427, 2521, 2621, 2731, 2849, 2979, 3121, 3277 , 3449, 3641, 3855, 4096, 4369,
    4681, 5041, 5461, 5958, 6554, 7282, 8192, 9362, 10923, 13107, 16384
};
#endif
static HI_VOID cmos_dgain_calc_table(ISP_DEV ViPipe, HI_U32 *pu32DgainLin, HI_U32 *pu32DgainDb)
{
//    CMOS_CHECK_POINTER_VOID(pu32DgainLin);
 //   CMOS_CHECK_POINTER_VOID(pu32DgainDb);
    if (*pu32DgainLin >= dgain_table[IMX577_DGAIN_TBL_RANGE - 1])
    {
        *pu32DgainLin = dgain_table[IMX577_DGAIN_TBL_RANGE - 1];
        //*pu32DgainDb = IMX577_DGAIN_TBL_RANGE-1;
        //*pu32DgainDb = *pu32DgainLin;
        //return ;
    }
    g_u32Imx577DGain[ViPipe] = *pu32DgainLin;
#if 0
    for (i = 1; i < IMX577_DGAIN_TBL_RANGE; i++)
    {
        if (*pu32DgainLin < dgain_table[i])
        {
            *pu32DgainLin = dgain_table[i - 1];
            //*pu32DgainDb = i - 1;
            break;
        }
    }
#endif
    *pu32DgainDb = *pu32DgainLin;
//	printf("%s\n", __FUNCTION__);
    return;
}

static HI_VOID cmos_gains_update(ISP_DEV ViPipe, HI_U32 u32Again, HI_U32 u32Dgain)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    
    double again_ratio = u32Again / 1024.;
    unsigned again_global;
    again_global = 1024 - (1024. / again_ratio);
   
    //Again
    pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = (again_global & 0xFF);
    pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = ((again_global >> 8) & 0x0003);

    //Dgain
    if (u32Dgain % 1024 != 0)
    {
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = (((u32Dgain % 1024) >> 2) & 0x00FF);
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = ((u32Dgain >> 10 ) & 0xF);
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 1;
    }
    else
    {
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0xFF;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = ((u32Dgain >> 10 ) & 0xF) - 1;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32Data = 1;

    }
    
    return;
}

static HI_S32 cmos_get_awb_default(ISP_DEV ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft)
{
    memset(pstAwbSnsDft, 0, sizeof(AWB_SENSOR_DEFAULT_S));
    pstAwbSnsDft->u16WbRefTemp = 5120;

    pstAwbSnsDft->au16GainOffset[0] = 491; //CALIBRATE_STATIC_WB_R_GAIN;
    pstAwbSnsDft->au16GainOffset[1] = 0X100;  //CALIBRATE_STATIC_WB_GR_GAIN;
    pstAwbSnsDft->au16GainOffset[2] = 0x100;  //CALIBRATE_STATIC_WB_GB_GAIN;
    pstAwbSnsDft->au16GainOffset[3] = 458; //CALIBRATE_STATIC_WB_B_GAIN;

    pstAwbSnsDft->as32WbPara[0] = (-4); //CALIBRATE_AWB_P1;
    pstAwbSnsDft->as32WbPara[1] = 235; //CALIBRATE_AWB_P2;
    pstAwbSnsDft->as32WbPara[2] = (-24); //CALIBRATE_AWB_Q1;
    pstAwbSnsDft->as32WbPara[3] = 153042; //CALIBRATE_AWB_A1;
    pstAwbSnsDft->as32WbPara[4] = 128; //CALIBRATE_AWB_B1;
    pstAwbSnsDft->as32WbPara[5] = (-101980); //CALIBRATE_AWB_C1;

    pstAwbSnsDft->u16GoldenRgain = 0; //GOLDEN_RGAIN;
    pstAwbSnsDft->u16GoldenBgain = 0; //GOLDEN_BGAIN;

    memcpy(&pstAwbSnsDft->stCcm, &g_stAwbCcm_NormalLens, sizeof(AWB_CCM_S));
    memcpy(&pstAwbSnsDft->stAgcTbl, &g_stAwbAgcTable, sizeof(AWB_AGC_TABLE_S));

    pstAwbSnsDft->u16InitRgain = g_au16InitWBGain[ViPipe][0];
    pstAwbSnsDft->u16InitGgain = g_au16InitWBGain[ViPipe][1];
    pstAwbSnsDft->u16InitBgain = g_au16InitWBGain[ViPipe][2];
    pstAwbSnsDft->u16SampleRgain = g_au16SampleRgain[ViPipe];
    pstAwbSnsDft->u16SampleBgain = g_au16SampleBgain[ViPipe];

    return HI_SUCCESS;
}

static HI_U32 cmos_get_isp_default(ISP_DEV ViPipe, ISP_CMOS_DEFAULT_S *pstDef)
{
	ISP_SNS_STATE_S *pstSnsState = HI_NULL;
	IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
	
	if (HI_NULL == pstDef)
    {
        printf("null pointer when get isp default value!\n");
        return -1;
    }
    memset(pstDef, 0, sizeof(ISP_CMOS_DEFAULT_S));
    switch (pstSnsState->enWDRMode)
    {
        default:
        case WDR_MODE_NONE:
            pstDef->stDrc.bEnable               = HI_FALSE;
            pstDef->stDrc.u32BlackLevel         = 0x00;
            pstDef->stDrc.u32WhiteLevel         = 0xD0000; 
            pstDef->stDrc.u32SlopeMax           = 0x30;
            pstDef->stDrc.u32SlopeMin           = 0x00;
            pstDef->stDrc.u32VarianceSpace      = 0x04;
            pstDef->stDrc.u32VarianceIntensity  = 0x01;
            pstDef->stDrc.u32Asymmetry          = 0x08;
            pstDef->stDrc.u32BrightEnhance      = 0xE6;
            pstDef->stDrc.bFilterMux            = 0x1;
            pstDef->stDrc.u32Svariance          = 0x8;
            pstDef->stDrc.u32BrightPr           = 0xB0;
            pstDef->stDrc.u32Contrast           = 0xB0;
            pstDef->stDrc.u32DarkEnhance        = 0x8000;
            
            memcpy(&pstDef->stAgcTbl, &g_stIspAgcTable, sizeof(ISP_CMOS_AGC_TABLE_S));
            memcpy(&pstDef->stNoiseTbl, &g_stIspNoiseTable, sizeof(ISP_CMOS_NOISE_TABLE_S));            
            memcpy(&pstDef->stDemosaic, &g_stIspDemosaic, sizeof(ISP_CMOS_DEMOSAIC_S));
            memcpy(&pstDef->stGamma, &g_stIspGamma, sizeof(ISP_CMOS_GAMMA_S));
            memcpy(&pstDef->stBayerSharpen, &g_stIspBayerSharpen, sizeof(ISP_CMOS_BAYER_SHARPEN_S));
            memcpy(&pstDef->stYuvSharpen, &g_stIspYuvSharpen, sizeof(ISP_CMOS_YUV_SHARPEN_S));
        break;
    }
    pstDef->stSensorMaxResolution.u32MaxWidth  = 4000;
    pstDef->stSensorMaxResolution.u32MaxHeight = 3000;
    pstDef->stSensorMode.u32SensorID = IMX577_ID;
    pstDef->stSensorMode.u8SensorMode = pstSnsState->u8ImgMode;
//	printf("cmos_get_isp_default\n");

    return HI_SUCCESS;
}

static HI_U32 cmos_get_isp_black_level(ISP_DEV ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel)
{
    CMOS_CHECK_POINTER(pstBlackLevel);

    /* Don't need to update black level when iso change */
    pstBlackLevel->bUpdate = HI_FALSE;

    //for (i=0; i<4; i++)
    //{
    //    pstBlackLevel->au16BlackLevel[i] = 0x100;/*10bit : 0x40*/
    //}

    if (HI_TRUE == pstBlackLevel->bUpdate)
    {
        if (g_u32Imx577AGain[ViPipe] >= IMX577_AGAIN_21X)
        {
            if ((g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_8X))
            {
                pstBlackLevel->au16BlackLevel[0] = 258;
                pstBlackLevel->au16BlackLevel[1] = 261;
                pstBlackLevel->au16BlackLevel[2] = 261;
                pstBlackLevel->au16BlackLevel[3] = 258;
            }
            if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_8X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_9X))
            {
                pstBlackLevel->au16BlackLevel[0] = 261;
                pstBlackLevel->au16BlackLevel[1] = 262;
                pstBlackLevel->au16BlackLevel[2] = 262;
                pstBlackLevel->au16BlackLevel[3] = 261;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_9X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_11X))
            {
                pstBlackLevel->au16BlackLevel[0] = 262;
                pstBlackLevel->au16BlackLevel[1] = 263;
                pstBlackLevel->au16BlackLevel[2] = 263;
                pstBlackLevel->au16BlackLevel[3] = 262;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_11X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_12X))
            {
                pstBlackLevel->au16BlackLevel[0] = 264;
                pstBlackLevel->au16BlackLevel[1] = 265;
                pstBlackLevel->au16BlackLevel[2] = 265;
                pstBlackLevel->au16BlackLevel[3] = 264;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_12X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_13X))
            {
                pstBlackLevel->au16BlackLevel[0] = 269;
                pstBlackLevel->au16BlackLevel[1] = 268;
                pstBlackLevel->au16BlackLevel[2] = 268;
                pstBlackLevel->au16BlackLevel[3] = 269;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_13X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_14X))
            {
                pstBlackLevel->au16BlackLevel[0] = 270;
                pstBlackLevel->au16BlackLevel[1] = 272;
                pstBlackLevel->au16BlackLevel[2] = 272;
                pstBlackLevel->au16BlackLevel[3] = 270;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_14X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_15X))
            {
                pstBlackLevel->au16BlackLevel[0] = 277;
                pstBlackLevel->au16BlackLevel[1] = 275;
                pstBlackLevel->au16BlackLevel[2] = 275;
                pstBlackLevel->au16BlackLevel[3] = 277;
            }
            else if ((g_u32Imx577DGain[ViPipe] > IMX577_DGAIN_15X) && (g_u32Imx577DGain[ViPipe] <= IMX577_DGAIN_16X))
            {
                pstBlackLevel->au16BlackLevel[0] = 282;
                pstBlackLevel->au16BlackLevel[1] = 278;
                pstBlackLevel->au16BlackLevel[2] = 278;
                pstBlackLevel->au16BlackLevel[3] = 282;
            }
        }
        else
        {
            pstBlackLevel->au16BlackLevel[0] = 257;
            pstBlackLevel->au16BlackLevel[1] = 257;
            pstBlackLevel->au16BlackLevel[2] = 257;
            pstBlackLevel->au16BlackLevel[3] = 257;
        }
    }
    else
    {
        pstBlackLevel->au16BlackLevel[0] = 257;
        pstBlackLevel->au16BlackLevel[1] = 257;
        pstBlackLevel->au16BlackLevel[2] = 257;
        pstBlackLevel->au16BlackLevel[3] = 257;
    }
//	printf("%s\n", __FUNCTION__);
    return HI_SUCCESS;
}

static HI_VOID cmos_set_pixel_detect(ISP_DEV ViPipe, HI_BOOL bEnable)
{
    ViPipe = ViPipe;
    bEnable = bEnable;
#if 0
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER_VOID(pstSnsState);

    /* Detect set 5fps */
    CHECK_RET(HI_MPI_ISP_GetHvSync(ViPipe, &gstImx577Sync[ViPipe]));

    if (bEnable)
    {
        switch (pstSnsState->u8ImgMode)
        {
            case IMX577_12M30FPS_LINER_MODE:
            case IMX577_8M30FPS_LINER_MODE:
            case IMX577_8M60FPS_LINER_MODE:
            case IMX577_9M50FPS_LINER_MODE:
			case IMX577_9M60FPS_LINER_MODE:
            case IMX577_2M240FPS_LINER_MODE:
            case IMX577_1M240FPS_LINER_MODE:
                gstImx577Sync[ViPipe].u32VsTime = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32InckPerVs) * g_astImx577ModeTbl[pstSnsState->u8ImgMode].f32MaxFps / 5;
                pstSnsState->u32FLStd = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32VertiLines) * g_astImx577ModeTbl[pstSnsState->u8ImgMode].f32MaxFps / 5;
                pstSnsState->astRegsInfo[0].astI2cData[0].u32Data = 0xFF;
                pstSnsState->astRegsInfo[0].astI2cData[1].u32Data = 0xFF;
                break;

            default:
                ISP_TRACE(HI_DBG_ERR, "NOT support this mode for PixDetect!\n");
                break;
        }

        /* Analog and Digital gains both must be programmed for their minimum values */
        pstSnsState->astRegsInfo[0].astI2cData[2].u32Data = 0x0;
        pstSnsState->astRegsInfo[0].astI2cData[3].u32Data = 0x0;
        pstSnsState->astRegsInfo[0].astI2cData[4].u32Data = 0x0;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32Data = 0x1;
    }
    else /* setup for ISP 'normal mode' */
    {
        gstImx577Sync[ViPipe].u32VsTime = (g_astImx577ModeTbl[pstSnsState->u8ImgMode].u32InckPerVs);
        pstSnsState->bSyncInit = HI_FALSE;
    }

    CHECK_RET(HI_MPI_ISP_SetHvSync(ViPipe, &gstImx577Sync[ViPipe]));
//	printf("%s\n", __FUNCTION__);
#endif
    return;
}

static HI_U32 cmos_get_sns_regs_info(ISP_DEV ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo)
{
    unsigned int i;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstSnsRegsInfo);
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    if ((HI_FALSE == pstSnsState->bSyncInit) || (HI_FALSE == pstSnsRegsInfo->bConfig))
    {
        pstSnsState->astRegsInfo[0].enSnsType = ISP_SNS_I2C_TYPE;
        pstSnsState->astRegsInfo[0].unComBus.s8I2cDev = g_aunImx577BusInfo[ViPipe].s8I2cDev;
        pstSnsState->astRegsInfo[0].u8Cfg2ValidDelayMax = 3;
        pstSnsState->astRegsInfo[0].u32RegNum = 11;

        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            pstSnsState->astRegsInfo[0].astI2cData[i].u8DevAddr = imx577_i2c_addr;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32AddrByteNum = imx577_addr_byte;
            pstSnsState->astRegsInfo[0].astI2cData[i].u32DataByteNum = imx577_data_byte;
        }

        /* Slave Sensor VsTime cfg */

        pstSnsState->astRegsInfo[0].astI2cData[0].u32RegAddr = IMX577_LINE_LENGTH_PCK_L;
        pstSnsState->astRegsInfo[0].astI2cData[0].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[1].u32RegAddr = IMX577_LINE_LENGTH_PCK_H;
        pstSnsState->astRegsInfo[0].astI2cData[1].u8DelayFrmNum = 0;

        // Again related
        pstSnsState->astRegsInfo[0].astI2cData[2].u32RegAddr = IMX577_ANA_GAIN_GLOBAL_L;
        pstSnsState->astRegsInfo[0].astI2cData[2].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[3].u32RegAddr = IMX577_ANA_GAIN_GLOBAL_H;
        pstSnsState->astRegsInfo[0].astI2cData[3].u8DelayFrmNum = 1;

        /* Dgain cfg */
        pstSnsState->astRegsInfo[0].astI2cData[4].u32RegAddr = IMX577_DIG_GAIN_GR_L;
        pstSnsState->astRegsInfo[0].astI2cData[4].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[5].u32RegAddr = IMX577_DIG_GAIN_GR_H;
        pstSnsState->astRegsInfo[0].astI2cData[5].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[6].u32RegAddr = IMX577_DPGA_USE_GLOBAL_GAIN;
        pstSnsState->astRegsInfo[0].astI2cData[6].u8DelayFrmNum = 1;

        pstSnsState->astRegsInfo[0].astI2cData[7].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[7].u32RegAddr = IMX577_COARSE_INTEG_TIME_L;

        pstSnsState->astRegsInfo[0].astI2cData[8].u8DelayFrmNum = 1;
        pstSnsState->astRegsInfo[0].astI2cData[8].u32RegAddr = IMX577_COARSE_INTEG_TIME_H;


        pstSnsState->astRegsInfo[0].astI2cData[9].u32RegAddr = IMX577_PRSH_LENGTH_LINE_L;
        pstSnsState->astRegsInfo[0].astI2cData[9].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[10].u32RegAddr = IMX577_PRSH_LENGTH_LINE_H;
        pstSnsState->astRegsInfo[0].astI2cData[10].u8DelayFrmNum = 0;

		pstSnsState->astRegsInfo[0].astI2cData[11].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[11].u32RegAddr = IMX577_COARSE_INTEG_TIME_L;
        pstSnsState->astRegsInfo[0].astI2cData[12].u8DelayFrmNum = 0;
        pstSnsState->astRegsInfo[0].astI2cData[12].u32RegAddr = IMX577_COARSE_INTEG_TIME_H;

        // Again related

        /* Dgain cfg */
        pstSnsState->astRegsInfo[0].stSlvSync.bUpdate = HI_TRUE;

        pstSnsState->astRegsInfo[0].stSlvSync.u8DelayFrmNum  = 2;
//        pstSnsState->astRegsInfo[0].stSlvSync.u32SlaveBindDev = g_Imx577SlaveBindDev[ViPipe];   //注释，不确定是否有影响

        pstSnsState->bSyncInit = HI_TRUE;
    }
    else
    {
        for (i = 0; i < pstSnsState->astRegsInfo[0].u32RegNum; i++)
        {
            if (pstSnsState->astRegsInfo[0].astI2cData[i].u32Data == pstSnsState->astRegsInfo[1].astI2cData[i].u32Data)
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_FALSE;
            }
            else
            {
                pstSnsState->astRegsInfo[0].astI2cData[i].bUpdate = HI_TRUE;
            }
        }

        if (pstSnsState->astRegsInfo[0].stSlvSync.u32SlaveVsTime == pstSnsState->astRegsInfo[1].stSlvSync.u32SlaveVsTime)
        {
            pstSnsState->astRegsInfo[0].stSlvSync.bUpdate = HI_FALSE;
        }
        else
        {
            pstSnsState->astRegsInfo[0].stSlvSync.bUpdate = HI_TRUE;
        }
    }

    pstSnsRegsInfo->bConfig = HI_FALSE;

    memcpy(pstSnsRegsInfo, &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));
    memcpy(&pstSnsState->astRegsInfo[1], &pstSnsState->astRegsInfo[0], sizeof(ISP_SNS_REGS_INFO_S));

    pstSnsState->au32FL[1] = pstSnsState->au32FL[0];
//	printf("%s\n", __FUNCTION__);
    return HI_SUCCESS;
}

static HI_S32 cmos_set_image_mode(ISP_DEV ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode)
{

//    HI_U8 u8SensorImageMode;
//    HI_U32 u32W, u32H;
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    CMOS_CHECK_POINTER(pstSensorImageMode);
    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
    CMOS_CHECK_POINTER(pstSnsState);

    pstSnsState->bSyncInit = HI_FALSE;


//    u32H = pstSensorImageMode->u16Height;
//    u32W = pstSensorImageMode->u16Width;

#if 0
    if (IMX577_RES_IS_8M(u32W, u32H))
    {
        if (u8M == 0)
        {
            u8SensorImageMode = IMX577_8M60FPS_LINER_MODE;

        }
        else
        {
            u8SensorImageMode = IMX577_8M30FPS_LINER_MODE;
        }
    }
    else if (IMX577_RES_IS_12M(u32W, u32H))
    {
        u8SensorImageMode = IMX577_12M30FPS_LINER_MODE;
    }
    else if (IMX577_RES_IS_2M(u32W, u32H))
    {
        u8SensorImageMode = IMX577_2M240FPS_LINER_MODE;
    }
    else if (IMX577_RES_IS_1M(u32W, u32H))
    {
        u8SensorImageMode = IMX577_1M240FPS_LINER_MODE;
    }
    else
    {
        if (u8M == 0)
        {
            u8SensorImageMode = IMX577_9M50FPS_LINER_MODE;
        }
        else
        {
            u8SensorImageMode = IMX577_9M60FPS_LINER_MODE;
        }

    }

    g_u8SensorImageMode = u8SensorImageMode;

    /* Sensor first init */
    if (HI_FALSE == pstSnsState->bInit)
    {
        pstSnsState->u8ImgMode = u8SensorImageMode;

        return HI_SUCCESS;
    }

    /* Switch SensorImageMode */

    if (u8SensorImageMode == pstSnsState->u8ImgMode)
    {
        /* Don't need to switch SensorImageMode */
        return HI_FAILURE;
    }
#endif
    pstSnsState->u8ImgMode = 0;   //  u8SensorImageMode;
//	printf("%s\n", __FUNCTION__);
    return HI_SUCCESS;
}

static HI_VOID sensor_global_init(ISP_DEV ViPipe)
{
    ISP_SNS_STATE_S *pstSnsState = HI_NULL;

    IMX577_SENSOR_GET_CTX(ViPipe, pstSnsState);
//    CMOS_CHECK_POINTER_VOID(pstSnsState);

    pstSnsState->enWDRMode = WDR_MODE_NONE;
    pstSnsState->bInit = HI_FALSE;
    pstSnsState->bSyncInit = HI_FALSE;

    pstSnsState->u32FLStd = 3500 + IMX577_FULL_LINES_STD_FPGA_COMP;
    pstSnsState->u8ImgMode = IMX577_12M30FPS_LINER_MODE;
    pstSnsState->au32FL[0] = pstSnsState->u32FLStd;
    pstSnsState->au32FL[1] = pstSnsState->u32FLStd;

    memset(&pstSnsState->astRegsInfo[0], 0, sizeof(ISP_SNS_REGS_INFO_S));
    memset(&pstSnsState->astRegsInfo[1], 0, sizeof(ISP_SNS_REGS_INFO_S));
//    printf("%s\n", __FUNCTION__);
}

static HI_VOID cmos_set_wdr_mode(ISP_DEV ViPipe, HI_U8 u8Mode)
{
    ViPipe = ViPipe;
    u8Mode = u8Mode;
    return ;
}

static HI_S32 sensor_ctx_init(ISP_DEV ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;

    IMX577_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);

    if (HI_NULL == pastSnsStateCtx)
    {
        pastSnsStateCtx = (ISP_SNS_STATE_S *)malloc(sizeof(ISP_SNS_STATE_S));
        if (HI_NULL == pastSnsStateCtx)
        {
            ISP_TRACE(HI_DBG_ERR, "Isp[%d] SnsCtx malloc memory failed!\n", ViPipe);
            return HI_ERR_ISP_NOMEM;
        }
    }

    memset(pastSnsStateCtx, 0, sizeof(ISP_SNS_STATE_S));

    IMX577_SENSOR_SET_CTX(ViPipe, pastSnsStateCtx);
//	printf("%s\n", __FUNCTION__);
    return HI_SUCCESS;
}

static HI_VOID sensor_ctx_exit(ISP_DEV ViPipe)
{
    ISP_SNS_STATE_S *pastSnsStateCtx = HI_NULL;

    IMX577_SENSOR_GET_CTX(ViPipe, pastSnsStateCtx);
    SENSOR_FREE(pastSnsStateCtx);
    IMX577_SENSOR_RESET_CTX(ViPipe);
}

/****************************************************************************
 * callback structure                                                       *
 ****************************************************************************/

void imx577_init(int dev) {
    sensor_ctx_init(dev);
//    g_aunImx577BusInfo[dev].s8I2cDev = 0;
    assert(dev < ISP_MAX_DEV_NUM && dev >= 0);
 
    ISP_SENSOR_REGISTER_S isp_cb = ZERO(ISP_SENSOR_REGISTER_S);
    AE_SENSOR_REGISTER_S ae_cb = ZERO(AE_SENSOR_REGISTER_S);
    AWB_SENSOR_REGISTER_S awb_cb = ZERO(AWB_SENSOR_REGISTER_S);
 
    ALG_LIB_S ae_lib = {dev, HI_AE_LIB_NAME};
    ALG_LIB_S awb_lib = {dev, HI_AWB_LIB_NAME};
 
    isp_cb.stSnsExp.pfn_cmos_sensor_init = imx577_sensor_init;
    isp_cb.stSnsExp.pfn_cmos_sensor_exit = imx577_sensor_exit;
    isp_cb.stSnsExp.pfn_cmos_sensor_global_init = sensor_global_init;
 
    isp_cb.stSnsExp.pfn_cmos_set_image_mode = cmos_set_image_mode;
    isp_cb.stSnsExp.pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    isp_cb.stSnsExp.pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
 
    isp_cb.stSnsExp.pfn_cmos_get_isp_default = cmos_get_isp_default;
    isp_cb.stSnsExp.pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    isp_cb.stSnsExp.pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;
 
    ae_cb.stSnsExp.pfn_cmos_get_ae_default = cmos_get_ae_default;
    ae_cb.stSnsExp.pfn_cmos_fps_set = cmos_fps_set;
    ae_cb.stSnsExp.pfn_cmos_inttime_update = cmos_inttime_update;
    ae_cb.stSnsExp.pfn_cmos_gains_update = cmos_gains_update;
    ae_cb.stSnsExp.pfn_cmos_again_calc_table = cmos_again_calc_table;
    ae_cb.stSnsExp.pfn_cmos_dgain_calc_table = cmos_dgain_calc_table;
 
    awb_cb.stSnsExp.pfn_cmos_get_awb_default = cmos_get_awb_default;
 
    HI_MPI_ISP_SensorRegCallBack(dev, IMX577_ID, &isp_cb);
    HI_MPI_AE_SensorRegCallBack(dev, &ae_lib, IMX577_ID, &ae_cb);
    HI_MPI_AWB_SensorRegCallBack(dev, &awb_lib, IMX577_ID, &awb_cb);
 
    HI_MPI_AE_Register(dev, &ae_lib);
    HI_MPI_AWB_Register(dev, &awb_lib);
}
 
void imx577_deinit(int dev) {
    sensor_ctx_exit(dev);
    ALG_LIB_S ae_lib = {dev, HI_AE_LIB_NAME};
    ALG_LIB_S awb_lib = {dev, HI_AWB_LIB_NAME};
    HI_MPI_ISP_SensorUnRegCallBack(dev, IMX577_ID);
    HI_MPI_AE_SensorUnRegCallBack(dev, &ae_lib, IMX577_ID);
    HI_MPI_AWB_SensorUnRegCallBack(dev, &awb_lib, IMX577_ID);
    HI_MPI_AE_UnRegister(dev, &ae_lib);
    HI_MPI_AWB_UnRegister(dev, &awb_lib);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
//
