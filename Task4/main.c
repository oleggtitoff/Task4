#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define OUTPUT_FILE_NAME "Output.wav"
#define FILE_HEADER_SIZE 44
#define SIGNAL_TIME 0.5	//seconds
#define SAMPLE_RATE 48000
#define CHANNELS 1
#define BYTES_PER_SAMPLE 2
#define FREQUENCY 1000.0
#define AMPLITUDE 30000

#define PI 3.14159265358979323846
#define FLOAT_MAX 3.402823466e+38F

#define BITS_PER_SAMPLE ((BYTES_PER_SAMPLE) * 8)
#define SAMPLES_NUM	((uint32_t)round((SAMPLE_RATE) * (SIGNAL_TIME)))


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


void initializeFileHeader(WavHeader *header);

FILE * openFile(char *fileName, _Bool mode);
void writeHeader(WavHeader *headerBuff, FILE *outputFilePtr);

void generateToneSignal(FILE *outputFilePtr);

int main()
{
	WavHeader header;
	initializeFileHeader(&header);
	FILE *outputFilePtr = openFile(OUTPUT_FILE_NAME, 1);
	writeHeader(&header, outputFilePtr);
	generateToneSignal(outputFilePtr);

	return 0;
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

	header->formatDataLength = 16;
	header->formatType = 1;
	header->channels = CHANNELS;
	header->sampleRate = SAMPLE_RATE;
	header->bitsPerSample = 16;
	header->byterate = (SAMPLE_RATE * BITS_PER_SAMPLE * CHANNELS) / 8;
	header->blockAlign = (BITS_PER_SAMPLE * CHANNELS) / 8;
	header->dataSize = SAMPLES_NUM;
	header->fileSize = SAMPLES_NUM + FILE_HEADER_SIZE;
}

FILE * openFile(char *fileName, _Bool mode)		//if 0 - read, if 1 - write
{
	FILE *filePtr;

	if (mode == 0)
	{
		if ((filePtr = fopen(fileName, "rb")) == NULL)
		{
			printf("Error opening input file\n");
			system("pause");
			exit(0);
		}
	}
	else
	{
		if ((filePtr = fopen(fileName, "wb")) == NULL)
		{
			printf("Error opening output file\n");
			system("pause");
			exit(0);
		}
	}

	return filePtr;
}

void writeHeader(WavHeader *headerBuff, FILE *outputFilePtr)
{
	if (fwrite(headerBuff, FILE_HEADER_SIZE, 1, outputFilePtr) != 1)
	{
		printf("Error writing output file (header)\n");
		system("pause");
		exit(0);
	}
}

void generateToneSignal(FILE *outputFilePtr)
{
	uint32_t i;
	int16_t sample;
	uint32_t timeCounter = 0;

	for (i = 0; i < SAMPLES_NUM; i++)
	{
		sample = (int16_t)round(AMPLITUDE * sin(2 * PI * FREQUENCY * (double)timeCounter / (SAMPLE_RATE)));
		fwrite(&sample, BYTES_PER_SAMPLE, 1, outputFilePtr);
		timeCounter++;
	}
}