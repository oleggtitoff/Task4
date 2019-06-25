#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>

#define SIGNAL_TIME 3	//seconds
#define SAMPLE_RATE 48000
#define CHANNELS 1
#define BITS_PER_SAMPLE 16


typedef struct {
	uint8_t fileFormat[4];
	uint32_t fileSize;
	uint8_t fileTypeHeader[4];
	uint8_t formatChunkMarker[4];
	uint32_t formatDataLength;
	uint16_t formatType;
	uint16_t channels;
	uint32_t sampleRate;
	uint32_t byterate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	uint8_t dataChunkHeader[4];
	uint32_t dataSize;
} WavHeader;


uint32_t swap_uint32(uint32_t number);
void initializeFileHeader(WavHeader *header);


int main()
{
	return 0;
}

uint16_t swap_uint16(uint16_t number)
{
	return ((number >> 8) & 0xff) |
		((number << 8) & 0xff00);
}

uint32_t swap_uint32(uint32_t number)
{
	return ((number >> 24) & 0xff) |
		((number << 8) & 0xff0000) |
		((number >> 8) & 0xff00) |
		((number << 24) & 0xff000000);
}

void initializeFileHeader(WavHeader *header)
{
	*(header->fileFormat) = 'R';
	*(header->fileFormat + 1) = 'I';
	*(header->fileFormat + 2) = 'F';
	*(header->fileFormat + 3) = 'F';

	*(header->fileTypeHeader) = 'W';
	*(header->fileTypeHeader + 1) = 'A';
	*(header->fileTypeHeader + 2) = 'V';
	*(header->fileTypeHeader + 3) = 'E';

	*(header->formatChunkMarker) = 'f';
	*(header->formatChunkMarker + 1) = 'm';
	*(header->formatChunkMarker + 2) = 't';
	*(header->formatChunkMarker + 3) = ' ';

	*(header->dataChunkHeader) = 'd';
	*(header->dataChunkHeader + 1) = 'a';
	*(header->dataChunkHeader + 2) = 't';
	*(header->dataChunkHeader + 3) = 'a';

	header->formatDataLength = swap_uint32(16);
	header->formatType = swap_uint16(1);
	header->channels = swap_uint16(1);
	header->sampleRate = swap_uint32(SAMPLE_RATE);
	header->bitsPerSample = swap_uint16(16);
	header->byterate = (SAMPLE_RATE * BITS_PER_SAMPLE * CHANNELS) / 8;
	header->blockAlign = (BITS_PER_SAMPLE * CHANNELS) / 8;
	//header->dataSize = //TODO
	//header->fileSize = //TODO
}