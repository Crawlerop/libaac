#include <aacnext/libaac.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdint.h>
#include "argparse.h"
#include "audiofile.h"
#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"
#include "indicator.h"

using namespace indicators;

class AACFileReader {
    private:
        int bufSize;
        std::ifstream file;
        uint8_t *buf;
        int dataRem;

    public:
        bool ok;
        int64_t fileOffset;
        int64_t fileSize;

        AACFileReader(std::string inFile, int bufferSize) {
            bufSize = bufferSize;
            file = std::ifstream(inFile, std::ios_base::binary);
            ok = file.is_open();
            if (!ok) return;

            fileOffset = 0;
            fileSize = file.tellg();

            file.seekg( 0, std::ios::end );
            fileSize = file.tellg() - fileSize;

            file.seekg( 0, std::ios::beg );
            buf = new uint8_t[bufferSize];

            file.read((char *)buf, bufferSize);
            dataRem = file.gcount();
        }   
        
        void Get(uint8_t *out, int max, uint32_t *len) {
            if (len != NULL) *len = dataRem;
            memcpy(out, buf, max > dataRem ? dataRem : max);
        }

        void Next(uint32_t count) {
            memmove(buf, buf + count, dataRem - count);
            file.read((char *)(buf + (dataRem - count)), count);

            fileOffset += count;
            dataRem = (dataRem - count) + file.gcount();
        }

        void Close() {
            file.close();
            delete buf;
        }

};

static int doRead(int64_t offset, void *buffer, size_t size, void *token)
{
    FILE *f = (FILE*)token;
    fseek(f, offset, SEEK_SET);
    return fread(buffer, 1, size, f) != size;
}

