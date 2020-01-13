
#ifndef HIMPP_H_
#define HIMPP_H_

#include "acodec.h"
#include "hi_mipi.h"
#include "hi_sns_ctrl.h"
#include "mpi_ae.h"
#include "mpi_aenc.h"
#include "mpi_af.h"
#include "mpi_ai.h"
#include "mpi_awb.h"
#include "mpi_isp.h"
#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_venc.h"
#include "mpi_vi.h"
#include "mpi_vpss.h"

#include <glib.h>
#include <sys/cdefs.h>

#define IFD0_OFFSET offsetof(TIFFMeta, n_entries)

#define IFD0_ENTRY_BEGIN (IFD0_OFFSET + 4)
#define IFD0_ENTRY_END offsetof(TIFFMeta, ifd_next_offset)
#define N_ENTRIES ((IFD0_ENTRY_END - IFD0_ENTRY_BEGIN) / 12)

#define MAKE_OFFSET offsetof(TIFFMeta, make_value)
#define MODEL_OFFSET offsetof(TIFFMeta, model_value)
#define EXPOSURE_TIME_OFFSET offsetof(TIFFMeta, exposure_time_value)
#define F_NUMBER_OFFSET offsetof(TIFFMeta, f_number_value)
#define COLOR_MATRIX_1_OFFSET offsetof(TIFFMeta, color_matrix_1_value)
#define AS_SHOT_NEUTRAL_OFFSET offsetof(TIFFMeta, as_shot_neutral_value)

typedef struct __attribute__((__packed__)) _TIFFEntry {
  guint16 tag;
  guint16 type;
  guint32 n;
  guint32 offset;
} TIFFEntry;

typedef struct __attribute__((__packed__)) _TIFFMeta {

  guint8 byte_order[2];
  guint16 magic_number;
  guint32 ifd0_offset;

  guint16 n_entries;
  TIFFEntry new_subfile_type;           // 254
  TIFFEntry image_width;                // 256
  TIFFEntry image_height;               // 257
  TIFFEntry bits_per_sample;            // 258
  TIFFEntry compression;                // 259
  TIFFEntry photometric_interpretation; // 262
  TIFFEntry make;                       // 271
  TIFFEntry model;                      // 272
  TIFFEntry strip_offsets;              // 273
  TIFFEntry orientation;                // 274
  TIFFEntry samples_per_pixel;          // 277
  TIFFEntry rows_per_strip;             // 278
  TIFFEntry strip_byte_counts;          // 279
  TIFFEntry planar_configuration;       // 284
  TIFFEntry sample_format;              // 339
  TIFFEntry cfa_repeat_pattern_dim;     // 33421
  TIFFEntry cfa_pattern;                // 33422
  TIFFEntry exposure_time;              // *33434
  TIFFEntry f_number;                   // 33437 ? 22 / 10
  TIFFEntry exposure_program;           // 34850 ? 2
  TIFFEntry iso_speed_ratings;          // *34855 ? 100 200
  TIFFEntry metering_mode;              // 37383 ? 2
  TIFFEntry contrast;                   // 41992 0
  TIFFEntry saturation;                 // 41993 0
  TIFFEntry sharpness;                  // 41994 0
  TIFFEntry dng_version;                // 50706
  TIFFEntry dng_backward_version;       // 50707
  TIFFEntry unique_camera_model;        // 50708
  TIFFEntry cfg_plane_color;            // 50710
  TIFFEntry cfa_layout;                 // 50711
  TIFFEntry black_level_repeat_dim;     // 50713
  TIFFEntry black_level;                // 50714
  TIFFEntry white_level;                // 50717
  TIFFEntry color_matrix_1;             // 50721
  TIFFEntry as_shot_neutral;            // 50728
  TIFFEntry calibration_illuminatn1;    // 50778
  guint32 ifd_next_offset;

  guint8 make_value[8];  // Teche
  guint8 model_value[8]; // PHIIMAX
  guint32 exposure_time_value[2];
  guint32 f_number_value[2];
  gint32 color_matrix_1_value[18];
  guint32 as_shot_neutral_value[6];

} TIFFMeta;

