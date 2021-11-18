#ifndef _MUSIC_CONTROL_H_
#define _MUSIC_CONTROL_H_

#include "Overhead Types.h"

enum MusicList
{
	MARIMBAD2_MUSIC,
	MARIMBAD2_B_MUSIC,
	MARIMBAD2_C_MUSIC,
	MARIMBAD2_D_MUSIC,
	MARIMBAD2_E_MUSIC,
	MARIMBAD2_F_MUSIC,
	MARIMBAD2_G_MUSIC,
	MARIMBAD2_H_MUSIC,
	MENUMIX_MUSIC,
	MENUMIX_B_MUSIC,
	MENUMIX_C_MUSIC,
	MENUMIX_D_MUSIC,
	NOTHING_A_MUSIC,
	NOTHING_B_MUSIC,
	NOTHING_C_MUSIC,
	NOTHING_D_MUSIC,
	NOTHING_E_MUSIC,
	NOTHING_F_MUSIC,
	NOTHING_G_MUSIC,
	NOTHING_H_MUSIC,
	NOTHING_I_MUSIC,
	NOTHING_J_MUSIC,
	NOTHING_K_MUSIC,
	NOTHING_L_MUSIC,
	NOTHING_M_MUSIC,
	NOTHING_N_MUSIC,
	NOTHING_O_MUSIC,
	NOTHING_P_MUSIC,
	TENSOR_A_MUSIC,
	TENSOR_B_MUSIC,
	TENSOR_C_MUSIC,
	TENSOR_D_MUSIC,
	TENSOR_E_MUSIC,
	TENSOR_F_MUSIC,
	TENSOR_G_MUSIC,
	TENSOR_H_MUSIC,
	TENSOR_I_MUSIC,
	TENSOR_J_MUSIC,
	TENSOR_K_MUSIC,
	TENSOR_L_MUSIC,
	TENSOR_M_MUSIC,
	TENSOR_N_MUSIC,
	TENSOR_O_MUSIC,
	TENSOR_P_MUSIC,
	TRIUMPH_MUSIC,
	DEATH_MUSIC,
	BATTLE_A_MUSIC,
	BATTLE_B_MUSIC, //same as tensor B
	BATTLE_C_MUSIC,
	BATTLE_D_MUSIC,
	BATTLE_E_MUSIC,
	BATTLE_F_MUSIC,
	BATTLE_G_MUSIC,
	BATTLE_H_MUSIC,
	BATTLE_I_MUSIC,
	BATTLE_J_MUSIC,
	BATTLE_K_MUSIC,
	BATTLE_L_MUSIC,
	BATTLE_M_MUSIC,
	BATTLE_N_MUSIC,
	BATTLE_O_MUSIC,
	BATTLE_P_MUSIC,
	CREEPY_MUSIC,
	CREEPY_MUSIC_B,
	CREEPY_MUSIC_C,
	CREEPY_MUSIC_D,
	CREATURE_BATTLE_MUSIC,
	CREATURE_BATTLE_MUSIC_B,
	CREATURE_BATTLE_MUSIC_C,
	CREATURE_BATTLE_MUSIC_D,
	CREDITS_MUSIC,
	CREDITS_MUSIC_B,
	CREDITS_MUSIC_C,
	CREDITS_MUSIC_D,
	MUSIC_DIR,
	NUM_MUSIC
};

enum MusicMode
{
	MUSIC_NONE,
	MUSIC_RESTORE,
	MUSIC_MAIN_MENU,
	MUSIC_TACTICAL_NOTHING,
	MUSIC_TACTICAL_ENEMYPRESENT,
	MUSIC_TACTICAL_BATTLE,
	MUSIC_TACTICAL_VICTORY,
	MUSIC_TACTICAL_DEATH,
	MUSIC_LAPTOP,
	MUSIC_OLD_TYPE,
	MUSIC_TACTICAL_BATTLE_MUSIC,
	MUSIC_TACTICAL_CREEPY_MUSIC,
	OTHER_MUSIC_TACTICAL,
	MUSIC_TACTICAL_CIV_GROUP_BATTLE,
	MUSIC_CREDITS,
};

#ifdef NEWMUSIC
typedef struct
{
	UINT16 uiIndex;
	INT32 SoundTacticalVictory[4];
	INT32 SoundTacticalBattle[4];
	INT32 SoundTacticalNothing[4];
	INT32 SoundTacticalTensor[4];
	INT32 SoundTacticalDeath[4];
	INT32 SoundTacticalBattleCreature[4];
	INT32 SoundTacticalBattleCreepy[4];
	INT32 SoundTacticalBattleGroup[NUM_CIV_GROUPS];

} MUSIC_SOUND_VALUES;

extern MUSIC_SOUND_VALUES MusicSoundValues[256];
extern INT32 GlobalSoundID;
#endif

//extern UINT32 uiMusicHandle;
//extern BOOLEAN fMusicPlaying;
//extern UINT8 gubMusicMode;
//extern BOOLEAN gfForceMusicToTense;

UINT8 GetMusicMode(void);
BOOLEAN SetMusicMode(UINT8 ubMusicMode);

// only for editor (editscreen.cpp)
#ifdef NEWMUSIC
BOOLEAN MusicPlay(UINT32 uiNum, UINT32 MusicMode, BOOLEAN NewSound);
#else
BOOLEAN MusicPlay(UINT32 uiNum);
#endif

#ifdef NEWMUSIC
BOOLEAN SetMusicModeID(UINT8 ubMusicMode, INT32 SoundID);
#endif
UINT32 MusicGetVolume(void);
BOOLEAN MusicSetVolume(UINT32 uiVolume);

BOOLEAN MusicPoll(BOOLEAN fForce);

void SetMusicFadeSpeed(INT8 bFadeSpeed);

BOOLEAN UsingCreatureMusic(void);
void UseCreatureMusic(BOOLEAN fUseCreatureMusic);

// only for luaglobal.cpp
BOOLEAN IsMusicPlaying(void);
UINT32 GetMusicHandle(void);

//BOOLEAN MusicStop(void);
//BOOLEAN MusicFadeOut(void);
//BOOLEAN MusicFadeIn(void);
//void FadeMusicForXSeconds( UINT32 uiDelay );

#endif
