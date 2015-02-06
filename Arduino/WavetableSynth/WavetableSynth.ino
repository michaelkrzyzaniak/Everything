/*!
 * @header Ceramics
 *
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Spring of 2013.
 *
 *  mkrzyzan@asu.edu
 *
 * @unsorted
 */

#include "Audio.h"
#include "MIDI_Parser.h"
void note_on_callback(midi_channel_t c , midi_pitch_t p  , midi_velocity_t v);
void note_off_callback(midi_channel_t c , midi_pitch_t p  , midi_velocity_t v);

/*------------------------------------------------------------*/
void setup(void)
{
  Serial.begin(9600);
  auStart();
  auSetVol(0.0);
  midi_note_on_event_handler  = note_on_callback;
  midi_note_off_event_handler = note_off_callback;
}

/*------------------------------------------------------------*/
void loop(void)
{
   while(Serial.available())
     midi_parse(Serial.read());
}

/*------------------------------------------------------------*/
void note_on_callback(midi_channel_t c , midi_pitch_t p  , midi_velocity_t v)
{
  auSetVol(v/127.0);
  auSetFreq(mMIDI2CPS(p));
}

/*------------------------------------------------------------*/
void note_off_callback(midi_channel_t c , midi_pitch_t p  , midi_velocity_t v)
{
  auSetVol(0.0);
}
