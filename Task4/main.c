#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define OUTPUT_FILE_NAME "Output.wav"

#define FILE_HEADER_SIZE 44
#define SAMPLE_RATE 48000
#define CHANNELS 2
#define BYTES_PER_SAMPLE 2
#define BITS_PER_SAMPLE ((BYTES_PER_SAMPLE) * 8)
#define FILTER_COEFFICIENTS_NUM 128

#define DEFAULT_START_FREQUENCY 20.0
#define DEFAULT_STOP_FREQUENCY 22000.0
#define DEFAULT_AMPLITUDE 0.0
#define DEFAULT_SIGNAL_TIME 2.0

#define PI 3.14159265358979323846
#define FLOAT_MAX 3.402823466e+38F

#define OPTSTRING "tsnf:e:a:d:"
// --- POSSIBLE CMD ARGUMENTS ---
// t - generate tone signal
// s - generate sweep signal
// n - generate noise signal
//
// f - frequency/start frequency
// e - stop frequency
// a - amplitude
// d - signal time (duration)


char* optarg = NULL;
int optind = 1;


typedef struct {
	uint8_t currNum;
	int16_t samples[FILTER_COEFFICIENTS_NUM];
} RingBuff;

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

typedef struct {
	uint8_t signalType;
	double startFrequency;
	double stopFrequency;
	double amplitude;
	double signalTime;
	uint32_t samplesNum;
} Signal;


int getopt(int argc, char *const argv[], const char *optstring);
void runGetopt(int argc, char *const argv[], const char *optstring, Signal *signal);

double dBtoGain(double dB);
int16_t doubleToFixed15(double x);
int32_t doubleToFixed32(double x);
void correctCoefficients(double *coeficients);
void coefsDoubleToFixed32(double *input, int32_t *output);
void ringInitialization(RingBuff *buff);
void signalInitialization(Signal *signal);

void FileHeaderInitialization(WavHeader *header, Signal *signal);
FILE * openFile(char *fileName, _Bool mode);
void writeHeader(WavHeader *headerBuff, FILE *outputFilePtr);

int16_t generateToneSignal(Signal *signal, uint32_t *timeCounter);
int16_t generateSweepSignal(Signal *signal, uint32_t *timeCounter);
int16_t generateNoiseSignal(Signal *signal);

int16_t firFilter(RingBuff *ringBuff, int32_t *coefsBuff);
void run(Signal *signal, RingBuff *ringBuff, int32_t *coefsBuff, FILE *outputFilePtr);


