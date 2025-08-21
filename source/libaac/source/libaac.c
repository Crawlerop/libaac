#include <aacnext/libaac.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ixheaac_type_def.h"
#include "iusace_cnst.h"
#include "ixheaacd_error_codes.h"
#include "ixheaace_api.h"
#include "ixheaac_error_standards.h"
#include "ixheaacd_error_handler.h"
#include "ixheaacd_apicmd_standards.h"
#include "ixheaacd_memory_standards.h"
#include "ixheaacd_aac_config.h"

void *malloc_global(unsigned int size, unsigned alignment) {
#ifdef WIN32
  return _aligned_malloc(size, alignment);
#else
  void *ptr = NULL;
  if (posix_memalign((void **)&ptr, alignment, size)) {
    ptr = NULL;
  }
  return ptr;
#endif
}

void free_global(void * ptr) {
#ifdef WIN32
  _aligned_free(ptr);
#else
  free(ptr);
#endif
  ptr = NULL;
}

// Encoder
/* 01 - API Errors */

/* Fatal Errors */
pWORD8 enc_err_api_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Memory allocation failed", 
  (pWORD8) "AOT unsupported"
};

/* 02 - Config Errors */
pWORD8 enc_err_config_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid configuration", 
  (pWORD8) "Insufficient bit-reservoir size"
};

pWORD8 enc_err_mps_config_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid configuration", 
  (pWORD8) "Invalid Parameter"
};

pWORD8 enc_err_drc_config_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Missing configuration"
};

/* Fatal Errors */
pWORD8 enc_err_config_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid sampling frequency of the stream",
  (pWORD8) "Invalid number of channels in the stream",
  (pWORD8) "Invalid stereo preprocessing flag, use 0 or 1",
  (pWORD8) "Invalid quality level",
  (pWORD8) "Invalid PCM wordsize",
  (pWORD8) "Parametric stereo not allowed with AAC classic profiles",
  (pWORD8) "Invalid TNS flag, use 0 or 1",
  (pWORD8) "Invalid channels mask",
  (pWORD8) "Invalid PCE (Program Configuration Element) flag, use 0 or 1",
  (pWORD8) "Invalid use full band width flag, use 0 or 1",
};

pWORD8 enc_err_usac_config_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid sampling frequency", 
  (pWORD8) "Invalid resampler ratio"
};

pWORD8 enc_err_drc_config_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid configuration",
  (pWORD8) "Unsupported configuration",
  (pWORD8) "Parameter out of range",
  (pWORD8) "Compand failure",
};

/* 03 - Init Errors */
pWORD8 enc_err_mps_init_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  NULL
};

pWORD8 enc_err_drc_init_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid DRC gain points",  
  (pWORD8) "Invalid start subband index" 
};

/* Fatal Errors */
pWORD8 enc_err_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Resampler initialization failed",
  (pWORD8) "AAC initialization failed",
  (pWORD8) "AAC Plus initialization failed",
  (pWORD8) "Bitrate not supported for the given sampling frequency",
  (pWORD8) "Invalid TNS parameter",
  (pWORD8) "Scale factor band not supported",
  (pWORD8) "Invalid core sample rate",
  (pWORD8) "Invalid element type",
  (pWORD8) "Number of channels not supported",
  (pWORD8) "Invalid number of channels in element",
  (pWORD8) "Scale factor band initalization failed",
  (pWORD8) "TNS configuration initalization failed" 
};

pWORD8 enc_err_mps_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "MPS Initialization failed"
};

pWORD8 enc_err_usac_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Resampler initialization failed", (pWORD8) "Insufficient bit-reservoir size",
  (pWORD8) "Invalid core sample rate", (pWORD8) "Invalid element type",
  (pWORD8) "Bitbuffer initialization failed", (pWORD8) "Invalid codec mode"
};

pWORD8 enc_err_drc_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid channel index"
};

pWORD8 enc_err_sbr_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid number of channels",     (pWORD8) "Invalid sample rate mode",
  (pWORD8) "Invalid frequency coefficients", (pWORD8) "Invalid number of bands",
  (pWORD8) "Invalid buffer length",          (pWORD8) "SBR noise band not supported"
};

pWORD8 enc_err_ps_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "PS Initialization failed" 
};

/* 04 - Execution Errors */
pWORD8 enc_err_aac_exe_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Quantization zero spectrum detected",
  (pWORD8) "Insufficient bit reservoir for non zero spectrum"
};

pWORD8 enc_err_mps_exe_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Encoding Failed", 
  (pWORD8) "Invalid MPS data bands"
};

pWORD8 enc_err_usac_exe_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Quantization zero spectrum detected",
  (pWORD8) "Insufficient bit reservoir for non zero spectrum"
};

