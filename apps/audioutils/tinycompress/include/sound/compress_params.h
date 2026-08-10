/****************************************************************************
 * apps/audioutils/tinycompress/include/sound/compress_params.h
 *
 * This header was automatically generated from a Linux kernel header
 * of the same name, to make information necessary for userspace to
 * call into the kernel available to libc.  It contains only constants,
 * structures, and macros generated from the original header, and thus,
 * contains no copyrightable information.
 *
 * To edit the content of this header, modify the corresponding
 * source file (e.g. under external/kernel-headers/original/) then
 * run bionic/libc/kernel/tools/update_all.py
 *
 * Any manual change here will be lost the next time this script will
 * be run. You've been warned!
 *
 ****************************************************************************/

#ifndef __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H
#define __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/audio/audio.h>

#ifdef __cplusplus
extern "C"
{
#endif

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

#define MAX_NUM_CODECS                      64
#define MAX_NUM_CODEC_DESCRIPTORS           32
#define MAX_NUM_BITRATES                    32
#define MAX_NUM_SAMPLE_RATES                32

#define SND_AUDIOCODEC_PCM                  AUDIO_FMT_PCM
#define SND_AUDIOCODEC_MP3                  AUDIO_FMT_MP3
#define SND_AUDIOCODEC_AMR                  AUDIO_FMT_AMR
#define SND_AUDIOCODEC_AMRWB                AUDIO_FMT_AMRWB
#define SND_AUDIOCODEC_AMRWBPLUS            AUDIO_FMT_AMRWBPLUS
#define SND_AUDIOCODEC_AAC                  AUDIO_FMT_AAC
#define SND_AUDIOCODEC_WMA                  AUDIO_FMT_WMA
#define SND_AUDIOCODEC_SBC                  AUDIO_FMT_SBC
#define SND_AUDIOCODEC_OPUS                 AUDIO_FMT_OPUS
#define SND_AUDIOCODEC_REAL                 AUDIO_FMT_REAL
#define SND_AUDIOCODEC_VORBIS               AUDIO_FMT_OGG_VORBIS
#define SND_AUDIOCODEC_FLAC                 AUDIO_FMT_FLAC
#define SND_AUDIOCODEC_IEC61937             AUDIO_FMT_IEC61937
#define SND_AUDIOCODEC_G723_1               AUDIO_FMT_G723_1
#define SND_AUDIOCODEC_G729                 AUDIO_FMT_G729
#define SND_AUDIOCODEC_BESPOKE              AUDIO_FMT_UNDEF
#define SND_AUDIOCODEC_ALAC                 AUDIO_FMT_ALAC
#define SND_AUDIOCODEC_APE                  AUDIO_FMT_APE
#define SND_AUDIOCODEC_SPEEX                AUDIO_FMT_SPEEX

#define SND_AUDIOCODEC_MAX                  AUDIO_FMT_SPEEX
#define SND_AUDIOPROFILE_PCM                AUDIO_PROFILE_PCM

#define SND_AUDIOMODE_PCM_MP1               AUDIO_SUBFMT_PCM_MP1
#define SND_AUDIOMODE_PCM_MP2               AUDIO_SUBFMT_PCM_MP2
#define SND_AUDIOMODE_PCM_MP3               AUDIO_SUBFMT_PCM_MP3
#define SND_AUDIOMODE_PCM_MU_LAW            AUDIO_SUBFMT_PCM_MU_LAW
#define SND_AUDIOMODE_PCM_A_LAW             AUDIO_SUBFMT_PCM_A_LAW
#define SND_AUDIOMODE_PCM_U8                AUDIO_SUBFMT_PCM_U8
#define SND_AUDIOMODE_PCM_S8                AUDIO_SUBFMT_PCM_S8
#define SND_AUDIOMODE_PCM_U16_LE            AUDIO_SUBFMT_PCM_U16_LE
#define SND_AUDIOMODE_PCM_S16_BE            AUDIO_SUBFMT_PCM_S16_BE
#define SND_AUDIOMODE_PCM_S16_LE            AUDIO_SUBFMT_PCM_S16_LE
#define SND_AUDIOMODE_PCM_U16_BE            AUDIO_SUBFMT_PCM_U16_BE
#define SND_AUDIOMODE_PCM_U24_LE            AUDIO_SUBFMT_PCM_U24_LE
#define SND_AUDIOMODE_PCM_S24_BE            AUDIO_SUBFMT_PCM_S24_BE
#define SND_AUDIOMODE_PCM_S24_LE            AUDIO_SUBFMT_PCM_S24_LE
#define SND_AUDIOMODE_PCM_U24_BE            AUDIO_SUBFMT_PCM_U24_BE
#define SND_AUDIOMODE_PCM_U32_LE            AUDIO_SUBFMT_PCM_U32_LE
#define SND_AUDIOMODE_PCM_U32_BE            AUDIO_SUBFMT_PCM_U32_BE
#define SND_AUDIOMODE_PCM_S32_LE            AUDIO_SUBFMT_PCM_S32_LE
#define SND_AUDIOMODE_PCM_S32_BE            AUDIO_SUBFMT_PCM_S32_BE

#define SND_AUDIOCHANMODE_MP3_MONO          AUDIO_CHANMODE_MONO
#define SND_AUDIOCHANMODE_MP3_STEREO        AUDIO_CHANMODE_STEREO
#define SND_AUDIOCHANMODE_MP3_JOINTSTEREO   AUDIO_CHANMODE_JOINTSTEREO
#define SND_AUDIOCHANMODE_MP3_DUAL          AUDIO_CHANMODE_DUAL

#define SND_AUDIOPROFILE_AMR                AUDIO_PROFILE_AMR

#define SND_AUDIOMODE_AMR_DTX_OFF           AUDIO_SUBFMT_AMR_DTX_OFF
#define SND_AUDIOMODE_AMR_VAD1              AUDIO_SUBFMT_AMR_VAD1
#define SND_AUDIOMODE_AMR_VAD2              AUDIO_SUBFMT_AMR_VAD2

#define SND_AUDIOSTREAMFORMAT_UNDEFINED     AUDIO_STREAMFORMAT_UNDEF
#define SND_AUDIOSTREAMFORMAT_CONFORMANCE   AUDIO_STREAMFORMAT_CONFORMANCE
#define SND_AUDIOSTREAMFORMAT_IF1           AUDIO_STREAMFORMAT_IF1
#define SND_AUDIOSTREAMFORMAT_IF2           AUDIO_STREAMFORMAT_IF2
#define SND_AUDIOSTREAMFORMAT_FSF           AUDIO_STREAMFORMAT_FSF
#define SND_AUDIOSTREAMFORMAT_RTPPAYLOAD    AUDIO_STREAMFORMAT_RTPPAYLOAD
#define SND_AUDIOSTREAMFORMAT_ITU           AUDIO_STREAMFORMAT_ITU

#define SND_AUDIOPROFILE_AMRWB              AUDIO_PROFILE_AMRWB

#define SND_AUDIOMODE_AMRWB_DTX_OFF         AUDIO_SUBFMT_AMRWB_DTX_OFF
#define SND_AUDIOMODE_AMRWB_VAD1            AUDIO_SUBFMT_AMRWB_VAD1
#define SND_AUDIOMODE_AMRWB_VAD2            AUDIO_SUBFMT_AMRWB_VAD2

#define SND_AUDIOPROFILE_AMRWBPLUS          AUDIO_PROFILE_AMRWBPLUS

#define SND_AUDIOPROFILE_AAC                AUDIO_PROFILE_AAC

#define SND_AUDIOMODE_AAC_MAIN              AUDIO_SUBFMT_AAC_MAIN
#define SND_AUDIOMODE_AAC_LC                AUDIO_SUBFMT_AAC_LC
#define SND_AUDIOMODE_AAC_SSR               AUDIO_SUBFMT_AAC_SSR
#define SND_AUDIOMODE_AAC_LTP               AUDIO_SUBFMT_AAC_LTP
#define SND_AUDIOMODE_AAC_HE                AUDIO_SUBFMT_AAC_HE
#define SND_AUDIOMODE_AAC_SCALABLE          AUDIO_SUBFMT_AAC_SCALABLE
#define SND_AUDIOMODE_AAC_ERLC              AUDIO_SUBFMT_AAC_ERLC
#define SND_AUDIOMODE_AAC_LD                AUDIO_SUBFMT_AAC_LD
#define SND_AUDIOMODE_AAC_HE_PS             AUDIO_SUBFMT_AAC_HE_PS
#define SND_AUDIOMODE_AAC_HE_MPS            AUDIO_SUBFMT_AAC_HE_MPS

#define SND_AUDIOSTREAMFORMAT_MP2ADTS       AUDIO_STREAMFORMAT_MP2ADTS
#define SND_AUDIOSTREAMFORMAT_MP4ADTS       AUDIO_STREAMFORMAT_MP4ADTS
#define SND_AUDIOSTREAMFORMAT_MP4LOAS       AUDIO_STREAMFORMAT_MP4LOAS
#define SND_AUDIOSTREAMFORMAT_MP4LATM       AUDIO_STREAMFORMAT_MP4LATM
#define SND_AUDIOSTREAMFORMAT_ADIF          AUDIO_STREAMFORMAT_ADIF
#define SND_AUDIOSTREAMFORMAT_MP4FF         AUDIO_STREAMFORMAT_MP4FF
#define SND_AUDIOSTREAMFORMAT_RAW           AUDIO_STREAMFORMAT_RAW
#define SND_AUDIOSTREAMFORMAT_LATM          AUDIO_STREAMFORMAT_LATM

#define SND_AUDIOPROFILE_WMA7               AUDIO_PROFILE_WMA7
#define SND_AUDIOPROFILE_WMA8               AUDIO_PROFILE_WMA8
#define SND_AUDIOPROFILE_WMA9               AUDIO_PROFILE_WMA9
#define SND_AUDIOPROFILE_WMA10              AUDIO_PROFILE_WMA10
#define SND_AUDIOPROFILE_WMA9_PRO           AUDIO_PROFILE_WMA9_PRO
#define SND_AUDIOPROFILE_WMA9_LOSSLESS      AUDIO_PROFILE_WMA9_LOSSLESS
#define SND_AUDIOPROFILE_WMA10_LOSSLESS     AUDIO_PROFILE_WMA10_LOSSLESS

#define SND_AUDIOMODE_WMA_LEVEL1            AUDIO_SUBFMT_WMA_LEVEL1
#define SND_AUDIOMODE_WMA_LEVEL2            AUDIO_SUBFMT_WMA_LEVEL2
#define SND_AUDIOMODE_WMA_LEVEL3            AUDIO_SUBFMT_WMA_LEVEL3
#define SND_AUDIOMODE_WMA_LEVEL4            AUDIO_SUBFMT_WMA_LEVEL4
#define SND_AUDIOMODE_WMAPRO_LEVELM         AUDIO_SUBFMT_WMAPRO_LEVELM0
#define SND_AUDIOMODE_WMAPRO_LEVELM1        AUDIO_SUBFMT_WMAPRO_LEVELM1
#define SND_AUDIOMODE_WMAPRO_LEVELM2        AUDIO_SUBFMT_WMAPRO_LEVELM2
#define SND_AUDIOMODE_WMAPRO_LEVELM3        AUDIO_SUBFMT_WMAPRO_LEVELM3

#define SND_AUDIOSTREAMFORMAT_WMA_ASF       AUDIO_STREAMFORMAT_WMA_ASF
#define SND_AUDIOSTREAMFORMAT_WMA_NOASF_HDR AUDIO_STREAMFORMAT_WMA_NOASF_HDR

#define SND_AUDIOPROFILE_SBC                AUDIO_PROFILE_SBC

#define SND_AUDIOMODE_SBC                   AUDIO_SUBFMT_SBC

#define SND_AUDIOSTREAMFORMAT_SBC_PACKED    AUDIO_STREAMFORMAT_SBC_PACKED

#define SND_AUDIOPROFILE_REALAUDIO          AUDIO_PROFILE_REALAUDIO

#define SND_AUDIOMODE_REALAUDIO_G2          AUDIO_SUBFMT_REALAUDIO_G2
#define SND_AUDIOMODE_REALAUDIO_8           AUDIO_SUBFMT_REALAUDIO_8
#define SND_AUDIOMODE_REALAUDIO_1           AUDIO_SUBFMT_REALAUDIO_10
#define SND_AUDIOMODE_REALAUDIO_SURROUND    AUDIO_SUBFMT_REALAUDIO_SURROUND

#define SND_AUDIOPROFILE_VORBIS             AUDIO_PROFILE_VORBIS

#define SND_AUDIOMODE_VORBIS                AUDIO_SUBFMT_VORBIS

#define SND_AUDIOPROFILE_FLAC               AUDIO_PROFILE_FLAC

#define SND_AUDIOMODE_FLAC_LEVEL0           AUDIO_SUBFMT_FLAC_LEVEL0
#define SND_AUDIOMODE_FLAC_LEVEL1           AUDIO_SUBFMT_FLAC_LEVEL1
#define SND_AUDIOMODE_FLAC_LEVEL2           AUDIO_SUBFMT_FLAC_LEVEL2
#define SND_AUDIOMODE_FLAC_LEVEL3           AUDIO_SUBFMT_FLAC_LEVEL3
#define SND_AUDIOMODE_FLAC_LEVEL4           AUDIO_SUBFMT_FLAC_LEVEL4
#define SND_AUDIOMODE_FLAC_LEVEL5           AUDIO_SUBFMT_FLAC_LEVEL5
#define SND_AUDIOMODE_FLAC_LEVEL6           AUDIO_SUBFMT_FLAC_LEVEL6
#define SND_AUDIOMODE_FLAC_LEVEL7           AUDIO_SUBFMT_FLAC_LEVEL7
#define SND_AUDIOMODE_FLAC_LEVEL8           AUDIO_SUBFMT_FLAC_LEVEL8

#define SND_AUDIOSTREAMFORMAT_FLAC          AUDIO_STREAMFORMAT_FLAC
#define SND_AUDIOSTREAMFORMAT_FLAC_OGG      AUDIO_STREAMFORMAT_FLAC_OGG

#define SND_AUDIOPROFILE_IEC61937           AUDIO_PROFILE_IEC61937
#define SND_AUDIOPROFILE_IEC61937_SPDIF     AUDIO_PROFILE_IEC61937_SPDIF

#define SND_AUDIOMODE_IEC_REF_STREAM_HEADER AUDIO_SUBFMT_IEC_REF_STREAM_HEADER
#define SND_AUDIOMODE_IEC_LPCM              AUDIO_SUBFMT_IEC_LPCM
#define SND_AUDIOMODE_IEC_AC3               AUDIO_SUBFMT_IEC_AC3
#define SND_AUDIOMODE_IEC_MPEG1             AUDIO_SUBFMT_IEC_MPEG1
#define SND_AUDIOMODE_IEC_MP3               AUDIO_SUBFMT_IEC_MP3
#define SND_AUDIOMODE_IEC_MPEG2             AUDIO_SUBFMT_IEC_MPEG2
#define SND_AUDIOMODE_IEC_AACLC             AUDIO_SUBFMT_IEC_AACLC
#define SND_AUDIOMODE_IEC_DTS               AUDIO_SUBFMT_IEC_DTS
#define SND_AUDIOMODE_IEC_ATRAC             AUDIO_SUBFMT_IEC_ATRAC
#define SND_AUDIOMODE_IEC_SACD              AUDIO_SUBFMT_IEC_SACD
#define SND_AUDIOMODE_IEC_EAC3              AUDIO_SUBFMT_IEC_EAC3
#define SND_AUDIOMODE_IEC_DTS_HD            AUDIO_SUBFMT_IEC_DTS_HD
#define SND_AUDIOMODE_IEC_MLP               AUDIO_SUBFMT_IEC_MLP
#define SND_AUDIOMODE_IEC_DST               AUDIO_SUBFMT_IEC_DST
#define SND_AUDIOMODE_IEC_WMAPRO            AUDIO_SUBFMT_IEC_WMAPRO
#define SND_AUDIOMODE_IEC_REF_CXT           AUDIO_SUBFMT_IEC_REF_CXT
#define SND_AUDIOMODE_IEC_HE_AAC            AUDIO_SUBFMT_IEC_HE_AAC
#define SND_AUDIOMODE_IEC_HE_AAC2           AUDIO_SUBFMT_IEC_HE_AAC2
#define SND_AUDIOMODE_IEC_MPEG_SURROUND     AUDIO_SUBFMT_IEC_MPEG_SURROUND

#define SND_AUDIOPROFILE_G723_1             AUDIO_PROFILE_G723_1

#define SND_AUDIOMODE_G723_1_ANNEX_A        AUDIO_SUBFMT_G723_1_ANNEX_A
#define SND_AUDIOMODE_G723_1_ANNEX_B        AUDIO_SUBFMT_G723_1_ANNEX_B
#define SND_AUDIOMODE_G723_1_ANNEX_C        AUDIO_SUBFMT_G723_1_ANNEX_C

#define SND_AUDIOPROFILE_G729               AUDIO_PROFILE_G729

#define SND_AUDIOMODE_G729_ANNEX_A          AUDIO_SUBFMT_G729_ANNEX_A
#define SND_AUDIOMODE_G729_ANNEX_B          AUDIO_SUBFMT_G729_ANNEX_B

#define SND_RATECONTROLMODE_CONSTANTBITRATE AUDIO_RATECONTROL_CONSTANT
#define SND_RATECONTROLMODE_VARIABLEBITRATE AUDIO_RATECONTROL_VARIABLE

#define snd_enc_wma                         audio_enc_wma_s
#define snd_enc_vorbis                      audio_enc_vorbis_s
#define snd_enc_real                        audio_enc_real_s
#define snd_enc_flac                        audio_enc_flac_s
#define snd_enc_sbc                         audio_enc_sbc_s
#define snd_enc_lc3                         audio_enc_lc3_s
#define snd_enc_spx                         audio_enc_spx_s
#define snd_enc_generic                     audio_enc_generic_s
#define snd_dec_flac                        audio_dec_flac_s
#define snd_dec_wma                         audio_dec_wma_s
#define snd_dec_alac                        audio_dec_alac_s
#define snd_dec_ape                         audio_dec_ape_s
#define snd_dec_lc3                         audio_dec_lc3_s
#define snd_codec_options                   audio_codec_options_u

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct snd_codec_desc
{
  uint32_t max_ch;
  uint32_t sample_rates[MAX_NUM_SAMPLE_RATES];
  uint32_t num_sample_rates;
  uint32_t bit_rate[MAX_NUM_BITRATES];
  uint32_t num_bitrates;
  uint32_t rate_control;
  uint32_t profiles;
  uint32_t modes;
  uint32_t formats;
  uint32_t min_buffer;
  uint32_t reserved[15];
};

struct snd_codec
{
  uint32_t id;
  uint32_t ch_in;
  uint32_t ch_out;
  uint32_t sample_rate;
  uint32_t bit_rate;
  uint32_t rate_control;
  uint32_t profile;
  uint32_t level;
  uint32_t ch_mode;
  uint32_t format;
  uint32_t align;
  union snd_codec_options options;
  uint32_t pcm_format;
  uint32_t reserved[2];
};

#ifdef __cplusplus
}
#endif

#endif /* __APPS_AUDIOUTILS_TINYCOMPRESS_INCLUDE_SOUND_COMPRESS_PARAMS_H */
