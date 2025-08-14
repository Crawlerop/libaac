#include "aacnext/aacnext_export.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  AAC_LC, // Low complexity
  AAC_HE, // High Efficiency (SBR)
  AAC_HEV2, // High Efficiency V2 (SBR+PS)
  AAC_LD, // Low delay
  AAC_ELD, // Enhanced low delay (LD-SBR)
  AAC_ELDV2, // Enhanced low delay V2 (LD-SBR+LD-MPS)
} profiles;

typedef struct {
  void * pstr_in_cfg; // XAAC input config 
  void * pstr_out_cfg; // XAAC output config
  uint32_t no_samples; // Number of samples
  uint32_t max_out_bytes; // Max output bytes
  uint8_t *asc; // AudioSpecific config
  uint32_t ascSize; // AudioSpecific config size
  uint32_t in_buf_offset; // Input buffer offset
  int32_t inputDelay; // Input delay
  void (* errorHandler)(uint32_t errorCode, const char *section, const char *errorMsg, bool isFatal, void *handle); // Error handler
  void * errorHandleCtx; // Error handler object
} AACContext;

typedef struct {
  uint64_t sampleRate; // Sample rate
  uint64_t noChannels; // Number of channels
  int32_t bitsPerSamples; // Bits per samples
  uint64_t bitrate; // Bitrate
  bool adts; // ADTS format
  uint64_t cutoff; // Cutoff rate
  profiles profile; // AAC profile
  bool tns; // enable TNS
  uint32_t frameSize; // Custom frame size
  bool eSBR; // Enhanced SBR
  uint32_t iq; // Inverse quant
  void (* errorHandler)(uint32_t errorCode, const char *section, const char *errorMsg, bool isFatal, void *handle); // Error handler
  void * errorHandleCtx; // Error handler object
} AACSettings;

AACNEXT_API AACContext * aac_encode_open(AACSettings info);
AACNEXT_API int aac_encode(AACContext *aac, unsigned char *inData, unsigned int inDataSize, unsigned char *outData, unsigned int *outSize);
AACNEXT_API void aac_encode_close(AACContext *aac);

typedef struct {
  void * apiObj; // API Object
  void * memBuffer[4]; // Memory buffers
  uint8_t * inBuf; // Input buffer
  uint32_t inBufSize; // Input buffer size
  uint8_t * outBuf; // Output buffer
  uint32_t outBufSize; // Output buffer size
  void * memInfoTab; // Memory info tab
  bool isInitDone; // Init done
  uint8_t *asc; // AudioSpecific config
  uint32_t ascSize; // AudioSpecific config size
  bool ascDone; // ASC done
  uint64_t sampleRate; // Sample rate
  uint64_t noChannels; // Numbers of channels
  int32_t bitsPerSamples; // Bits per samples
  int32_t aot; // AOT
  uint32_t sbrMode; // SBR mode
  bool eSBR; // Enhanced SBR
  uint64_t frameCounter; // Frame counter
  void (* errorHandler)(uint32_t errorCode, const char *section, const char *errorMsg, bool isFatal, void *handle); // Error handler
  void * errorHandleCtx; // Error handler object
} AACDecode;

typedef struct {
  uint8_t *asc; // AudioSpecific config
  uint32_t ascSize; // AudioSpecific config size
  uint64_t noChannels; // Number of channels
  int32_t bitsPerSamples; // Bits per samples
  uint32_t frameSize; // Frame size
  int32_t eSBR; // Enhanced SBR
  int32_t errorConceal; // Error concealment
  void (* errorHandler)(uint32_t errorCode, const char *section, const char *errorMsg, bool isFatal, void *handle); // Error handler
  void * errorHandleCtx; // Error handler object
} AACDecodeSettings;

AACNEXT_API AACDecode * aac_decode_open(AACDecodeSettings cfg);
AACNEXT_API int aac_decode(AACDecode *aacd, unsigned char *inData, unsigned int inDataSize, unsigned char *outData, unsigned int *outSize, unsigned int *bytesRead);
AACNEXT_API void aac_decode_reset(AACDecode *aacd);
AACNEXT_API void aac_decode_flush_buffer(AACDecode *aacd);
AACNEXT_API void aac_decode_close(AACDecode *aacd);

#ifdef __cplusplus
}
#endif