pWORD8 enc_err_esbr_exe_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid bandwidth index encountered",
  (pWORD8) "Invalid number of patches",
  (pWORD8) "Invalid vocoder buffer",
  (pWORD8) "Invalid PVC mode",
  (pWORD8) "Invalid FFT size",
  (pWORD8) "Invalid start band",
  (pWORD8) "Invalid value encountered"
};

/* Fatal Errors */
pWORD8 enc_err_exe_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid SBR time slots",
  (pWORD8) "Invalid SBR input channels",
  (pWORD8) "Invalid PS hybrid resolution",
  (pWORD8) "Unsupported Audio Object Type",
  (pWORD8) "Invalid block type",
  (pWORD8) "Invalid SBR frame type",
  (pWORD8) "Invalid SBR number of envelope",
  (pWORD8) "Invalid SBR bit stream",
  (pWORD8) "Invalid SBR code book",
  (pWORD8) "Invalid scale factor gain",
  (pWORD8) "Invalid bit reservoir level",
  (pWORD8) "Invalid bit consumption",
  (pWORD8) "Invalid side information bits",
  (pWORD8) "Invalid huffman bits",
  (pWORD8) "Invalid scale factor bits",
  (pWORD8) "Invalid amplitude resolution",
  (pWORD8) "Invalid output bytes",
  (pWORD8) "Invalid TNS filter order",
  (pWORD8) "Invalid SBR sample rate"
};

pWORD8 enc_err_mps_exe_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Null data handle",
  (pWORD8) "Invalid Huffman data type",
  (pWORD8) "Invalid number of MPS parameter sets",
  (pWORD8) "Guided envelope shaping is not supported",
  (pWORD8) "3D stereo mode is not supported",
  (pWORD8) "Residual coding is not supported",
  (pWORD8) "Arbitrary Downmix residual coding is not supported",
  (pWORD8) "Arbitrary trees are not supported",
  (pWORD8) "Invalid quant coarse",
  (pWORD8) "Invalid res type",
  (pWORD8) "Invalid number of PCB levels",
  (pWORD8) "Error in complex FFT processing"
};

pWORD8 enc_err_usac_exe_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid FAC length",
  (pWORD8) "Invalid number of SBK",
  (pWORD8) "Invalid number of channels",
  (pWORD8) "Invalid bit reservoir level",
  (pWORD8) "Invalid mapping",
  (pWORD8) "Invalid window type",
  (pWORD8) "Invalid window length",
  (pWORD8) "Invalid window shape",
};

/* 05 - Error modules */
pWORD8 * enc_err_mod_api_non_fatal[6] = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL, 
  NULL
};

pWORD8 * enc_err_mod_api_fatal[6] = {
  enc_err_api_fatal,
  NULL,
  NULL,
  NULL,
  NULL, 
  NULL
};

pWORD8 * enc_err_mod_config_non_fatal[6] = {
  enc_err_config_non_fatal,
  enc_err_mps_config_non_fatal,
  NULL,
  enc_err_drc_config_non_fatal,
  NULL, 
  NULL
};

pWORD8 * enc_err_mod_config_fatal[6] = {
  enc_err_config_fatal,
  NULL,
  enc_err_usac_config_fatal,
  enc_err_drc_config_fatal,
  NULL, 
  NULL
};

pWORD8 * enc_err_mod_init_non_fatal[6] = {
  NULL,
  enc_err_mps_init_non_fatal,
  NULL,
  enc_err_drc_init_non_fatal,
  NULL, 
  NULL
};

pWORD8 * enc_err_mod_init_fatal[6] = {
  enc_err_init_fatal,
  enc_err_mps_init_fatal,
  enc_err_usac_init_fatal,
  enc_err_drc_init_fatal,
  enc_err_sbr_init_fatal, 
  enc_err_ps_init_fatal
};

pWORD8 * enc_err_mod_exe_non_fatal[6] = {
  enc_err_aac_exe_non_fatal,
  enc_err_mps_exe_non_fatal,
  enc_err_usac_exe_non_fatal,
  NULL,
  enc_err_esbr_exe_non_fatal, 
  NULL
};

pWORD8 * enc_err_mod_exe_fatal[6] = {
  enc_err_exe_fatal,
  enc_err_mps_exe_fatal,
  enc_err_usac_exe_fatal,
  NULL,
  NULL,
  NULL
};

/* 06 - All errors */
pWORD8 ** enc_errors_nonfatal[4] = {
  enc_err_mod_api_non_fatal,
  enc_err_mod_config_non_fatal,
  enc_err_mod_init_non_fatal,
  enc_err_mod_exe_non_fatal
};