int main(int argc, char *argv[]) {
    argparse::ArgumentParser ap("aacdec");

    ap.add_argument("input");
    ap.add_argument("output");

    ap.add_argument("-e", "--enhanced-sbr").help("Enhanced SBR").flag();
    ap.add_argument("-f", "--frame-size").help("Frame size").default_value(0).scan<'i', int>();

    try {
        ap.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << ap;
        return 1;
    }

    auto inFile = ap.get<std::string>("input");
    auto outFile = ap.get<std::string>("output");
    auto eSBR = ap.get<bool>("-e");
    auto frameSize = ap.get<int>("-f");

    AACDecodeSettings aacConf = {};
    aacConf.bitsPerSamples = 16;
    aacConf.eSBR = eSBR;
    aacConf.frameSize = frameSize;

    FILE * inpFile = fopen(inFile.c_str(), "rb");
    if (inpFile == NULL) {
        std::cerr << "Unable to open " << inFile << ": " << std::strerror(errno) << std::endl;
        return 1;
    }

    MP4D_demux_t mp4 = { 0 };
    bool isMP4;
    int audioTrackNum = -1;
    uint8_t adtsSync[2];
    if (fread(adtsSync, 1, 2, inpFile) != 2) {
        std::cerr << "Unable to read " << inFile << ": " << std::strerror(errno) << std::endl;
        fclose(inpFile);
        return 1;
    }

    if (adtsSync[0] == 0xff && (adtsSync[1] & 0xf0) == 0xf0) {
        std::cout << "ADTS" << std::endl;
        isMP4 = false;
        /* ADTS decoder */
        fclose(inpFile);
        std::cout << "Input file: " << inFile << std::endl;
        std::cout << "Output file: " << outFile << std::endl;
    } else {
        std::cout << "MP4" << std::endl;
        isMP4 = true;
        /* MP4 decoder */
        fseek(inpFile, 0, SEEK_END);
        size_t fileSize = ftell(inpFile);
        rewind(inpFile);

        if (!MP4D_open(&mp4, doRead, inpFile, fileSize)) {
            std::cerr << "Unable to open " << inFile << " as a MP4 file" << std::endl;
            fclose(inpFile);
            return 1;
        };

        for (unsigned int i = 0; i < mp4.track_count; i++) {
            if (mp4.track[i].handler_type == MP4D_HANDLER_TYPE_SOUN) {
                audioTrackNum = i;
                break;
            }
        }

        if (audioTrackNum == -1) {
            std::cerr << "Could not find an audio track from this file." << std::endl;
            MP4D_close(&mp4);
            fclose(inpFile);
            return 1;
        }

        aacConf.asc = mp4.track[audioTrackNum].dsi;
        aacConf.ascSize = mp4.track[audioTrackNum].dsi_bytes;

        std::cout << "Input file: " << inFile << std::endl;
        std::cout << "Output file: " << outFile << std::endl;
        
        printf("Extradata: ");
        for (uint32_t i = 0; i < aacConf.ascSize; i++) {
            printf("%02x ", aacConf.asc[i]);
        }
        printf("\n");
    }

    auto decoder = aac_decode_open(aacConf);
    if (decoder == NULL) {
        std::cerr << "Failed opening AAC decoder" << std::endl;
        return 1;
    }

    AudioFile<int16_t> outpFile;
    std::vector<int16_t> samples[8];
    bool isDecodeFirst = false;
    uint8_t readBuf[65536];
    int16_t *outBuf = new int16_t[decoder->outBufSize / sizeof(int16_t)];
    uint32_t outSize;
    uint32_t nBytes;

    ProgressBar bar{
        option::BarWidth{50},
        option::Start{"["},
        option::Fill{"="},
        option::Lead{">"},
        option::Remainder{" "},
        option::End{"]"},
        option::PostfixText{"Processing frames"},
        option::ShowPercentage{true},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
    };

    if (isMP4) {
        show_console_cursor(false);
        for (size_t i = 0; i < mp4.track[audioTrackNum].sample_count; i++) {
            unsigned int frameSize, timeStamp, duration;
            if (isDecodeFirst) bar.set_progress(((double)i / (double)mp4.track[audioTrackNum].sample_count) * 100);

            size_t frOffset = MP4D_frame_offset(&mp4, audioTrackNum, i, &frameSize, &timeStamp, &duration);
            fseek(inpFile, frOffset, SEEK_SET);
            fread(readBuf, 1, frameSize, inpFile);

            int ret = aac_decode(decoder, readBuf, frameSize, (uint8_t *)outBuf, &outSize, &nBytes);
            if (ret == -1) {
                std::cerr << "Decode frame " << i << " failed" << std::endl;
                continue;
            }

            if (!isDecodeFirst) {
                isDecodeFirst = true;
                std::cout << "Sample Rate: " << decoder->sampleRate << std::endl;
                std::cout << "Channels: " << decoder->noChannels << std::endl;
                std::cout << "AOT: " << decoder->aot << std::endl;
                outpFile.setNumChannels(decoder->noChannels);
                outpFile.setSampleRate(decoder->sampleRate);
            }

            uint32_t samplesDecoded = outSize / sizeof(int16_t) / decoder->noChannels;

            for (size_t k = 0; k < samplesDecoded; k++) {
                for (size_t j = 0; j < decoder->noChannels; j++) {
                    samples[j].push_back(outBuf[(decoder->noChannels * k) + j]);
                }
            }
        }
        bar.set_progress(100);
        show_console_cursor(true);
    } else {
        AACFileReader adtsData(inFile, 4096);
        uint32_t bufReadSize;
        uint32_t adtsCount = 0;

        if (!adtsData.ok) {
            std::cerr << "Unable to open " << inFile << " for ADTS read: " << std::strerror(errno) << std::endl;
            return 1;
        }

        show_console_cursor(false);
        while (1) {
            adtsData.Get(readBuf, 4096, &bufReadSize);    
            if (!bufReadSize) {
                bar.set_progress(100);
                break;
            }
            if (isDecodeFirst) bar.set_progress(((double)adtsData.fileOffset / (double)adtsData.fileSize) * 100);

            int ret = aac_decode(decoder, readBuf, bufReadSize, (uint8_t *)outBuf, &outSize, &nBytes);
            if (ret == -1) {
                std::cerr << "Decode frame " << adtsCount << " failed" << std::endl;
                break;
            }

            if (!isDecodeFirst) {
                isDecodeFirst = true;
                std::cout << "Sample Rate: " << decoder->sampleRate << std::endl;
                std::cout << "Channels: " << decoder->noChannels << std::endl;
                std::cout << "AOT: " << decoder->aot << std::endl;
                outpFile.setNumChannels(decoder->noChannels);
                outpFile.setSampleRate(decoder->sampleRate);
            }

            uint32_t samplesDecoded = outSize / sizeof(int16_t) / decoder->noChannels;

            for (size_t k = 0; k < samplesDecoded; k++) {
                for (size_t j = 0; j < decoder->noChannels; j++) {
                    samples[j].push_back(outBuf[(decoder->noChannels * k) + j]);
                }
            }

            adtsCount++;
            adtsData.Next(nBytes);
        }
        show_console_cursor(true);
    }

    outpFile.setNumSamplesPerChannel(samples[0].size());
    for (size_t i = 0; i < decoder->noChannels; i++) {
        memcpy(outpFile.samples[i].data(), samples[i].data(), samples[0].size() * sizeof(int16_t));
    }

    outpFile.save(outFile);
    aac_decode_close(decoder);

#if 0
#if 0
    AACFileReader inData("asc.es", 4096);
    if (!inData.ok) return 1;

    uint8_t *asc = new uint8_t[2];
    inData.Get((char *)asc, 2, NULL);

    AACDecodeSettings aacConf = {};
    aacConf.asc = asc;
    aacConf.ascSize = 2;
    aacConf.bitsPerSamples = 16;

    auto aac = aac_decode_open(aacConf);
#else
    AACFileReader inData("adts.aac", 4096);
    if (!inData.ok) return 1;

    AACDecodeSettings aacConf = {};
    aacConf.ascSize = 0;
    aacConf.bitsPerSamples = 16;

    auto aac = aac_decode_open(aacConf);
#endif
    uint8_t *tempBuf = new uint8_t[4096];
    uint32_t tempBufReadSize;

    uint8_t *tempBufOut = new uint8_t[aac->outBufSize];
    uint32_t tempBufOutSize;
    uint32_t bytesNext;
    std::cout << aac->inBufSize << std::endl;

    //uint8_t *last_okay;
    std::ofstream testOut("aac_decode_test.bin", std::ios_base::binary);

    while (1) {
        inData.Get(tempBuf, 4096, &tempBufReadSize);    
        if (!tempBufReadSize) break;
        // std::cout << "sync: " << std::hex << (int)tempBuf[0] << std::endl;
        // std::cout << "sync (2): " << std::hex << (int)tempBuf[1] << std::endl;

        auto ret = aac_decode(aac, tempBuf, tempBufReadSize, tempBufOut, &tempBufOutSize, &bytesNext);
        if (ret == -1) break;

        testOut.write((char *)tempBufOut, tempBufOutSize);

        inData.Next(bytesNext);
    }

    // inData.Get(tempBuf, 4096, &tempBufReadSize);
    // std::cout << "last (4095):" << std::hex << (int)tempBuf[4094] << std::endl;
    // std::cout << "last (4096):" << std::hex << (int)tempBuf[4095] << std::endl;

    // inData.Next(2);

    // inData.Get(tempBuf, 4096, &tempBufReadSize);
    // std::cout << "last (4095, 2):" << std::hex << (int)tempBuf[4094] << std::endl;
    // std::cout << "last (4096, 2):" << std::hex << (int)tempBuf[4095] << std::endl;

    // aac_decode(aac, tempBuf, tempBufReadSize, &tempBufOut, &tempBufOutSize, &bytesNext);

    // inData.Next(tempBufOutSize);
#endif
}