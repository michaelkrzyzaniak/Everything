#include "MIDI_Parser.h"

/*----------------------------------------------------------------------*/
typedef enum midi_state_enum
{
  MIDI_AWAITING_STATUS,
  MIDI_AWAITING_ARGUMENTS,
  MIDI_PARSING_SYSEX,
}midi_state_t;

/*----------------------------------------------------------------------*/
//instance variables
midi_state_t state = MIDI_AWAITING_STATUS;
int num_awaited_arguments = 0;
#define MIDI_BUFFER_SIZE 100 //must be at least 3;
uint8_t midi_buffer[MIDI_BUFFER_SIZE];
uint8_t* b = midi_buffer;

void midi_message_dispatch(uint8_t* message);

void (*midi_note_on_event_handler        ) (midi_channel_t  , midi_pitch_t   , midi_velocity_t) = NULL;
void (*midi_note_off_event_handler       ) (midi_channel_t  , midi_pitch_t   , midi_velocity_t) = NULL;
void (*midi_poly_aftertouch_event_handler) (midi_channel_t  , midi_pitch_t   , midi_velocity_t) = NULL;
void (*midi_control_change_event_handler ) (midi_channel_t  , midi_control_t , uint8_t arg    ) = NULL;
void (*midi_mode_change_event_handler    ) (midi_channel_t  , midi_mode_t    , uint8_t arg    ) = NULL;
void (*midi_program_change_event_handler ) (midi_channel_t  , midi_program_t                  ) = NULL;
void (*midi_chan_aftertouch_event_handler) (midi_channel_t  , midi_velocity_t                 ) = NULL;
void (*midi_pitch_wheel_event_handler    ) (midi_channel_t  , midi_uint14_t                   ) = NULL;
void (*midi_sysex_event_handler          ) (midi_sysex_mfr_t, char* message                   ) = NULL;
void (*midi_realtime_event_handler       ) (midi_realtime_status_t                            ) = NULL;


/*----------------------------------------------------------------------*/
void midi_parse(uint8_t stream)
{ 
  if((stream == MIDI_STATUS_END_OF_EXCLUSIVE) && (state == MIDI_PARSING_SYSEX))
    {
      if((b - midi_buffer) >= MIDI_BUFFER_SIZE)
        b = midi_buffer + (MIDI_BUFFER_SIZE - 1);
      else if((b - midi_buffer) < 2)
        *b++ = MIDI_MFR_OTHER;
        
      *b++ = '\0';
      midi_message_dispatch(midi_buffer);
      state = MIDI_AWAITING_STATUS;
    }
    
  //these can interrupt other messages
  else if(stream > MIDI_STATUS_SYSTEM_EXCLUSIVE)
      midi_message_dispatch(&stream);
  
  //dont explicitley wait incase the previous message was corrupt (not all bytes recd)
  //if(state == MIDI_AWAITING_STATUS)
  else if(stream >= 0x80)
    {
      b = midi_buffer; *b++ = stream;

      switch(stream & MIDI_CHANNEL_MASK_ANY)
        {    
          case MIDI_STATUS_NOTE_OFF:
          case MIDI_STATUS_NOTE_ON:
          case MIDI_STATUS_POLYPHONIC_AFTERTOUCH:
          case MIDI_STATUS_PITCH_WHEEL:
          case MIDI_STATUS_MODE_CHANGE: // MIDI_STATUS_CONTROL_CHANGE:
            num_awaited_arguments = 2;
            state = MIDI_AWAITING_ARGUMENTS;
            break;

          case MIDI_STATUS_PROGRAM_CHANGE:
          case MIDI_STATUS_CHANNEL_AFTERTOUCH:
            num_awaited_arguments = 1;
            state = MIDI_AWAITING_ARGUMENTS;
            break;

          case MIDI_STATUS_SYSTEM_EXCLUSIVE:
            state = MIDI_PARSING_SYSEX;
            break;
          default: break;
        }
    }

  // (MIDI_PARSING_SYSEX or MIDI_AWAITING_ARGUMENTS)
  else if((b - midi_buffer) <= MIDI_BUFFER_SIZE)
      {
        *b++ = stream;
        if(state == MIDI_AWAITING_ARGUMENTS)
          if(--num_awaited_arguments == 0)
            {
              midi_message_dispatch(midi_buffer);
              state = MIDI_AWAITING_STATUS;
            }
      }
}

/*----------------------------------------------------------------------*/
void midi_message_dispatch(uint8_t* message)
{ 
  midi_status_t  status  = message[0] & MIDI_CHANNEL_MASK_ANY;
  midi_channel_t channel = 1 + (message[0] & MIDI_CHANNEL_MASK);
  
  switch(status)
    {    
      case MIDI_STATUS_NOTE_ON:
        if(message[2] != 0)
          {
            if(midi_note_on_event_handler != NULL)
              midi_note_on_event_handler(channel, message[1], message[2]);
            break;
          }
        //else cascade

      case MIDI_STATUS_NOTE_OFF:
        if(midi_note_off_event_handler != NULL)
          midi_note_off_event_handler(channel, message[1], message[2]);
        break;

      case MIDI_STATUS_POLYPHONIC_AFTERTOUCH:
        if(midi_poly_aftertouch_event_handler != NULL)
          midi_poly_aftertouch_event_handler(channel, message[1], message[2]);
        break;

      case MIDI_STATUS_PITCH_WHEEL:
        if(midi_pitch_wheel_event_handler != NULL)
          midi_pitch_wheel_event_handler(channel, midi_make_uint14(message[2], message[1]));
        break;

      case MIDI_STATUS_MODE_CHANGE: // MIDI_STATUS_CONTROL_CHANGE:
        if(message[1] >= MIDI_MODE_ALL_SOUND_OFF)
          {
            if(midi_mode_change_event_handler != NULL)
              midi_mode_change_event_handler(channel, message[1], message[2]);
          }
        else
          {
            if(midi_control_change_event_handler != NULL)
              midi_control_change_event_handler(channel, message[1], message[2]);          
          }
        break;

      case MIDI_STATUS_PROGRAM_CHANGE:
        if(midi_program_change_event_handler != NULL)
          midi_program_change_event_handler(channel, message[1]);
        break;

      case MIDI_STATUS_CHANNEL_AFTERTOUCH:
        if(midi_chan_aftertouch_event_handler != NULL)
          midi_chan_aftertouch_event_handler(channel, message[1]);
        break;

      case MIDI_STATUS_SYSTEM_EXCLUSIVE:
        {
          switch(message[0])
            {
              case MIDI_STATUS_MTC_QUARTER_FRAME:
              case MIDI_STATUS_SONG_POSITION_POINTER:
              case MIDI_STATUS_SONG_SELECT:
              case MIDI_STATUS_TUNE_REQUEST:
              case MIDI_STATUS_TIMING_CLOCK:
              case MIDI_STATUS_START:
              case MIDI_STATUS_CONTINUE:
              case MIDI_STATUS_STOP:
              case MIDI_STATUS_ACTIVE_SENSING:
              case MIDI_STATUS_SYSTEM_RESET:
                if(midi_realtime_event_handler != NULL)
                  midi_realtime_event_handler(message[0]);
                break;
              case MIDI_STATUS_SYSTEM_EXCLUSIVE:
                if(midi_sysex_event_handler != NULL)
                  midi_sysex_event_handler(message[1], (char*)(message+2));
                break;
              default: break;
            }
          break;
        }
        
      default: break;
    }
}