pWORD8 ** enc_errors_fatal[4] = {
  enc_err_mod_api_fatal,
  enc_err_mod_config_fatal,
  enc_err_mod_init_fatal,
  enc_err_mod_exe_fatal
};

void aac_encode_strerror(unsigned int error, char *errorStr, int *isFatal) {
  int err_class, mod, err_sub_code;  
  
  *isFatal =  (((UWORD32)error & 0x8000) >> 15);
  err_class = (((UWORD32)error & 0x7800) >> 11);
  mod = (((UWORD)error & 0x0700) >> 8);
  err_sub_code = (((UWORD32)error & 0x00FF));

  if (err_class == 15) {
    sprintf(errorStr, "aacenc error code: %d", err_sub_code);
  } else if (error == IA_FATAL_ERROR) {
    sprintf(errorStr, "error occurred");
  } else {
    strcpy(errorStr, (const char *)(*isFatal ? enc_errors_fatal : enc_errors_nonfatal)[err_class][mod][err_sub_code]);
  }
}

int aac_encode_handle_error(AACContext * aac, IA_ERRORCODE ret, const char *section) {
  char strerror[8192] = {0};
  int isFatal;

  if (ret == IA_NO_ERROR) return 0;

  aac_encode_strerror(ret, strerror, &isFatal);
  if (aac->errorHandler == NULL) {
    fprintf(stderr, "libaac%s %s: %s\n", isFatal ? " fatal error:" : "", section, strerror);
  } else {
    aac->errorHandler(ret, section, strerror, isFatal, aac->errorHandleCtx);
  }

  return isFatal;
}

// LC Delay
#define CORE_DELAY_LC (1600)
#define CORE_DELAY_LC_960 (1500)
#define INPUT_DELAY_LC(coreDelay) ((coreDelay) * 2 + 6 * 64 - 2048 + 1)
#define INPUT_DELAY_SBR(coreDelay, dsFactor) INPUT_DELAY_LC(coreDelay) + ((coreDelay) * (dsFactor)) - (6 * 48 * (dsFactor)) + 1
#define INPUT_DELAY_PS(coreDelay, dsFactor) INPUT_DELAY_SBR(coreDelay, dsFactor) + (6 * 48 * (dsFactor)) + 32 + 352

// LD Delay
#define CORE_DELAY_LD_512 (512)
#define CORE_DELAY_LD_480 (480)
#define CORE_DELAY_ELD(coreDelay) ((coreDelay) / 2)
#define INPUT_DELAY_ELD_SBR(coreDelay, dsFactor) (CORE_DELAY_ELD(coreDelay) * (dsFactor)) + (32 * (dsFactor))
#define INPUT_DELAY_ELD_SBRV2(coreDelay, dsFactor) (CORE_DELAY_ELD(coreDelay) * (dsFactor)) + (128 * (dsFactor))