gint mpp_disable(void);
gint mpp_enable(guint n_pools, ...);
gint mpp_bind(MOD_ID_E src_mod, guint src_dev, guint src_chn, MOD_ID_E dest_mod,
              guint dest_dev, guint dest_chn);
gint mpp_unbind(MOD_ID_E src_mod, guint src_dev, guint src_chn,
                MOD_ID_E dest_mod, guint dest_dev, guint dest_chn);
guint64 mpp_get_pts(void);
void mpp_sync_pts(guint64 pts);
void mpp_init_pts(guint64 pts);
gint mpp_acodec_set_volume(guint volume);
gint mpp_acodec_restart(ACODEC_MIXER_E input_mode,
                        AUDIO_SAMPLE_RATE_E sample_rate, guint volume);
gint mpp_ai_dev_disable(guint dev);
gint mpp_ai_dev_enable(guint dev, gboolean stereo,
                       AUDIO_SAMPLE_RATE_E sample_rate, guint n_channels);
gint mpp_ai_chn_disable(guint dev, guint chn);
gint mpp_ai_chn_enable(guint dev, guint chn);
gint mpp_ai_chn_get_fd(guint dev, guint chn);
gint mpp_ai_chn_read(guint dev, guint chn, guint8 **buf, guint *size,
                     guint64 *pts, gint timeout);
gint mpp_aenc_chn_disable(guint chn);
gint mpp_aenc_chn_enable_lpcm(guint chn);
gint mpp_mipi_restart(guint dev, raw_data_type_e type, guint x, guint y,
                      guint w, guint h, guint n_lanes, ...);
gint mpp_ov4689_mipi_restart(guint dev, guint x, guint y, guint w, guint h);
gint mpp_imx117_lvds_restart(guint dev, guint x, guint y, guint w, guint h,
                             guint width, guint channels);
// gint mpp_imx226_lvds_restart(guint dev, guint x, guint y, guint w, guint h);
// gint mpp_isp_register_ov4689(guint dev);
gint mpp_isp_register_imx117(guint dev);
// gint mpp_isp_register_imx226(guint dev);
void mpp_isp_hv_sync_disable(guint dev);
void mpp_isp_hv_sync_enable(guint dev);
gint mpp_isp_disable(guint dev);
gint mpp_isp_enable(guint dev, gboolean stitch, gfloat framerate,
                    ISP_BAYER_FORMAT_E bayer, guint x, guint y, guint w,
                    guint h);
gint mpp_isp_disable_dynamic_correction(guint dev);
gint mpp_isp_enable_dynamic_correction(guint dev, guint16 slope, guint16 thresh,
                                       guint16 hp_thresh);
gint mpp_vi_dev_disable(guint dev);
gint mpp_vi_dev_enable_mipi(guint dev, guint x, guint y, guint w, guint h,
                            gboolean bypass_isp);
gint mpp_vi_dev_enable_lvds(guint dev, guint x, guint y, guint w, guint h,
                            gboolean bypass_isp);
gint mpp_vi_dev_disable_dump(guint dev);
gint mpp_vi_dev_enable_dump(guint dev, guint depth);
gint mpp_vi_dev_disable_snap(guint dev);
gint mpp_vi_dev_enable_snap(guint dev, guint isp, guint depth);
gint mpp_vi_dev_read(guint dev, guint8 **buf, guint *size, guint64 *pts,
                     gint timeout);
gint mpp_vi_chn_disable(guint chn);
gint mpp_vi_chn_enable(guint chn, guint x, guint y, guint w, guint h,
                       gboolean mirror, gboolean flip, gboolean bypass_isp);
