
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "mpp.h"

#define ASSERT(_exp, _var, _assert, _ret, ...)                                 \
    ({                                                                         \
        typeof(_exp) _var = (_exp);                                            \
        if (!(_assert)) {                                                      \
            __VA_ARGS__;                                                       \
            return (_ret);                                                     \
        }                                                                      \
        _var;                                                                  \
    })

#define WARN_HEX(_str, _val) g_warning("%s:0x%08X", _str, _val)
#define WARN_BOOL(_str, _val) g_warning("%s:%s", _str, _val ? "TRUE" : "FALSE")
#define WARN_SYS(_str) g_warning("%s:%m", _str)

#define ASSERT_HEX(_exp, _assert, _ret, ...)                                   \
    ASSERT(_exp, r, _assert, _ret, WARN_HEX(#_exp, r), ##__VA_ARGS__)
#define ASSERT_BOOL(_exp, _assert, _ret, ...)                                  \
    ASSERT(_exp, r, _assert, _ret, WARN_BOOL(#_exp, r), ##__VA_ARGS__)
#define ASSERT_SYS(_exp, _assert, _ret, ...)                                   \
    ASSERT(_exp, r, _assert, _ret, WARN_SYS(#_exp), ##__VA_ARGS__)

#define INIT0(_type) \
    (((union { _type a; char b[sizeof(_type)]; }){.b = {0}}).a)

#define ALIGN_WIDTH 64
#define SAMPLE_POINTS 1024
#define FRAME_NUMBER 16

static void *mpp_isp_thread_func(void *data);

gint mpp_disable(void) {

    g_message("MPP Disable");

    ASSERT_HEX(HI_MPI_SYS_Exit(), r == 0, -1);
    ASSERT_HEX(HI_MPI_VB_Exit(), r == 0, -1);

    return 0;
}

/* guint block_size, guint n_blocks */
gint mpp_enable(guint n_pools, ...) {

    g_message("MPP Enable");

    ASSERT_BOOL(n_pools <= VB_MAX_POOLS, r == TRUE, -1);

    if (n_pools > 0) {

        va_list ap;
        va_start(ap, n_pools);

        VB_CONF_S vb_conf = INIT0(VB_CONF_S);
        vb_conf.u32MaxPoolCnt = n_pools;

        for (guint i = 0; i < n_pools; i++) {
            vb_conf.astCommPool[i].u32BlkSize = va_arg(ap, guint);
            vb_conf.astCommPool[i].u32BlkCnt = va_arg(ap, guint);
        }

        va_end(ap);

        ASSERT_HEX(HI_MPI_VB_SetConf(&vb_conf), r == 0, -1);
        // ASSERT_HEX(HI_MPI_VB_SetSupplementConf(
        //     &(VB_SUPPLEMENT_CONF_S){VB_SUPPLEMENT_ISPSTAT_MASK}), r == 0, r);
        ASSERT_HEX(HI_MPI_VB_Init(), r == 0, -1);

        MPP_SYS_CONF_S sys_conf = {ALIGN_WIDTH};
        ASSERT_HEX(HI_MPI_SYS_SetConf(&sys_conf), r == 0, -1);
    }

    ASSERT_HEX(HI_MPI_SYS_Init(), r == 0, -1);

    return 0;
}

gint mpp_bind(MOD_ID_E src_mod, guint src_dev, guint src_chn,
              MOD_ID_E dest_mod, guint dest_dev, guint dest_chn) {

    g_message("Bind [src]%u:%u.%u [dest]%u:%u.%u", src_mod, src_dev, src_chn,
              dest_mod, dest_dev, dest_chn);

    MPP_CHN_S src = {src_mod, src_dev, src_chn};
    MPP_CHN_S dest = {dest_mod, dest_dev, dest_chn};

    ASSERT_HEX(HI_MPI_SYS_Bind(&src, &dest), r == 0, -1);
    return 0;
}

gint mpp_unbind(MOD_ID_E src_mod, guint src_dev, guint src_chn,
                MOD_ID_E dest_mod, guint dest_dev, guint dest_chn) {

    g_message("UnBind [src]%u:%u.%u [dest]%u:%u.%u", src_mod, src_dev, src_chn,
              dest_mod, dest_dev, dest_chn);

    MPP_CHN_S src = {src_mod, src_dev, src_chn};
    MPP_CHN_S dest = {dest_mod, dest_dev, dest_chn};

    ASSERT_HEX(HI_MPI_SYS_UnBind(&src, &dest), r == 0, -1);
    return 0;
}

guint64 mpp_get_pts(void) {
    guint64 pts = 0;
    ASSERT_HEX(HI_MPI_SYS_GetCurPts((void *)&pts), r == 0, 0);
    return pts * 1000ull;
}

void mpp_sync_pts(guint64 pts) {
    HI_MPI_SYS_SyncPts(pts / 1000ull);
}

void mpp_init_pts(guint64 pts) {
    HI_MPI_SYS_InitPtsBase(pts / 1000ull);
}

gint mpp_acodec_set_volume(guint volume) {

    gint fd = ASSERT_SYS(open("/dev/acodec", O_RDWR), r != -1, -1);
    ASSERT_SYS(ioctl(fd, ACODEC_SET_INPUT_VOL, &volume), r != -1, -1, close(fd));

    close(fd);
    return 0;
}

gint mpp_acodec_restart(ACODEC_MIXER_E input_mode,
                        AUDIO_SAMPLE_RATE_E sample_rate, guint volume) {

    g_message("ACodec Restart");

    ACODEC_FS_E freq;
    switch (sample_rate) {
    case AUDIO_SAMPLE_RATE_8000:
        freq = ACODEC_FS_8000;
        break;
    case AUDIO_SAMPLE_RATE_11025:
        freq = ACODEC_FS_11025;
        break;
    case AUDIO_SAMPLE_RATE_12000:
        freq = ACODEC_FS_12000;
        break;
    case AUDIO_SAMPLE_RATE_16000:
        freq = ACODEC_FS_16000;
        break;
    case AUDIO_SAMPLE_RATE_22050:
        freq = ACODEC_FS_22050;
        break;
    case AUDIO_SAMPLE_RATE_24000:
        freq = ACODEC_FS_24000;
        break;
    case AUDIO_SAMPLE_RATE_32000:
        freq = ACODEC_FS_32000;
        break;
    case AUDIO_SAMPLE_RATE_44100:
        freq = ACODEC_FS_44100;
        break;
    case AUDIO_SAMPLE_RATE_48000:
        freq = ACODEC_FS_48000;
        break;
    case AUDIO_SAMPLE_RATE_64000:
        freq = ACODEC_FS_64000;
        break;
    case AUDIO_SAMPLE_RATE_96000:
        freq = ACODEC_FS_96000;
        break;
    default:
        ASSERT_HEX(sample_rate, FALSE, -1);
    }

    gint fd = ASSERT_SYS(open("/dev/acodec", O_RDWR), r != -1, -1);
    ASSERT_SYS(ioctl(fd, ACODEC_SOFT_RESET_CTRL), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, ACODEC_SET_I2S1_FS, &freq), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, ACODEC_SET_MIXER_MIC, &input_mode), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, ACODEC_SET_INPUT_VOL, &volume), r != -1, -1, close(fd));
    close(fd);

    return 0;
}

gint mpp_ai_dev_disable(guint dev) {
    g_message("AiDev %u Disable", dev);
    ASSERT_HEX(HI_MPI_AI_Disable(dev), r == 0 || r == HI_ERR_AI_NOT_ENABLED,
               -1);
    return 0;
}

gint mpp_ai_dev_enable(guint dev, gboolean stereo,
                       AUDIO_SAMPLE_RATE_E sample_rate, guint n_channels) {

    g_message("AiDev %u Enable", dev);

    AIO_ATTR_S attr = INIT0(AIO_ATTR_S);
    attr.enSoundmode = stereo ? AUDIO_SOUND_MODE_STEREO : AUDIO_SOUND_MODE_MONO;
    attr.enSamplerate = sample_rate;
    attr.u32ChnCnt = n_channels;
    attr.enBitwidth = AUDIO_BIT_WIDTH_16;
    attr.enWorkmode = AIO_MODE_I2S_MASTER;
    attr.u32FrmNum = FRAME_NUMBER;
    attr.u32PtNumPerFrm = SAMPLE_POINTS;
    attr.u32ClkSel = 0;
    ASSERT_HEX(HI_MPI_AI_SetPubAttr(dev, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_AI_Enable(dev), r == 0, -1);

    return 0;
}

gint mpp_ai_chn_disable(guint dev, guint chn) {
    g_message("AiChn %u.%u Disable", dev, chn);
    ASSERT_HEX(HI_MPI_AI_DisableChn(dev, chn),
               r == 0 || r == HI_ERR_AI_NOT_ENABLED, -1);
    return 0;
}

gint mpp_ai_chn_enable(guint dev, guint chn) {

    g_message("AiChn %u.%u Enable", dev, chn);

    AI_CHN_PARAM_S param = {FRAME_NUMBER};

    ASSERT_HEX(HI_MPI_AI_SetChnParam(dev, chn, &param), r == 0, -1);
    ASSERT_HEX(HI_MPI_AI_EnableChn(dev, chn), r == 0, -1);

    return 0;
}

gint mpp_ai_chn_get_fd(guint dev, guint chn) {
    return ASSERT_HEX(HI_MPI_AI_GetFd(dev, chn), r > 0, -1);
}

gint mpp_ai_chn_read(guint dev, guint chn, guint8 **buf, guint *size,
                     guint64 *pts, gint timeout) {

    AUDIO_FRAME_S frame = INIT0(AUDIO_FRAME_S);
    ASSERT_HEX(HI_MPI_AI_GetFrame(dev, chn, &frame, NULL, timeout), r == 0, -1);

    gint width;
    switch (frame.enBitwidth) {
    case AUDIO_BIT_WIDTH_8:
        width = 1;
        break;
    case AUDIO_BIT_WIDTH_16:
        width = 2;
        break;
    case AUDIO_BIT_WIDTH_24:
        width = 3;
        break;
    default:
        ASSERT_HEX(frame.enBitwidth, FALSE, -1,
                   HI_MPI_AI_ReleaseFrame(dev, chn, &frame, NULL));
    }

    guint64 pts_ = frame.u64TimeStamp * 1000ull;
    guint size_;
    guint8 *buf_;

    switch (frame.enSoundmode) {
    case AUDIO_SOUND_MODE_MONO:
        size_ = frame.u32Len;
        // buf_ = ASSERT_SYS(malloc(size_), r != NULL, -1,
        // HI_MPI_AI_ReleaseFrame(dev, chn, &frame, NULL));
        // memcpy(buf, frame.pVirAddr[0], size_);
        buf_ = g_memdup(frame.pVirAddr[0], size_);
        break;
    case AUDIO_SOUND_MODE_STEREO:
        size_ = frame.u32Len * 2;
        // buf_ = ASSERT_SYS(malloc(size_), r != NULL, -1,
        // HI_MPI_AI_ReleaseFrame(dev, chn, &frame, NULL));
        buf_ = g_malloc(size_);
        guint8 *pbuf = buf_;
        guint8 *pl = frame.pVirAddr[0];
        guint8 *pr = frame.pVirAddr[1];
        for (guint i = 0; i < size_; i += width * 2) {
            memcpy(pbuf, pl, width);
            pbuf += width;
            pl += width;
            memcpy(pbuf, pr, width);
            pbuf += width;
            pr += width;
        }
        break;
    default:
        ASSERT_HEX(frame.enSoundmode, FALSE, -1,
                   HI_MPI_AI_ReleaseFrame(dev, chn, &frame, NULL));
    }

    HI_MPI_AI_ReleaseFrame(dev, chn, &frame, NULL);

    *buf = buf_;
    *size = size_;
    *pts = pts_;

    return 0;
}

#if 0
gint mpp_aenc_chn_disable(guint chn) {
    ASSERT_HEX(HI_MPI_AENC_DestroyChn(chn), r == 0, -1);
    return 0;
}

gint mpp_aenc_chn_enable_lpcm(guint chn) {

    AENC_ATTR_LPCM_S lpcm_attr = {};
    AENC_CHN_ATTR_S attr = {PT_LPCM, SAMPLE_POINTS, FRAME_NUMBER, &lpcm_attr};

    ASSERT_HEX(HI_MPI_AENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}
#endif

#if 0
gint mpp_mipi_restart(guint dev, raw_data_type_e type, guint x, guint y,
                      guint w, guint h, guint n_lanes, ...) {

    g_message("MIPI %u Restart", dev);

    gint fd = ASSERT_SYS(open("/dev/hi_mipi", O_RDWR), r != -1, -1);

    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_SENSOR, &dev), r != -1, -1, close(fd));

    combo_dev_attr_t attr = {};
    attr.devno = dev;
    attr.input_mode = INPUT_MODE_MIPI;
    attr.img_rect.x = x;
    attr.img_rect.y = y;
    attr.img_rect.width = w;
    attr.img_rect.height = h;
    attr.mipi_attr.raw_data_type = type;

    va_list ap;
    va_start(ap, n_lanes);

    for (guint i = 0; i < G_N_ELEMENTS(attr.mipi_attr.lane_id); i++) {
        attr.mipi_attr.lane_id[i] = -1;
    }

    for (guint i = 0; i < n_lanes; i++) {
        guint idx = va_arg(ap, guint);
        attr.mipi_attr.lane_id[idx] = i;
    }

    va_end(ap);

    ASSERT_SYS(ioctl(fd, HI_MIPI_SET_DEV_ATTR, &attr), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_SENSOR, &dev), r != -1, -1, close(fd));

    close(fd);
    return 0;
}
#endif

gint mpp_ov4689_mipi_restart(guint dev, guint x, guint y, guint w, guint h) {

    g_message("MIPI %u Restart", dev);

    gint fd = ASSERT_SYS(open("/dev/hi_mipi", O_RDWR), r != -1, -1);

    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_SENSOR, &dev), r != -1, -1, close(fd));

    combo_dev_attr_t attr = INIT0(combo_dev_attr_t);
    attr.devno = dev;
    attr.input_mode = INPUT_MODE_MIPI;
    attr.img_rect.x = x;
    attr.img_rect.y = y;
    attr.img_rect.width = w;
    attr.img_rect.height = h;

    attr.mipi_attr.raw_data_type = RAW_DATA_12BIT;

    attr.mipi_attr.lane_id[0] = 0;
    attr.mipi_attr.lane_id[1] = 1;
    attr.mipi_attr.lane_id[2] = 2;
    attr.mipi_attr.lane_id[3] = 3;
    attr.mipi_attr.lane_id[4] = -1;
    attr.mipi_attr.lane_id[5] = -1;
    attr.mipi_attr.lane_id[6] = -1;
    attr.mipi_attr.lane_id[7] = -1;

    ASSERT_SYS(ioctl(fd, HI_MIPI_SET_DEV_ATTR, &attr), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_SENSOR, &dev), r != -1, -1, close(fd));

    close(fd);
    return 0;
}
#if 0
gint mpp_imx117_lvds_restart(guint dev, guint x, guint y, guint w, guint h, guint width, guint channels) {

    g_message("LVDS %u Restart", dev);

    ASSERT_BOOL(width == 10 || width == 12, r == TRUE, -1);
    ASSERT_BOOL(channels == 4 || channels == 10, r == TRUE, -1);

    gint fd = ASSERT_SYS(open("/dev/hi_mipi", O_RDWR), r != -1, -1);

    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_SENSOR, &dev), r != -1, -1, close(fd));

    combo_dev_attr_t attr = INIT0(combo_dev_attr_t);

    attr.devno = dev;
    attr.input_mode = INPUT_MODE_LVDS;
    attr.phy_clk_share = PHY_CLK_SHARE_PHY0;
    attr.img_rect.x = x;
    attr.img_rect.y = y;
    attr.img_rect.width = w;
    attr.img_rect.height = h;

    attr.lvds_attr.sync_mode = LVDS_SYNC_MODE_SAV;
    attr.lvds_attr.vsync_type.sync_type = LVDS_VSYNC_NORMAL;
    attr.lvds_attr.data_endian = LVDS_ENDIAN_BIG;
    attr.lvds_attr.sync_code_endian = LVDS_ENDIAN_BIG;

    switch (width) {
        case 10:
            attr.lvds_attr.raw_data_type = RAW_DATA_10BIT;
            for (gsize i = 0; i < G_N_ELEMENTS(attr.lvds_attr.sync_code); i++) {
                for (gsize j = 0; j < G_N_ELEMENTS(attr.lvds_attr.sync_code[i]); j++) {
                    attr.lvds_attr.sync_code[i][j][0] = 0x2ac;
                    attr.lvds_attr.sync_code[i][j][1] = 0x2d8;
                    attr.lvds_attr.sync_code[i][j][2] = 0x200;
                    attr.lvds_attr.sync_code[i][j][3] = 0x274;
                }
            }
            break;
        case 12:
            attr.lvds_attr.raw_data_type = RAW_DATA_12BIT;
            for (gsize i = 0; i < G_N_ELEMENTS(attr.lvds_attr.sync_code); i++) {
                for (gsize j = 0; j < G_N_ELEMENTS(attr.lvds_attr.sync_code[i]); j++) {
                    attr.lvds_attr.sync_code[i][j][0] = 0xab0;
                    attr.lvds_attr.sync_code[i][j][1] = 0xb60;
                    attr.lvds_attr.sync_code[i][j][2] = 0x800;
                    attr.lvds_attr.sync_code[i][j][3] = 0x9d0;
                }
            }
            break;
        default:
            abort();
    }

    switch (channels) {
        case 4:
            attr.lvds_attr.lane_id[0] = -1;
            attr.lvds_attr.lane_id[1] = -1;
            attr.lvds_attr.lane_id[2] = 0;
            attr.lvds_attr.lane_id[3] = -1;
            attr.lvds_attr.lane_id[4] = 1;
            attr.lvds_attr.lane_id[5] = 2;
            attr.lvds_attr.lane_id[6] = -1;
            attr.lvds_attr.lane_id[7] = 3;
            attr.lvds_attr.lane_id[8] = -1;
            attr.lvds_attr.lane_id[9] = -1;
            attr.lvds_attr.lane_id[10] = -1;
            attr.lvds_attr.lane_id[11] = -1;
            break;
        case 10:
            attr.lvds_attr.lane_id[0] = 0;
            attr.lvds_attr.lane_id[1] = 1;
            attr.lvds_attr.lane_id[2] = 2;
            attr.lvds_attr.lane_id[3] = 3;
            attr.lvds_attr.lane_id[4] = 4;
            attr.lvds_attr.lane_id[5] = 5;
            attr.lvds_attr.lane_id[6] = 6;
            attr.lvds_attr.lane_id[7] = 7;
            attr.lvds_attr.lane_id[8] = 8;
            attr.lvds_attr.lane_id[9] = 9;
            attr.lvds_attr.lane_id[10] = -1;
            attr.lvds_attr.lane_id[11] = -1;
            break;
        default:
            abort();
    }

    ASSERT_SYS(ioctl(fd, HI_MIPI_SET_DEV_ATTR, &attr), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_SENSOR, &dev), r != -1, -1, close(fd));

    close(fd);
    return 0;
}
#endif