/* Segment */
AACContext * aac_encode_open(AACSettings info) {
  IA_ERRORCODE ret;
  WORD32 aot;
  WORD32 delay;
  AACContext *aac = NULL;

  switch (info.profile) {
    case AAC_LC:
      aot = AOT_AAC_LC;
      delay = info.frameSize == 960 ? CORE_DELAY_LC_960 : CORE_DELAY_LC;
      break;

    case AAC_HE:
      aot = AOT_SBR;
      delay = INPUT_DELAY_SBR(info.frameSize == 960 ? CORE_DELAY_LC_960 : CORE_DELAY_LC, 2) + 4;
      break;

    case AAC_HEV2:
      if (info.noChannels != 2) goto fail;
      aot = AOT_PS;
      delay = INPUT_DELAY_PS(info.frameSize == 960 ? CORE_DELAY_LC_960 : CORE_DELAY_LC, 2);
      break;

    case AAC_LD:
      if (info.adts) goto fail;
      aot = AOT_AAC_LD;
      delay = info.frameSize == 480 ? CORE_DELAY_LD_480 : CORE_DELAY_LD_512;
      break;

    case AAC_ELD:
      if (info.adts) goto fail;
      aot = AOT_AAC_ELD;
      delay = INPUT_DELAY_ELD_SBR(info.frameSize == 480 ? CORE_DELAY_LD_480 : CORE_DELAY_LD_512, 2) + 4;
      break;
      
    case AAC_ELDV2:
      if (info.adts) goto fail;
      aot = AOT_AAC_ELD;
      delay = INPUT_DELAY_ELD_SBRV2(info.frameSize == 480 ? CORE_DELAY_LD_480 : CORE_DELAY_LD_512, 2) + 4;
      break;

    default:
      goto fail;
  }

  aac = calloc(1, sizeof(AACContext));
  if (aac == NULL) goto fail;
  
  aac->pstr_in_cfg = calloc(1, sizeof(ixheaace_input_config));
  if (aac->pstr_in_cfg == NULL) goto fail;

  aac->pstr_out_cfg = calloc(1, sizeof(ixheaace_output_config));
  if (aac->pstr_out_cfg == NULL) goto fail;

  ixheaace_input_config * aac_in_config = (ixheaace_input_config *)aac->pstr_in_cfg;
  ixheaace_output_config * aac_out_config = (ixheaace_output_config *)aac->pstr_out_cfg;

  aac_in_config->aac_config.bitrate = info.bitrate;
  aac_in_config->aac_config.bandwidth = info.cutoff;
  aac_in_config->aac_config.use_tns = info.tns;
  aac_in_config->aac_config.bitreservoir_size = -2;
  aac_in_config->aac_config.inv_quant = info.iq;

  aac_in_config->aac_config.full_bandwidth = aac_in_config->aac_config.bandwidth <= 0 ? 1 : 0;

  aac_in_config->i_bitrate = aac_in_config->aac_config.bitrate;
  aac_in_config->aot = aot;
  aac_in_config->i_channels = info.noChannels;
  aac_in_config->i_samp_freq = info.sampleRate;
  aac_in_config->i_use_mps = info.profile == AAC_ELDV2;
  aac_in_config->i_mps_tree_config = info.profile == AAC_ELDV2 ? TREE_212 : 0;

  aac_in_config->i_use_adts = info.adts;
  aac_in_config->i_use_es = !info.adts;
  aac_in_config->esbr_flag = info.eSBR;

  aac_out_config->malloc_xheaace = &malloc_global;
  aac_out_config->free_xheaace = &free_global;

  aac_in_config->frame_cmd_flag = info.frameSize > 0;
  aac_in_config->frame_length = info.frameSize;
  aac_in_config->out_bytes_flag = 0;

  aac_in_config->user_tns_flag = aac_in_config->aac_config.use_tns;
  aac_in_config->user_esbr_flag = aac_in_config->esbr_flag;

  aac_in_config->random_access_interval = DEFAULT_RAP_INTERVAL_IN_MS;

  aac_in_config->ui_pcm_wd_sz = info.bitsPerSamples; // Fixed point only although float computing is possible
  aac_in_config->aac_config.length = 0;

  aac->errorHandler = info.errorHandler;
  aac->errorHandleCtx = info.errorHandleCtx;

  ret = ixheaace_create(aac->pstr_in_cfg, aac->pstr_out_cfg);
  if (aac_encode_handle_error(aac, ret, "init-encoder")) goto fail;

  aac->no_samples = aac_out_config->input_size / (aac_in_config->ui_pcm_wd_sz == 32 ? 4 : 2);
  aac->max_out_bytes = aac_out_config->mem_info_table[IA_MEMTYPE_OUTPUT].ui_size;
  aac->in_buf_offset = 0;
  aac->inputDelay = delay;

  if (!info.adts) {
      aac->asc = (unsigned char *)aac_out_config->mem_info_table[IA_MEMTYPE_OUTPUT].mem_ptr;
      aac->ascSize = aac_out_config->i_out_bytes;
  }

  return aac;

fail:
  if (aac) {
    free(aac->pstr_in_cfg);
    free(aac->pstr_out_cfg);
    free(aac);
  }
  return NULL;
}

int aac_encode(AACContext *aac, unsigned char *inData, unsigned int inDataSize, unsigned char *outData, unsigned int *outSize) {
  ixheaace_output_config * aac_out_config = (ixheaace_output_config *)aac->pstr_out_cfg;

  pUWORD8 aac_in_buf = (pUWORD8)aac_out_config->mem_info_table[IA_MEMTYPE_INPUT].mem_ptr;
  pUWORD8 aac_out_buf = (pUWORD8)aac_out_config->mem_info_table[IA_MEMTYPE_OUTPUT].mem_ptr;

  UWORD32 samplesRemain = aac_out_config->input_size - aac->in_buf_offset;

  if (!inDataSize) {
    memset(aac_in_buf + aac->in_buf_offset, 0, samplesRemain);
    aac->in_buf_offset += samplesRemain;
  } else {
    if (inDataSize > samplesRemain) return -1; // Overflow

    memcpy(aac_in_buf + aac->in_buf_offset, inData, inDataSize);
    aac->in_buf_offset += inDataSize;
  }

  if (aac->in_buf_offset < (unsigned int)aac_out_config->input_size) return 0; // Underflow, need data
  aac->in_buf_offset = 0;

  IA_ERRORCODE ret = ixheaace_process(aac_out_config->pv_ia_process_api_obj, aac->pstr_in_cfg, aac->pstr_out_cfg);
  if (aac_encode_handle_error(aac, ret, "encode")) return -1;

  memcpy(outData, aac_out_buf, aac_out_config->i_out_bytes);
  *outSize = aac_out_config->i_out_bytes;

  return 1;
}

