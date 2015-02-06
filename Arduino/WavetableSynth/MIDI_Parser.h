#ifndef MIDI_PARSER
#define MIDI_PARSER 1

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus            
extern "C" {                
#endif 

#define mMIDI2CPS(x) (440 * pow(2, ((x)-69) / 12.0))

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_status_nibble_enum
{
  MIDI_STATUS_NOTE_OFF                   = 0x80, // (nibble)channel, (byte)num, (byte)vel
  MIDI_STATUS_NOTE_ON                    = 0x90, // (nibble)channel, (byte)num, (byte)vel
  MIDI_STATUS_POLYPHONIC_AFTERTOUCH      = 0xA0, // (nibble)channel, (byte)num, (byte)vel
  MIDI_STATUS_MODE_CHANGE                = 0xB0, // (nibble)channel, (byte)mode,(byte)see below
  MIDI_STATUS_CONTROL_CHANGE             = 0xB0, // (nibble)channel, (byte)mode,(byte)see below
  MIDI_STATUS_PROGRAM_CHANGE             = 0xC0, // (nibble)channel, (byte)mode
  MIDI_STATUS_CHANNEL_AFTERTOUCH         = 0xD0, // (nibble)channel, (byte)vel
  MIDI_STATUS_PITCH_WHEEL                = 0xE0, // (byte)  lsb    , (byte)msb; 2000 = no change
  MIDI_STATUS_SYSTEM_EXCLUSIVE           = 0xF0, // (byte)manufacturer, (bytes)data, (byte)EOX
  MIDI_STATUS_END_OF_EXCLUSIVE           = 0xF7, // none
  //for MIDI files only
  MIDI_STATUS_META_EVENT                 = 0xFF, // data bytes, described below
}midi_status_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_realtime_status_enum
{
  /* system common messages */
  MIDI_STATUS_MTC_QUARTER_FRAME          = 0xF1,
  MIDI_STATUS_SONG_POSITION_POINTER      = 0xF2,
  MIDI_STATUS_SONG_SELECT                = 0xF3,
  //undefined                            = 0xF4,
  //undefined                            = 0xF5,
  MIDI_STATUS_TUNE_REQUEST               = 0xF6, // none
  //MIDI_STATUS_END_OF_EXCLUSIVE         = 0xF7, // none
  /* realtime messages */
  MIDI_STATUS_TIMING_CLOCK               = 0xF8, // none
  //undefined                            = 0xF9, 
  MIDI_STATUS_START                      = 0xFA, // none
  MIDI_STATUS_CONTINUE                   = 0xFB, // none
  MIDI_STATUS_STOP                       = 0xFC, // none
  //undefined                            = 0xFD,
  MIDI_STATUS_ACTIVE_SENSING             = 0xFE, // none
  MIDI_STATUS_SYSTEM_RESET               = 0xFF, // none
}midi_realtime_status_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_channel_nibble_enum
{
  MIDI_CHANNEL_1                         = 0x00,
  MIDI_CHANNEL_2                         = 0x01,
  MIDI_CHANNEL_3                         = 0x02,
  MIDI_CHANNEL_4                         = 0x03,
  MIDI_CHANNEL_5                         = 0x04,
  MIDI_CHANNEL_6                         = 0x05,
  MIDI_CHANNEL_7                         = 0x06,
  MIDI_CHANNEL_8                         = 0x07,
  MIDI_CHANNEL_9                         = 0x08,
  MIDI_CHANNEL_10                        = 0x09,
  MIDI_CHANNEL_11                        = 0x0A,
  MIDI_CHANNEL_12                        = 0x0B,
  MIDI_CHANNEL_13                        = 0x0C,
  MIDI_CHANNEL_14                        = 0x0D,
  MIDI_CHANNEL_15                        = 0x0E,
  MIDI_CHANNEL_16                        = 0x0F,
  MIDI_CHANNEL_GRAND_PIANO               = 0x00,
  MIDI_CHANNEL_POLYSYNTH                 = 0x01,
  MIDI_CHANNEL_FINGERED_BASS             = 0x02,
  MIDI_CHANNEL_MUTED_GUITAR              = 0x03,
  MIDI_CHANNEL_STRINGS                   = 0x04,
  MIDI_CHANNEL_PIZZICATO_STRINGS         = 0x05,
  MIDI_CHANNEL_BRASS                     = 0x06,
  MIDI_CHANNEL_TRUMPET                   = 0x07,
  MIDI_CHANNEL_REVERSE_CYMBAL            = 0x08,
  MIDI_CHANNEL_DRUMS                     = 0x09,
  MIDI_CHANNEL_DRAW_ORGAN                = 0x0A,
  MIDI_CHANNEL_ELECTRIC_PIANO            = 0x0B,
  MIDI_CHANNEL_SYNTH_STRINGS             = 0x0C,
  MIDI_CHANNEL_SQUARE_WAVE               = 0x0D,
  MIDI_CHANNEL_SAW_WAVE                  = 0x0E,
  MIDI_CHANNEL_SOUNDTRACK                = 0x0F,
  MIDI_CHANNEL_MASK                      = 0x0F,
  MIDI_CHANNEL_MASK_ANY                  = 0xF0,
}midi_channel_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_pitch_enum
{
  MIDI_PITCH_C_NEG1                      = 0x00,
  MIDI_PITCH_Cs_NEG1                     = 0x01,
  MIDI_PITCH_Db_NEG1                     = 0x01,
  MIDI_PITCH_D_NEG1                      = 0x02,
  MIDI_PITCH_Ds_NEG1                     = 0x03,
  MIDI_PITCH_Eb_NEG1                     = 0x03,
  MIDI_PITCH_E_NEG1                      = 0x04,
  MIDI_PITCH_F_NEG1                      = 0x05,
  MIDI_PITCH_Fs_NEG1                     = 0x06,
  MIDI_PITCH_Gb_NEG1                     = 0x06,
  MIDI_PITCH_G_NEG1                      = 0x07,
  MIDI_PITCH_Gs_NEG1                     = 0x08,
  MIDI_PITCH_Ab_NEG1                     = 0x08,
  MIDI_PITCH_A_NEG1                      = 0x09,
  MIDI_PITCH_As_NEG1                     = 0x0A,
  MIDI_PITCH_Bb_NEG1                     = 0x0A,
  MIDI_PITCH_BNEG1                       = 0x0B,
  
  MIDI_PITCH_C_0                         = 0x0C,
  MIDI_PITCH_Cs_0                        = 0x0D,
  MIDI_PITCH_Db_0                        = 0x0D,
  MIDI_PITCH_D_0                         = 0x0E,
  MIDI_PITCH_Ds_0                        = 0x0F,
  MIDI_PITCH_Eb_0                        = 0x0F,
  MIDI_PITCH_E_0                         = 0x10,
  MIDI_PITCH_F_0                         = 0x11,
  MIDI_PITCH_Fs_0                        = 0x12,
  MIDI_PITCH_Gb_0                        = 0x12,
  MIDI_PITCH_G_0                         = 0x13,
  MIDI_PITCH_Gs_0                        = 0x14,
  MIDI_PITCH_Ab_0                        = 0x14,
  MIDI_PITCH_A_0                         = 0x15,
  MIDI_PITCH_As_0                        = 0x16,
  MIDI_PITCH_Bb_0                        = 0x16,
  MIDI_PITCH_B_0                         = 0x17,
  
  MIDI_PITCH_C_1                         = 0x18,
  MIDI_PITCH_Cs_1                        = 0x19,
  MIDI_PITCH_Db_1                        = 0x19,
  MIDI_PITCH_D_1                         = 0x1A,
  MIDI_PITCH_Ds_1                        = 0x1B,
  MIDI_PITCH_Eb_1                        = 0x1B,
  MIDI_PITCH_E_1                         = 0x1C,
  MIDI_PITCH_F_1                         = 0x1D,
  MIDI_PITCH_Fs_1                        = 0x1E,
  MIDI_PITCH_Gb_1                        = 0x1E,
  MIDI_PITCH_G_1                         = 0x1F,
  MIDI_PITCH_Gs_1                        = 0x20,
  MIDI_PITCH_Ab_1                        = 0x20,
  MIDI_PITCH_A_1                         = 0x21,
  MIDI_PITCH_As_1                        = 0x22,
  MIDI_PITCH_Bb_1                        = 0x22,
  MIDI_PITCH_B_1                         = 0x23,    
  
  MIDI_PITCH_C_2                         = 0x24,
  MIDI_PITCH_Cs_2                        = 0x25,
  MIDI_PITCH_Db_2                        = 0x25,
  MIDI_PITCH_D_2                         = 0x26,
  MIDI_PITCH_Ds_2                        = 0x27,
  MIDI_PITCH_Eb_2                        = 0x27,
  MIDI_PITCH_E_2                         = 0x28,
  MIDI_PITCH_F_2                         = 0x29,
  MIDI_PITCH_Fs_2                        = 0x2A,
  MIDI_PITCH_Gb_2                        = 0x2A,
  MIDI_PITCH_G_2                         = 0x2B,
  MIDI_PITCH_Gs_2                        = 0x2C,
  MIDI_PITCH_Ab_2                        = 0x2C,
  MIDI_PITCH_A_2                         = 0x2D,
  MIDI_PITCH_As_2                        = 0x2E,
  MIDI_PITCH_Bb_2                        = 0x2E,
  MIDI_PITCH_B_2                         = 0x2F,
  
  MIDI_PITCH_C_3                         = 0x30,
  MIDI_PITCH_Cs_3                        = 0x31,
  MIDI_PITCH_Db_3                        = 0x31,
  MIDI_PITCH_D_3                         = 0x32,
  MIDI_PITCH_Ds_3                        = 0x33,
  MIDI_PITCH_Eb_3                        = 0x33,
  MIDI_PITCH_E_3                         = 0x34,
  MIDI_PITCH_F_3                         = 0x35,
  MIDI_PITCH_Fs_3                        = 0x36,
  MIDI_PITCH_Gb_3                        = 0x36,
  MIDI_PITCH_G_3                         = 0x37,
  MIDI_PITCH_Gs_3                        = 0x38,
  MIDI_PITCH_Ab_3                        = 0x38,
  MIDI_PITCH_A_3                         = 0x39,
  MIDI_PITCH_As_3                        = 0x3A,
  MIDI_PITCH_Bb_3                        = 0x3A,
  MIDI_PITCH_B_3                         = 0x3B,
  
  MIDI_PITCH_C_4                         = 0x3C,
  MIDI_PITCH_Cs_4                        = 0x3D,
  MIDI_PITCH_Db_4                        = 0x3D,
  MIDI_PITCH_D_4                         = 0x3E,
  MIDI_PITCH_Ds_4                        = 0x3F,
  MIDI_PITCH_Eb_4                        = 0x3F,
  MIDI_PITCH_E_4                         = 0x40,
  MIDI_PITCH_F_4                         = 0x41,
  MIDI_PITCH_Fs_4                        = 0x42,
  MIDI_PITCH_Gb_4                        = 0x42,
  MIDI_PITCH_G_4                         = 0x43,
  MIDI_PITCH_Gs_4                        = 0x44,
  MIDI_PITCH_Ab_4                        = 0x44,
  MIDI_PITCH_A_4                         = 0x45,
  MIDI_PITCH_As_4                        = 0x46,
  MIDI_PITCH_Bb_4                        = 0x46,
  MIDI_PITCH_B_4                         = 0x47,
  
  MIDI_PITCH_C_5                         = 0x48,
  MIDI_PITCH_Cs_5                        = 0x49,
  MIDI_PITCH_Db_5                        = 0x49,
  MIDI_PITCH_D_5                         = 0x4A,
  MIDI_PITCH_Ds_5                        = 0x4B,
  MIDI_PITCH_Eb_5                        = 0x4B,
  MIDI_PITCH_E_5                         = 0x4C,
  MIDI_PITCH_F_5                         = 0x4D,
  MIDI_PITCH_Fs_5                        = 0x4E,
  MIDI_PITCH_Gb_5                        = 0x4E,
  MIDI_PITCH_G_5                         = 0x4F,
  MIDI_PITCH_Gs_5                        = 0x50,
  MIDI_PITCH_Ab_5                        = 0x50,
  MIDI_PITCH_A_5                         = 0x51,
  MIDI_PITCH_As_5                        = 0x52,
  MIDI_PITCH_Bb_5                        = 0x52,
  MIDI_PITCH_B_5                         = 0x53,
  
  MIDI_PITCH_C_6                         = 0x54,
  MIDI_PITCH_Cs_6                        = 0x55,
  MIDI_PITCH_Db_6                        = 0x55,
  MIDI_PITCH_D_6                         = 0x56,
  MIDI_PITCH_Ds_6                        = 0x57,
  MIDI_PITCH_Eb_6                        = 0x57,
  MIDI_PITCH_E_6                         = 0x58,
  MIDI_PITCH_F_6                         = 0x59,
  MIDI_PITCH_Fs_6                        = 0x5A,
  MIDI_PITCH_Gb_6                        = 0x5A,
  MIDI_PITCH_G_6                         = 0x5B,
  MIDI_PITCH_Gs_6                        = 0x5C,
  MIDI_PITCH_Ab_6                        = 0x5C,
  MIDI_PITCH_A_6                         = 0x5D,
  MIDI_PITCH_As_6                        = 0x5E,
  MIDI_PITCH_Bb_6                        = 0x5E,
  MIDI_PITCH_B_6                         = 0x5F,
  
  MIDI_PITCH_C_7                         = 0x60,
  MIDI_PITCH_Cs_7                        = 0x61,
  MIDI_PITCH_Db_7                        = 0x61,
  MIDI_PITCH_D_7                         = 0x62,
  MIDI_PITCH_Ds_7                        = 0x63,
  MIDI_PITCH_Eb_7                        = 0x63,
  MIDI_PITCH_E_7                         = 0x64,
  MIDI_PITCH_F_7                         = 0x65,
  MIDI_PITCH_Fs_7                        = 0x66,
  MIDI_PITCH_Gb_7                        = 0x66,
  MIDI_PITCH_G_7                         = 0x67,
  MIDI_PITCH_Gs_7                        = 0x68,
  MIDI_PITCH_Ab_7                        = 0x68,
  MIDI_PITCH_A_7                         = 0x69,
  MIDI_PITCH_As_7                        = 0x6A,
  MIDI_PITCH_Bb_7                        = 0x6A,
  MIDI_PITCH_B_7                         = 0x6B,
  
  MIDI_PITCH_C_8                         = 0x6C,
  MIDI_PITCH_Cs_8                        = 0x6D,
  MIDI_PITCH_Db_8                        = 0x6D,
  MIDI_PITCH_D_8                         = 0x6E,
  MIDI_PITCH_Ds_8                        = 0x6F,
  MIDI_PITCH_Eb_8                        = 0x6F,
  MIDI_PITCH_E_8                         = 0x70,
  MIDI_PITCH_F_8                         = 0x71,
  MIDI_PITCH_Fs_8                        = 0x72,
  MIDI_PITCH_Gb_8                        = 0x72,
  MIDI_PITCH_G_8                         = 0x73,
  MIDI_PITCH_Gs_8                        = 0x74,
  MIDI_PITCH_Ab_8                        = 0x74,
  MIDI_PITCH_A_8                         = 0x75,
  MIDI_PITCH_As_8                        = 0x76,
  MIDI_PITCH_Bb_8                        = 0x76,
  MIDI_PITCH_B_8                         = 0x77,
  
  MIDI_PITCH_C_9                         = 0x78,
  MIDI_PITCH_Cs_9                        = 0x79,
  MIDI_PITCH_Db_9                        = 0x79,
  MIDI_PITCH_D_9                         = 0x7A,
  MIDI_PITCH_Ds_9                        = 0x7B,
  MIDI_PITCH_Eb_9                        = 0x7B,
  MIDI_PITCH_E_9                         = 0x7C,
  MIDI_PITCH_F_9                         = 0x7D,
  MIDI_PITCH_Fs_9                        = 0x7E,
  MIDI_PITCH_Gb_9                        = 0x7E,
  MIDI_PITCH_G_9                         = 0x7F,
  
  MIDI_PITCH_BASS_DRUM_2                 = 0x23,
  MIDI_PITCH_BASS_DRUM_1                 = 0x24,
  MIDI_PITCH_RIMSHOT                     = 0x25,
  MIDI_PITCH_SNARE_DRUM_1                = 0x26,
  MIDI_PITCH_HAND_CLAP                   = 0x27,
  MIDI_PITCH_SNARE_DRUM_2                = 0x28,
  MIDI_PITCH_LOW_TOM_2                   = 0x29,
  MIDI_PITCH_CLOSED_HI_HAT               = 0x2A,
  MIDI_PITCH_LOW_TOM_1                   = 0x2B,
  MIDI_PITCH_PEDAL_HI_HAT                = 0x2C,
  MIDI_PITCH_MID_TOM_2                   = 0x2D,
  MIDI_PITCH_OPEN_HI_HAT                 = 0x2E,
  MIDI_PITCH_MID_TOM_1                   = 0x2F,
  MIDI_PITCH_HIGH_TOM_2                  = 0x30,
  MIDI_PITCH_CRASH_CYMBAL_1              = 0x31,
  MIDI_PITCH_HIGH_TOM_1                  = 0x32,
  MIDI_PITCH_RIDE_CYMBAL_1               = 0x33,
  MIDI_PITCH_CHINESE_CYMBAL              = 0x34,
  MIDI_PITCH_RIDE_BELL                   = 0x35,
  MIDI_PITCH_TAMBURINE                   = 0x36,
  MIDI_PITCH_SPLASH_CYMBAL               = 0x37,
  MIDI_PITCH_COWBELL                     = 0x38,
  MIDI_PITCH_CRASH_CYMBAL_2              = 0x39,
  MIDI_PITCH_VIBRA_SLAP                  = 0x3A,
  MIDI_PITCH_RIDE_CYMBAL_2               = 0x3B,
  MIDI_PITCH_HIGH_BONGO                  = 0x3C,
  MIDI_PITCH_LOW_BONGO                   = 0x3D,
  MIDI_PITCH_MUTE_HIGH_CONGA             = 0x3E,
  MIDI_PITCH_OPEN_HIGH_CONGA             = 0x3F,
  MIDI_PITCH_LOW_CONGA                   = 0x40,
  MIDI_PITCH_HIGH_TIMBALE                = 0x41,
  MIDI_PITCH_LOW_TIMBALE                 = 0x42,
  MIDI_PITCH_HIGH_AGOGO                  = 0x43,
  MIDI_PITCH_LOW_AGOGO                   = 0x44,
  MIDI_PITCH_CABASA                      = 0x45,
  MIDI_PITCH_MARACAS                     = 0x46,
  MIDI_PITCH_SHORT_WHISTLE               = 0x47,
  MIDI_PITCH_LONG_WHISTLE                = 0x48,
  MIDI_PITCH_SHORT_GUIRO                 = 0x49,
  MIDI_PITCH_LONG_GUIRO                  = 0x4A,
  MIDI_PITCH_CLAVES                      = 0x4B,
  MIDI_PITCH_HIGH_WOOD_BLOCK             = 0x4C,
  MIDI_PITCH_LOW_WOOD_BLOCK              = 0x4D,
  MIDI_PITCH_MUTE_CUICA                  = 0x4E,
  MIDI_PITCH_OPEN_CUICA                  = 0x4F,
  MIDI_PITCH_MUTE_TRIANGLE               = 0x50,
  MIDI_PITCH_OPEN_TRIANGLE               = 0x51,
}midi_pitch_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_velocitiy_enum
{
  MIDI_VELOCITY_OFF                      = 0  ,
  MIDI_VELOCITY_PPP                      = 15 ,
  MIDI_VELOCITY_PP                       = 31 ,
  MIDI_VELOCITY_P                        = 47 ,
  MIDI_VELOCITY_MP                       = 63 ,
  MIDI_VELOCITY_MF                       = 79 ,
  MIDI_VELOCITY_F                        = 95 ,
  MIDI_VELOCITY_FF                       = 111,
  MIDI_VELOCITY_FFF                      = 127,
}midi_velocity_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_sysex_mfr_enum //manufacturer
{
  MIDI_MFR_OTHER                         = 0x00,
  MIDI_MFR_SEQUENTIAL                    = 0x01,
  MIDI_MFR_IDP                           = 0x02,
  MIDI_MFR_VOYETRA                       = 0x03,
  MIDI_MFR_MOOG                          = 0x04,
  MIDI_MFR_PASSPORT                      = 0x05,
  MIDI_MFR_LEXICON                       = 0x06,
  MIDI_MFR_KURZWEIL                      = 0x07,
  MIDI_MFR_FENDER                        = 0x08,
  MIDI_MFR_GULBRANSEN                    = 0x09,
  MIDI_MFR_AKG                           = 0x0A,
  MIDI_MFR_VOYCE                         = 0x0B,
  MIDI_MFR_WAVEFRAME                     = 0x0C,
  MIDI_MFR_ADA                           = 0x0D,
  MIDI_MFR_GARFIELD                      = 0x0E,
  MIDI_MFR_ENSONIQ                       = 0x0F,
  MIDI_MFR_OBERHEIM                      = 0x10,
  MIDI_MFR_APPLE                         = 0x11,
  MIDI_MFR_DIGIDESIGN                    = 0x13,
  MIDI_MFR_STERINBERG                    = 0x3A,
  MIDI_MFR_KAWAI                         = 0x40,
  MIDI_MFR_ROLAND                        = 0x41,
  MIDI_MFR_KORG                          = 0x42,
  MIDI_MFR_YAMAHA                        = 0x43,
  MIDI_MFR_AKAI                          = 0x47,
  MIDI_MFR_SONY                          = 0x4C,
  MIDI_MFR_TASCAM                        = 0x4E,
  MIDI_MFR_ZOOM                          = 0x52,
  MIDI_MFR_SUZUKI                        = 0x55,
  MIDI_MFR_NON_COMMERCIAL                = 0x7D,
  MIDI_MFR_NON_REAL_TIME                 = 0x7E,
  MIDI_MFR_REAL_TIME                     = 0x7F,
}midi_sysex_mfr_t;


/*--------------------------------------------------------------------------------------------*/
typedef enum midi_control_enum
{
  MIDI_CTRL_BANK_SELECT_MSB              = 0x00,
  MIDI_CTRL_MODULATION_WHEEL_MSB         = 0x01,
  MIDI_CTRL_BREATH_CONTROLLER_MSB        = 0x02,
  //undefined                            = 0x03,
  MIDI_CTRL_FOOT_CONTROLLER_MSB          = 0x04,
  MIDI_CTRL_PORTAMENTO_TIME_MSB          = 0x05,
  MIDI_CTRL_DATA_ENTRY_MSB               = 0x06,
  MIDI_CTRL_CHANNEL_VOLUME_MSB           = 0x07,
  MIDI_CTRL_BALANCE_MSB                  = 0x08,
  //undefined                            = 0x09,
  MIDI_CTRL_PAN_MSB                      = 0x0A,
  MIDI_CTRL_EXPRESSION_CONTROLLER_MSB    = 0x0B,
  MIDI_CTRL_EFFECT_CTRL_1_MSB            = 0x0C,
  MIDI_CTRL_EFFECT_CTRL_2_MSB            = 0x0D,
  //undefined                            = 0x0E,
  //undefined                            = 0x0F,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_1_MSB   = 0x10,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_2_MSB   = 0x11,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_3_MSB   = 0x12,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_4_MSB   = 0x13,
  //undefined                            = 0x14,
  //undefined                            = 0x15,
  //undefined                            = 0x16,
  //undefined                            = 0x17,
  //undefined                            = 0x18,
  //undefined                            = 0x19,
  //undefined                            = 0x1A,
  //undefined                            = 0x1B,
  //undefined                            = 0x1C,
  //undefined                            = 0x1D,
  //undefined                            = 0x1E,
  //undefined                            = 0x1F,
  MIDI_CTRL_BANK_SELECT_LSB              = 0x20,
  MIDI_CTRL_MODULATION_WHEEL_LSB         = 0x21,
  MIDI_CTRL_BREATH_CONTROLLER_LSB        = 0x22,
  //undefined                            = 0x23,
  MIDI_CTRL_FOOT_CONTROLLER_LSB          = 0x24,
  MIDI_CTRL_PORTAMENTO_TIME_LSB          = 0x25,
  MIDI_CTRL_DATA_ENTRY_LSB               = 0x26,
  MIDI_CTRL_CHANNEL_VOLUME_LSB           = 0x27,
  MIDI_CTRL_BALANCE_LSB                  = 0x28,
  //undefined                            = 0x29,
  MIDI_CTRL_PAN_LSB                      = 0x2A,
  MIDI_CTRL_EXPRESSION_CONTROLLER_LSB    = 0x2B,
  MIDI_CTRL_EFFECT_CTRL_1_LSB            = 0x2C,
  MIDI_CTRL_EFFECT_CTRL_2_LSB            = 0x2D,
  //undefined                            = 0x2E,
  //undefined                            = 0x2F,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_1_LSB   = 0x30,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_2_LSB   = 0x31,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_3_LSB   = 0x32,
  MIDI_CTRL_GENERAL_PURPOSE_CTRL_4_LSB   = 0x33,
  //undefined                            = 0x34,
  //undefined                            = 0x35,
  //undefined                            = 0x36,
  //undefined                            = 0x37,
  //undefined                            = 0x38,
  //undefined                            = 0x39,
  //undefined                            = 0x3A,
  //undefined                            = 0x3B,
  //undefined                            = 0x3C,
  //undefined                            = 0x3D,
  //undefined                            = 0x3E,
  //undefined                            = 0x3F,
  /*
                                         = 0x40,
                                         = 0x41,
                                         = 0x42,
                                         = 0x43,
                                         = 0x44,
                                         = 0x45,
                                         = 0x46,
                                         = 0x47,
                                         = 0x48,
                                         = 0x49,
                                         = 0x4A,
                                         = 0x4B,
                                         = 0x4C,
                                         = 0x4D,
                                         = 0x4E,
                                         = 0x4F,
                                         = 0x50,
                                         = 0x51,
                                         = 0x52,
                                         = 0x53,
                                         = 0x54,
                                         = 0x55,
                                         = 0x56,
                                         = 0x57,
                                         = 0x58,
                                         = 0x59,
                                         = 0x5A,
                                         = 0x5B,
                                         = 0x5C,
                                         = 0x5D,
                                         = 0x5E,
                                         = 0x5F,
  */
  MIDI_CTRL_DATA_INCREMENT               = 0x60,
  MIDI_CTRL_DATA_DECREMENT               = 0x61,
  MIDI_CTRL_UNREGISTERED_PARAMETER_LSB   = 0x62,
  MIDI_CTRL_UNREGISTERED_PARAMETER_MSB   = 0x63,
  MIDI_CTRL_REGISTERED_PARAMETER_LSB     = 0x64,
  MIDI_CTRL_REGISTERED_PARAMETER_MSB     = 0x65,
  /*
                                         = 0x66,
                                         = 0x67,
                                         = 0x68,
                                         = 0x69,
                                         = 0x6A,
                                         = 0x6B,
                                         = 0x6C,
                                         = 0x6D,
                                         = 0x6E,
                                         = 0x6F,
                                         = 0x70,
                                         = 0x71,
                                         = 0x72,
                                         = 0x73,
                                         = 0x74,
                                         = 0x75,
                                         = 0x76,
                                         = 0x77,
*/
}midi_control_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_mode_enum
{
  MIDI_MODE_ALL_SOUND_OFF                = 0x78, // 0
  MIDI_MODE_RESET_ALL_CONTROLLERS        = 0x79, // 0
  MIDI_MODE_TOGGLE_LOCAL_CONTROL         = 0x79, // 0 off, 127 on
  MIDI_MODE_ALL_NOTES_OFF                = 0x7B, // 0
  MIDI_MODE_OMNI_OFF                     = 0x7C, // 0
  MIDI_MODE_OMNI_ON                      = 0x7D, // 0
  MIDI_MODE_MONO_MODE_ON                 = 0x7E,
  MIDI_MODE_POLY_MODE_ON                 = 0x7F, // 0
}midi_mode_t;


/*--------------------------------------------------------------------------------------------*/
typedef enum midi_program_enum
{
  MIDI_PROGRAM_ACOUSTIC_GRAND_PIANO       = 0x00,
  MIDI_PROGRAM_BRIGHT_ACOUSTIC_PIANO      = 0x01,
  MIDI_PROGRAM_ELECTRIC_GRAND_PIANO       = 0x02,
  MIDI_PROGRAM_HONKY_TONK_PIANO           = 0x03,
  MIDI_PROGRAM_ELECTRIC_PIANO_1           = 0x04,
  MIDI_PROGRAM_ELECTRIC_PIANO_2           = 0x05,
  MIDI_PROGRAM_HARPSICHORD                = 0x06,
  MIDI_PROGRAM_CLAVINET                   = 0x07,
  MIDI_PROGRAM_CELESTA                    = 0x08,
  MIDI_PROGRAM_GLOCKENSPIEL               = 0x09,
  MIDI_PROGRAM_MUSIC_BOX                  = 0x0A,
  MIDI_PROGRAM_VIBRAPHONE                 = 0x0B,
  MIDI_PROGRAM_MARIMBA                    = 0x0C,
  MIDI_PROGRAM_XYLOPHONE                  = 0x0D,
  MIDI_PROGRAM_TUBULAR_BELLS              = 0x0E,
  MIDI_PROGRAM_DULCIMER                   = 0x0F,
  MIDI_PROGRAM_DRAWBAR_ORGAN              = 0x10,
  MIDI_PROGRAM_PERCUSSIVE_ORGAN           = 0x11,
  MIDI_PROGRAM_ROCK_ORGAN                 = 0x12,
  MIDI_PROGRAM_CHURCH_ORGAN               = 0x13,
  MIDI_PROGRAM_REED_ORGAN                 = 0x14,
  MIDI_PROGRAM_ACCORDION                  = 0x15,
  MIDI_PROGRAM_HARMONICA                  = 0x16,
  MIDI_PROGRAM_TANGO_ACCORDION            = 0x17,
  MIDI_PROGRAM_ACOUSTIC_GUITAR_NYLON      = 0x18,
  MIDI_PROGRAM_ACOUSTIC_GUITAR_STEEL      = 0x19,
  MIDI_PROGRAM_ELECTRIC_GUITAR_JAZZ       = 0x1A,
  MIDI_PROGRAM_ELECTRIC_GUITAR_CLEAN      = 0x1B,
  MIDI_PROGRAM_ELECTRIC_GUITAR_MUTED      = 0x1C,
  MIDI_PROGRAM_OVERDRIVEN_GUITAR          = 0x1D,
  MIDI_PROGRAM_DISTORTION_GUITAR          = 0x1E,
  MIDI_PROGRAM_GUITAR_HARMONICS           = 0x1F,
  MIDI_PROGRAM_ACOUSTIC_BASS              = 0x20,
  MIDI_PROGRAM_ELECTRIC_BASS_FINGER       = 0x21,
  MIDI_PROGRAM_ELECTRIC_BASS_PICK         = 0x22,
  MIDI_PROGRAM_FRETLESS_BASS              = 0x23,
  MIDI_PROGRAM_SLAP_BASS_2                = 0x24,
  MIDI_PROGRAM_SLAP_BASS_1                = 0x25,
  MIDI_PROGRAM_SYNTH_BASS_1               = 0x26,
  MIDI_PROGRAM_SYNTH_BASS_2               = 0x27,
  MIDI_PROGRAM_VIOLIN                     = 0x28,
  MIDI_PROGRAM_VIOLA                      = 0x29,
  MIDI_PROGRAM_CELLO                      = 0x2A,
  MIDI_PROGRAM_CONTRABASS                 = 0x2B,
  MIDI_PROGRAM_TREMOLO_STRINGS            = 0x2C,
  MIDI_PROGRAM_PIZZICATO_STRINGS          = 0x2D,
  MIDI_PROGRAM_ORCHESTRAL_HARP            = 0x2E,
  MIDI_PROGRAM_TIMPANI                    = 0x2F,
  MIDI_PROGRAM_STRING_ENSEMBLE_1          = 0x30,
  MIDI_PROGRAM_STRING_ENSEMBLE_2          = 0x31,
  MIDI_PROGRAM_SYNTH_STRINGS_1            = 0x32,
  MIDI_PROGRAM_SYNTH_STRINGS_2            = 0x33,
  MIDI_PROGRAM_CHOIR_AAHS                 = 0x34,
  MIDI_PROGRAM_CHOIR_OOHS                 = 0x35,
  MIDI_PROGRAM_SYNTH_CHOIR                = 0x36,
  MIDI_PROGRAM_ORCHESTRA_HIT              = 0x37,
  MIDI_PROGRAM_TRUMPET                    = 0x38,
  MIDI_PROGRAM_TROMBONE                   = 0x39,
  MIDI_PROGRAM_TUBA                       = 0x3A,
  MIDI_PROGRAM_MUTED_TRUMPET              = 0x3B,
  MIDI_PROGRAM_FRENCH_HORN                = 0x3C,
  MIDI_PROGRAM_BRASS_SECTION              = 0x3D,
  MIDI_PROGRAM_SYNTH_BRASS_1              = 0x3E,
  MIDI_PROGRAM_SYNTH_BRASS_2              = 0x3F,
  MIDI_PROGRAM_SOPRANO_SAX                = 0x40,
  MIDI_PROGRAM_ALTO_SAX                   = 0x41,
  MIDI_PROGRAM_TENOR_SAX                  = 0x42,
  MIDI_PROGRAM_BARITONE_SAX               = 0x43,
  MIDI_PROGRAM_OBOE                       = 0x44,
  MIDI_PROGRAM_ENGLISH_HORN               = 0x45,
  MIDI_PROGRAM_BASSOON                    = 0x46,
  MIDI_PROGRAM_CLARINET                   = 0x47,
  MIDI_PROGRAM_PICCOLO                    = 0x48,
  MIDI_PROGRAM_FLUTE                      = 0x49,
  MIDI_PROGRAM_RECORDER                   = 0x4A,
  MIDI_PROGRAM_PAN_FLUTE                  = 0x4B,
  MIDI_PROGRAM_BLOWN_BOTTLE               = 0x4C,
  MIDI_PROGRAM_SHAKUHACHI                 = 0x4D,
  MIDI_PROGRAM_WHISTLE                    = 0x4E,
  MIDI_PROGRAM_OCARINA                    = 0x4F,
  MIDI_PROGRAM_SQUARE_WAVE                = 0x50,
  MIDI_PROGRAM_SAWTOOTH_WAVE              = 0x51,
  MIDI_PROGRAM_CALLIOPE                   = 0x52,
  MIDI_PROGRAM_CHIFF                      = 0x53,
  MIDI_PROGRAM_CHARANG                    = 0x54,
  MIDI_PROGRAM_LEAD_VOICE                 = 0x55,
  MIDI_PROGRAM_LEAD_FIFTHS                = 0x56,
  MIDI_PROGRAM_BASS_AND_LEAD              = 0x57,
  MIDI_PROGRAM_PAD_NEW_AGE                = 0x58,
  MIDI_PROGRAM_PAD_WARM                   = 0x59,
  MIDI_PROGRAM_PAD_POLYSYNTH              = 0x5A,
  MIDI_PROGRAM_PAD_CHOIR                  = 0x5B,
  MIDI_PROGRAM_PAD_BOWED                  = 0x5C,
  MIDI_PROGRAM_PAD_METALLIC               = 0x5D,
  MIDI_PROGRAM_PAD_HALO                   = 0x5E,
  MIDI_PROGRAM_PAD_SWEEP                  = 0x5F,
  MIDI_PROGRAM_FX_RAIN                    = 0x60,
  MIDI_PROGRAM_FX_SOUNDTRACK              = 0x61,
  MIDI_PROGRAM_FX_CRYSTAL                 = 0x62,
  MIDI_PROGRAM_FX_ATMOSPHERE              = 0x63,
  MIDI_PROGRAM_FX_BRIGHTNESS              = 0x64,
  MIDI_PROGRAM_FX_GOBLINS                 = 0x65,
  MIDI_PROGRAM_FX_ECHOES                  = 0x66,
  MIDI_PROGRAM_FX_SCI_FI                  = 0x67,
  MIDI_PROGRAM_SITAR                      = 0x68,
  MIDI_PROGRAM_BANJO                      = 0x69,
  MIDI_PROGRAM_SHAMISEN                   = 0x6A,
  MIDI_PROGRAM_KOTO                       = 0x6B,
  MIDI_PROGRAM_KALIMBA                    = 0x6C,
  MIDI_PROGRAM_BAGPIPE                    = 0x6D,
  MIDI_PROGRAM_FIDDLE                     = 0x6E,
  MIDI_PROGRAM_SHANAI                     = 0x6F,
  MIDI_PROGRAM_TINKLE_BELL                = 0x70,
  MIDI_PROGRAM_AGOGO                      = 0x71,
  MIDI_PROGRAM_STEEL_DRUMS                = 0x72,
  MIDI_PROGRAM_WOODBLOCK                  = 0x73,
  MIDI_PROGRAM_TAIKO_DRUM                 = 0x74,
  MIDI_PROGRAM_MELODIC_TOM                = 0x75,
  MIDI_PROGRAM_SYNTH_DRUM                 = 0x76,
  MIDI_PROGRAM_REVERSE_CYMBAL             = 0x77,
  MIDI_PROGRAM_GUITAR_FRET_NOISE          = 0x78,
  MIDI_PROGRAM_BREATH_NOISE               = 0x79,
  MIDI_PROGRAM_SEASHORE                   = 0x7A,
  MIDI_PROGRAM_BIRD_TWEET                 = 0x7B, 
  MIDI_PROGRAM_TELEPHONE                  = 0x7C,
  MIDI_PROGRAM_HELICOPTER                 = 0x7D,
  MIDI_PROGRAM_APPLAUSE                   = 0x7E,
  MIDI_PROGRAM_GUNSOT                     = 0x7F,
}midi_program_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_meta_events_enum //followed by length of data to follow
{
  //MIDI Files only
  MIDI_META_SEQUENCE_NUMBER              = 0x00, // length 2
  MIDI_META_INSERT_TEXT                  = 0x01, // variable length, text
  MIDI_META_COPYRIGHT                    = 0x02, // variable length, text
  MIDI_META_TRACK_NAME                   = 0x03, // variable length, text
  MIDI_META_INSTRUMENT_NAME              = 0x04, // variable length, text
  MIDI_META_INSERT_LYRIC                 = 0x05, // variable length, text
  MIDI_META_MARKER                       = 0x06, // variable length, text
  MIDI_META_CUE_POINT                    = 0x07, // variable length, text

  MIDI_META_MIDI_CHANNEL                 = 0x20, // length 1, channel
  MIDI_META_MIDI_PORT                    = 0x21, // length 1, port
  MIDI_META_END_OF_TRACK                 = 0x2F, // length 0
  MIDI_META_TEMPO_CHANGE                 = 0x51, // length 3, microsends per quarter
  MIDI_META_SMPTE_OFFSET                 = 0x54, // length 5, hours, minutes, seconds, frames, subframes
  MIDI_META_TIME_SIGNATURE               = 0x58, // length 4, numerator, denominator, clocks/metronome, 32nd notes/quarter note
  MIDI_META_KEY_SIGNATURE                = 0x59, // length 2, num accidentals(neg for flat), mode (0=MAJ, 1 = MIN)
}midi_meta_events_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_registered_parameters_enum
{
  MIDI_PARAM_PITCH_BEND_SENSITIVITY      = 0x00,
  MIDI_PARAM_CHANNEL_FINE_TUNING         = 0x01, // resolution 100/8192 cents  
  MIDI_PARAM_CHANNEL_COARSE_TUNING       = 0x02, // resolution 100 cents, MSB only
  MIDI_PARAM_TUNING_PROGRAM_CHANGE       = 0x03,
  MIDI_PARAM_TUNING_BANK_SELECT          = 0x04,
  MIDI_PARAM_MODULATION_DEPTH            = 0x05,
  MIDI_PARAM_NULL_PARAMATER              = 0x7F7F,
}midi_registered_parameters_t;

/*--------------------------------------------------------------------------------------------*/
typedef enum midi_unregistered_parameters_enum
{
  MIDI_UNREGISTERED_FAKE_PARAM           = 0x00,
  //user defined, not part of MIDI spec
}midi_unregistered_parameters_t;

/*--------------------------------------------------------------------------------------------*/
typedef        uint16_t midi_uint14_t;
uint8_t        midi_low_7_bits         (midi_uint14_t val);
uint8_t        midi_high_7_bits        (midi_uint14_t val);
midi_uint14_t  midi_make_uint14        (uint8_t high_7_bits, uint8_t low_7_bits);

#define midi_low_7_bits(x)  (((uint16_t) (x))       & 0x007F)
#define midi_high_7_bits(x) (((uint16_t)((x) >> 7)) & 0x007F)
#define midi_make_uint14(msb, lsb) (((((uint16_t)(msb)) & 0x007F) << 7) | (((uint16_t)(lsb)) & 0x007F))

/*--------------------------------------------------------------------------------------------*/
void midi_parse(uint8_t stream);

extern void (*midi_note_on_event_handler        ) (midi_channel_t  , midi_pitch_t   , midi_velocity_t);
extern void (*midi_note_off_event_handler       ) (midi_channel_t  , midi_pitch_t   , midi_velocity_t);
extern void (*midi_poly_aftertouch_event_handler) (midi_channel_t  , midi_pitch_t   , midi_velocity_t);
extern void (*midi_control_change_event_handler ) (midi_channel_t  , midi_control_t , uint8_t arg    );
extern void (*midi_mode_change_event_handler    ) (midi_channel_t  , midi_mode_t    , uint8_t arg    );
extern void (*midi_program_change_event_handler ) (midi_channel_t  , midi_program_t                  );
extern void (*midi_chan_aftertouch_event_handler) (midi_channel_t  , midi_velocity_t                 );
extern void (*midi_pitch_wheel_event_handler    ) (midi_channel_t  , midi_uint14_t                   );
extern void (*midi_sysex_event_handler          ) (midi_sysex_mfr_t, char* message                   );
extern void (*midi_realtime_event_handler       ) (midi_realtime_status_t                            );

/*--------------------------------------------------------------------------------------------*/
#ifdef __cplusplus            
}                             
#endif

#endif //MIDI_PARSER