gint mpp_imx117_lvds_restart(guint dev, guint x, guint y, guint w, guint h, guint width, guint channels) {
 
    g_message("MIPI %u Restart", dev);
    width = width;
    channels = channels;   //avoid warning
    gint fd = ASSERT_SYS(open("/dev/hi_mipi", O_RDWR), r != -1, -1);
 
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_SENSOR, &dev), r != -1, -1, close(fd));
 
    combo_dev_attr_t attr = INIT0(combo_dev_attr_t);
    attr.devno = dev;
    attr.input_mode = INPUT_MODE_MIPI;
    attr.phy_clk_share = PHY_CLK_SHARE_NONE;
    attr.img_rect.x = x;
    attr.img_rect.y = y;
    attr.img_rect.width = w;
    attr.img_rect.height = h;
#if 1
    attr.img_rect.x = 0;
    attr.img_rect.y = 0;
    attr.img_rect.width = 4000;
    attr.img_rect.height = 3000;
#endif 
    attr.mipi_attr.raw_data_type = RAW_DATA_12BIT;
    attr.mipi_attr.wdr_mode = HI_MIPI_WDR_MODE_NONE;

    attr.mipi_attr.lane_id[0] = 0;
    attr.mipi_attr.lane_id[1] = 1;
    attr.mipi_attr.lane_id[2] = 2;
    attr.mipi_attr.lane_id[3] = 3;
    attr.mipi_attr.lane_id[4] = -1;
    attr.mipi_attr.lane_id[5] = -1;
    attr.mipi_attr.lane_id[6] = -1;
    attr.mipi_attr.lane_id[7] = -1;
 
    ASSERT_SYS(ioctl(fd, HI_MIPI_SET_DEV_ATTR, &attr), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_SENSOR, &dev), r != -1, -1, close(fd));
 
    close(fd);
    return 0;
}

#if 0
gint mpp_imx226_lvds_restart(guint dev, guint x, guint y, guint w, guint h) {

    g_message("LVDS %u Restart", dev);

    gint fd = ASSERT_SYS(open("/dev/hi_mipi", O_RDWR), r != -1, -1);

    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_RESET_SENSOR, &dev), r != -1, -1, close(fd));

    combo_dev_attr_t attr = {};

    attr.devno = dev;
    attr.input_mode = INPUT_MODE_LVDS;
    attr.phy_clk_share = PHY_CLK_SHARE_PHY0;
    attr.img_rect.x = x;
    attr.img_rect.y = y;
    attr.img_rect.width = w;
    attr.img_rect.height = h;

    attr.lvds_attr.raw_data_type = RAW_DATA_12BIT;
    attr.lvds_attr.sync_mode = LVDS_SYNC_MODE_SAV;
    attr.lvds_attr.vsync_type.sync_type = LVDS_VSYNC_NORMAL;
    attr.lvds_attr.data_endian = LVDS_ENDIAN_BIG;
    attr.lvds_attr.sync_code_endian = LVDS_ENDIAN_BIG;

    attr.lvds_attr.lane_id[0] = 0;
    attr.lvds_attr.lane_id[1] = 1;
    attr.lvds_attr.lane_id[2] = 2;
    attr.lvds_attr.lane_id[3] = 3;
    attr.lvds_attr.lane_id[4] = 4;
    attr.lvds_attr.lane_id[5] = 5;
    attr.lvds_attr.lane_id[6] = 6;
    attr.lvds_attr.lane_id[7] = 7;
    attr.lvds_attr.lane_id[8] = 8;
    attr.lvds_attr.lane_id[9] = 9;
    attr.lvds_attr.lane_id[10] = -1;
    attr.lvds_attr.lane_id[11] = -1;

    for (gsize i = 0; i < G_N_ELEMENTS(attr.lvds_attr.sync_code); i++) {
        for (gsize j = 0; j < G_N_ELEMENTS(attr.lvds_attr.sync_code[i]); j++) {
            attr.lvds_attr.sync_code[i][j][0] = 0xab0;
            attr.lvds_attr.sync_code[i][j][1] = 0xb60;
            attr.lvds_attr.sync_code[i][j][2] = 0x800;
            attr.lvds_attr.sync_code[i][j][3] = 0x9d0;
        }
    }

    ASSERT_SYS(ioctl(fd, HI_MIPI_SET_DEV_ATTR, &attr), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_MIPI, &dev), r != -1, -1, close(fd));
    ASSERT_SYS(ioctl(fd, HI_MIPI_UNRESET_SENSOR, &dev), r != -1, -1, close(fd));

    close(fd);
    return 0;
}
#endif

#if 0
gint mpp_isp_register_sensor(guint dev, const ISP_SNS_OBJ_S *sensor) {

    ALG_LIB_S ae_lib = {dev, HI_AE_LIB_NAME};
    ALG_LIB_S awb_lib = {dev, HI_AWB_LIB_NAME};

    ASSERT_HEX(sensor->pfnRegisterCallback(dev, &ae_lib, &awb_lib), r == 0, -1);
    ASSERT_HEX(HI_MPI_AE_Register(dev, &ae_lib), r == 0, -1);
    ASSERT_HEX(HI_MPI_AWB_Register(dev, &awb_lib), r == 0, -1);

    return 0;
}

gint mpp_isp_register_ov4689(guint dev) {
    return mpp_isp_register_sensor(dev, &stSnsOv4689SlaveObj);
}

extern void imx117_init(int dev);

gint mpp_isp_register_imx117(guint dev) {
    //return mpp_isp_register_sensor(dev, &stSnsImx117Obj);
    imx117_init(dev);
    return 0;
}

gint mpp_isp_register_imx226(guint dev) {
    return mpp_isp_register_sensor(dev, &stSnsImx226Obj);
}
#endif

extern void imx117_init(int dev);
extern void imx117_deinit(int dev);

extern void imx577_init(int dev);
extern void imx577_deinit(int dev);

gint mpp_isp_register_imx117(guint dev) {
    HI_U64 pu64CurPts;
    HI_MPI_SYS_GetCurPts(&pu64CurPts);
    printf("pu64CurPts before is %llu\n",pu64CurPts);
    imx577_init(dev);
    
    HI_MPI_SYS_GetCurPts(&pu64CurPts);
    printf("pu64CurPts after is %llu\n",pu64CurPts);
    return 0;
}
 
gint mpp_isp_unregister_imx117(guint dev) {
    imx577_deinit(dev);
    return 0;
}

static pthread_t isp_pid[VIU_MAX_DEV_NUM];

gint mpp_isp_disable(guint dev) {
    g_message("ISP %d Disable", dev);
    ASSERT_HEX(HI_MPI_ISP_Exit(dev), r == 0, -1);
    pthread_join(isp_pid[dev], NULL);
    //g_message("ISP Thread Exit: %d", ret);
    return 0;
}

