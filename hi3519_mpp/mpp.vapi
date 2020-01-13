
[CCode (cheader_filename = "mpp.h")]
namespace Mpp {

    [CCode (cname = "IFD0_OFFSET")]
    public const uint32 IFD0_OFFSET;

    [CCode (cname = "IFD0_ENTRY_BEGIN")]
    public const uint32 IFD0_ENTRY_BEGIN;

    [CCode (cname = "IFD0_ENTRY_END")]
    public const uint32 IFD0_ENTRY_END;

    [CCode (cname = "N_ENTRIES")]
    public const uint16 N_ENTRIES;

    [CCode (cname = "MAKE_OFFSET")]
    public const uint32 MAKE_OFFSET;

    [CCode (cname = "MODEL_OFFSET")]
    public const uint32 MODEL_OFFSET;

    [CCode (cname = "EXPOSURE_TIME_OFFSET")]
    public const uint32 EXPOSURE_TIME_OFFSET;

    [CCode (cname = "F_NUMBER_OFFSET")]
    public const uint32 F_NUMBER_OFFSET;

    [CCode (cname = "COLOR_MATRIX_1_OFFSET")]
    public const uint32 COLOR_MATRIX_1_OFFSET;

    [CCode (cname = "AS_SHOT_NEUTRAL_OFFSET")]
    public const uint32 AS_SHOT_NEUTRAL_OFFSET;

    [CCode (cname = "TIFFEntry", has_destroy_function = false, has_copy_function = false, has_type_id = false)]
    public struct TIFFEntry {
        uint16 tag;
        uint16 type;
        uint32 n;
        uint32 offset;
    }

    [CCode (cname = "TIFFMeta", has_destroy_function = false, has_copy_function = false, has_type_id = false)]
    public struct TIFFMeta {

        uint8 byte_order[2];
        uint16 magic_number;
        uint32 ifd0_offset;

        uint16 n_entries;
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
        uint32 ifd_next_offset;

        uint8 make_value[8];  // Teche
        uint8 model_value[8]; // PHIIMAX
        uint32 exposure_time_value[2];
        uint32 f_number_value[2];
        int32 color_matrix_1_value[18];
        uint32 as_shot_neutral_value[6];
    }

    [CCode (cname = "VIDEO_FRAME_INFO_S", has_destroy_function = false, has_copy_function = false, has_type_id = false)]
    public struct VideoFrameInfo {
        public uint64 get_pts();
        [CCode (array_length_type = "guint")]
        public uint8[]? read_data();
    }

    [CCode (cname = "AUDIO_SAMPLE_RATE_E", cprefix = "AUDIO_SAMPLE_RATE_", has_type_id = false)]
    public enum AudioSampleRate {
        @8000,
        @11025,
        @12000,
        @16000,
        @22050,
        @24000,
        @32000,
        @44100,
        @48000,
        @64000,
        @96000
    }

    [CCode (cname = "ACODEC_MIXER_E", cprefix = "ACODEC_MIXER_", has_type_id = false)]
    public enum AudioInputMode {
        IN0,
        IN1,
        IN_D
    }

    [CCode (cname = "raw_data_type_e", cprefix = "RAW_DATA_", has_type_id = false)]
    public enum MipiDataType {
        @8BIT,
        @10BIT,
        @12BIT,
        @14BIT,
        @16BIT
    }

    [CCode (cname = "MOD_ID_E", cprefix = "HI_ID_", has_type_id = false)]
    public enum Mod {
        CMPI,
        VB,
        SYS,
        RGN,
        CHNL,
        VDEC,
        GROUP,
        VPSS,
        VENC,
        VDA,
        H264E,
        JPEGE,
        MPEG4E,
        H264D,
        JPEGD,
        VOU,
        VIU,
        DSU,
        VALG,
        RC,
        AIO,
        AI,
        AO,
        AENC,
        ADEC,
        AVENC,
        PCIV,
        PCIVFMW,
        ISP,
        IVE,
        DCCM,
        DCCS,
        PROC,
        LOG,
        MST_LOG,
        VD,
        VCMP,
        FB,
        HDMI,
        VOIE,
        TDE,
        USR,
        VEDU,
        VGS,
        H265E,
        FD,
        ODT,
        VQA,
        LPR,
        FISHEYE,
        PHOTO,
        EXTAO
    }