void aac_encode_close(AACContext *aac) {
  ixheaace_delete(aac->pstr_out_cfg);
  free(aac->pstr_in_cfg);
  free(aac->pstr_out_cfg);
  free(aac);
}

// Decoder
IA_ERRORCODE ixheaacd_dec_api(pVOID p_ia_module_obj, WORD32 i_cmd, WORD32 i_idx, pVOID pv_value);

/* 01 - API Errors */
pWORD8 dec_err_api_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "No Error",
  (pWORD8) "API Command not supported",
  (pWORD8) "API Command type not supported"
};

/* Fatal Errors */
pWORD8 dec_err_api_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid Memory Table Index",
  (pWORD8) "Invalid Library ID String Index",
  (pWORD8) "NULL Pointer: Memory Allocation Error",
  (pWORD8) "Invalid Config Param",
  (pWORD8) "Invalid Execute type",
  (pWORD8) "Invalid Command",
  (pWORD8) "Memory Allocation Error: Alignment requirement not met"
};

/* 02 - Config Errors */
pWORD8 dec_err_config_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid Output PCM WORD Size. Setting to default, 16 ",
  (pWORD8) "Invalid Down-mix flag option. Setting to default, 0 ",
  (pWORD8) "Invalid eSBR PS flag option. Setting to default, 0 ",
  (pWORD8) "Invalid downmix to stereo flag option. Setting to default, 0 ",
  (pWORD8) "Invalid interleave to stereo flag option. Setting to default, 1 ",
  (pWORD8) "Invalid downsample flag option. Setting to default, 0 ",
  (pWORD8) "Invalid peak limiter flag option. Setting to default, 1 ",
  (pWORD8) "Invalid MP4 flag option. Setting to default, 0 ",
  (pWORD8) "Invalid maximum number of channels. limiting to between 2 and 8",
  (pWORD8) "Invalid instance for coupling channel. Setting to default 1",
  (pWORD8) "Invalid error concealment flag option. Setting to default 0",
  (pWORD8) "Invalid Disable Sync flag option. Setting to default, 0 ",
  (pWORD8) "Invalid Auto SBR upsample option. Setting to default, 1 ",
  (pWORD8) "Invalid DRC heavy compression flag option. Setting to default 0",
  (pWORD8) "Invalid DRC cut value",
  (pWORD8) "Invalid DRC boost value",
  (pWORD8) "Invalid DRC target",
  (pWORD8) "Invalid Frame size",
  (pWORD8) "Invalid LD testing flag option. Setting to default 0",
  (pWORD8) "Invalid effect type",
  (pWORD8) "Invalid target loudness value",
  (pWORD8) "Invalid HQ eSBR flag option. Setting to default 0",
  (pWORD8) "Invalid frame length flag option. Setting to default 0",
  (pWORD8) "Invalid eSBR flag option. Setting to default 1"
};

/* Fatal Errors */
pWORD8 dec_err_config_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Invalid Sample rate specified for RAW decoding"
};

/* 03 - Init Errors */
pWORD8 dec_err_init_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Header not found at the beginning of input data continuing syncing",
  (pWORD8) "Invalid number of QMF bands", (pWORD8) "Decoder initialization failed",
  (pWORD8) "Input bytes insufficient for decoding", 
  (pWORD8) "Error in AAC decoding"
};

/* Fatal Errors */
pWORD8 dec_err_init_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "AAC Decoder initialization failed",
  (pWORD8) "End of input reached during initialization",
  (pWORD8) "No. of channels in stream greater than max channels defined",
  (pWORD8) "Audio object type is not supported",
  (pWORD8) "Decoder initialization failed",
  (pWORD8) "Channel coupling not supported"
};

/* 04 - Execution Errors */
pWORD8 dec_err_exe_non_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "ADTS syncronization is lost. Re-syncing",
  (pWORD8) "Though SBR was present in previous frame, not present in current"
            "frame (SBR turned off)",
  (pWORD8) "SBR was not present in previous frame, but it is present in"
            "current frame (SBR turned on)",
  (pWORD8) "ADTS Header CRC failed.Re-syncing",
  (pWORD8) "Input bytes insufficient for decoding",
  (pWORD8) "Element instance tag mismatch, because of new channel mode",
  (pWORD8) "max huffman decoded value exceeded",
  (pWORD8) "Error in AAC decoding",
  (pWORD8) "Scale factor exceeds the transmitted boundary",
  (pWORD8) "Gain control not supported",
  (pWORD8) "Filter Order of TNS data is greater than maximum order",
  (pWORD8) "LTP data found, not supported",
  (pWORD8) "The base sampling frequency has changed in ADTS header",
  (pWORD8) "Pulse Data exceeds the permitted boundary",
  (pWORD8) "Invalid code ixheaacd_book number in ia_huffman_data_type decoding",
  (pWORD8) "Channel index not within allowed range",
  (pWORD8) "Smoothing mode not within allowed range",
  (pWORD8) "Smoothing time not within allowed range",
  (pWORD8) "Extension type in the bitstream not within allowed range",
  (pWORD8) "QMF update type in the bitstream not within allowed range",
  (pWORD8) "Window type in the bitstream not within allowed range",
  (pWORD8) "Evaluated sine parameter not within allowed range"
};