int main(int argc, char *argv[])
{
	Signal signal;
	signalInitialization(&signal);
	runGetopt(argc, argv, OPTSTRING, &signal);
	signal.samplesNum = (uint32_t)round(signal.signalTime * SAMPLE_RATE);

	WavHeader header;
	FileHeaderInitialization(&header, &signal);
	FILE *outputFilePtr = openFile(OUTPUT_FILE_NAME, 1);
	writeHeader(&header, outputFilePtr);

	double coefsDoubleBuff[FILTER_COEFFICIENTS_NUM] = {
		-0.000162164358122296535080070212231362348,
		0.001871990160621829881251731997338083602,
		0.000177421074278205637142491468694061041,
		-0.000799938339957202400667957142843533802,
		0.000029445930738341338089858467697013111,
		0.000975875559570133681644432677160239109,
		-0.000174332633474075845286810348966355377,
		-0.001142739689442697631482914744083245751,
		0.000370491577463436688212594649627362742,
		0.001301881690598231770816806118773456546,
		-0.000622077986506033506208801497905369615,
		-0.001441449932844418493210758569489371439,
		0.000932402583730824563272576721573159375,
		0.001550174587877081733289230669470271096,
		-0.001302822619389340953049982729794464831,
		-0.001613386681590385333950887769560722518,
		0.001732469863172050698885140462834897335,
		0.001616617300957494916813228513774447492,
		-0.00221862576667492894869071484720279841,
		-0.001543657072345475025215710829229465162,
		0.002754940419298467901149107817104777496,
		0.001378463028443757676377856569160940126,
		-0.003333276559124515384113784932651469717,
		-0.001103707153510167897347837850929863635,
		0.003941689681336679740297768148593604565,
		0.000703262526727135503512577940909977769,
		-0.004565291704341306600056782372121233493,
		-0.000160412088138313022045444711238815216,
		0.005184773540519031602424870897038999829,
		-0.000539621548383245391508622468279554596,
		-0.005778912866566685360703026219653111184,
		0.001411823120786187893416219196751626441,
		0.006322138141064522626200172794597165193,
		-0.00246974610702698545855460210418641509,
		-0.006786253611573941565970891076631232863,
		0.003728315587545509915412544899027125211,
		0.00713721779746414979050950222472238238,
		-0.005200340179343024282765384214144432917,
		-0.007338326806005588710812848063369528973,
		0.006901001742960421840578089103246384184,
		0.007346158056545955619687227056147094117,
		-0.00884755574351601785376697506535492721,
		-0.007111774028954947340919190423846885096,
		0.011067298609159212374741798612376442179,
		0.006570507333481337766678542777754046256,
		-0.013595462859419383747083820423995348392,
		-0.005644429523601306696034551890761576942,
		0.016492964039346743460034971917593793478,
		0.004223704726346500307188058798146812478,
		-0.019858876451125911932749090738070663065,
		-0.002145249494208243887238829472607903881,
		0.02386518959950843093986705412135052029,
		-0.000856104435109718980845272540136647876,
		-0.028832854859967373128970535844928235747,
		0.005258383178987129091819241466509993188,
		0.035404998106579571581775667254987638444,
		-0.012038069712799946003878304168210888747,
		-0.045046331468904660111363114083360414952,
		0.023589425202695950972708871518079831731,
		0.06186864353234792363034344475636316929,
		-0.047784444108668819306551256431703222916,
		-0.103231863934425432960395596637681592256,
		0.134497338921223313912278740644978825003,
		0.464599278002360449590923963114619255066,
		0.464599278002360449590923963114619255066,
		0.134497338921223313912278740644978825003,
		-0.103231863934425432960395596637681592256,
		-0.047784444108668819306551256431703222916,
		0.06186864353234792363034344475636316929,
		0.023589425202695950972708871518079831731,
		-0.045046331468904660111363114083360414952,
		-0.012038069712799946003878304168210888747,
		0.035404998106579571581775667254987638444,
		0.005258383178987129091819241466509993188,
		-0.028832854859967373128970535844928235747,
		-0.000856104435109718980845272540136647876,
		0.02386518959950843093986705412135052029,
		-0.002145249494208243887238829472607903881,
		-0.019858876451125911932749090738070663065,
		0.004223704726346500307188058798146812478,
		0.016492964039346743460034971917593793478,
		-0.005644429523601306696034551890761576942,
		-0.013595462859419383747083820423995348392,
		0.006570507333481337766678542777754046256,
		0.011067298609159212374741798612376442179,
		-0.007111774028954947340919190423846885096,
		-0.00884755574351601785376697506535492721,
		0.007346158056545955619687227056147094117,
		0.006901001742960421840578089103246384184,
		-0.007338326806005588710812848063369528973,
		-0.005200340179343024282765384214144432917,
		0.00713721779746414979050950222472238238,
		0.003728315587545509915412544899027125211,
		-0.006786253611573941565970891076631232863,
		-0.00246974610702698545855460210418641509,
		0.006322138141064522626200172794597165193,
		0.001411823120786187893416219196751626441,
		-0.005778912866566685360703026219653111184,
		-0.000539621548383245391508622468279554596,
		0.005184773540519031602424870897038999829,
		-0.000160412088138313022045444711238815216,
		-0.004565291704341306600056782372121233493,
		0.000703262526727135503512577940909977769,
		0.003941689681336679740297768148593604565,
		-0.001103707153510167897347837850929863635,
		-0.003333276559124515384113784932651469717,
		0.001378463028443757676377856569160940126,
		0.002754940419298467901149107817104777496,
		-0.001543657072345475025215710829229465162,
		-0.00221862576667492894869071484720279841,
		0.001616617300957494916813228513774447492,
		0.001732469863172050698885140462834897335,
		-0.001613386681590385333950887769560722518,
		-0.001302822619389340953049982729794464831,
		0.001550174587877081733289230669470271096,
		0.000932402583730824563272576721573159375,
		-0.001441449932844418493210758569489371439,
		-0.000622077986506033506208801497905369615,
		0.001301881690598231770816806118773456546,
		0.000370491577463436688212594649627362742,
		-0.001142739689442697631482914744083245751,
		-0.000174332633474075845286810348966355377,
		0.000975875559570133681644432677160239109,
		0.000029445930738341338089858467697013111,
		-0.000799938339957202400667957142843533802,
		0.000177421074278205637142491468694061041,
		0.001871990160621829881251731997338083602,
		-0.000162164358122296535080070212231362348
	};
	int32_t coefsBuff[FILTER_COEFFICIENTS_NUM];
	RingBuff samplesBuff;
	
	correctCoefficients(coefsDoubleBuff);
	coefsDoubleToFixed32(coefsDoubleBuff, coefsBuff);
	ringInitialization(&samplesBuff);
	run(&signal, &samplesBuff, coefsBuff, outputFilePtr);
	fclose(outputFilePtr);

	system("pause");
	return 0;
}