    [CCode (cname = "ISP_BAYER_FORMAT_E", cprefix = "BAYER_", has_type_id = false)]
    public enum BayerFormat {
        RGGB,
        GRBG,
        GBRG,
        BGGR
    }

    int reg_read(uint address, out uint value);
    int reg_write(uint address, uint value);

    int disable();
    int enable(uint n_pools, ...);
    int bind(Mod src_mod, uint src_dev, uint src_chn, Mod dest_mod, uint dest_dev, uint dest_chn);
    int unbind(Mod src_mod, uint src_dev, uint src_chn, Mod dest_mod, uint dest_dev, uint dest_chn);
    uint64 get_pts();
    void sync_pts(uint64 pts);
    void init_pts(uint64 pts);
    int acodec_set_volume(uint volume);
    int acodec_restart(AudioInputMode input_mode, AudioSampleRate sample_rate, uint volume);
    int ai_dev_disable(uint dev);
    int ai_dev_enable(uint dev, bool stereo, AudioSampleRate sample_rate, uint n_channels);
    int ai_chn_disable(uint dev, uint chn);
    int ai_chn_enable(uint dev, uint chn);
    int ai_chn_get_fd(uint dev, uint chn);
    int ai_chn_read(uint dev, uint chn, [CCode (array_length_type = "guint")] out uint8[] buf, out uint64 pts, int timeout);
    int aenc_chn_disable(uint chn);
    int aenc_chn_enable_lpcm(uint chn);
    int vi_dev_disable(uint dev);
    int vi_dev_enable_mipi(uint dev, uint x, uint y, uint w, uint h, bool bypass_isp = false);
    int vi_dev_enable_lvds(uint dev, uint x, uint y, uint w, uint h, bool bypass_isp = false);
    int vi_dev_disable_dump(uint dev);
    int vi_dev_enable_dump(uint dev, uint depth);
    int vi_dev_disable_snap(uint dev);
    int vi_dev_enable_snap(uint dev, uint isp, uint depth);
    int vi_dev_read(uint dev, [CCode (array_length_type = "guint")] out uint8[] buf, out uint64 pts, int timeout);
    int vi_chn_disable(uint chn);
    int vi_chn_enable(uint chn, uint x, uint y, uint w, uint h, bool mirror = false, bool flip = false, bool bypass_isp = false);
    int vi_chn_get_depth(uint chn, out uint depth);
    int vi_chn_set_depth(uint chn, uint depth);
    int vi_chn_get_frame(uint chn, ref VideoFrameInfo frame, int timeout);
    int vi_chn_release_frame(uint chn, VideoFrameInfo frame);
    // int vi_chn_get_frame_info(uint chn, out uint w, out uint h, out uint64 pts, int timeout);
    // int vi_chn_get_pts(uint chn, out uint64 pts);
    int vi_chn_get_fd(uint chn);
    // int vi_chn_read(uint chn, [CCode (array_length_type = "guint")] out uint8[] buf, out uint64 pts, int timeout);
    //int vi_chn_enable_ext(uint chn, uint bind_chn, uint w, uint h);
    //int mipi_restart(uint dev, MipiDataType type, uint x, uint y, uint w, uint h, uint n_lanes, ...);
    int ov4689_mipi_restart(uint dev, uint x, uint y, uint w, uint h);
    int imx117_lvds_restart(uint dev, uint x, uint y, uint w, uint h, uint width, uint channels);
    //int imx226_lvds_restart(uint dev, uint x, uint y, uint w, uint h);
    //int isp_register_ov4689(uint dev);
    int isp_register_imx117(uint dev);
    int isp_unregister_imx117(uint dev);
    //int isp_register_imx226(uint dev);
    void isp_hv_sync_disable(uint dev);
    void isp_hv_sync_enable(uint dev);
    int isp_disable(uint dev);
    int isp_enable(uint dev, bool stitch, float framerate, BayerFormat bayer, uint x, uint y, uint w, uint h);
    int isp_disable_dynamic_correction(uint dev);
    int isp_enable_dynamic_correction(uint dev, uint16 slope, uint16 thresh, uint16 hp_thresh);
    int vpss_grp_disable(uint grp);
    int vpss_grp_enable(uint grp, uint wl, uint wr, uint h, bool nr_enable = true);
    int vpss_grp_send_frame(uint grp, uint pipe, VideoFrameInfo frame, int timeout);
    int vpss_chn_get_depth(uint grp, uint chn, out uint depth);
    int vpss_chn_set_depth(uint grp, uint chn, uint depth);
    int vpss_chn_disable(uint grp, uint chn);
    int vpss_chn_enable(uint grp, uint chn, uint w, uint h, bool mirror = false, bool flip = false, int src_fr = -1, int dest_fr = -1);
    int vpss_chn_get_pts(uint grp, uint chn, out uint64 pts);
    int vpss_chn_get_frame(uint grp, uint chn, ref VideoFrameInfo frame, int timeout);
    int vpss_chn_release_frame(uint grp, uint chn, VideoFrameInfo frame);
    //int vpss_chn_enable_ext(uint grp, uint chn, uint bind_chn, uint w, uint h);
    int venc_chn_disable(uint chn);
    int venc_chn_enable_jpeg(uint chn, uint w, uint h, uint q = 90);
    int venc_chn_enable_mjpegvbr(uint chn, uint framerate, uint kbps, uint w, uint h);
    int venc_chn_enable_h265cbr(uint chn, uint profile, uint framerate, uint kbps, uint w, uint h);
    int venc_chn_enable_h265fixqp(uint chn, uint profile, uint framerate, uint qp, uint w, uint h);
    int venc_chn_enable_h264cbr(uint chn, uint profile, uint framerate, uint kbps, uint w, uint h);
    int venc_chn_enable_h264fixqp(uint chn, uint profile, uint framerate, uint qp, uint w, uint h);
    int venc_chn_enable_h264avbr(uint chn, uint profile, uint framerate, uint kbps, uint w, uint h);
    int venc_chn_stop(uint chn);
    int venc_chn_start(uint chn, uint n_frames);
    int venc_chn_get_fd(uint chn);
    int venc_chn_close_fd(uint chn);
    int venc_chn_send_frame(uint chn, VideoFrameInfo frame, int timeout);
    int venc_chn_send_data(uint chn, uint8* data, uint32 width, uint32 height, uint64 pts, int timeout);
    int venc_chn_read(uint chn, [CCode (array_length_type = "guint")] out uint8[] buf, out uint64 pts, int timeout);

