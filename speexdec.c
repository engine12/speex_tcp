
#include <speex/speex.h>
#include <stdio.h>

#define HAVE_SYS_SOUNDCARD_H

#ifdef HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#elif defined HAVE_SYS_AUDIOIO_H
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/audioio.h>
#ifndef AUDIO_ENCODING_SLINEAR
#define AUDIO_ENCODING_SLINEAR AUDIO_ENCODING_LINEAR /* Solaris */
#endif
#endif



FILE *out_file_open(char *outFile, int rate, int *channels)
{
   FILE *fout=NULL;
   /*Open output file*/
   if (strlen(outFile)==0)
   {
#if defined HAVE_SYS_SOUNDCARD_H
      int audio_fd, format, stereo;
      audio_fd=open("/dev/dsp", O_WRONLY);
      if (audio_fd<0)
      {
         perror("Cannot open /dev/dsp");
         exit(1);         
      }

     format=AFMT_S16_NE;
//       format=AFMT_S16_LE;
      if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format)==-1)
      {
         perror("SNDCTL_DSP_SETFMT");
         close(audio_fd);
         exit(1);
      }

      stereo=0;
      if (*channels==2)
         stereo=1;
      if (ioctl(audio_fd, SNDCTL_DSP_STEREO, &stereo)==-1)
      {
         perror("SNDCTL_DSP_STEREO");
         close(audio_fd);
         exit(1);
      }
      if (stereo!=0)
      {
         if (*channels==1)
            fprintf (stderr, "Cannot set mono mode, will decode in stereo\n");
         *channels=2;
      }

      if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &rate)==-1)
      {
         perror("SNDCTL_DSP_SPEED");
         close(audio_fd);
         exit(1);
      }
      fout = fdopen(audio_fd, "w");
	  
	  printf ("/dev/dsp opened");
	  
#elif defined HAVE_SYS_AUDIOIO_H
      audio_info_t info;
      int audio_fd;
      
      audio_fd = open("/dev/audio", O_WRONLY);
      if (audio_fd<0)
      {
         perror("Cannot open /dev/audio");
         exit(1);
      }

      AUDIO_INITINFO(&info);
#ifdef AUMODE_PLAY    /* NetBSD/OpenBSD */
      info.mode = AUMODE_PLAY;
#endif
      info.play.encoding = AUDIO_ENCODING_SLINEAR;
      info.play.precision = 16;
      info.play.sample_rate = rate;
      info.play.channels = *channels;
      
      if (ioctl(audio_fd, AUDIO_SETINFO, &info) < 0)
      {
         perror ("AUDIO_SETINFO");
         exit(1);
      }
      fout = fdopen(audio_fd, "w");
#elif defined WIN32 || defined _WIN32
      {
         unsigned int speex_channels = *channels;
         if (Set_WIN_Params (INVALID_FILEDESC, rate, SAMPLE_SIZE, speex_channels))
         {
            fprintf (stderr, "Can't access %s\n", "WAVE OUT");
            exit(1);
         }
      }
#else
      fprintf (stderr, "No soundcard support\n");
      exit(1);
#endif
   } 
   
   return fout;
}

/*The frame size in hardcoded for this sample code but it doesn’t have to be*/
#define FRAME_SIZE 320
#define FRAMES_PER_RTP_PACKET 21

int main(int argc, char **argv)
{
//	char *outFile;
//	FILE *fout;
	FILE *fout=NULL;
	int channels=1;       
	int rate=16000;
	
	/*Holds the audio that will be written to file (16 bits per sample)*/
	short out[FRAME_SIZE];
	/*Speex handle samples as float, so we need an array of floats*/
	short output[FRAME_SIZE];
	char cbits[1500];
	int nbBytes;
	/*Holds the state of the decoder*/
	void *state;
	/*Holds bits so they can be read and written to by the Speex routines*/
	SpeexBits bits;
	int i, tmp;
	
	fout = out_file_open("", rate, &channels);
	
	/*Create a new decoder state in narrowband mode*/
	state = speex_decoder_init(&speex_wb_mode);

	/*Set the perceptual enhancement on*/
	tmp=1;
	speex_decoder_ctl(state, SPEEX_SET_ENH, &tmp);

int frame_size;
   speex_decoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size); 
   
//	outFile = argv[1];
//	fout = fopen(outFile, "w");

	/*Initialization of the structure that holds the bits*/
	speex_bits_init(&bits);
	while (1)
	{
		/*Read the size encoded by sampleenc, this part will likely be
		different in your application*/
		fread(&nbBytes, sizeof(int), 1, stdin);
		fprintf (stderr, "nbBytes: %d\n", nbBytes);
		if (feof(stdin))
		break;

		/*Read the "packet" encoded by sampleenc*/
		fread(cbits, 1, nbBytes, stdin);
		/*Copy the data into the bit-stream struct*/
		speex_bits_read_from(&bits, cbits, nbBytes);

		int nFrames = 1;
		while(nFrames<= FRAMES_PER_RTP_PACKET){
			/*Decode the data*/
			speex_decode_int(state, &bits, output);
	//		speex_decode(state, &bits, output);

			/*Copy from float to short (16 bits) for output*/
		//	for (i=0;i<frame_size;i++)
		//		out[i]=output[i];

			/*Write the decoded audio to file*/
			fwrite(output, sizeof(short), FRAME_SIZE, fout);
			fflush(fout);
			
			nFrames++;
		}			
	}

  if (fout != NULL)
      fclose(fout);  
	  
	/*Destroy the decoder state*/
	speex_decoder_destroy(state);
	/*Destroy the bit-stream truct*/
	speex_bits_destroy(&bits);
//	fclose(fout);
	return 0;

}
