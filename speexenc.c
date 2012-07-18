
#include <speex/speex.h>
#include <stdio.h>

/*The frame size in hardcoded for this sample code but it doesn’t have to be*/
//#define FRAME_SIZE 320
#define FRAME_SIZE 320
#define FRAMES_PER_RTP_PACKET 21

int main(int argc, char **argv)
{
//	char *inFile;
//	FILE *fin;
	short in[FRAME_SIZE];
	short input[FRAME_SIZE];
	char cbits[1500];
	int nbBytes;
	/*Holds the state of the encoder*/
	void *state;
	/*Holds bits so they can be read and written to by the Speex routines*/
	SpeexBits bits;
	int i, tmp;

	/*Create a new encoder state in narrowband mode*/
	state = speex_encoder_init(&speex_wb_mode);

	/*Set the quality to 8 (15 kbps)*/
	tmp=2;
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &tmp);
//	inFile = argv[1];
//	fin = fopen(inFile, "r");
	/*Initialization of the structure that holds the bits*/
	speex_bits_init(&bits);
	fflush(stdin);
	while (1)
	{
		/*Read a 16 bits/sample audio frame*/

//		if (feof(fin))
//			break;

		/*Copy the 16 bits values to float so Speex can work on them*/
//		for (i=0;i<FRAME_SIZE;i++)
//			input[i]=in[i];

		/*Flush all the bits in the struct so we can encode a new frame*/
		speex_bits_reset(&bits);

#if 0
		fread(in, sizeof(short), FRAME_SIZE, stdin);		
		/*Encode the frame*/
		speex_encode_int(state, in, &bits);
		/*Copy the bits to an array of char that can be written*/
		
#else
		int nFrames = 1;
			
		while(nFrames<=FRAMES_PER_RTP_PACKET){
		//	int nRawAudio_Bytes = read(fileno(fFid), in, FRAME_SIZE_ME);
			int nRawAudio_Bytes = fread(in, sizeof(short), FRAME_SIZE, stdin);		
//			int nRawAudio_Bytes = read(fileno(stdin), in, FRAME_SIZE);


	//		fprintf (stderr, "nRawAudio_Bytes:= %d \n",nRawAudio_Bytes);

			speex_encode_int(state, in, &bits);

			nFrames ++;

		}
#endif
	
	
		nbBytes = speex_bits_write(&bits, cbits, 1500);
	//	fprintf (stderr, "nbBytes: %d\n", nbBytes);
		
		/*Write the size of the frame first. This is what sampledec expects but
		it’s likely to be different in your own application*/
		fwrite(&nbBytes, sizeof(int), 1, stdout);
		/*Write the compressed data*/
		fwrite(cbits, 1, nbBytes, stdout);
		fflush(stdout);
	}

	/*Destroy the encoder state*/
	speex_encoder_destroy(state);
	/*Destroy the bit-packing struct*/
	speex_bits_destroy(&bits);
//	fclose(fin);
	return 0;

}