/* Fatal Errors */
pWORD8 dec_err_exe_fatal[IA_MAX_ERROR_SUB_CODE] = {
  (pWORD8) "Channel coupling not supported",
  (pWORD8) "TNS data range is errorneous",
  (pWORD8) "Invalid LOAS header",
  (pWORD8) "Invalid DRC data",
  (pWORD8) "MPS reshaping input not valid",
  (pWORD8) "Tree config present in bit stream not valid",
  (pWORD8) "Number of timeslots not valid",
  (pWORD8) "MPS dequantization parameter not valid",
  (pWORD8) "MPS quantization mode not valid",
  (pWORD8) "MPS input channels not valid",
  (pWORD8) "Bitstream data in arbitrary downmix spatial frame not valid",
  (pWORD8) "Window sequence value not valid",
  (pWORD8) "Temporal shape config in the bitstream not valid",
  (pWORD8) "3D audio HRTF set present in the bitstream not valid",
  (pWORD8) "TTT mode read from the bitstream not valid",
  (pWORD8) "Number of OTT boxes in the bitstream not valid",
  (pWORD8) "Number of parameter sets present in the bitstream not valid",
  (pWORD8) "Mapping of index data failed during decoding",
  (pWORD8) "Number of parameter bands present in the bitstream not valid"
};

pWORD8 *dec_errors_nonfatal[4] = {
  dec_err_api_non_fatal,
  dec_err_config_non_fatal,
  dec_err_init_non_fatal,
  dec_err_exe_non_fatal
};

pWORD8 *dec_errors_fatal[4] = {
  dec_err_api_fatal,
  dec_err_config_fatal,
  dec_err_init_fatal,
  dec_err_exe_fatal
};

void aac_decode_strerror(unsigned int error, char *errorStr, int *isFatal) {
  int err_class, err_sub_code;  

  if (error == IA_FATAL_ERROR)
    *isFatal = 1;

  else
    *isFatal = (((UWORD)error & 0x8000) >> 15);

  err_class = (((UWORD)error & 0x7800) >> 11);
  err_sub_code = (((UWORD)error & 0x07FF));

  if (err_class == 15) {
    sprintf(errorStr, "aacdec error code: %d", err_sub_code);
  } else if (error == IA_FATAL_ERROR) {
    sprintf(errorStr, "error occurred");
  } else {
    strcpy(errorStr, (const char *)(*isFatal ? dec_errors_fatal : dec_errors_nonfatal)[err_class][err_sub_code]);
  }
}

int aac_decode_handle_error(AACDecode * aacDec, IA_ERRORCODE ret, const char *section) {
  char strerror[8192] = {0};
  int isFatal;

  if (ret == IA_NO_ERROR) return 0;

  aac_decode_strerror(ret, strerror, &isFatal);
  if (aacDec->errorHandler == NULL) {
    fprintf(stderr, "libaac%s %s: %s\n", isFatal ? " fatal error:" : "", section, strerror);
  } else {
    aacDec->errorHandler(ret, section, strerror, isFatal, aacDec->errorHandleCtx);
  }

  return isFatal;
}