gint mpp_vi_chn_get_depth(guint chn, guint *depth);
gint mpp_vi_chn_set_depth(guint chn, guint depth);
gint mpp_vi_chn_get_frame(guint chn, VIDEO_FRAME_INFO_S *frame, gint timeout);
gint mpp_vi_chn_release_frame(guint chn, VIDEO_FRAME_INFO_S *frame);
guint64 mpp_video_frame_info_get_pts(VIDEO_FRAME_INFO_S *info);
gpointer mpp_video_frame_info_read_data(VIDEO_FRAME_INFO_S *info, guint *size);
gint mpp_vi_chn_get_pts(guint chn, guint64 *pts);
gint mpp_vi_chn_get_fd(guint chn);
// gint mpp_vi_chn_read(guint chn, gpointer *buf, guint *size, guint64 *pts,
// gint timeout);
gint mpp_vpss_grp_disable(guint grp);
gint mpp_vpss_grp_enable(guint grp, guint wl, guint wr, guint h,
                         gboolean nr_enable);
gint mpp_vpss_grp_send_frame(guint grp, guint pipe, VIDEO_FRAME_INFO_S *frame,
                             gint timeout);
gint mpp_vpss_chn_disable(guint grp, guint chn);
gint mpp_vpss_chn_enable(guint grp, guint chn, guint w, guint h,
                         gboolean mirror, gboolean flip, gint src_fr,
                         gint dest_fr);
gint mpp_vpss_chn_get_pts(guint grp, guint chn, guint64 *pts);
gint mpp_vpss_chn_get_depth(guint grp, guint chn, guint *depth);
gint mpp_vpss_chn_set_depth(guint grp, guint chn, guint depth);
// gint mpp_vpss_chn_get_fd(guint grp, guint chn);
gint mpp_vpss_chn_get_frame(guint grp, guint chn, VIDEO_FRAME_INFO_S *frame,
                            gint timeout);
gint mpp_vpss_chn_release_frame(guint grp, guint chn,
                                VIDEO_FRAME_INFO_S *frame);
// gint mpp_vpss_chn_enable_ext(guint grp, guint chn, guint bind_chn, guint w,
// guint h);
gint mpp_venc_chn_disable(guint chn);
gint mpp_venc_chn_enable_jpeg(guint chn, guint w, guint h, guint32 q);
gint mpp_venc_chn_enable_mjpegvbr(guint chn, guint framerate, guint kbps,
                                  guint w, guint h);
gint mpp_venc_chn_enable_h265cbr(guint chn, guint profile, guint framerate,
                                 guint kbps, guint w, guint h);
gint mpp_venc_chn_enable_h265fixqp(guint chn, guint profile, guint framerate,
                                   guint qp, guint w, guint h);
gint mpp_venc_chn_enable_h264cbr(guint chn, guint profile, guint framerate,
                                 guint kbps, guint w, guint h);
gint mpp_venc_chn_enable_h264fixqp(guint chn, guint profile, guint framerate,
                                   guint qp, guint w, guint h);
gint mpp_venc_chn_enable_h264avbr(guint chn, guint profile, guint framerate,
                                  guint kbps, guint w, guint h);
gint mpp_venc_chn_stop(guint chn);
gint mpp_venc_chn_start(guint chn, guint n_frames);
gint mpp_venc_chn_get_fd(guint chn);
gint mpp_venc_chn_send_frame(guint chn, VIDEO_FRAME_INFO_S *frame,
                             gint timeout);
gint mpp_venc_chn_send_data(guint chn, guint8 *data, guint32 width,
                            guint32 height, guint64 pts, gint timeout);
gint mpp_venc_chn_read(guint chn, guint8 **buf, guint *size, guint64 *pts,
                       gint timeout);

gint mpp_isp_nr_set_manual(guint dev, guint8 thresh);
gint mpp_isp_nr_set_auto(guint dev);
gint mpp_isp_nr_disable(guint dev);

gint mpp_isp_expo_query_info(guint dev, guint *expo_time, guint *a_gain,
                             guint *d_gain, guint *isp_gain, guint *expo,
                             guint *iso);
gint mpp_isp_expo_set_auto(guint dev, guint8 compensation, gboolean low_light,
                           gint expo_time_min, gint expo_time_max,
                           gint a_gain_min, gint a_gain_max, gint d_gain_min,
                           gint d_gain_max, gint isp_gain_min,
                           gint isp_gain_max);
gint mpp_isp_expo_set_manual(guint dev, gint expo_time, gint a_gain,
                             gint d_gain, gint isp_gain);
