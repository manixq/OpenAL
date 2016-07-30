#define _CRT_SECURE_NO_WARNINGS

#include <al.h>
#include <alc.h>
#include <alut.h>
#include <efx.h>
#include <EFX-Util.h>
#include <efx-creative.h>
#include <xram.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::string;


typedef struct  WAV_HEADER
{
 /* RIFF Chunk Descriptor */
 uint8_t         RIFF[4];        // RIFF Header Magic header
 uint32_t        ChunkSize;      // RIFF Chunk Size
 uint8_t         WAVE[4];        // WAVE Header
                                 /* "fmt" sub-chunk */
 uint8_t         fmt[4];         // FMT header
 uint32_t        Subchunk1Size;  // Size of the fmt chunk
 uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
 uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
 uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
 uint32_t        bytesPerSec;    // bytes per second
 uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
 uint16_t        bitsPerSample;  // Number of bits per sample
                                 /* "data" sub-chunk */
 uint8_t         Subchunk2ID[4]; // "data"  string
 uint32_t        Subchunk2Size;  // Sampled data length
} wav_hdr;

// Function prototypes
int getFileSize(FILE* inFile);
wav_hdr wavHeader;
int Loadwav()
{
 
 int headerSize = sizeof(wav_hdr), filelength = 0;

 const char* filePath;
 string input = "wav/blood.wav";
 filePath = input.c_str();
 FILE* wavFile = fopen(filePath, "r");
 if (wavFile == nullptr)
 {
  fprintf(stderr, "Unable to open wave file: %s\n", filePath);
  return 1;
 }

 //Read the header
 size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile);
 cout << "Header Read " << bytesRead << " bytes." << endl;
 if (bytesRead > 0)
 {
  //Read the data
  uint16_t bytesPerSample = wavHeader.bitsPerSample / 8;      //Number     of bytes per sample
  uint64_t numSamples = wavHeader.ChunkSize / bytesPerSample; //How many samples are in the wav file?
  static const uint16_t BUFFER_SIZE = 4096;
  int8_t* buffer = new int8_t[BUFFER_SIZE];
  while ((bytesRead = fread(buffer, sizeof buffer[0], BUFFER_SIZE / (sizeof buffer[0]), wavFile)) > 0)
  {
   /** DO SOMETHING WITH THE WAVE DATA HERE **/
   cout << "Read " << bytesRead << " bytes." << endl;
  }
  delete[] buffer;
  buffer = nullptr;
  filelength = getFileSize(wavFile);

  cout << "File is                    :" << filelength << " bytes." << endl;
  cout << "RIFF header                :" << wavHeader.RIFF[0] << wavHeader.RIFF[1] << wavHeader.RIFF[2] << wavHeader.RIFF[3] << endl;
  cout << "WAVE header                :" << wavHeader.WAVE[0] << wavHeader.WAVE[1] << wavHeader.WAVE[2] << wavHeader.WAVE[3] << endl;
  cout << "FMT                        :" << wavHeader.fmt[0] << wavHeader.fmt[1] << wavHeader.fmt[2] << wavHeader.fmt[3] << endl;
  cout << "Data size                  :" << wavHeader.ChunkSize << endl;

  // Display the sampling Rate from the header
  cout << "Sampling Rate              :" << wavHeader.SamplesPerSec << endl;
  cout << "Number of bits used        :" << wavHeader.bitsPerSample << endl;
  cout << "Number of channels         :" << wavHeader.NumOfChan << endl;
  cout << "Number of bytes per second :" << wavHeader.bytesPerSec << endl;
  cout << "Data length                :" << wavHeader.Subchunk2Size << endl;
  cout << "Audio Format               :" << wavHeader.AudioFormat << endl;
  // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM

  cout << "Block align                :" << wavHeader.blockAlign << endl;
  cout << "Data string                :" << wavHeader.Subchunk2ID[0] << wavHeader.Subchunk2ID[1] << wavHeader.Subchunk2ID[2] << wavHeader.Subchunk2ID[3] << endl;
 }
 fclose(wavFile);
 system("pause");
 return 0;

}

// find the file size
int getFileSize(FILE* inFile)
{
 int fileSize = 0;
 fseek(inFile, 0, SEEK_END);

 fileSize = ftell(inFile);

 fseek(inFile, 0, SEEK_SET);
 return fileSize;
}
int main(int argc, char **argv)
{
 ALCdevice* Device;
 Device = alcOpenDevice(nullptr);

 ALCcontext* Context;
 if (Device) {
  Context = alcCreateContext(Device, nullptr);
  alcMakeContextCurrent(Context);
 }

 ALboolean g_bEAX;
 // Check for EAX 2.0 support
 g_bEAX = alIsExtensionPresent("EAX2.0");

 // Generate Buffers
 alGetError(); // clear error code

 ALsizei NUM_BUFFERS=1;
 ALuint* g_Buffers=new ALuint[2];
 alGenBuffers(NUM_BUFFERS, g_Buffers);
 ALenum error;
 if ((error = alGetError()) != AL_NO_ERROR) {
  printf("GenBuffer \n"); return 0;
 }

 // Load test.wav
 ALenum format;
 const ALvoid *data;
 ALsizei size;
 ALsizei freq;
 //LoadWAVFile("test.wav", &format, &data, &size, &freq, &loop);<----
 Loadwav();


 if ((error = alGetError()) != AL_NO_ERROR)
 { 
  printf("LOADWAV \n");
  alDeleteBuffers(NUM_BUFFERS, g_Buffers);
  return 0;
 }

 alBufferData(g_Buffers[0], wavHeader.AudioFormat, wavHeader.Subchunk2ID, wavHeader.ChunkSize, wavHeader.SamplesPerSec);
 if ((error = alGetError()) != AL_NO_ERROR)
 {
  printf("albufferdata \n");
  alDeleteBuffers(NUM_BUFFERS, g_Buffers);
  return 0;
 }

 //---
 // Unload test.wav ~cleanup
 //---

 ALuint* source=nullptr;
 // Generate Sources
 alGenSources(1, source);
 if ((error = alGetError()) != AL_NO_ERROR)
 {
  printf("gensources \n");  
  return 0;
 }

 // Attach buffer 0 to source 
 alSourcei(source[0], AL_BUFFER, g_Buffers[0]);
 if ((error = alGetError()) != AL_NO_ERROR) {
  printf("alsourcei"); return 0;
 }

 // Exit
 Context = alcGetCurrentContext();
 Device = alcGetContextsDevice(Context);
 alcMakeContextCurrent(NULL);
 alcDestroyContext(Context);
 alcCloseDevice(Device);

 return 0;
}