int getopt(int argc, char *const argv[], const char *optstring)
{
	if ((optind >= argc) || (argv[optind][0] != '-') || (argv[optind][0] == 0))
	{
		return -1;
	}

	int opt = argv[optind][1];
	const int8_t *p = strchr(optstring, opt);

	if (p == NULL)
	{
		printf("Unknown option %s\n", argv[optind]);
		return '?';
	}

	if (p[1] == ':')
	{
		optind++;

		if (optind >= argc || (argv[optind][0] == '-' && (argv[optind][1] < 48 || argv[optind][1] > 57)))
		{
			printf("Option %s needs a value\n", argv[optind - 1]);
			return ':';
		}

		optarg = argv[optind];
	}

	optind++;
	return opt;
}

void runGetopt(int argc, char *const argv[], const char *optstring, Signal *signal)
{
	int opt;

	while ((opt = getopt(argc, argv, OPTSTRING)) != -1)
	{
		switch (opt)
		{
		case 't':
			signal->signalType = 1;
			break;
		case 's':
			signal->signalType = 2;
			break;
		case 'n':
			signal->signalType = 3;
			break;
		case 'f':
			if (atof(optarg) < 0 || atof(optarg) > SAMPLE_RATE / 2)
			{
				printf("Wrong start frequency value specified, it will be %f\n", DEFAULT_START_FREQUENCY);
			}
			else
			{
				signal->startFrequency = atof(optarg);
			}
			break;
		case 'e':
			if (atof(optarg) < 0 || atof(optarg) > SAMPLE_RATE / 2)
			{
				printf("Wrong stop frequency value specified, it will be %f\n", DEFAULT_STOP_FREQUENCY);
			}
			else
			{
				signal->stopFrequency = atof(optarg);
			}
			break;
		case 'a':
			if (atof(optarg) > 0)
			{
				printf("Wrong amplitude value specified, it will be %f\n", DEFAULT_AMPLITUDE);
			}
			else
			{
				signal->amplitude = dBtoGain(atof(optarg));
			}
			break;
		case 'd':
			if (atof(optarg) < 0 || atof(optarg) * SAMPLE_RATE > UINT32_MAX)
			{
				printf("Wrong amplitude value specified, if will be %f\n", DEFAULT_SIGNAL_TIME);
			}
			else
			{
				signal->signalTime = atof(optarg);
			}
			break;
		default:
			exit(0);
		}
	}
}

double dBtoGain(double dB)
{
	return powf(10, dB / 20.0f);
}

int16_t doubleToFixed15(double x)
{
	if (x >= 1)
	{
		return INT16_MAX;
	}
	else if (x < -1)
	{
		return INT16_MIN;
	}

	return (int16_t)(x * (double)(1LL << 15));
}

int32_t doubleToFixed32(double x)
{
	if (x >= 1)
	{
		return INT32_MAX;
	}
	else if (x < -1)
	{
		return INT32_MIN;
	}

	return (int32_t)(x * (double)(1LL << 31));
}

void correctCoefficients(double *coeficients)
{
	uint16_t i;
	double sum = 0;

	for (i = 0; i < FILTER_COEFFICIENTS_NUM; i++)
	{
		sum += coeficients[i];
	}

	if (sum != 1)
	{
		for (i = 0; i < FILTER_COEFFICIENTS_NUM; i++)
		{
			coeficients[i] *= (2 - sum);
		}
	}
}

