//devmaster.net
#
#include <conio.h>
#include <stdlib.h>
#include <al.h>
#include <alc.h>
#include <alut.h>


// Buffers hold sound data.
ALuint Buffer;

// Sources are points emitting sound.
ALuint Source;

// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };


// Position of the listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

ALboolean LoadALData()
{
 // Variables to load into.

 ALenum format;
 ALsizei size;
 ALvoid* data;
 ALsizei freq;
 ALboolean loop;
 // Load wav data into a buffer.
 alGenBuffers(1, &Buffer);
 if (alGetError() != AL_NO_ERROR)
  return AL_FALSE;

 alutLoadWAVFile("wavdata/FancyPants.wav", &format, &data, &size, &freq, &loop);
 alBufferData(Buffer, format, data, size, freq);
 alutUnloadWAV(format, data, size, freq);
 // Bind buffer with a source.
 alGenSources(1, &Source);

 if (alGetError() != AL_NO_ERROR)
  return AL_FALSE;

 alSourcei(Source, AL_BUFFER, Buffer);
 alSourcef(Source, AL_PITCH, 1.0f);
 alSourcef(Source, AL_GAIN, 1.0f);
 alSourcefv(Source, AL_POSITION, SourcePos);
 alSourcefv(Source, AL_VELOCITY, SourceVel);
 alSourcei(Source, AL_LOOPING, loop);
 // Do another error check and return.
 if (alGetError() == AL_NO_ERROR)
  return AL_TRUE;

 return AL_FALSE;
}

void SetListenerValues()
{
 alListenerfv(AL_POSITION, ListenerPos);
 alListenerfv(AL_VELOCITY, ListenerVel);
 alListenerfv(AL_ORIENTATION, ListenerOri);
}
void KillALData()
{
 alDeleteBuffers(1, &Buffer);
 alDeleteSources(1, &Source);
 alutExit();
}
int main(int argc, char *argv[])
{
 // Initialize OpenAL and clear the error bit.

 alutInit(&argc, argv);
 alGetError();

 // Load the wav data.
 if (LoadALData() == AL_FALSE)
  return -1;

 SetListenerValues();

 // Setup an exit procedure.
 atexit(KillALData);
 ALubyte c = ' ';

 while (c != 'q')
 {
  c = getche();

  switch (c)
  {
   // Pressing 'p' will begin playing the sample.
  case 'p': alSourcePlay(Source); break;

   // Pressing 's' will stop the sample from playing.
  case 's': alSourceStop(Source); break;

   // Pressing 'h' will pause (hold) the sample.
  case 'h': alSourcePause(Source); break;
  };
 }

 return 0;

}