AACDecode * aac_decode_open(AACDecodeSettings cfg) {
  IA_ERRORCODE ret;
  WORD32 api_size;
  WORD32 mem_info_size;
  AACDecode * aacDec = NULL;

  aacDec = calloc(1, sizeof(AACDecode));
  if (!aacDec) goto fail;

  /* 01 - Allocate */
  ixheaacd_dec_api(NULL, IA_API_CMD_GET_API_SIZE, 0, &api_size);

  /* Allocate memory for API */
  aacDec->apiObj = malloc(api_size);
  if (aacDec->apiObj == NULL) goto fail;

  /* 02 - API Init */
  aacDec->errorHandler = cfg.errorHandler;
  aacDec->errorHandleCtx = cfg.errorHandleCtx;

  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_INIT_API_PRE_CONFIG_PARAMS, NULL);
  if (aac_decode_handle_error(aacDec, ret, "preconfig")) goto fail;

  /* 03 - Configuration starts here */
  int isMP4 = cfg.ascSize > 0;
  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_ISMP4, &isMP4);
  if (aac_decode_handle_error(aacDec, ret, "updating mp4 flag")) goto fail;

  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_PCM_WDSZ, &cfg.bitsPerSamples);
  if (aac_decode_handle_error(aacDec, ret, "updating bits per samples")) goto fail;

  int is480FrameSize = cfg.frameSize == 480;
  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_FRAMESIZE, &is480FrameSize);
  if (aac_decode_handle_error(aacDec, ret, "updating frame length (ld)")) goto fail;

  int is960FrameSize = cfg.frameSize == 960;
  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_FRAMELENGTH_FLAG, &is960FrameSize);
  if (aac_decode_handle_error(aacDec, ret, "updating frame length")) goto fail;

  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_ESBR, &cfg.eSBR);
  if (aac_decode_handle_error(aacDec, ret, "updating esbr")) goto fail;

  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_ERROR_CONCEALMENT, &cfg.errorConceal);
  if (aac_decode_handle_error(aacDec, ret, "updating error conceal")) goto fail;

  aacDec->eSBR = cfg.eSBR;
  aacDec->asc = cfg.asc;
  aacDec->ascSize = cfg.ascSize;
  aacDec->frameCounter = 0;

  /* 04 - Decoder Memory */
  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_GET_MEMTABS_SIZE, 0, &mem_info_size);
  if (aac_decode_handle_error(aacDec, ret, "memtab size")) goto fail;

  aacDec->memInfoTab = malloc(mem_info_size);
  if (aacDec->memInfoTab == NULL) goto fail;

  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_MEMTABS_PTR, 0, aacDec->memInfoTab);
  if (aac_decode_handle_error(aacDec, ret, "set memtabs ptr")) goto fail;

  /* 05 - Post Config */
  ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_INIT_API_POST_CONFIG_PARAMS, NULL);
  if (aac_decode_handle_error(aacDec, ret, "postconfig")) goto fail;

  for (int i = 0; i < 4; i++) {
    int ui_size = 0, ui_alignment = 0, ui_type = 0;

    /* Get memory size */
    ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_GET_MEM_INFO_SIZE, i, &ui_size);
    if (aac_decode_handle_error(aacDec, ret, "get meminfo 1")) goto fail;

    /* Get memory alignment */
    ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_GET_MEM_INFO_ALIGNMENT, i, &ui_alignment);
    if (aac_decode_handle_error(aacDec, ret, "get meminfo 2")) goto fail;

    /* Get memory type */
    ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_GET_MEM_INFO_TYPE, i, &ui_type);
    if (aac_decode_handle_error(aacDec, ret, "get meminfo 3")) goto fail;

    ui_alignment = (ui_alignment + sizeof(void *) - 1) / sizeof(void *);
    ui_alignment = ui_alignment * sizeof(void *);
    pVOID temp = malloc_global(ui_size, ui_alignment);
    
    if (!temp) {
      fprintf(stderr, "libaac malloc_global failure?\n");
      goto fail;
    }

    /* Set the buffer pointer */
    ret = ixheaacd_dec_api(aacDec->apiObj, IA_API_CMD_SET_MEM_PTR, i, temp);
    if (aac_decode_handle_error(aacDec, ret, "set meminfo")) goto fail;

    aacDec->memBuffer[i] = temp;

    if (ui_type == IA_MEMTYPE_INPUT) {
      aacDec->inBuf = (pUWORD8)temp;
      aacDec->inBufSize = ui_size;
    } else if (ui_type == IA_MEMTYPE_OUTPUT) {
      aacDec->outBuf = (pUWORD8)temp;
      aacDec->outBufSize = ui_size;
    }
  }

  return aacDec;

fail:
  if (aacDec) {
    for (int i = 0; i < 4; i++) free_global(aacDec->memBuffer[i]);

    free(aacDec->memInfoTab);
    free(aacDec->apiObj);
    free(aacDec);
  }
  return NULL;
}

void aac_decode_write_buffer(AACDecode *aacd, unsigned char *data, unsigned int dataSize) {
  IA_ERRORCODE ret;
  
  memcpy(aacd->inBuf, data, dataSize);
  ret = ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_SET_INPUT_BYTES, 0, &dataSize);
  aac_decode_handle_error(aacd, ret, "upload buffer");
}

int aac_decode_init(AACDecode *aacd, bool *isInitDone, unsigned int *noBytes) {
  IA_ERRORCODE ret;
  int done;

  ret = ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_INIT_PROCESS, NULL);

  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_INIT_DONE_QUERY, &done);
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CURIDX_INPUT_BUF, 0, noBytes);

  *isInitDone = done;
  return ret;
}