void coefsDoubleToFixed32(double *input, int32_t *output)
{
	int16_t i;

	for (i = 0; i < FILTER_COEFFICIENTS_NUM; i++)
	{
		output[i] = doubleToFixed32(input[i]);
	}
}

void ringInitialization(RingBuff *buff)
{
	int i;

	for (i = 0; i < FILTER_COEFFICIENTS_NUM; i++)
	{
		buff->samples[i] = 0;
	}

	buff->currNum = 0;
}

void signalInitialization(Signal *signal)
{
	signal->signalType = 0;
	signal->startFrequency = DEFAULT_START_FREQUENCY;
	signal->stopFrequency = DEFAULT_STOP_FREQUENCY;
	signal->amplitude = dBtoGain(DEFAULT_AMPLITUDE);
	signal->signalTime = DEFAULT_SIGNAL_TIME;
	signal->samplesNum = (uint32_t)round(signal->signalTime * SAMPLE_RATE);
}

void FileHeaderInitialization(WavHeader *header, Signal *signal)
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
	header->dataSize = signal->samplesNum * header->blockAlign;
	header->fileSize = header->dataSize  + FILE_HEADER_SIZE;
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

int16_t generateToneSignal(Signal *signal, uint32_t *timeCounter)
{
	int16_t res = doubleToFixed15(signal->amplitude * sin(2 * PI * signal->startFrequency * 
		(double)*timeCounter / (SAMPLE_RATE)));
	*timeCounter += 1;

	return res;
}

int16_t generateSweepSignal(Signal *signal, uint32_t *timeCounter)
{
	double Omega1 = 2 * PI * signal->startFrequency;
	double Omega2 = 2 * PI * signal->stopFrequency;
	double log1 = log(Omega2 / Omega1);
	double t1;
	double exp1;
	int16_t res;

	t1 = (double)*timeCounter / signal->samplesNum;
	exp1 = exp(t1 * log1) - 1;
	res = doubleToFixed15(signal->amplitude * sin(Omega1 * signal->samplesNum * exp1 / (SAMPLE_RATE * log1)));
	*timeCounter += 1;

	return res;
}

int16_t generateNoiseSignal(Signal *signal)
{
	return doubleToFixed15(signal->amplitude * ((double)rand() / RAND_MAX * 2.0 - 1.0));
}

int16_t firFilter(RingBuff *ringBuff, int32_t *coefsBuff)
{
	uint8_t index;
	uint16_t i;
	int16_t sample;
	int64_t accum = 0;

	for (i = 0; i < FILTER_COEFFICIENTS_NUM; i++)
	{
		index = ((ringBuff->currNum - i) & (FILTER_COEFFICIENTS_NUM - 1));
		sample = ringBuff->samples[index];
		accum += (int64_t)sample * coefsBuff[i];
	}

	ringBuff->currNum = (ringBuff->currNum + 1) & (FILTER_COEFFICIENTS_NUM - 1);

	return (int16_t)((accum + (1LL << 30)) >> 31);
}


void run(Signal *signal, RingBuff *ringBuff, int32_t *coefsBuff, FILE *outputFilePtr)
{
	uint32_t i;
	uint32_t timeCounter = 0;
	int16_t *buff = malloc(sizeof(int16_t) * signal->samplesNum * CHANNELS);
	
	for (i = 0; i < signal->samplesNum * CHANNELS; i += CHANNELS)
	{
		switch (signal->signalType)
		{
		case 1:
			buff[i] = generateToneSignal(signal, &timeCounter);
			buff[i + 1] = buff[i];
			break;
		case 2:
			buff[i] = generateSweepSignal(signal, &timeCounter);
			buff[i + 1] = buff[i];
			break;
		case 3:
			buff[i] = generateNoiseSignal(signal);
			buff[i + 1] = buff[i];
			break;
		default:
			printf("Wrong signal type specified\n");
			return;
		}

		ringBuff->samples[ringBuff->currNum] = buff[i + 1];
		buff[i + 1] = firFilter(ringBuff, coefsBuff);
	}

	fwrite(buff, BYTES_PER_SAMPLE, signal->samplesNum * CHANNELS, outputFilePtr);
}