    int isp_nr_set_manual(uint dev, uint8 thresh);
    int isp_nr_set_auto(uint dev);
    int isp_nr_disable(uint dev);

    int isp_expo_query_info(uint dev, out uint expo_time, out uint a_gain, out uint d_gain, out uint isp_gain, out uint expo, out uint iso);
    // int isp_expo_set_auto(uint dev, uint8 compensation, bool low_light);
    // int isp_expo_set_auto(uint dev, uint8 compensation, bool low_light = false, int expo_time = -1, int a_gain = -1, int d_gain = -1, int isp_gain = -1);
    int isp_expo_set_auto(uint dev, uint8 compensation, bool low_light,
                          int expo_time_min, int expo_time_max,
                          int a_gain_min, int a_gain_max,
                          int d_gain_min, int d_gain_max,
                          int isp_gain_min, int isp_gain_max);
    int isp_expo_set_manual(uint dev, int expo_time, int a_gain, int d_gain, int isp_gain);
    int isp_expo_get_compensation(uint dev, out uint8 compensation);
    int isp_expo_set_compensation(uint dev, uint8 compensation);

    int isp_wb_query_info(uint dev, out uint16 r, out uint16 gr, out uint16 gb, out uint16 b, out uint16 saturation, out uint16 temp);
    int isp_wb_set_bypass(uint dev);
    int isp_wb_set_manual(uint dev, uint16 r, uint16 gr, uint16 gb, uint16 b);
    int isp_wb_set_auto(uint dev);
    int isp_wb_set_fine_tune(uint dev, bool enable);
    int isp_wb_get_fine_tune(uint dev, out bool enable);