void *mpp_isp_thread_func(void *data) {

    //ASSERT_SYS(pthread_detach(pthread_self()), r == 0, NULL);

    gint dev = (gint)(glong)data;
    g_message("ISP %d Start", dev);
    ASSERT_HEX(HI_MPI_ISP_Run((gint)(long)data), r == 0, NULL);
    g_message("ISP %d Stop", dev);

    return NULL;
}

void mpp_isp_hv_sync_disable(guint dev) {
    ISP_SLAVE_SNS_SYNC_S sync = INIT0(ISP_SLAVE_SNS_SYNC_S);
    HI_MPI_ISP_GetHvSync(dev, &sync);
    sync.unCfg.stBits.bitHEnable = 0;
    sync.unCfg.stBits.bitVEnable = 0;
    HI_MPI_ISP_SetFMWState(dev, ISP_FMW_STATE_FREEZE);
    HI_MPI_ISP_SetHvSync(dev, &sync);
}

void mpp_isp_hv_sync_enable(guint dev) {
    ISP_SLAVE_SNS_SYNC_S sync = INIT0(ISP_SLAVE_SNS_SYNC_S);
    HI_MPI_ISP_GetHvSync(dev, &sync);
    sync.unCfg.stBits.bitHEnable = 1;
    sync.unCfg.stBits.bitVEnable = 1;
    HI_MPI_ISP_SetHvSync(dev, &sync);
    HI_MPI_ISP_SetFMWState(dev, ISP_FMW_STATE_RUN);
}

gint mpp_isp_enable_dynamic_correction(guint dev, guint16 slope, guint16 thresh, guint16 hp_thresh) {
    ISP_DP_ATTR_S dp_attr = INIT0(ISP_DP_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetDPAttr(dev, &dp_attr), r == 0, -1);
    dp_attr.stDynamicAttr.bEnable = TRUE;
    dp_attr.stDynamicAttr.u16Slope = slope;
    dp_attr.stDynamicAttr.u16Thresh = thresh;
    dp_attr.stDynamicAttr.u16HpThresh = hp_thresh;
    ASSERT_HEX(HI_MPI_ISP_SetDPAttr(dev, &dp_attr), r == 0, -1);
    return 0;
}

gint mpp_isp_disable_dynamic_correction(guint dev) {
    ISP_DP_ATTR_S dp_attr = INIT0(ISP_DP_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetDPAttr(dev, &dp_attr), r == 0, -1);
    dp_attr.stDynamicAttr.bEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetDPAttr(dev, &dp_attr), r == 0, -1);
    return 0;
}

gint mpp_isp_enable(guint dev, gboolean stitch, float framerate,
                    ISP_BAYER_FORMAT_E bayer, guint x, guint y, guint w,
                    guint h) {

    g_message("ISP %d Enable", dev);

    ISP_PUB_ATTR_S attr = INIT0(ISP_PUB_ATTR_S);
    attr.f32FrameRate = framerate;
    attr.enBayer = bayer;
    attr.stWndRect.s32X = x;
    attr.stWndRect.s32Y = y;
    attr.stWndRect.u32Width = w;
    attr.stWndRect.u32Height = h;
    attr.stSnsSize.u32Width = w;
    attr.stSnsSize.u32Height = h;

    ASSERT_HEX(HI_MPI_ISP_MemInit(dev), r == 0, -1);
    ASSERT_HEX(HI_MPI_ISP_SetPubAttr(dev, &attr), r == 0, -1);

    ISP_WDR_MODE_S wdr = {WDR_MODE_NONE};
    ASSERT_HEX(HI_MPI_ISP_SetWDRMode(dev, &wdr), r == 0, -1);

    if (stitch) {
        ASSERT_HEX(HI_MPI_ISP_SetMode(dev, ISP_MODE_STITCH), r == 0, -1);
    }

    ASSERT_HEX(HI_MPI_ISP_Init(dev), r == 0, -1);

    ISP_MODULE_CTRL_U module = INIT0(ISP_MODULE_CTRL_U);
    ASSERT_HEX(HI_MPI_ISP_GetModuleControl(dev, &module), r == 0, -1);
    module.bitBypassISPDGain = 1;
    ASSERT_HEX(HI_MPI_ISP_SetModuleControl(dev, &module), r == 0, -1);

    ISP_NR_ATTR_S nr_attr = INIT0(ISP_NR_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetNRAttr(dev, &nr_attr), r == 0, -1);
    nr_attr.bEnable = TRUE;
    nr_attr.enOpType = OP_TYPE_MANUAL;
    nr_attr.stManual.u8Thresh = 0x10;
    nr_attr.stManual.u8ThreshLong = 0x10;
    ASSERT_HEX(HI_MPI_ISP_SetNRAttr(dev, &nr_attr), r == 0, -1);

    guint8 weight[AE_ZONE_ROW][AE_ZONE_COLUMN] = {
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0},
        {0,0,0,0,0,8,8,8,8,8,8,8,0,0,0,0,0}
    };

    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);
    memcpy(expo_attr.stAuto.au8Weight, weight, sizeof(expo_attr.stAuto.au8Weight));
    // expo_attr.stAuto.enAEStrategyMode = AE_EXP_LOWLIGHT_PRIOR;
    expo_attr.stAuto.u8MaxHistOffset = 0x80;
    ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);

    // g_print("Weight: \n");
    // for (guint i = 0; i < AE_ZONE_ROW; i++) {
    //     for (guint j = 0; j < AE_ZONE_COLUMN; j++) {
    //         g_print("%hhu ", expo_attr.stAuto.au8Weight[i][j]);
    //     }
    //     g_print("\n");
    // }

    // ISP_SNAP_ATTR_S snap_attr = INIT0(ISP_SNAP_ATTR_S);
    // ASSERT_HEX(HI_MPI_ISP_GetSnapAttr(dev, &snap_attr), r == 0, -1);

    // g_print("snap type: %d\n", snap_attr.enSnapType);
    // g_print("pipe mode: %d\n", snap_attr.enSnapPipeMode);
    // g_print("use ccm: %d\n", snap_attr.bUseInfoCCM);

    // ISP_ACM_ATTR_S acm_attr = INIT0(ISP_ACM_ATTR_S);
    // acm_attr.bEnable = TRUE;
    // acm_attr.bDemoEnable = FALSE;
    // acm_attr.u32Stretch = 1;
    // acm_attr.u32ClipRange = 1;
    // acm_attr.u32AcmClipOrWrap = 0;
    // acm_attr.u32CbcrThr = 0;
    // acm_attr.u32GainLuma = 200;
    // acm_attr.u32GainHue = 200;
    // acm_attr.u32GainSat = 200;
    // ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &acm_attr), r == 0, -1);

    ASSERT_SYS(
        pthread_create(&isp_pid[dev], NULL, mpp_isp_thread_func, (void *)(long)dev),
        r == 0, -1);

    return 0;
}

gint mpp_vi_dev_disable(guint dev) {
    g_message("ViDev %u Disable", dev);
    ASSERT_HEX(HI_MPI_VI_DisableDev(dev), r == 0, -1);
    return 0;
}

