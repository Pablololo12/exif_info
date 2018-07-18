#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 10000
#define FIRST_DIRECTORY_OFFSET 18
#define OFFSET_START_OFFSET 10

typedef struct {
	unsigned short Tag;
	char * Desc;
}TagTable_t;

static TagTable_t TagTable[] = {
  {   0x100,   "ImageWidth"},
  {   0x101,   "ImageLength"},
  {   0x102,   "BitsPerSample"},
  {   0x103,   "Compression"},
  {   0x106,   "PhotometricInterpretation"},
  {   0x10A,   "FillOrder"},
  {   0x10D,   "DocumentName"},
  {   0x10E,   "ImageDescription"},
  {   0x10F,   "Make"},
  {   0x110,   "Model"},
  {   0x111,   "StripOffsets"},
  {   0x112,   "Orientation"},
  {   0x115,   "SamplesPerPixel"},
  {   0x116,   "RowsPerStrip"},
  {   0x117,   "StripByteCounts"},
  {   0x11A,   "XResolution"},
  {   0x11B,   "YResolution"},
  {   0x11C,   "PlanarConfiguration"},
  {   0x128,   "ResolutionUnit"},
  {   0x12D,   "TransferFunction"},
  {   0x131,   "Software"},
  {   0x132,   "DateTime"},
  {   0x13B,   "Artist"},
  {   0x13E,   "WhitePoint"},
  {   0x13F,   "PrimaryChromaticities"},
  {   0x156,   "TransferRange"},
  {   0x200,   "JPEGProc"},
  {   0x201,   "ThumbnailOffset"},
  {   0x202,   "ThumbnailLength"},
  {   0x211,   "YCbCrCoefficients"},
  {   0x212,   "YCbCrSubSampling"},
  {   0x213,   "YCbCrPositioning"},
  {   0x214,   "ReferenceBlackWhite"},
  {   0x828D,  "CFARepeatPatternDim"},
  {   0x828E,  "CFAPattern"},
  {   0x828F,  "BatteryLevel"},
  {   0x8298,  "Copyright"},
  {   0x829A,  "ExposureTime"},
  {   0x829D,  "FNumber"},
  {   0x83BB,  "IPTC/NAA"},
  {   0x8769,  "ExifOffset"},
  {   0x8773,  "InterColorProfile"},
  {   0x8822,  "ExposureProgram"},
  {   0x8824,  "SpectralSensitivity"},
  {   0x8825,  "GPSInfo"},
  {   0x8827,  "ISOSpeedRatings"},
  {   0x8828,  "OECF"},
  {   0x9000,  "ExifVersion"},
  {   0x9003,  "DateTimeOriginal"},
  {   0x9004,  "DateTimeDigitized"},
  {   0x9101,  "ComponentsConfiguration"},
  {   0x9102,  "CompressedBitsPerPixel"},
  {   0x9201,  "ShutterSpeedValue"},
  {   0x9202,  "ApertureValue"},
  {   0x9203,  "BrightnessValue"},
  {   0x9204,  "ExposureBiasValue"},
  {   0x9205,  "MaxApertureValue"},
  {   0x9206,  "SubjectDistance"},
  {   0x9207,  "MeteringMode"},
  {   0x9208,  "LightSource"},
  {   0x9209,  "Flash"},
  {   0x920A,  "FocalLength"},
  {   0x927C,  "MakerNote"},
  {   0x9286,  "UserComment"},
  {   0x9290,  "SubSecTime"},
  {   0x9291,  "SubSecTimeOriginal"},
  {   0x9292,  "SubSecTimeDigitized"},
  {   0xA000,  "FlashPixVersion"},
  {   0xA001,  "ColorSpace"},
  {   0xA002,  "ExifImageWidth"},
  {   0xA003,  "ExifImageLength"},
  {   0xA005,  "InteroperabilityOffset"},
  {   0xA20B,  "FlashEnergy"}, 
  {   0xA20C,  "SpatialFrequencyResponse"},
  {   0xA20E,  "FocalPlaneXResolution"},
  {   0xA20F,  "FocalPlaneYResolution"},
  {   0xA210,  "FocalPlaneResolutionUnit"},
  {   0xA214,  "SubjectLocation"},
  {   0xA215,  "ExposureIndex"},
  {   0xA217,  "SensingMethod"},
  {   0xA300,  "FileSource"},
  {   0xA301,  "SceneType"},
  {      0, "Not recognized"}
} ;