    int isp_colortone_set(uint dev, uint16 r, uint16 g, uint16 b);
    int isp_colortone_get(uint dev, out uint16 r, out uint16 g, out uint16 b);

    int isp_acm_is_enable(uint dev, out bool is_enable);
    int isp_acm_enable(uint dev);
    int isp_acm_disable(uint dev);
    int isp_acm_demo_is_enable(uint dev, out bool is_enable);
    int isp_acm_enable_demo(uint dev);
    int isp_acm_disable_demo(uint dev);
    int isp_acm_get_stretch_range(uint dev, out uint32 range);
    int isp_acm_set_stretch_range(uint dev, uint32 range);
    int isp_acm_get_clip_range(uint dev, out uint32 range);
    int isp_acm_set_clip_range(uint dev, uint32 range);
    int isp_acm_get_clip_or_wrap(uint dev, out uint32 mode);
    int isp_acm_set_clip_or_wrap(uint dev, uint32 mode);
    int isp_acm_get_cbcr_thr(uint dev, out uint32 thr);
    int isp_acm_set_cbcr_thr(uint dev, uint32 thr);
    int isp_acm_get_luma_gain(uint dev, out uint32 gain);
    int isp_acm_set_luma_gain(uint dev, uint32 gain);
    int isp_acm_get_hue_gain(uint dev, out uint32 gain);
    int isp_acm_set_hue_gain(uint dev, uint32 gain);
    int isp_acm_get_sat_gain(uint dev, out uint32 gain);
    int isp_acm_set_sat_gain(uint dev, uint32 gain);

    int isp_acm_get_coeff(uint dev,
                          [CCode (array_length_type = "guint")] out uint16[] y,
                          [CCode (array_length_type = "guint")] out uint16[] h,
                          [CCode (array_length_type = "guint")] out uint16[] s);

    int isp_acm_set_coeff(uint dev,
                          [CCode (array_length_type = "guint")] uint16[] y,
                          [CCode (array_length_type = "guint")] uint16[] h,
                          [CCode (array_length_type = "guint")] uint16[] s);

    int isp_gamma_is_enable(uint dev, out bool is_enable);
    int isp_gamma_enable(uint dev);
    int isp_gamma_disable(uint dev);
    int isp_gamma_set_default(uint dev);
    int isp_gamma_set_srgb(uint dev);
    int isp_gamma_set_user(uint dev, [CCode (array_length_type = "guint")] uint16[] table);

    int isp_saturation_set_manual(uint dev, uint8 sat);
    int isp_saturation_get_manual(uint dev, out uint8 sat);
    int isp_saturation_set_auto(uint dev, uint8[] sat);

    int isp_colormatrix_set_manual(uint dev, uint16[] ccm, bool enable_sat);
    int isp_colormatrix_set_auto(uint dev,
                                 uint16 high_temp, uint16[] high_ccm,
                                 uint16 mid_temp, uint16[] mid_ccm,
                                 uint16 low_temp, uint16[] low_len,
                                 bool bypass_iso, bool bypass_temp);


    int isp_set_bayer_sharpen_attr(uint dev, string base64);
    int isp_get_bayer_sharpen_attr(uint dev, out string base64);
    int isp_set_yuv_sharpen_attr(uint dev, string base64);
    int isp_get_yuv_sharpen_attr(uint dev, out string base64);
    int isp_set_nr_attr(uint dev, string base64);
    int isp_get_nr_attr(uint dev, out string base64);
    int isp_set_np_table(uint dev, string base64);
    int isp_get_np_table(uint dev, out string base64);
    int isp_set_expo_attr(uint dev, string base64);
    int isp_get_expo_attr(uint dev, out string base64);
}