gint mpp_vi_dev_enable_mipi(guint dev, guint x, guint y, guint w, guint h, gboolean bypass_isp) {

    g_message("ViDev %u Enable MIPI", dev);

    VI_DEV_ATTR_S attr = INIT0(VI_DEV_ATTR_S);

    attr.enIntfMode = VI_MODE_MIPI;
    attr.stDevRect.s32X = x;
    attr.stDevRect.s32Y = y;
    attr.stDevRect.u32Width = w;
    attr.stDevRect.u32Height = h;
    attr.stBasAttr.stSacleAttr.stBasSize.u32Width = w;
    attr.stBasAttr.stSacleAttr.stBasSize.u32Height = h;

    attr.enScanMode = VI_SCAN_PROGRESSIVE;
    attr.enWorkMode = VI_WORK_MODE_1Multiplex;
    attr.s32AdChnId[0] = -1;
    attr.s32AdChnId[1] = -1;
    attr.s32AdChnId[2] = -1;
    attr.s32AdChnId[3] = -1;

    attr.au32CompMask[0] = 0xFFF00000;
    attr.enDataPath = bypass_isp ? VI_PATH_BYPASS : VI_PATH_ISP;
    attr.enInputDataType = VI_DATA_TYPE_RGB;

    ASSERT_HEX(HI_MPI_VI_SetDevAttr(dev, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_EnableDev(dev), r == 0, -1);

    return 0;
}

gint mpp_vi_dev_enable_lvds(guint dev, guint x, guint y, guint w, guint h, gboolean bypass_isp) {

    g_message("ViDev %u Enable LVDS", dev);

    VI_DEV_ATTR_S attr = INIT0(VI_DEV_ATTR_S);

    attr.enIntfMode = VI_MODE_LVDS;
    attr.stDevRect.s32X = x;
    attr.stDevRect.s32Y = y;
    attr.stDevRect.u32Width = w;
    attr.stDevRect.u32Height = h;
    attr.stBasAttr.stSacleAttr.stBasSize.u32Width = w;
    attr.stBasAttr.stSacleAttr.stBasSize.u32Height = h;

    attr.enScanMode = VI_SCAN_PROGRESSIVE;
    attr.enWorkMode = VI_WORK_MODE_1Multiplex;
    attr.s32AdChnId[0] = -1;
    attr.s32AdChnId[1] = -1;
    attr.s32AdChnId[2] = -1;
    attr.s32AdChnId[3] = -1;

    attr.au32CompMask[0] = 0xFFF00000;
    attr.enDataPath = bypass_isp ? VI_PATH_BYPASS : VI_PATH_ISP;
    attr.enInputDataType = VI_DATA_TYPE_RGB;

    ASSERT_HEX(HI_MPI_VI_SetDevAttr(dev, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_EnableDev(dev), r == 0, -1);

    return 0;
}

gint mpp_vi_dev_disable_dump(guint dev) {
    ASSERT_HEX(HI_MPI_VI_DisableBayerDump(dev), r == 0, -1);
    return 0;
}

gint mpp_vi_dev_enable_dump(guint dev, guint depth) {

    VI_DUMP_ATTR_S attr = INIT0(VI_DUMP_ATTR_S);
    attr.enDumpType = VI_DUMP_TYPE_RAW;
    attr.enPixelFormat = PIXEL_FORMAT_RGB_BAYER_12BPP;
    attr.stCropInfo.bEnable = FALSE;
    attr.enDumpSel = VI_DUMP_SEL_PORT;
    ASSERT_HEX(HI_MPI_VI_SetDevDumpAttr(dev, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_EnableBayerDump(dev), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_SetFrameDepth(19, depth), r == 0, -1);

    return 19;
}

gint mpp_vi_dev_disable_snap(guint dev) {
    ASSERT_HEX(HI_MPI_VI_DisableSnap(dev), r == 0, r);
    return 0;
}

gint mpp_vi_dev_enable_snap(guint dev, guint isp, guint depth) {

    ISP_SNAP_ATTR_S isp_attr = INIT0(ISP_SNAP_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetSnapAttr(isp, &isp_attr), r == 0, r);
    isp_attr.enSnapType = SNAP_TYPE_NORMAL;
    isp_attr.enSnapPipeMode = ISP_SNAP_PREVIEW;
    isp_attr.bUseInfoCCM = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetSnapAttr(isp, &isp_attr), r == 0, r);

    VI_SNAP_ATTR_S attr = INIT0(VI_SNAP_ATTR_S);
    attr.enSnapType = SNAP_TYPE_USER;
    attr.IspDev = isp;
    attr.u32RefFrameNum = 0;
    attr.unAttr.stUserAttr.s32SrcFrameRate = -1;
    attr.unAttr.stUserAttr.s32DstFrameRate = -1;
    attr.unAttr.stUserAttr.u32FrameDepth = depth;
    ASSERT_HEX(HI_MPI_VI_SetSnapAttr(dev, &attr), r == 0, r);
    ASSERT_HEX(HI_MPI_VI_EnableSnap(dev), r == 0, r);

    return 0;
}

gint mpp_vi_chn_disable(guint chn) {
    g_message("ViChn %u Disable", chn);
    ASSERT_HEX(HI_MPI_VI_DisableChn(chn), r == 0, r);
    return 0;
}

gint mpp_vi_chn_enable(guint chn, guint x, guint y, guint w, guint h,
                       gboolean mirror, gboolean flip, gboolean bypass_isp) {

    g_message("ViChn %u Enable", chn);

    VI_CHN_ATTR_S attr = INIT0(VI_CHN_ATTR_S);

    attr.stCapRect.s32X = x;
    attr.stCapRect.s32Y = y;
    attr.stCapRect.u32Width = w;
    attr.stCapRect.u32Height = h;
    attr.stDestSize.u32Width = w;
    attr.stDestSize.u32Height = h;
    attr.bMirror = mirror;
    attr.bFlip = flip;

    attr.enPixFormat = bypass_isp ? PIXEL_FORMAT_RGB_BAYER : PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    attr.enCapSel = VI_CAPSEL_BOTH;
    attr.s32SrcFrameRate = -1;
    attr.s32DstFrameRate = -1;

    if (bypass_isp)
        ASSERT_HEX(HI_MPI_VI_SetFrameDepth(chn, 1), r == 0, -1);

    ASSERT_HEX(HI_MPI_VI_SetChnAttr(chn, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_EnableChn(chn), r == 0, -1);

    return 0;
}

gint mpp_vi_chn_get_depth(guint chn, guint *depth) {
    ASSERT_HEX(HI_MPI_VI_GetFrameDepth(chn, depth), r == 0, -1);
    return 0;
}

gint mpp_vi_chn_set_depth(guint chn, guint depth) {
    ASSERT_HEX(HI_MPI_VI_SetFrameDepth(chn, depth), r == 0, -1);
    return 0;
}

gint mpp_vi_chn_get_pts(guint chn, guint64 *pts) {

    ASSERT_HEX(HI_MPI_VI_SetFrameDepth(chn, 0), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_SetFrameDepth(chn, 1), r == 0, -1);

    VIDEO_FRAME_INFO_S info = INIT0(VIDEO_FRAME_INFO_S);
    while (ASSERT_HEX(HI_MPI_VI_GetFrame(chn, &info, -1),
                      r == 0 || r == HI_ERR_VPSS_BUF_EMPTY,
                      -1) == HI_ERR_VPSS_BUF_EMPTY) {
    }

    *pts = info.stVFrame.u64pts * 1000ull;
    HI_MPI_VI_ReleaseFrame(chn, &info);

    ASSERT_HEX(HI_MPI_VI_SetFrameDepth(chn, 0), r == 0, -1);

    return 0;
}

gint mpp_vi_chn_get_fd(guint chn) {
    return ASSERT_HEX(HI_MPI_VI_GetFd(chn), r > 0, -1);
}

gint mpp_vi_chn_get_frame(guint chn, VIDEO_FRAME_INFO_S *frame, gint timeout) {
    ASSERT_HEX(HI_MPI_VI_GetFrame(chn, frame, timeout), r == 0, -1);
    return 0;
}

gint mpp_vi_chn_release_frame(guint chn, VIDEO_FRAME_INFO_S *frame) {
    ASSERT_HEX(HI_MPI_VI_ReleaseFrame(chn, frame), r == 0, -1);
    return 0;
}

guint64 mpp_video_frame_info_get_pts(VIDEO_FRAME_INFO_S *info) {
    return info->stVFrame.u64pts * 1000ull;
}

gpointer mpp_video_frame_info_read_data(VIDEO_FRAME_INFO_S *info, guint *size) {

    const VIDEO_FRAME_S *frame = &info->stVFrame;

    guint32 stride = frame->u32Stride[0];
    guint32 width = frame->u32Width;
    guint32 height = frame->u32Height;
    guint32 stride_;

    switch (frame->enPixelFormat) {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            stride_ = width;
            break;
        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            stride_ = (width * 10) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            stride_ = (width * 12) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            stride_ = (width * 14) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER:
            stride_ = width * 2;
            break;
        default:
            ASSERT_HEX(frame->enPixelFormat, FALSE, NULL);
    }

    guint8(*src)[height][stride] =
        HI_MPI_SYS_Mmap(frame->u32PhyAddr[0], sizeof(*src));

    guint8(*dst)[height][stride_] = g_malloc(sizeof(*dst));

    if (stride == stride_) {
        memcpy(dst, src, sizeof(*dst));
    } else {
        for (guint32 i = 0; i < height; i++) {
            memcpy((*dst)[i], (*src)[i], sizeof((*dst)[i]));
        }
    }

    HI_MPI_SYS_Munmap(src, sizeof(*src));

    *size = sizeof(*dst);
    return dst;
}

#if 0
gint mpp_vi_chn_read(guint chn, gpointer *buf, guint *size, guint64 *pts, gint timeout) {

    VIDEO_FRAME_INFO_S info = INIT0(VIDEO_FRAME_INFO_S);
    ASSERT_HEX(HI_MPI_VI_GetFrame(chn, &info, timeout), r == 0, -1);

    VIDEO_FRAME_S *frame = &info.stVFrame;

    guint64 pts_ = frame->u64pts * 1000ull;
    guint32 stride = frame->u32Stride[0];
    guint32 width = frame->u32Width;
    guint32 height = frame->u32Height;
    guint32 stride_;

    switch (frame->enPixelFormat) {
        case PIXEL_FORMAT_RGB_BAYER_8BPP:
            stride_ = width;
            break;
        case PIXEL_FORMAT_RGB_BAYER_10BPP:
            ASSERT_HEX((width * 10) % 8, 0, -1, HI_MPI_VI_ReleaseFrame(chn, &info));
            stride_ = (width * 10) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER_12BPP:
            ASSERT_HEX((width * 12) % 8, 0, -1, HI_MPI_VI_ReleaseFrame(chn, &info));
            stride_ = (width * 12) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER_14BPP:
            ASSERT_HEX((width * 14) % 8, 0, -1, HI_MPI_VI_ReleaseFrame(chn, &info));
            stride_ = (width * 14) / 8;
            break;
        case PIXEL_FORMAT_RGB_BAYER:
            stride_ = width * 2;
            break;
        default:
            ASSERT_HEX(frame->enPixelFormat, FALSE, -1,
                       HI_MPI_VI_ReleaseFrame(chn, &info));
    }

    guint8(*src)[height][stride] =
        HI_MPI_SYS_Mmap(frame->u32PhyAddr[0], sizeof(*src));
    guint8(*dst)[height][stride_] = g_malloc(sizeof(*dst));
    if (stride == stride_) {
        memcpy(dst, src, sizeof(*dst));
    } else {
        for (guint32 i = 0; i < height; i++) {
            memcpy((*dst)[i], (*src)[i], sizeof((*dst)[i]));
        }
    }
    HI_MPI_SYS_Munmap(src, sizeof(*src));
    HI_MPI_VI_ReleaseFrame(chn, &info);

    *buf = *dst;
    *size = sizeof(*dst);
    *pts = pts_;

    return 0;
}
#endif

#if 0
gint mpp_vi_chn_enable_ext(guint chn, guint bind_chn, guint w, guint h) {

    VI_EXT_CHN_ATTR_S attr = {};
    attr.s32BindChn = bind_chn;
    attr.stDestSize.u32Width = w;
    attr.stDestSize.u32Height = h;

    attr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    attr.s32SrcFrameRate = -1;
    attr.s32DstFrameRate = -1;

    ASSERT_HEX(HI_MPI_VI_SetExtChnAttr(chn, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VI_EnableChn(chn), r == 0, -1);

    return 0;
}
#endif

gint mpp_vpss_grp_disable(guint grp) {

    g_message("VpssGrp %u Disable", grp);

    ASSERT_HEX(HI_MPI_VPSS_StopGrp(grp), r == 0 || r == HI_ERR_VPSS_UNEXIST, -1);
    ASSERT_HEX(HI_MPI_VPSS_DestroyGrp(grp), r == 0 || r == HI_ERR_VPSS_UNEXIST, -1);

    return 0;
}

gint mpp_vpss_grp_enable(guint grp, guint wl, guint wr, guint h, gboolean nr_enable) {

    g_message("VpssGrp %u Enable", grp);

    VPSS_GRP_ATTR_S attr = INIT0(VPSS_GRP_ATTR_S);

    attr.u32MaxW = wl + wr;
    attr.u32MaxH = h;
    attr.enDieMode = VPSS_DIE_MODE_NODIE;
    attr.enPixFmt = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    attr.bNrEn = nr_enable;

    if (nr_enable) {
        attr.stNrAttr.enNrType = VPSS_NR_TYPE_VIDEO;
        attr.stNrAttr.stNrVideoAttr.enNrRefSource = VPSS_NR_REF_FROM_RFR;
        attr.stNrAttr.stNrVideoAttr.enNrOutputMode = VPSS_NR_OUTPUT_NORMAL;
        attr.stNrAttr.u32RefFrameNum = 2;
    }

    if (wr > 0) {
        attr.bStitchBlendEn = TRUE;
        attr.enStitchBlendMode = VPSS_STITCH_BLEND_ADJUST;
        attr.stStitchBlendAttr.u32OutWidth = wl + wr;
        attr.stStitchBlendAttr.u32OutHeight = h;
        attr.stStitchBlendAttr.stOverlapPoint[0].s32X = wl;
        attr.stStitchBlendAttr.stOverlapPoint[0].s32Y = 0;
        attr.stStitchBlendAttr.stOverlapPoint[1].s32X = wl;
        attr.stStitchBlendAttr.stOverlapPoint[1].s32Y = 0;
        attr.stStitchBlendAttr.stOverlapPoint[2].s32X = wl;
        attr.stStitchBlendAttr.stOverlapPoint[2].s32Y = h - 1;
        attr.stStitchBlendAttr.stOverlapPoint[3].s32X = wl;
        attr.stStitchBlendAttr.stOverlapPoint[3].s32Y = h - 1;
    }

    ASSERT_HEX(HI_MPI_VPSS_CreateGrp(grp, &attr), r == 0, -1);

    if (nr_enable) {
        VPSS_GRP_NRS_PARAM_S nrs_param = INIT0(VPSS_GRP_NRS_PARAM_S);
        nrs_param.enNRVer = VPSS_NR_V3;
        ASSERT_HEX(HI_MPI_VPSS_GetGrpNRSParam(grp, &nrs_param), r == 0, -1);
        nrs_param.stNRSParam_V3.sf0_bright = 200;
        nrs_param.stNRSParam_V3.sf0_dark = 200;
        nrs_param.stNRSParam_V3.sfk_bright = 100;
        nrs_param.stNRSParam_V3.sfk_dark = 100;
        ASSERT_HEX(HI_MPI_VPSS_SetGrpNRSParam(grp, &nrs_param), r == 0, -1);
    }

    ASSERT_HEX(HI_MPI_VPSS_StartGrp(grp), r == 0, -1);

    return 0;
}

gint mpp_vpss_grp_send_frame(guint grp, guint pipe, VIDEO_FRAME_INFO_S *frame, gint timeout) {
    ASSERT_HEX(HI_MPI_VPSS_SendFrameEx(grp, pipe, frame, timeout), r == 0, -1);
    return 0;
}

gint mpp_vpss_chn_get_depth(guint grp, guint chn, guint *depth) {
    ASSERT_HEX(HI_MPI_VPSS_GetDepth(grp, chn, depth), r == 0, -1);
    return 0;
}

gint mpp_vpss_chn_set_depth(guint grp, guint chn, guint depth) {
    ASSERT_HEX(HI_MPI_VPSS_SetDepth(grp, chn, depth), r == 0, -1);
    return 0;
}

// gint mpp_vpss_chn_get_fd(guint grp, guint chn) {
//     return ASSERT_HEX(HI_MPI_VPSS_GetChnFd(grp, chn), r > 0, -1);
// }

gint mpp_vpss_chn_get_frame(guint grp, guint chn, VIDEO_FRAME_INFO_S *frame, gint timeout) {
    ASSERT_HEX(HI_MPI_VPSS_GetChnFrame(grp, chn, frame, timeout), r == 0, -1);
    return 0;
}

gint mpp_vpss_chn_release_frame(guint grp, guint chn, VIDEO_FRAME_INFO_S *frame) {
    ASSERT_HEX(HI_MPI_VPSS_ReleaseChnFrame(grp, chn, frame), r == 0, -1);
    return 0;
}

gint mpp_vpss_chn_disable(guint grp, guint chn) {
    g_message("VpssChn %u.%u Disable", grp, chn);
    ASSERT_HEX(HI_MPI_VPSS_DisableChn(grp, chn),
               r == 0 || r == HI_ERR_VPSS_UNEXIST, -1);
    return 0;
}

gint mpp_vpss_chn_enable(guint grp, guint chn, guint w, guint h,
                         gboolean mirror, gboolean flip,
                         gint src_fps, gint dest_fps) {

    g_message("VpssChn %u.%u Enable", grp, chn);

    VPSS_CHN_ATTR_S attr = INIT0(VPSS_CHN_ATTR_S);

    attr.bMirror = mirror;
    attr.bFlip = flip;
    attr.s32SrcFrameRate = src_fps;
    attr.s32DstFrameRate = dest_fps;

    VPSS_CHN_MODE_S mode = INIT0(VPSS_CHN_MODE_S);

    mode.enChnMode = VPSS_CHN_MODE_USER;
    mode.u32Width = w;
    mode.u32Height = h;
    mode.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    ASSERT_HEX(HI_MPI_VPSS_SetChnAttr(grp, chn, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VPSS_SetChnMode(grp, chn, &mode), r == 0, -1);
    ASSERT_HEX(HI_MPI_VPSS_EnableChn(grp, chn), r == 0, -1);

    return 0;
}

gint mpp_vpss_chn_get_pts(guint grp, guint chn, guint64 *pts) {

    ASSERT_HEX(HI_MPI_VPSS_SetDepth(grp, chn, 0), r == 0, -1);
    ASSERT_HEX(HI_MPI_VPSS_SetDepth(grp, chn, 1), r == 0, -1);

    VIDEO_FRAME_INFO_S info = INIT0(VIDEO_FRAME_INFO_S);
    while (ASSERT_HEX(HI_MPI_VPSS_GetChnFrame(grp, chn, &info, -1),
                      r == 0 || r == HI_ERR_VPSS_BUF_EMPTY,
                      -1) == HI_ERR_VPSS_BUF_EMPTY) {
    }

    *pts = info.stVFrame.u64pts * 1000ull;
    HI_MPI_VPSS_ReleaseChnFrame(grp, chn, &info);

    ASSERT_HEX(HI_MPI_VPSS_SetDepth(grp, chn, 0), r == 0, -1);

    return 0;
}

#if 0
gint mpp_vpss_chn_enable_ext(guint grp, guint chn, guint bind_chn, guint w, guint h) {

    VPSS_EXT_CHN_ATTR_S attr = {};

    attr.s32BindChn = bind_chn;
    attr.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    attr.u32Width = w;
    attr.u32Height = h;
    attr.s32SrcFrameRate = -1;
    attr.s32DstFrameRate = -1;

    ASSERT_HEX(HI_MPI_VPSS_SetExtChnAttr(grp, chn, &attr), r == 0, -1);
    ASSERT_HEX(HI_MPI_VPSS_EnableChn(grp, chn), r == 0, -1);

    return 0;
}
#endif

gint mpp_venc_chn_disable(guint chn) {

    g_message("VencChn %u Disable", chn);

    ASSERT_HEX(HI_MPI_VENC_StopRecvPic(chn), r == 0 || r == HI_ERR_VENC_UNEXIST,
               -1);
    ASSERT_HEX(HI_MPI_VENC_DestroyChn(chn), r == 0 || r == HI_ERR_VENC_UNEXIST,
               -1);

    return 0;
}

gint mpp_venc_chn_enable_jpeg(guint chn, guint w, guint h, guint q) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_JPEG;
    attr.stVeAttr.stAttrJpege.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrJpege.u32PicWidth = w;
    attr.stVeAttr.stAttrJpege.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrJpege.u32PicHeight = h;
    attr.stVeAttr.stAttrJpege.bByFrame = TRUE;
    attr.stVeAttr.stAttrJpege.bSupportDCF = FALSE;
    attr.stVeAttr.stAttrJpege.u32BufSize = w * h * 3 / 2;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    VENC_PARAM_JPEG_S param = INIT0(VENC_PARAM_JPEG_S);
    ASSERT_HEX(HI_MPI_VENC_GetJpegParam(chn, &param), r == 0, -1);
    param.u32Qfactor = q;
    ASSERT_HEX(HI_MPI_VENC_SetJpegParam(chn, &param), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_mjpegvbr(guint chn, guint framerate, guint kbps,
                                  guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_MJPEG;
    attr.stVeAttr.stAttrMjpege.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrMjpege.u32PicWidth = w;
    attr.stVeAttr.stAttrMjpege.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrMjpege.u32PicHeight = h;
    attr.stVeAttr.stAttrMjpege.bByFrame = TRUE;
    attr.stVeAttr.stAttrMjpege.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
    attr.stRcAttr.stAttrMjpegeVbr.u32StatTime = 1;
    attr.stRcAttr.stAttrMjpegeVbr.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrMjpegeVbr.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrMjpegeVbr.u32MaxBitRate = kbps;
    attr.stRcAttr.stAttrMjpegeVbr.u32MaxQfactor = 95;
    attr.stRcAttr.stAttrMjpegeVbr.u32MinQfactor = 90;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_h265cbr(guint chn, guint profile, guint framerate,
                                 guint kbps, guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_H265;
    attr.stVeAttr.stAttrH265e.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrH265e.u32PicWidth = w;
    attr.stVeAttr.stAttrH265e.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrH265e.u32PicHeight = h;
    attr.stVeAttr.stAttrH265e.u32Profile = profile;
    attr.stVeAttr.stAttrH265e.bByFrame = TRUE;
    attr.stVeAttr.stAttrH265e.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
    attr.stRcAttr.stAttrH265Cbr.u32StatTime = 1;
    attr.stRcAttr.stAttrH265Cbr.u32FluctuateLevel = 1;
    attr.stRcAttr.stAttrH265Cbr.u32Gop = framerate;
    attr.stRcAttr.stAttrH265Cbr.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrH265Cbr.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrH265Cbr.u32BitRate = kbps;
    attr.stGopAttr.enGopMode = VENC_GOPMODE_DUALP;
    attr.stGopAttr.stDualP.u32SPInterval = 3;
    attr.stGopAttr.stDualP.s32IPQpDelta = 0;
    attr.stGopAttr.stDualP.s32SPQpDelta = 0;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_h265fixqp(guint chn, guint profile, guint framerate,
                                   guint qp, guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_H265;
    attr.stVeAttr.stAttrH265e.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrH265e.u32PicWidth = w;
    attr.stVeAttr.stAttrH265e.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrH265e.u32PicHeight = h;
    attr.stVeAttr.stAttrH265e.u32Profile = profile;
    attr.stVeAttr.stAttrH265e.bByFrame = TRUE;
    attr.stVeAttr.stAttrH265e.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
    attr.stRcAttr.stAttrH265FixQp.u32Gop = framerate;
    attr.stRcAttr.stAttrH265FixQp.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrH265FixQp.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrH265FixQp.u32IQp = qp;
    attr.stRcAttr.stAttrH265FixQp.u32PQp = qp;
    attr.stRcAttr.stAttrH265FixQp.u32BQp = qp;
    attr.stGopAttr.enGopMode = VENC_GOPMODE_DUALP;
    attr.stGopAttr.stDualP.u32SPInterval = 3;
    attr.stGopAttr.stDualP.s32IPQpDelta = 0;
    attr.stGopAttr.stDualP.s32SPQpDelta = 0;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_h264cbr(guint chn, guint profile, guint framerate,
                                 guint kbps, guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_H264;
    attr.stVeAttr.stAttrH264e.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrH264e.u32PicWidth = w;
    attr.stVeAttr.stAttrH264e.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrH264e.u32PicHeight = h;
    attr.stVeAttr.stAttrH264e.u32Profile = profile;
    attr.stVeAttr.stAttrH264e.bByFrame = TRUE;
    attr.stVeAttr.stAttrH264e.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    attr.stRcAttr.stAttrH264Cbr.u32StatTime = 1;
    attr.stRcAttr.stAttrH264Cbr.u32FluctuateLevel = 1;
    attr.stRcAttr.stAttrH264Cbr.u32Gop = framerate;
    attr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrH264Cbr.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrH264Cbr.u32BitRate = kbps;
    attr.stGopAttr.enGopMode = VENC_GOPMODE_DUALP;
    attr.stGopAttr.stDualP.u32SPInterval = 3;
    attr.stGopAttr.stDualP.s32IPQpDelta = 0;
    attr.stGopAttr.stDualP.s32SPQpDelta = 0;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_h264fixqp(guint chn, guint profile, guint framerate,
                                   guint qp, guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);
    attr.stVeAttr.enType = PT_H264;
    attr.stVeAttr.stAttrH264e.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrH264e.u32PicWidth = w;
    attr.stVeAttr.stAttrH264e.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrH264e.u32PicHeight = h;
    attr.stVeAttr.stAttrH264e.u32Profile = profile;
    attr.stVeAttr.stAttrH264e.bByFrame = TRUE;
    attr.stVeAttr.stAttrH264e.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
    attr.stRcAttr.stAttrH264FixQp.u32Gop = framerate;
    attr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrH264FixQp.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrH264FixQp.u32IQp = qp;
    attr.stRcAttr.stAttrH264FixQp.u32PQp = qp;
    attr.stRcAttr.stAttrH264FixQp.u32BQp = qp;
    attr.stGopAttr.enGopMode = VENC_GOPMODE_DUALP;
    attr.stGopAttr.stDualP.u32SPInterval = 3;
    attr.stGopAttr.stDualP.s32IPQpDelta = 0;
    attr.stGopAttr.stDualP.s32SPQpDelta = 0;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_enable_h264avbr(guint chn, guint profile, guint framerate,
                                  guint kbps, guint w, guint h) {

    g_message("VencChn %u Enable", chn);

    VENC_CHN_ATTR_S attr = INIT0(VENC_CHN_ATTR_S);

    attr.stVeAttr.enType = PT_H264;
    attr.stVeAttr.stAttrH264e.u32MaxPicWidth = w;
    attr.stVeAttr.stAttrH264e.u32PicWidth = w;
    attr.stVeAttr.stAttrH264e.u32MaxPicHeight = h;
    attr.stVeAttr.stAttrH264e.u32PicHeight = h;
    attr.stVeAttr.stAttrH264e.u32Profile = profile;
    attr.stVeAttr.stAttrH264e.bByFrame = TRUE;
    attr.stVeAttr.stAttrH264e.u32BufSize = w * h * 3 / 2;
    attr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
    attr.stRcAttr.stAttrH264AVbr.u32StatTime = 1;
    attr.stRcAttr.stAttrH264AVbr.u32Gop = framerate;
    attr.stRcAttr.stAttrH264AVbr.u32SrcFrmRate = framerate;
    attr.stRcAttr.stAttrH264AVbr.fr32DstFrmRate = framerate;
    attr.stRcAttr.stAttrH264AVbr.u32MaxBitRate = kbps;
    attr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    attr.stGopAttr.stNormalP.s32IPQpDelta = 0;
    ASSERT_HEX(HI_MPI_VENC_CreateChn(chn, &attr), r == 0, -1);

    VENC_RC_PARAM_S param = INIT0(VENC_RC_PARAM_S);
    ASSERT_HEX(HI_MPI_VENC_GetRcParam(chn, &param), r == 0, -1);
    param.stParamH264AVbr.s32MinStillPercent = 60;
    ASSERT_HEX(HI_MPI_VENC_SetRcParam(chn, &param), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_stop(guint chn) {
    g_message("VencChn %u Stop", chn);
    ASSERT_HEX(HI_MPI_VENC_StopRecvPic(chn), r == 0 || r == HI_ERR_VENC_UNEXIST,
               -1);
    return 0;
}

gint mpp_venc_chn_start(guint chn, guint n_frames) {

    g_message("VencChn %u Start", chn);

    VENC_RECV_PIC_PARAM_S param = {n_frames};
    ASSERT_HEX(HI_MPI_VENC_StartRecvPicEx(chn, &param), r == 0, -1);

    return 0;
}

gint mpp_venc_chn_get_fd(guint chn) {
    return ASSERT_HEX(HI_MPI_VENC_GetFd(chn), r > 0, -1);
}

gint mpp_venc_chn_close_fd(guint chn) {
    return HI_MPI_VENC_CloseFd(chn);
}

gint mpp_venc_chn_send_frame(guint chn, VIDEO_FRAME_INFO_S *frame, gint timeout) {

    // g_message("u32PoolId: %u", frame->u32PoolId);
    // g_message("u32Width: %u", frame->stVFrame.u32Width);
    // g_message("u32Height: %u", frame->stVFrame.u32Height);
    // g_message("u32Field: %u", frame->stVFrame.u32Field);
    // g_message("enPixelFormat: %u", frame->stVFrame.enPixelFormat);
    // g_message("enVideoFormat: %u", frame->stVFrame.enVideoFormat);
    // g_message("enCompressMode: %u", frame->stVFrame.enCompressMode);

    // g_message("u32PhyAddr: {%u,%u,%u}", frame->stVFrame.u32PhyAddr[0], frame->stVFrame.u32PhyAddr[1], frame->stVFrame.u32PhyAddr[2]);
    // g_message("pVirAddr: {%p,%p,%p}", frame->stVFrame.pVirAddr[0], frame->stVFrame.pVirAddr[1], frame->stVFrame.pVirAddr[2]);
    // g_message("u32Stride: {%u,%u,%u}", frame->stVFrame.u32Stride[0], frame->stVFrame.u32Stride[1], frame->stVFrame.u32Stride[2]);

    // g_message("u32HeaderPhyAddr: {%u,%u,%u}", frame->stVFrame.u32HeaderPhyAddr[0], frame->stVFrame.u32HeaderPhyAddr[1], frame->stVFrame.u32HeaderPhyAddr[2]);
    // g_message("pHeaderVirAddr: {%p,%p,%p}", frame->stVFrame.pHeaderVirAddr[0], frame->stVFrame.pHeaderVirAddr[1], frame->stVFrame.pHeaderVirAddr[2]);
    // g_message("u32HeaderStride: {%u,%u,%u}", frame->stVFrame.u32HeaderStride[0], frame->stVFrame.u32HeaderStride[1], frame->stVFrame.u32HeaderStride[2]);

    // g_message("s16OffsetTop: %hd", frame->stVFrame.s16OffsetTop);
    // g_message("s16OffsetBottom: %hd", frame->stVFrame.s16OffsetBottom);
    // g_message("s16OffsetLeft: %hd", frame->stVFrame.s16OffsetLeft);
    // g_message("s16OffsetRight: %hd", frame->stVFrame.s16OffsetRight);

    // g_message("u64pts: %llu", frame->stVFrame.u64pts);
    // g_message("u32TimeRef: %u", frame->stVFrame.u32TimeRef);

    ASSERT_HEX(HI_MPI_VENC_SendFrame(chn, frame, timeout), r == 0, -1);
    return 0;
}

// MPP-Message: 00:52:29.712: u32PoolId: 0
// MPP-Message: 00:52:29.712: u32Width: 3840
// MPP-Message: 00:52:29.713: u32Height: 2880
// MPP-Message: 00:52:29.713: u32Field: 4
// MPP-Message: 00:52:29.714: enPixelFormat: 23
// MPP-Message: 00:52:29.714: enVideoFormat: 0
// MPP-Message: 00:52:29.715: enCompressMode: 0
// MPP-Message: 00:52:29.715: u32PhyAddr: {2751426560,2762485760,2762485760}
// MPP-Message: 00:52:29.716: pVirAddr: {(nil),(nil),(nil)}
// MPP-Message: 00:52:29.716: u32Stride: {3840,3840,3840}
// MPP-Message: 00:52:29.717: u32HeaderPhyAddr: {0,0,0}
// MPP-Message: 00:52:29.717: pHeaderVirAddr: {(nil),(nil),(nil)}
// MPP-Message: 00:52:29.717: u32HeaderStride: {0,0,0}
// MPP-Message: 00:52:29.718: s16OffsetTop: 0
// MPP-Message: 00:52:29.718: s16OffsetBottom: 0
// MPP-Message: 00:52:29.718: s16OffsetLeft: 0
// MPP-Message: 00:52:29.718: s16OffsetRight: 0
// MPP-Message: 00:52:29.718: u64pts: 28921102
// MPP-Message: 00:52:29.719: u32TimeRef: 1724

gint mpp_venc_chn_send_data(guint chn, guint8 *data, guint32 width, guint32 height, guint64 pts, gint timeout) {
    // static guint32 ref = 2000;
    VIDEO_FRAME_INFO_S frame = INIT0(VIDEO_FRAME_INFO_S);
    guint32 paddr;
    void *vaddr;
    HI_MPI_SYS_MmzAlloc(&paddr, &vaddr, NULL, NULL, width * height * 3 /2);
    memcpy(vaddr, data, width * height * 3 / 2);
    frame.stVFrame.u32Width = width;
    frame.stVFrame.u32Height = height;
    frame.stVFrame.u32Field = VIDEO_FIELD_FRAME;
    frame.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    // frame.stVFrame.pVirAddr[0] = data;
    // frame.stVFrame.pVirAddr[1] = data + width * height;
    // frame.stVFrame.pVirAddr[2] = data + width * height;
    frame.stVFrame.u32PhyAddr[0] = paddr;
    frame.stVFrame.u32PhyAddr[1] = paddr + width * height;
    frame.stVFrame.u32PhyAddr[2] = paddr + width * height;
    frame.stVFrame.u32Stride[0] = width;
    frame.stVFrame.u32Stride[1] = width;
    frame.stVFrame.u32Stride[2] = width;
    frame.stVFrame.u64pts = pts;
    // frame.stVFrame.u32TimeRef = ref += 2;
    ASSERT_HEX(HI_MPI_VENC_SendFrame(chn, &frame, timeout), r == 0, -1, HI_MPI_SYS_MmzFree(paddr, vaddr));
    HI_MPI_SYS_MmzFree(paddr, vaddr);
    return 0;
}

// gint mpp_venc_chn_query(guint chn, VENC_CHN_STAT_S *stat, gint timeout) {
//     ASSERT_HEX(HI_MPI_VENC_Query(chn, stat), r == 0, -1, free(pack));
//     return 0;
// }
//
// gint mpp_venc_chn_get_stream(guint chn, VENC_STREAM_S *stream, gint timeout) {
//     ASSERT_HEX(HI_MPI_VENC_GetStream(chn, stream, timeout), r == 0, -1, free(pack));
//     return 0;
// }

gint mpp_venc_chn_read(guint chn, guint8 **buf, guint *size, guint64 *pts, gint timeout) {

    VENC_CHN_STAT_S stat = INIT0(VENC_CHN_STAT_S);
    ASSERT_HEX(HI_MPI_VENC_Query(chn, &stat), r == 0, -1);

    guint count = stat.u32CurPacks;
    if (count == 0 && timeout == 0) {
        return -1;
    }

    VENC_PACK_S *pack = ASSERT_SYS(calloc(count, sizeof(*pack)), r != NULL, -1);

    VENC_STREAM_S stream = INIT0(VENC_STREAM_S);
    stream.u32PackCount = count;
    stream.pstPack = pack;
    ASSERT_HEX(HI_MPI_VENC_GetStream(chn, &stream, timeout), r == 0, -1, free(pack));

    guint64 pts_ = stream.pstPack[0].u64PTS * 1000ull;

    guint size_ = 0;
    for (guint i = 0; i < count; i++) {
        size_ += pack[i].u32Len;
    }

    // guint8 *buf_ = ASSERT_SYS(malloc(size_), r != NULL, -1,
    // HI_MPI_VENC_ReleaseStream(chn, &stream), free(pack));
    guint8 *buf_ = g_malloc(size_);

    guint8 *pbuf = buf_;
    for (guint i = 0; i < count; i++) {
        guint len = pack[i].u32Len;
        guint8 *addr = pack[i].pu8Addr + pack[i].u32Offset;
        memcpy(pbuf, addr, len);
        pbuf += len;
    }

    HI_MPI_VENC_ReleaseStream(chn, &stream);
    free(pack);

    *buf = buf_;
    *size = size_;
    *pts = pts_;

    return 0;
}

/* ISP */

/* NR */

gint mpp_isp_nr_set_manual(guint dev, guint8 thresh) {

    ISP_NR_ATTR_S attr = INIT0(ISP_NR_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetNRAttr(dev, &attr), r == 0, -1);

    g_message("ISP:%u", dev);
    g_message("%u:%u:%hhu", attr.bEnable, attr.enOpType, attr.stManual.u8Thresh);

    attr.bEnable = TRUE;
    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.u8Thresh = thresh;

    ASSERT_HEX(HI_MPI_ISP_SetNRAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_nr_set_auto(guint dev) {

    ISP_NR_ATTR_S attr = INIT0(ISP_NR_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetNRAttr(dev, &attr), r == 0, -1);

    g_message("ISP:%u", dev);
    g_message("%u:%u", attr.bEnable, attr.enOpType);

    attr.bEnable = TRUE;
    attr.enOpType = OP_TYPE_AUTO;

    ASSERT_HEX(HI_MPI_ISP_SetNRAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_nr_disable(guint dev) {

    ISP_NR_ATTR_S attr = INIT0(ISP_NR_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetNRAttr(dev, &attr), r == 0, -1);

    g_message("ISP:%u", dev);
    g_message("%u:%u", attr.bEnable, attr.enOpType);

    attr.bEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetNRAttr(dev, &attr), r == 0, -1);

    return 0;
}

/* EXPO */

gint mpp_isp_expo_query_info(guint dev, guint *expo_time, guint *a_gain, guint *d_gain, guint *isp_gain, guint *expo, guint *iso) {

    ISP_EXP_INFO_S info = INIT0(ISP_EXP_INFO_S);
    ASSERT_HEX(HI_MPI_ISP_QueryExposureInfo(dev, &info), r == 0, -1);

    *expo_time = info.u32ExpTime; // us
    *a_gain = info.u32AGain;
    *d_gain = info.u32DGain;
    *isp_gain = info.u32ISPDGain;
    *expo = info.u32Exposure;
    *iso = info.u32ISO;

    return 0;
}

gint mpp_isp_expo_get(guint dev, gint *compensation, gint *expo_time, gint *a_gain, gint *d_gain, gint *isp_gain) {

    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);

    if (expo_attr.enOpType == OP_TYPE_MANUAL) {

        *compensation = -1;

        if (expo_attr.stManual.enExpTimeOpType == OP_TYPE_AUTO) {
            *expo_time = -1;
        } else {
            *expo_time = expo_attr.stManual.u32ExpTime;
        }

        if (expo_attr.stManual.enAGainOpType == OP_TYPE_AUTO) {
            *a_gain = -1;
        } else {
            *a_gain = expo_attr.stManual.u32AGain;
        }

        if (expo_attr.stManual.enDGainOpType == OP_TYPE_AUTO) {
            *d_gain = -1;
        } else {
            *d_gain = expo_attr.stManual.u32DGain;
        }

        if (expo_attr.stManual.enISPDGainOpType == OP_TYPE_AUTO) {
            *isp_gain = -1;
        } else {
            *isp_gain = expo_attr.stManual.u32ISPDGain;
        }

    } else {
        *compensation = expo_attr.stAuto.u8Compensation;
        *expo_time = -1;
        *a_gain = -1;
        *d_gain = -1;
        *isp_gain = -1;
    }

    return 0;
}

/*
gint mpp_isp_expo_set(guint dev, gint compensation, gint expo_time, gint a_gain, gint d_gain, gint isp_gain) {

    ISP_EXPOSURE_ATTR_S expo_attr = {};
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);

    if (compensation < 0) { // manual

        expo_attr.enOpType = OP_TYPE_MANUAL;

        if (expo_time < 0) {
            expo_attr.stManual.enExpTimeOpType = OP_TYPE_AUTO;
        } else {
            expo_attr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
            expo_attr.stManual.u32ExpTime = expo_time;
        }

        if (a_gain < 1024) {
            expo_attr.stManual.enAGainOpType = OP_TYPE_AUTO;
        } else {
            expo_attr.stManual.enAGainOpType = OP_TYPE_MANUAL;
            expo_attr.stManual.u32AGain = a_gain;
        }

        if (d_gain < 1024) {
            expo_attr.stManual.enDGainOpType = OP_TYPE_AUTO;
        } else {
            expo_attr.stManual.enDGainOpType = OP_TYPE_MANUAL;
            expo_attr.stManual.u32DGain = d_gain;
        }

        if (isp_gain < 1024) {
            expo_attr.stManual.enISPDGainOpType = OP_TYPE_AUTO;
        } else {
            expo_attr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
            expo_attr.stManual.u32ISPDGain = isp_gain;
        }
    } else { // auto

        expo_attr.enOpType = OP_TYPE_AUTO;

        if (expo_time < 0) {
            expo_attr.stAuto.stExpTimeRange.u32Min = 0;
            expo_attr.stAuto.stExpTimeRange.u32Max = -1;
        } else {
            expo_attr.stAuto.stExpTimeRange.u32Min = expo_time;
            expo_attr.stAuto.stExpTimeRange.u32Max = expo_time;
        }

        if (a_gain < 1024) {
            expo_attr.stAuto.stAGainRange.u32Min = 1024;
            expo_attr.stAuto.stAGainRange.u32Max = -1;
        } else {
            expo_attr.stAuto.stAGainRange.u32Min = a_gain;
            expo_attr.stAuto.stAGainRange.u32Max = a_gain;
        }

        if (d_gain < 1024) {
            expo_attr.stAuto.stDGainRange.u32Min = 1024;
            expo_attr.stAuto.stDGainRange.u32Max = -1;
        } else {
            expo_attr.stAuto.stDGainRange.u32Min = d_gain;
            expo_attr.stAuto.stDGainRange.u32Max = d_gain;
        }

        if (isp_gain < 1024) {
            expo_attr.stAuto.stISPDGainRange.u32Min = 1024;
            expo_attr.stAuto.stISPDGainRange.u32Max = -1;
        } else {
            expo_attr.stAuto.stISPDGainRange.u32Min = isp_gain;
            expo_attr.stAuto.stISPDGainRange.u32Max = isp_gain;
        }
    }

    ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);
    return 0;
}
*/

gint mpp_isp_expo_get_compensation(guint dev, guint8 *compensation) {
    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);
    *compensation = expo_attr.stAuto.u8Compensation;
    return 0;
}

gint mpp_isp_expo_set_compensation(guint dev, guint8 compensation) {
    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);
    expo_attr.stAuto.u8Compensation = compensation;
    ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);
    return 0;
}

// gint mpp_isp_expo_set_auto(guint dev, guint8 compensation, gboolean low_light, gint expo_time, gint a_gain, gint d_gain, gint isp_gain) {
//
//     ISP_EXPOSURE_ATTR_S expo_attr = {};
//     ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);
//
//     expo_attr.enOpType = OP_TYPE_AUTO;
//
//     expo_attr.stAuto.u8Compensation = compensation;
//     expo_attr.stAuto.enAEStrategyMode = low_light ? AE_EXP_LOWLIGHT_PRIOR : AE_EXP_HIGHLIGHT_PRIOR;
//
//     if (expo_time >= 0) {
//         expo_attr.stAuto.stExpTimeRange.u32Min = expo_time;
//         expo_attr.stAuto.stExpTimeRange.u32Max = expo_time + 128;
//     } else {
//         expo_attr.stAuto.stExpTimeRange.u32Min = 0;
//         expo_attr.stAuto.stExpTimeRange.u32Max = -1;
//     }
//
//     if (a_gain >= 0x400) {
//         expo_attr.stAuto.stAGainRange.u32Min = a_gain;
//         expo_attr.stAuto.stAGainRange.u32Max = a_gain + 128;
//     } else {
//         expo_attr.stAuto.stAGainRange.u32Min = 0x400;
//         expo_attr.stAuto.stAGainRange.u32Max = -1;
//     }
//
//     if (d_gain >= 0x400) {
//         expo_attr.stAuto.stDGainRange.u32Min = d_gain;
//         expo_attr.stAuto.stDGainRange.u32Max = d_gain + 128;
//     } else {
//         expo_attr.stAuto.stDGainRange.u32Min = 0x400;
//         expo_attr.stAuto.stDGainRange.u32Max = -1;
//     }
//
//     if (isp_gain >= 0x400) {
//         expo_attr.stAuto.stISPDGainRange.u32Min = isp_gain;
//         expo_attr.stAuto.stISPDGainRange.u32Max = isp_gain + 128;
//     } else {
//         expo_attr.stAuto.stISPDGainRange.u32Min = 0x400;
//         expo_attr.stAuto.stISPDGainRange.u32Max = -1;
//     }
//
//     ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);
//
//     return 0;
// }

gint mpp_isp_expo_set_auto(guint dev, guint8 compensation, gboolean low_light,
                           gint expo_time_min, gint expo_time_max,
                           gint a_gain_min, gint a_gain_max,
                           gint d_gain_min, gint d_gain_max,
                           gint isp_gain_min, gint isp_gain_max) {

    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);

    expo_attr.enOpType = OP_TYPE_AUTO;

    expo_attr.stAuto.u8Compensation = compensation;
    expo_attr.stAuto.enAEStrategyMode = low_light ? AE_EXP_LOWLIGHT_PRIOR : AE_EXP_HIGHLIGHT_PRIOR;

    expo_attr.stAuto.stExpTimeRange.u32Min = expo_time_min;
    expo_attr.stAuto.stExpTimeRange.u32Max = expo_time_max;

    expo_attr.stAuto.stAGainRange.u32Min = a_gain_min;
    expo_attr.stAuto.stAGainRange.u32Max = a_gain_max;

    expo_attr.stAuto.stDGainRange.u32Min = d_gain_min;
    expo_attr.stAuto.stDGainRange.u32Max = d_gain_max;

    expo_attr.stAuto.stISPDGainRange.u32Min = isp_gain_min;
    expo_attr.stAuto.stISPDGainRange.u32Max = isp_gain_max;

    ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);

    return 0;
}

gint mpp_isp_expo_set_manual(guint dev, gint expo_time, gint a_gain, gint d_gain, gint isp_gain) {

    ISP_EXPOSURE_ATTR_S expo_attr = INIT0(ISP_EXPOSURE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);

    expo_attr.enOpType = OP_TYPE_MANUAL;
    expo_attr.stAuto.stExpTimeRange.u32Min = 0;
    expo_attr.stAuto.stExpTimeRange.u32Max = -1;
    expo_attr.stAuto.stAGainRange.u32Min = 0x400;
    expo_attr.stAuto.stAGainRange.u32Max = -1;
    expo_attr.stAuto.stDGainRange.u32Min = 0x400;
    expo_attr.stAuto.stDGainRange.u32Max = -1;
    expo_attr.stAuto.stISPDGainRange.u32Min = 0x400;
    expo_attr.stAuto.stISPDGainRange.u32Max = -1;

    if (expo_time < 0) {
        expo_attr.stManual.enExpTimeOpType = OP_TYPE_AUTO;
    } else {
        expo_attr.stManual.enExpTimeOpType = OP_TYPE_MANUAL;
        expo_attr.stManual.u32ExpTime = expo_time;
    }

    if (a_gain < 0) {
        expo_attr.stManual.enAGainOpType = OP_TYPE_AUTO;
    } else {
        expo_attr.stManual.enAGainOpType = OP_TYPE_MANUAL;
        expo_attr.stManual.u32AGain = a_gain;
    }

    if (d_gain < 0) {
        expo_attr.stManual.enDGainOpType = OP_TYPE_AUTO;
    } else {
        expo_attr.stManual.enDGainOpType = OP_TYPE_MANUAL;
        expo_attr.stManual.u32DGain = d_gain;
    }

    if (isp_gain < 0) {
        expo_attr.stManual.enISPDGainOpType = OP_TYPE_AUTO;
    } else {
        expo_attr.stManual.enISPDGainOpType = OP_TYPE_MANUAL;
        expo_attr.stManual.u32ISPDGain = isp_gain;
    }

    ASSERT_HEX(HI_MPI_ISP_SetExposureAttr(dev, &expo_attr), r == 0, -1);

    /*
    ASSERT_HEX(HI_MPI_ISP_GetExposureAttr(dev, &expo_attr), r == 0, -1);
    g_message("ISP:%u", dev);
    g_message("%u:%u", expo_attr.stManual.enExpTimeOpType, expo_attr.stManual.u32ExpTime);
    g_message("%u:%u", expo_attr.stManual.enAGainOpType, expo_attr.stManual.u32AGain);
    g_message("%u:%u", expo_attr.stManual.enDGainOpType, expo_attr.stManual.u32DGain);
    g_message("%u:%u", expo_attr.stManual.enISPDGainOpType, expo_attr.stManual.u32ISPDGain);
    */

    return 0;
}

/* WB */

gint mpp_isp_wb_query_info(guint dev, guint16 *r, guint16 *gr, guint16 *gb, guint16 *b, guint16 *saturation, guint16 *temp) {

    ISP_WB_INFO_S info = INIT0(ISP_WB_INFO_S);
    ASSERT_HEX(HI_MPI_ISP_QueryWBInfo(dev, &info), r == 0, -1);

    *r = info.u16Rgain;
    *gr = info.u16Grgain;
    *gb = info.u16Gbgain;
    *b = info.u16Bgain;
    *saturation = info.u16Saturation;
    *temp = info.u16ColorTemp;

    return 0;
}

gint mpp_isp_wb_set_bypass(guint dev) {

    ISP_WB_ATTR_S attr = INIT0(ISP_WB_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetWBAttr(dev, &attr), r == 0, -1);

    attr.bByPass = TRUE;
    ASSERT_HEX(HI_MPI_ISP_SetWBAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_wb_set_manual(guint dev, guint16 r, guint16 gr, guint16 gb, guint16 b) {

    ISP_WB_ATTR_S attr = INIT0(ISP_WB_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetWBAttr(dev, &attr), r == 0, -1);

    attr.bByPass = FALSE;
    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.u16Rgain = r;
    attr.stManual.u16Grgain = gr;
    attr.stManual.u16Gbgain = gb;
    attr.stManual.u16Bgain = b;
    ASSERT_HEX(HI_MPI_ISP_SetWBAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_wb_set_auto(guint dev) {

    ISP_WB_ATTR_S attr = INIT0(ISP_WB_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetWBAttr(dev, &attr), r == 0, -1);

    attr.bByPass = FALSE;
    attr.enOpType = OP_TYPE_AUTO;
    ASSERT_HEX(HI_MPI_ISP_SetWBAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_wb_set_fine_tune(guint dev, gboolean enable) {

    ISP_AWB_ATTR_EX_S attr = INIT0(ISP_AWB_ATTR_EX_S);
    ASSERT_HEX(HI_MPI_ISP_GetAWBAttrEx(dev, &attr), r == 0, -1);

    attr.bFineTunEn = enable;
    ASSERT_HEX(HI_MPI_ISP_SetAWBAttrEx(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_wb_get_fine_tune(guint dev, gboolean *enable) {

    ISP_AWB_ATTR_EX_S attr = INIT0(ISP_AWB_ATTR_EX_S);
    ASSERT_HEX(HI_MPI_ISP_GetAWBAttrEx(dev, &attr), r == 0, -1);
    *enable = attr.bFineTunEn;

    return 0;
}

/* COLORTONE */

gint mpp_isp_colortone_set(guint dev, guint16 r, guint16 g, guint16 b) {

    ISP_COLOR_TONE_ATTR_S attr = {r, g, b};
    ASSERT_HEX(HI_MPI_ISP_SetColorToneAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_colortone_get(guint dev, guint16 *r, guint16 *g, guint16 *b) {

    ISP_COLOR_TONE_ATTR_S attr = INIT0(ISP_COLOR_TONE_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetColorToneAttr(dev, &attr), r == 0, -1);

    *r = attr.u16RedCastGain;
    *g = attr.u16GreenCastGain;
    *b = attr.u16BlueCastGain;

    return 0;
}

/* CCM */

/*
gint mpp_isp_saturation_is_manual(guint dev, gboolean *is_manual) {

    ISP_SATURATION_ATTR_S attr = {};
    ASSERT_HEX(HI_MPI_ISP_GetSaturationAttr(dev, &attr), r == 0, -1);

    *is_manual = attr.enOpType == OP_TYPE_MANUAL;

    return 0;
}

gint mpp_isp_saturation_is_auto(guint dev, gboolean *is_auto) {

    ISP_SATURATION_ATTR_S attr = {};
    ASSERT_HEX(HI_MPI_ISP_GetSaturationAttr(dev, &attr), r == 0, -1);

    *is_auto = attr.enOpType == OP_TYPE_AUTO;

    return 0;
}
*/

gint mpp_isp_saturation_set_manual(guint dev, guint8 sat) {

    ISP_SATURATION_ATTR_S attr = INIT0(ISP_SATURATION_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetSaturationAttr(dev, &attr), r == 0, -1);

    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.u8Saturation = sat;
    ASSERT_HEX(HI_MPI_ISP_SetSaturationAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_saturation_get_manual(guint dev, guint8 *sat) {

    ISP_SATURATION_ATTR_S attr = INIT0(ISP_SATURATION_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetSaturationAttr(dev, &attr), r == 0, -1);

    *sat = attr.stManual.u8Saturation;

    return 0;
}

gint mpp_isp_saturation_set_auto(guint dev, const guint8 *sat, guint len) {

    ASSERT_BOOL(len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);

    ISP_SATURATION_ATTR_S attr = INIT0(ISP_SATURATION_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetSaturationAttr(dev, &attr), r == 0, -1);

    attr.enOpType = OP_TYPE_AUTO;
    memcpy(&(attr.stAuto), sat, len);
    ASSERT_HEX(HI_MPI_ISP_SetSaturationAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_colormatrix_set_manual(guint dev, const guint16 *ccm, guint len,
                                    gboolean enable_sat) {

    ASSERT_BOOL(len <= CCM_MATRIX_SIZE, r == TRUE, -1);

    ISP_COLORMATRIX_ATTR_S attr = INIT0(ISP_COLORMATRIX_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetCCMAttr(dev, &attr), r == 0, -1);

    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.bSatEn = enable_sat;
    memcpy(&(attr.stManual.au16CCM), ccm, len * sizeof(*ccm));
    ASSERT_HEX(HI_MPI_ISP_SetCCMAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_colormatrix_set_auto(guint dev, guint16 high_temp,
                                  const guint16 *high_ccm, guint high_len,
                                  guint16 mid_temp, const guint16 *mid_ccm,
                                  guint mid_len, guint16 low_temp,
                                  const guint16 *low_ccm, guint low_len,
                                  gboolean bypass_iso, gboolean bypass_temp) {

    ASSERT_BOOL(high_len <= CCM_MATRIX_SIZE, r == TRUE, -1);
    ASSERT_BOOL(mid_len <= CCM_MATRIX_SIZE, r == TRUE, -1);
    ASSERT_BOOL(low_len <= CCM_MATRIX_SIZE, r == TRUE, -1);

    ASSERT_BOOL(high_temp <= 10000 && high_temp >= 2800, r == TRUE, -1);
    ASSERT_BOOL(mid_temp <= high_temp - 400 && mid_temp >= 2400, r == TRUE, -1);
    ASSERT_BOOL(low_temp <= mid_temp - 400 && low_temp >= 2000, r == TRUE, -1);

    ISP_COLORMATRIX_ATTR_S attr = INIT0(ISP_COLORMATRIX_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetCCMAttr(dev, &attr), r == 0, -1);

    attr.enOpType = OP_TYPE_AUTO;
    attr.stAuto.bISOActEn = bypass_iso;
    attr.stAuto.bTempActEn = bypass_temp;

    attr.stAuto.u16HighColorTemp = high_temp;
    memcpy(attr.stAuto.au16HighCCM, high_ccm, high_len * sizeof(*high_ccm));

    attr.stAuto.u16MidColorTemp = mid_temp;
    memcpy(attr.stAuto.au16MidCCM, mid_ccm, mid_len * sizeof(*mid_ccm));

    attr.stAuto.u16LowColorTemp = low_temp;
    memcpy(attr.stAuto.au16LowCCM, low_ccm, low_len * sizeof(*low_ccm));

    ASSERT_HEX(HI_MPI_ISP_SetCCMAttr(dev, &attr), r == 0, -1);

    return 0;
}

/* ACM */

gint mpp_isp_acm_is_enable(guint dev, gboolean *is_enable) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *is_enable = attr.bEnable;

    return 0;
}

gint mpp_isp_acm_enable(guint dev) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.bEnable = TRUE;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_disable(guint dev) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.bEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_demo_is_enable(guint dev, gboolean *is_enable) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *is_enable = attr.bDemoEnable;

    return 0;
}

gint mpp_isp_acm_enable_demo(guint dev) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.bDemoEnable = TRUE;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_disable_demo(guint dev) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.bDemoEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_stretch_range(guint dev, guint32 *range) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *range = attr.u32Stretch;

    return 0;
}

gint mpp_isp_acm_set_stretch_range(guint dev, guint32 range) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32Stretch = range;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_clip_range(guint dev, guint32 *range) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *range = attr.u32ClipRange;

    return 0;
}

gint mpp_isp_acm_set_clip_range(guint dev, guint32 range) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32ClipRange = range;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_clip_or_wrap(guint dev, guint32 *mode) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *mode = attr.u32AcmClipOrWrap;

    return 0;
}

gint mpp_isp_acm_set_clip_or_wrap(guint dev, guint32 mode) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32AcmClipOrWrap = mode;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_cbcr_thr(guint dev, guint32 *thr) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *thr = attr.u32CbcrThr;

    return 0;
}

gint mpp_isp_acm_set_cbcr_thr(guint dev, guint32 thr) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32CbcrThr = thr;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_luma_gain(guint dev, guint32 *gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *gain = attr.u32GainLuma;

    return 0;
}

gint mpp_isp_acm_set_luma_gain(guint dev, guint32 gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32GainLuma = gain;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_hue_gain(guint dev, guint32 *gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *gain = attr.u32GainHue;

    return 0;
}

gint mpp_isp_acm_set_hue_gain(guint dev, guint32 gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32GainHue = gain;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_sat_gain(guint dev, guint32 *gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);
    *gain = attr.u32GainSat;

    return 0;
}

gint mpp_isp_acm_set_sat_gain(guint dev, guint32 gain) {

    ISP_ACM_ATTR_S attr = INIT0(ISP_ACM_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetAcmAttr(dev, &attr), r == 0, -1);

    attr.u32GainSat = gain;
    ASSERT_HEX(HI_MPI_ISP_SetAcmAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_acm_get_coeff(guint dev, guint16 **y, guint *y_len, guint16 **h,
                           guint *h_len, guint16 **s, guint *s_len) {

    ISP_ACM_LUT_S lut = INIT0(ISP_ACM_LUT_S);
    HI_MPI_ISP_GetAcmCoeff(dev, &lut);

    *y = g_memdup(&lut.as16Y, sizeof(lut.as16Y));
    *y_len = sizeof(lut.as16Y) / sizeof(**y);

    *h = g_memdup(&lut.as16H, sizeof(lut.as16H));
    *h_len = sizeof(lut.as16H) / sizeof(**h);

    *s = g_memdup(&lut.as16S, sizeof(lut.as16S));
    *s_len = sizeof(lut.as16S) / sizeof(**s);

    return 0;
}

gint mpp_isp_acm_set_coeff(guint dev, const guint16 *y, guint y_len,
                           const guint16 *h, guint h_len, const guint16 *s,
                           guint s_len) {

    ASSERT_BOOL((y_len <= ACM_Y_NUM * ACM_H_NUM * ACM_S_NUM), r == TRUE, -1);
    ASSERT_BOOL((h_len <= ACM_Y_NUM * ACM_H_NUM * ACM_S_NUM), r == TRUE, -1);
    ASSERT_BOOL((s_len <= ACM_Y_NUM * ACM_H_NUM * ACM_S_NUM), r == TRUE, -1);

    ISP_ACM_LUT_S lut = INIT0(ISP_ACM_LUT_S);
    memcpy(lut.as16Y, y, sizeof(*y) * y_len);
    memcpy(lut.as16H, h, sizeof(*h) * h_len);
    memcpy(lut.as16S, s, sizeof(*s) * s_len);

    ASSERT_HEX(HI_MPI_ISP_SetAcmCoeff(dev, &lut), r == 0, -1);
    return 0;
}

/* GAMMA */

gint mpp_isp_gamma_is_enable(guint dev, gboolean *is_enable) {

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);
    *is_enable = attr.bEnable;

    return 0;
}

gint mpp_isp_gamma_enable(guint dev) {

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);

    attr.bEnable = TRUE;
    ASSERT_HEX(HI_MPI_ISP_SetGammaAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_gamma_disable(guint dev) {

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);

    attr.bEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetGammaAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_gamma_set_default(guint dev) {

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);

    attr.enCurveType = ISP_GAMMA_CURVE_DEFAULT;
    ASSERT_HEX(HI_MPI_ISP_SetGammaAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_gamma_set_srgb(guint dev) {

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);

    attr.enCurveType = ISP_GAMMA_CURVE_SRGB;
    ASSERT_HEX(HI_MPI_ISP_SetGammaAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_gamma_set_user(guint dev, const guint16 *table, guint table_len) {

    ASSERT_BOOL(table_len <= GAMMA_NODE_NUM, r == TRUE, -1);

    ISP_GAMMA_ATTR_S attr = INIT0(ISP_GAMMA_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetGammaAttr(dev, &attr), r == 0, -1);

    attr.enCurveType = ISP_GAMMA_CURVE_SRGB;

    memcpy(attr.u16Table, table, sizeof(*table) * table_len);
    // memset(attr.u16Table, 0, sizeof(*table) * (GAMMA_NODE_NUM - table_len));
    ASSERT_HEX(HI_MPI_ISP_SetGammaAttr(dev, &attr), r == 0, -1);

    return 0;
}

/* SHARPEN */

gint mpp_isp_sharpen_bayer_is_enable(guint dev, gboolean *is_enable) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);
    *is_enable = attr.bEnable;

    return 0;
}

gint mpp_isp_sharpen_bayer_enable(guint dev) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.bEnable = TRUE;
    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_disable(guint dev) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.bEnable = FALSE;
    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_set_manual(guint dev, guint8 sharpen_d,
                                      guint8 sharpen_ud, guint16 luma_thd) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.enShpAlgSel = 0;
    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.stShp.u8SharpenD = sharpen_d;
    attr.stManual.stShp.u8SharpenUd = sharpen_ud;
    attr.stManual.stShp.u16LumaThd = luma_thd;
    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_get_manual(guint dev, guint8 *sharpen_d,
                                      guint8 *sharpen_ud, guint16 *luma_thd) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    *sharpen_d = attr.stManual.stShp.u8SharpenD;
    *sharpen_ud = attr.stManual.stShp.u8SharpenUd;
    *luma_thd = attr.stManual.stShp.u16LumaThd;

    return 0;
}

gint mpp_isp_sharpen_bayer_set_manual_ex(guint dev, guint8 sharpen_hf,
                                         guint8 sharpen_mf, guint8 sharpen_lf,
                                         guint8 sad_amplifer) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.enShpAlgSel = 1;
    attr.enOpType = OP_TYPE_MANUAL;
    attr.stManual.stShpEx.u8SharpenHF = sharpen_hf;
    attr.stManual.stShpEx.u8SharpenMF = sharpen_mf;
    attr.stManual.stShpEx.u8SharpenLF = sharpen_lf;
    attr.stManual.stShpEx.u8SadAmplifier = sad_amplifer;
    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_get_manual_ex(guint dev, guint8 *sharpen_hf,
                                         guint8 *sharpen_mf, guint8 *sharpen_lf,
                                         guint8 *sad_amplifer) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    *sharpen_hf = attr.stManual.stShpEx.u8SharpenHF;
    *sharpen_mf = attr.stManual.stShpEx.u8SharpenMF;
    *sharpen_lf = attr.stManual.stShpEx.u8SharpenLF;
    *sad_amplifer = attr.stManual.stShpEx.u8SadAmplifier;

    return 0;
}

gint mpp_isp_sharpen_bayer_set_auto(guint dev, const guint8 *sharpen_d,
                                    guint sharpen_d_len,
                                    const guint8 *sharpen_ud,
                                    guint sharpen_ud_len,
                                    const guint16 *luma_thd,
                                    guint luma_thd_len) {

    ASSERT_BOOL(sharpen_d_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);
    ASSERT_BOOL(sharpen_ud_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);
    ASSERT_BOOL(luma_thd_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.enShpAlgSel = 0;
    attr.enOpType = OP_TYPE_AUTO;

    for (guint i = 0; i < sharpen_d_len; i++) {
        attr.stAuto.astShp[i].u8SharpenD = sharpen_d[i];
    }

    for (guint i = 0; i < sharpen_ud_len; i++) {
        attr.stAuto.astShp[i].u8SharpenUd = sharpen_ud[i];
    }

    for (guint i = 0; i < luma_thd_len; i++) {
        attr.stAuto.astShp[i].u16LumaThd = luma_thd[i];
    }

    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_get_auto(guint dev, guint8 **sharpen_d,
                                    guint *sharpen_d_len, guint8 **sharpen_ud,
                                    guint *sharpen_ud_len, guint16 **luma_thd,
                                    guint *luma_thd_len) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    *sharpen_d = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sharpen_d));
    *sharpen_d_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sharpen_d)[i] = attr.stAuto.astShp[i].u8SharpenD;
    }

    *sharpen_ud = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sharpen_ud));
    *sharpen_ud_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sharpen_ud)[i] = attr.stAuto.astShp[i].u8SharpenUd;
    }

    *luma_thd = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**luma_thd));
    *luma_thd_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*luma_thd)[i] = attr.stAuto.astShp[i].u16LumaThd;
    }

    return 0;
}

gint mpp_isp_sharpen_bayer_set_auto_ex(
    guint dev, const guint8 *sharpen_hf, guint sharpen_hf_len,
    const guint8 *sharpen_mf, guint sharpen_mf_len, const guint8 *sharpen_lf,
    guint sharpen_lf_len, const guint8 *sad_amplifer, guint sad_amplifer_len) {

    ASSERT_BOOL(sharpen_hf_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);
    ASSERT_BOOL(sharpen_mf_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);
    ASSERT_BOOL(sharpen_lf_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);
    ASSERT_BOOL(sad_amplifer_len <= ISP_AUTO_STENGTH_NUM, r == TRUE, -1);

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    attr.enShpAlgSel = 1;
    attr.enOpType = OP_TYPE_AUTO;

    for (guint i = 0; i < sharpen_hf_len; i++) {
        attr.stAuto.astShpEx[i].u8SharpenHF = sharpen_hf[i];
    }

    for (guint i = 0; i < sharpen_mf_len; i++) {
        attr.stAuto.astShpEx[i].u8SharpenMF = sharpen_mf[i];
    }

    for (guint i = 0; i < sharpen_lf_len; i++) {
        attr.stAuto.astShpEx[i].u8SharpenLF = sharpen_lf[i];
    }

    for (guint i = 0; i < sad_amplifer_len; i++) {
        attr.stAuto.astShpEx[i].u8SadAmplifier = sad_amplifer[i];
    }

    ASSERT_HEX(HI_MPI_ISP_SetBayerSharpenAttr(dev, &attr), r == 0, -1);

    return 0;
}

gint mpp_isp_sharpen_bayer_get_auto_ex(
    guint dev, guint8 **sharpen_hf, guint *sharpen_hf_len, guint8 **sharpen_mf,
    guint *sharpen_mf_len, guint8 **sharpen_lf, guint *sharpen_lf_len,
    guint16 **sad_amplifer, guint *sad_amplifer_len) {

    ISP_BAYER_SHARPEN_ATTR_S attr = INIT0(ISP_BAYER_SHARPEN_ATTR_S);
    ASSERT_HEX(HI_MPI_ISP_GetBayerSharpenAttr(dev, &attr), r == 0, -1);

    *sharpen_hf = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sharpen_hf));
    *sharpen_hf_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sharpen_hf)[i] = attr.stAuto.astShpEx[i].u8SharpenHF;
    }

    *sharpen_mf = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sharpen_mf));
    *sharpen_mf_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sharpen_mf)[i] = attr.stAuto.astShpEx[i].u8SharpenMF;
    }

    *sharpen_lf = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sharpen_lf));
    *sharpen_lf_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sharpen_lf)[i] = attr.stAuto.astShpEx[i].u8SharpenLF;
    }

    *sad_amplifer = g_malloc_n(ISP_AUTO_STENGTH_NUM, sizeof(**sad_amplifer));
    *sad_amplifer_len = ISP_AUTO_STENGTH_NUM;
    for (guint i = 0; i < ISP_AUTO_STENGTH_NUM; i++) {
        (*sad_amplifer)[i] = attr.stAuto.astShpEx[i].u8SadAmplifier;
    }

    return 0;
}