static unsigned int app1_size = 0;
static unsigned int offset_start = 0;

int show_data(unsigned char * buffer, unsigned int index, unsigned int size)
{
	unsigned int i;
	for (i=0; i<size; i++) {
		printf("%c",buffer[index+i]);
	}
	printf("\n");
	return 1;
}
	

int list_info(unsigned char *buffer, unsigned int index, unsigned int size)
{
	unsigned int i,d,j;
	unsigned int exifoffset = 0;
	d = index;
	printf("Number of entries: %d\n",size);
	for (i=0; i<size; i++) {
		unsigned short tag = buffer[d]<<8|buffer[d+1];
		d+=2;
		//unsigned char format = buffer[d]<<8|buffer[d+1];
		d+=2;
		unsigned int data_size = buffer[d]<<24|buffer[d+1]<<16
								|buffer[d+2]<<8|buffer[d+3];
		d+=4;
		unsigned long data = buffer[d]<<24|buffer[d+1]<<16
							|buffer[d+2]<<8|buffer[d+3];
		d+=4;
		for(j=0;;j++){
			if(TagTable[j].Tag == tag || TagTable[j].Tag == 0){
				printf("%X: %s : ",tag, TagTable[j].Desc);
				if(tag == 0x8769) exifoffset = data;
				break;
			}
		}
		if(data_size<=4) {
			printf("%ld\n", data);
		} else {
			show_data(buffer,offset_start+data,data_size);;
		}
	}
	if(exifoffset != 0){
		exifoffset+=offset_start;
		list_info(buffer,
				exifoffset+2, buffer[exifoffset]<<8|buffer[exifoffset+1]);
	}
	return 1;
}

int read_info(char *name)
{
	FILE *image;
	unsigned char *buffer;
	unsigned int i, index;

	if(!(image=fopen(name, "r"))){
		printf("Error opening file\n");
		return 0;
	}
	// Alloc memory for data
	buffer=malloc(sizeof(unsigned char)*BUFF_SIZE);
	// Read into buffer
	if(!fread(buffer, sizeof(unsigned char), BUFF_SIZE, image)) return 0;
	// Search for EXIF tag 0xFFE1
	for(i=0; i<BUFF_SIZE-1; i=i+2)
		if(buffer[i]==0xff && buffer[i+1]==0xe1) break;
	// In case of not being able to find the tag exit the function
	if (i>=BUFF_SIZE) return 0;
	// Get the data index and exif size
	index = i+FIRST_DIRECTORY_OFFSET;
	offset_start = i+OFFSET_START_OFFSET;
	app1_size = buffer[i+2]<<8 | buffer[i+3];

	list_info(buffer, index+2, buffer[index]<<8|buffer[index+1]);

	// Print the data
	/*
	for(i=0; i<app1_size+2; i++)
		printf("%c ", buffer[index+i]);
	printf("\n");
	*/
	fclose(image);
	free(buffer);
	return 1;
}

int main(int argc, char **argv)
{
	if(argc != 3){
		printf("Argument error\n");
		return 1;
	}

	if(strcmp(argv[1], "-l")==0){
		printf("List exif data\n");
		if(!read_info(argv[2])) return 1;
	} else if(strcmp(argv[1], "-d")==0){
		printf("Delete exif data\n");
	} else {
		printf("Bad argument\n");
		return 1;
	}
	return 0;
}