int aac_decode_execute(AACDecode *aacd, bool *isDone, unsigned int *noBytesIn, int *preroll) {
  IA_ERRORCODE ret;
  int done;
  
  ret = ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_EXECUTE, IA_CMD_TYPE_DO_EXECUTE, NULL);

  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_EXECUTE, IA_CMD_TYPE_DONE_QUERY, &done);
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CURIDX_INPUT_BUF, 0, noBytesIn);
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_GET_NUM_PRE_ROLL_FRAMES, preroll);

  *isDone = done;
  return ret;
}

int aac_decode(AACDecode *aacd, unsigned char *inData, unsigned int inDataSize, unsigned char *outData, unsigned int *outSize, unsigned int *bytesRead) {
  IA_ERRORCODE ret;
  int tempValue;
  int timeInited = 0;

  if (inDataSize > aacd->inBufSize || !inDataSize) return -1;
  
  while (!aacd->isInitDone) {
    if (aacd->ascSize > 0 && !aacd->ascDone) {
      aac_decode_write_buffer(aacd, aacd->asc, aacd->ascSize);
      ret = aac_decode_init(aacd, &aacd->isInitDone, bytesRead);
      if (aac_decode_handle_error(aacd, ret, "init-decoder")) return -1;

      aacd->ascDone = 1;
      timeInited++;
    }
    
    aac_decode_write_buffer(aacd, inData, inDataSize);
    ret = aac_decode_init(aacd, &aacd->isInitDone, bytesRead);
    if (aac_decode_handle_error(aacd, ret, "init-decoder")) return -1;

    if (ret == IA_XHEAAC_DEC_EXE_NONFATAL_INSUFFICIENT_INPUT_BYTES) return 0;

    timeInited++;
    if (timeInited > 1 && !aacd->isInitDone) {
      aacd->frameCounter++;
    }

    if (aacd->isInitDone) {
      ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_SAMP_FREQ, &tempValue);
      aacd->sampleRate = tempValue;

      ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_NUM_CHANNELS, &tempValue);
      aacd->noChannels = tempValue;

      ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_PCM_WDSZ, &tempValue);
      aacd->bitsPerSamples = tempValue;

      ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_AOT, &tempValue);
      aacd->aot = tempValue;

      ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_CONFIG_PARAM, IA_XHEAAC_DEC_CONFIG_PARAM_SBR_MODE, &tempValue);
      aacd->sbrMode = tempValue;
    }
  };
  
  bool isDecodeDone;
  int outByteTemp;
  int prerolls;

  aac_decode_write_buffer(aacd, inData, inDataSize);
  ret = aac_decode_execute(aacd, &isDecodeDone, bytesRead, &prerolls);
  if (aac_decode_handle_error(aacd, ret, "decode")) return -1;

  int prerollOffset = 0;
  *outSize = 0;

  do {
    ret = ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_GET_OUTPUT_BYTES, 0, &outByteTemp);
    if (aac_decode_handle_error(aacd, ret, "preroll handling")) return -1;

    if (aacd->sbrMode && (aacd->aot < 23) && aacd->eSBR) {
      if (aacd->frameCounter > 0)
        *outSize += outByteTemp;
    } else {
      *outSize += outByteTemp;
    }

    memmove(aacd->outBuf, aacd->outBuf + prerollOffset, outByteTemp);
    prerollOffset += outByteTemp;

    prerolls--;
  } while (prerolls > 0);

  memcpy(outData, aacd->outBuf, *outSize);
  aacd->frameCounter++;
  return 1;
}

void aac_decode_reset(AACDecode *aacd) {
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_GA_HDR, NULL);
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_GA_HDR, NULL);

  aacd->isInitDone = false;
  aacd->ascDone = false;

  int isMP4 = aacd->ascSize > 0;
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_ISMP4, &isMP4);
}

void aac_decode_flush_buffer(AACDecode *aacd) {
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_FLUSH_MEM, NULL);
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INIT, IA_CMD_TYPE_FLUSH_MEM, NULL);

  aacd->isInitDone = false;
  aacd->ascDone = false;

  int isMP4 = aacd->ascSize > 0;
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_SET_CONFIG_PARAM, IA_ENHAACPLUS_DEC_CONFIG_PARAM_ISMP4, &isMP4);
}

void aac_decode_close(AACDecode *aacd) {
  ixheaacd_dec_api(aacd->apiObj, IA_API_CMD_INPUT_OVER, 0, NULL);
  for (int i = 0; i < 4; i++) free_global(aacd->memBuffer[i]);

  free(aacd->memInfoTab);
  free(aacd->apiObj);
  free(aacd);
}