gint mpp_isp_expo_get_compensation(guint dev, guint8 *compensation);
gint mpp_isp_expo_set_compensation(guint dev, guint8 compensation);

gint mpp_isp_wb_query_info(guint dev, guint16 *r, guint16 *gr, guint16 *gb,
                           guint16 *b, guint16 *saturation, guint16 *temp);
gint mpp_isp_wb_set_bypass(guint dev);
gint mpp_isp_wb_set_manual(guint dev, guint16 r, guint16 gr, guint16 gb,
                           guint16 b);
gint mpp_isp_wb_set_auto(guint dev);
gint mpp_isp_wb_set_fine_tune(guint dev, gboolean enable);
gint mpp_isp_wb_get_fine_tune(guint dev, gboolean *enable);

gint mpp_isp_colortone_set(guint dev, guint16 r, guint16 g, guint16 b);
gint mpp_isp_colortone_get(guint dev, guint16 *r, guint16 *g, guint16 *b);

gint mpp_isp_acm_is_enable(guint dev, gboolean *is_enable);
gint mpp_isp_acm_enable(guint dev);
gint mpp_isp_acm_disable(guint dev);
gint mpp_isp_acm_demo_is_enable(guint dev, gboolean *is_enable);
gint mpp_isp_acm_enable_demo(guint dev);
gint mpp_isp_acm_disable_demo(guint dev);
gint mpp_isp_acm_get_stretch_range(guint dev, guint32 *range);
gint mpp_isp_acm_set_stretch_range(guint dev, guint32 range);
gint mpp_isp_acm_get_clip_range(guint dev, guint32 *range);
gint mpp_isp_acm_set_clip_range(guint dev, guint32 range);
gint mpp_isp_acm_get_clip_or_wrap(guint dev, guint32 *mode);
gint mpp_isp_acm_set_clip_or_wrap(guint dev, guint32 mode);
gint mpp_isp_acm_get_cbcr_thr(guint dev, guint32 *thr);
gint mpp_isp_acm_set_cbcr_thr(guint dev, guint32 thr);
gint mpp_isp_acm_get_luma_gain(guint dev, guint32 *gain);
gint mpp_isp_acm_set_luma_gain(guint dev, guint32 gain);
gint mpp_isp_acm_get_hue_gain(guint dev, guint32 *gain);
gint mpp_isp_acm_set_hue_gain(guint dev, guint32 gain);
gint mpp_isp_acm_get_sat_gain(guint dev, guint32 *gain);
gint mpp_isp_acm_set_sat_gain(guint dev, guint32 gain);
gint mpp_isp_acm_get_coeff(guint dev, guint16 **y, guint *y_len, guint16 **h,
                           guint *h_len, guint16 **s, guint *s_len);
gint mpp_isp_acm_set_coeff(guint dev, const guint16 *y, guint y_len,
                           const guint16 *h, guint h_len, const guint16 *s,
                           guint s_len);

gint mpp_isp_gamma_is_enable(guint dev, gboolean *is_enable);
gint mpp_isp_gamma_enable(guint dev);
gint mpp_isp_gamma_disable(guint dev);
gint mpp_isp_gamma_set_default(guint dev);
gint mpp_isp_gamma_set_srgb(guint dev);
gint mpp_isp_gamma_set_user(guint dev, const guint16 *table, guint table_len);

gint mpp_isp_saturation_set_manual(guint dev, guint8 sat);
gint mpp_isp_saturation_get_manual(guint dev, guint8 *sat);
gint mpp_isp_saturation_set_auto(guint dev, const guint8 *sat, guint len);
gint mpp_isp_colormatrix_set_manual(guint dev, const guint16 *ccm, guint len,
                                    gboolean enable_sat);
gint mpp_isp_colormatrix_set_auto(guint dev, guint16 high_temp,
                                  const guint16 *high_ccm, guint high_len,
                                  guint16 mid_temp, const guint16 *mid_ccm,
                                  guint mid_len, guint16 low_temp,
                                  const guint16 *low_ccm, guint low_len,
                                  gboolean bypass_iso, gboolean bypass_temp);

#endif
