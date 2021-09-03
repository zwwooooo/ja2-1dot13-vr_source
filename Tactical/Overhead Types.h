#ifndef __OVERHEAD_TYPES_H
#define __OVERHEAD_TYPES_H
// GLOBAL HEADER FOR DATA, TYPES FOR TACTICAL ENGINE


#define REFINE_AIM_1					0
#define REFINE_AIM_MID1				1
#define REFINE_AIM_2					2
#define REFINE_AIM_MID2				3
#define REFINE_AIM_3					4
#define REFINE_AIM_MID3				5
#define REFINE_AIM_4					6
#define REFINE_AIM_MID4				7
#define REFINE_AIM_5					8
#define REFINE_AIM_BURST			-1	// SANDRO - I've changed this from 10 to -1, so when the code asks about how much we aim, it does not return this high value when we don't aim at all
										// hopefully, it will not cause any troubles anywhere .. it shouldn't 

#define AIM_SHOT_RANDOM				0
#define AIM_SHOT_HEAD					1
#define AIM_SHOT_TORSO				2
#define AIM_SHOT_LEGS					3
#define AIM_SHOT_GLAND				4

#define MIN_AMB_LEVEL_FOR_MERC_LIGHTS		9


#define		MAXTEAMS_SP				6 // WANNE: for singleplayer
#define     MAXTEAMS				11 //hayden
#define		MAXMERCS				MAX_NUM_SOLDIERS

//TACTICAL OVERHEAD STUFF
//#define			NO_SOLDIER			TOTAL_SOLDIERS // SAME AS NOBODY
//#define			NOBODY				NO_SOLDIER
#define				NOBODY				TOTAL_SOLDIERS




// TACTICAL ENGINE STATUS FLAGS
#define			REALTIME						0x000000002
#define			TURNBASED						0x000000004
#define			IN_ENDGAME_SEQUENCE			 		0x000000008
#define			SHOW_ALL_ITEMS											0x000000010
#define			SHOW_AP_LEFT												0x000000020
#define			SHOW_ALL_MERCS											0x000000040
#define			TRANSLUCENCY_TYPE										0x000000080
#define			GODMODE															0x000000100
#define			DEMOMODE														0x000000200
#define			PLAYER_TEAM_DEAD										0x000000400
#define			NPC_TEAM_DEAD												0x000000800
#define			DISALLOW_SIGHT											0x000001000
#define			CHECK_SIGHT_AT_END_OF_ATTACK				0x000002000
#define	 IN_CREATURE_LAIR					0x000004000
#define			HIDE_TREES													0x000008000
#define			NOHIDE_REDUNDENCY										0x000010000
#define			DEBUGCLIFFS													0x000020000
#define			INCOMBAT														0x000040000
#define			WANT_MILITIA_REINFORCEMENTS										0x000080000
#define			ACTIVE															0x000100000
#define			SHOW_Z_BUFFER												0x000200000
#define			SLOW_ANIMATION											0x000400000
#define			ENGAGED_IN_CONV											0x000800000
#define			LOADING_SAVED_GAME									0x001000000
#define			OUR_MERCS_AUTO_MOVE									0x002000000
#define			SHOW_ALL_ROOFS											0x004000000
#define			NEWLY_ENTERED_SECTOR								0x008000000
#define			RED_ITEM_GLOW_ON										0x010000000				
#define			IGNORE_ENGAGED_IN_CONV_UI_UNLOCK		0x020000000
#define			IGNORE_ALL_OBSTACLES								0x040000000
#define			IN_DEIDRANNA_ENDGAME								0x080000000
#define			DONE_DEIDRANNA_ENDGAME							0x100000000
	
#define			OKBREATH						10
#define			OKLIFE							15
#define			CONSCIOUSNESS				10

// VIEWRANGE DEFINES
#define	 NORMAL_VIEW_RANGE					13
#define			MIN_RANGE_FOR_BLOWNAWAY		40

// MODIFIERS FOR AP COST FOR MOVEMENT 
#define RUNDIVISOR				1.8
#define WALKCOST					-1
#define SWATCOST					0
#define CRAWLCOST					1

// CHRISL: AP Cost Movement modifiers when wearing a backpack
#define RUNDIVISORBPACK				1.5
#define WALKCOSTBPACK					0
#define SWATCOSTBPACK					1
#define CRAWLCOSTBPACK					2

// defines
// ######################################################
#define MAX_PATH_LIST_SIZE		30
#define NUM_SOLDIER_SHADES		48
#define NUM_SOLDIER_EFFECTSHADES		2

// TIMER DELAYS
#define	DAMAGE_DISPLAY_DELAY		250
#define	FADE_DELAY							150
#define	FLASH_SELECTOR_DELAY		4000
#define	BLINK_SELECTOR_DELAY		250

#define PTR_OURTEAM	 (pSoldier->bTeam == gbPlayerNum)


#define DONTLOOK				0
#define LOOK						1


#define NOLOCATE		0
#define LOCATE			1

#define DONTSETLOCATOR									0
#define SETLOCATOR											1
#define SETANDREMOVEPREVIOUSLOCATOR			2
#define SETLOCATORFAST									3

#define NOCENTERING	 0


#define NOUPDATE		0
#define UPDATE			1


// ORDERS
enum
{
	STATIONARY = 0,		// moves max 1 sq., no matter what's going on
	ONGUARD,					// moves max 2 sqs. until alerted by something
	CLOSEPATROL,			// patrols within 5 spaces until alerted
	FARPATROL,				// patrols within 15 spaces
	POINTPATROL,			// patrols using patrolGrids
	ONCALL,						// helps buddies anywhere within the sector
	SEEKENEMY,				// not tied down to any one particular spot
	RNDPTPATROL,			// patrols randomly using patrolGrids
	SNIPER,					// snipes
	MAXORDERS
};

// ATTITUDES
enum
{
	DEFENSIVE = 0,
	BRAVESOLO,
	BRAVEAID,
	CUNNINGSOLO,
	CUNNINGAID,
	AGGRESSIVE,
	MAXATTITUDES,
	ATTACKSLAYONLY // special hyperaggressive vs Slay only value for Carmen the bounty hunter
};

// alert status types
enum
{
	STATUS_GREEN = 0,	// everything's OK, no suspicion
	STATUS_YELLOW,		// he or his friend heard something
	STATUS_RED,				// has definite evidence of opponent
	STATUS_BLACK,			// currently sees an active opponent
	NUM_STATUS_STATES
};

enum
{
	MORALE_HOPELESS = 0,
	MORALE_WORRIED,
	MORALE_NORMAL,
	MORALE_CONFIDENT,
	MORALE_FEARLESS,
	NUM_MORALE_STATES
};

// DEFINES FOR WEAPON HIT EVENT SPECIAL PARAM
#define		FIRE_WEAPON_NO_SPECIAL				0
#define		FIRE_WEAPON_BURST_SPECIAL			1
#define		FIRE_WEAPON_HEAD_EXPLODE_SPECIAL	2
#define		FIRE_WEAPON_CHEST_EXPLODE_SPECIAL	3
#define		FIRE_WEAPON_LEG_FALLDOWN_SPECIAL	4
#define		FIRE_WEAPON_HIT_BY_KNIFE_SPECIAL	5
#define		FIRE_WEAPON_SLEEP_DART_SPECIAL		6
#define		FIRE_WEAPON_BLINDED_BY_SPIT_SPECIAL 7
#define		FIRE_WEAPON_TOSSED_OBJECT_SPECIAL   8
#define		FIRE_WEAPON_BLINDED					9
#define		FIRE_WEAPON_DEAFENED				10
#define		FIRE_WEAPON_BLINDED_AND_DEAFENED	11
#define		FIRE_WEAPON_BLINDED_SPECIAL			12	// Flugente: works like FIRE_WEAPON_BLINDED_BY_SPIT_SPECIAL but without the damage dependency


#define NO_INTERRUPTS			0
#define ALLOW_INTERRUPTS		1


#define SIGHT_LOOK		0x1
//#define SIGHT_SEND		0x2	// no longer needed using LOCAL OPPLISTs
#define SIGHT_RADIO	 0x4
#define SIGHT_INTERRUPT 0x8
#define SIGHT_ALL		0xF


// CHANGE THIS VALUE TO AFFECT TOTAL SIGHT RANGE
//#define STRAIGHT_RANGE gGameExternalOptions.ubStraightSightRange

// CHANGE THESE VALUES TO ADJUST VARIOUS FOV ANGLES
#define STRAIGHT_RATIO	1 //* 1.5
#define ANGLE_RATIO			0.857 //* 1.5
#define SIDE_RATIO			0.571 //* 1.5
// CJC: Changed SBEHIND_RATIO (side-behind ratio) to be 0 to make stealth attacks easier
// Changed on September 21, 1998
//#define SBEHIND_RATIO		0.142
#define SBEHIND_RATIO		0
#define BEHIND_RATIO		0

// looking distance defines
//#define BEHIND			(INT8)( BEHIND_RATIO * STRAIGHT_RANGE )
//#define SBEHIND		 (INT8)( SBEHIND_RATIO * STRAIGHT_RANGE )
//#define SIDE			(INT8)( SIDE_RATIO * STRAIGHT_RANGE )
//#define ANGLE			(INT8)( ANGLE_RATIO * STRAIGHT_RANGE )
//#define STRAIGHT		(INT8)( STRAIGHT_RATIO * STRAIGHT_RANGE )
 

// opplist value constants
#define HEARD_3_TURNS_AGO		-4
#define HEARD_2_TURNS_AGO		-3
#define HEARD_LAST_TURN		 -2
#define HEARD_THIS_TURN		 -1
#define NOT_HEARD_OR_SEEN		0
#define SEEN_CURRENTLY			1
#define SEEN_THIS_TURN			2
#define SEEN_LAST_TURN			3
#define SEEN_2_TURNS_AGO		 4
#define SEEN_3_TURNS_AGO		 5

#define OLDEST_SEEN_VALUE			SEEN_3_TURNS_AGO
#define OLDEST_HEARD_VALUE	 HEARD_3_TURNS_AGO

#define UNDER_FIRE							2
#define UNDER_FIRE_LAST_TURN		1


#define		MAX_DISTANCE_FOR_PROXIMITY_SIGHT		15		




// DEFINES FOR BODY TYPE SUBSTITUTIONS
#define			SUB_ANIM_BIGGUYSHOOT2							0x00000001
#define			SUB_ANIM_BIGGUYTHREATENSTANCE			0x00000002


// Enumerate directions
enum WorldDirections
{
	NORTH = 0,
	NORTHEAST,
	EAST,
	SOUTHEAST,
	SOUTH,
	SOUTHWEST,
	WEST,
	NORTHWEST,
	NUM_WORLD_DIRECTIONS,
	DIRECTION_IRRELEVANT,
	DIRECTION_EXITGRID = 255,
};


// Starting Sector
// HEADROCK HAM 3.5: Externalized.
//UINT8 startingX = 9;
//UINT8 startingY = 1;
const int startingZ = 0;


// ENUMERATION OF SOLDIER POSIITONS IN GLOBAL SOLDIER LIST
// WDS - make number of mercenaries, etc. be configurable
#define CODE_MAXIMUM_NUMBER_OF_PLAYER_MERCS 32
extern UINT8	giMAXIMUM_NUMBER_OF_PLAYER_MERCS;
#define CODE_MAXIMUM_NUMBER_OF_PLAYER_VEHICLES 6
extern UINT8	giMAXIMUM_NUMBER_OF_PLAYER_VEHICLES;
#define CODE_MAXIMUM_NUMBER_OF_PLAYER_SLOTS CODE_MAXIMUM_NUMBER_OF_PLAYER_MERCS+CODE_MAXIMUM_NUMBER_OF_PLAYER_VEHICLES
extern UINT8	giMAXIMUM_NUMBER_OF_PLAYER_SLOTS;
#define CODE_MAXIMUM_NUMBER_OF_ENEMIES 64
extern UINT8	giMAXIMUM_NUMBER_OF_ENEMIES;
#define CODE_MAXIMUM_NUMBER_OF_CREATURES 40
extern UINT8	giMAXIMUM_NUMBER_OF_CREATURES;
#define CODE_MAXIMUM_NUMBER_OF_REBELS 64
extern UINT8	giMAXIMUM_NUMBER_OF_REBELS;
#define CODE_MAXIMUM_NUMBER_OF_CIVS 40
extern UINT8	giMAXIMUM_NUMBER_OF_CIVS;

// The following should be the largest from the above set of constants.
//   Note: Is there any way to compute this via the preprocessor?
#define LARGEST_NUMBER_IN_ANY_GROUP 64

#define	MAX_NUM_SOLDIERS		(CODE_MAXIMUM_NUMBER_OF_PLAYER_MERCS+CODE_MAXIMUM_NUMBER_OF_PLAYER_VEHICLES+CODE_MAXIMUM_NUMBER_OF_ENEMIES+CODE_MAXIMUM_NUMBER_OF_CREATURES+CODE_MAXIMUM_NUMBER_OF_REBELS+CODE_MAXIMUM_NUMBER_OF_CIVS)
#define	NUM_PLANNING_MERCS	8
#define	TOTAL_SOLDIERS			( NUM_PLANNING_MERCS + MAX_NUM_SOLDIERS )

// If there are more than (one less the size of the field used to store IDs (currently a char)) then fail
#if TOTAL_SOLDIERS > 254
#error Too many!
#endif
#if CODE_MAXIMUM_NUMBER_OF_PLAYER_SLOTS > LARGEST_NUMBER_IN_ANY_GROUP
#error _LARGEST_NUMBER_IN_ANY_GROUP is too small
#endif
#if CODE_MAXIMUM_NUMBER_OF_ENEMIES > LARGEST_NUMBER_IN_ANY_GROUP
#error _LARGEST_NUMBER_IN_ANY_GROUP is too small
#endif
#if CODE_MAXIMUM_NUMBER_OF_CREATURES > LARGEST_NUMBER_IN_ANY_GROUP
#error _LARGEST_NUMBER_IN_ANY_GROUP is too small
#endif
#if CODE_MAXIMUM_NUMBER_OF_REBELS > LARGEST_NUMBER_IN_ANY_GROUP
#error _LARGEST_NUMBER_IN_ANY_GROUP is too small
#endif
#if CODE_MAXIMUM_NUMBER_OF_CIVS > LARGEST_NUMBER_IN_ANY_GROUP
#error _LARGEST_NUMBER_IN_ANY_GROUP is too small
#endif

// DEFINE TEAMS
#define OUR_TEAM				0
#define ENEMY_TEAM			1
#define CREATURE_TEAM		2
#define MILITIA_TEAM		3
#define CIV_TEAM				4
#define LAST_TEAM				LAN_TEAM_FOUR
#define	PLAYER_PLAN			5

#define LAN_TEAM_ONE 6
#define LAN_TEAM_TWO 7
#define LAN_TEAM_THREE 8
#define LAN_TEAM_FOUR 9
//hayden

//-----------------------------------------------
//
// civilian "sub teams":
enum
{
	NON_CIV_GROUP = 0,
	REBEL_CIV_GROUP,
	KINGPIN_CIV_GROUP,
	SANMONA_ARMS_GROUP,
	ANGELS_GROUP,
	BEGGARS_CIV_GROUP,
	TOURISTS_CIV_GROUP,
	ALMA_MILITARY_CIV_GROUP,
	DOCTORS_CIV_GROUP,
	COUPLE1_CIV_GROUP,
	HICKS_CIV_GROUP,
	WARDEN_CIV_GROUP,
	JUNKYARD_CIV_GROUP,
	FACTORY_KIDS_GROUP,
	QUEENS_CIV_GROUP,
	UNNAMED_CIV_GROUP_15,
	UNNAMED_CIV_GROUP_16,
	UNNAMED_CIV_GROUP_17,
	UNNAMED_CIV_GROUP_18,
	UNNAMED_CIV_GROUP_19,
	ASSASSIN_CIV_GROUP,		// Flugente: enemy assassins belong to this group
	POW_PRISON_CIV_GROUP,	// Flugente: prisoners of war the player caught are in this group
	UNNAMED_CIV_GROUP_22,
	UNNAMED_CIV_GROUP_23,
	UNNAMED_CIV_GROUP_24,
	UNNAMED_CIV_GROUP_25,
	UNNAMED_CIV_GROUP_26,
	UNNAMED_CIV_GROUP_27,
	UNNAMED_CIV_GROUP_28,
	UNNAMED_CIV_GROUP_29,
	UNNAMED_CIV_GROUP_30,
	CIA_OPERATIVES_GROUP,		// anv: VR
	TRACONA_OPERATIVES_GROUP,	// anv: VR
	COCKEYE_THUGS,			// anv: VR
	CIA_STANLEY_GROUP,	// anv: VR
	TRACONA_DRAGON_GROUP,	// anv: VR
	UNNAMED_CIV_GROUP_36,
	KINGPIN_FORT_CIV_GROUP,	// anv: VR
	UNNAMED_CIV_GROUP_38,
	UNNAMED_CIV_GROUP_39,
	UNNAMED_CIV_GROUP_40,
	UNNAMED_CIV_GROUP_41,
	UNNAMED_CIV_GROUP_42,
	UNNAMED_CIV_GROUP_43,
	UNNAMED_CIV_GROUP_44,
	UNNAMED_CIV_GROUP_45,
	UNNAMED_CIV_GROUP_46,
	UNNAMED_CIV_GROUP_47,
	UNNAMED_CIV_GROUP_48,
	UNNAMED_CIV_GROUP_49,
	UNNAMED_CIV_GROUP_50,
	UNNAMED_CIV_GROUP_51,
	UNNAMED_CIV_GROUP_52,
	UNNAMED_CIV_GROUP_53,
	UNNAMED_CIV_GROUP_54,
	UNNAMED_CIV_GROUP_55,
	UNNAMED_CIV_GROUP_56,
	UNNAMED_CIV_GROUP_57,
	UNNAMED_CIV_GROUP_58,
	UNNAMED_CIV_GROUP_59,
	UNNAMED_CIV_GROUP_60,	
	UNNAMED_CIV_GROUP_61,
	UNNAMED_CIV_GROUP_62,
	UNNAMED_CIV_GROUP_63,
	UNNAMED_CIV_GROUP_64,
	UNNAMED_CIV_GROUP_65,
	UNNAMED_CIV_GROUP_66,
	UNNAMED_CIV_GROUP_67,
	UNNAMED_CIV_GROUP_68,
	UNNAMED_CIV_GROUP_69,
	UNNAMED_CIV_GROUP_70,
	UNNAMED_CIV_GROUP_71,
	UNNAMED_CIV_GROUP_72,
	UNNAMED_CIV_GROUP_73,
	UNNAMED_CIV_GROUP_74,
	UNNAMED_CIV_GROUP_75,
	UNNAMED_CIV_GROUP_76,
	UNNAMED_CIV_GROUP_77,
	UNNAMED_CIV_GROUP_78,
	UNNAMED_CIV_GROUP_79,
	UNNAMED_CIV_GROUP_80,
	UNNAMED_CIV_GROUP_81,
	UNNAMED_CIV_GROUP_82,
	UNNAMED_CIV_GROUP_83,
	UNNAMED_CIV_GROUP_84,
	UNNAMED_CIV_GROUP_85,
	UNNAMED_CIV_GROUP_86,
	UNNAMED_CIV_GROUP_87,
	UNNAMED_CIV_GROUP_88,
	UNNAMED_CIV_GROUP_89,
	UNNAMED_CIV_GROUP_90,
	UNNAMED_CIV_GROUP_91,
	UNNAMED_CIV_GROUP_92,
	UNNAMED_CIV_GROUP_93,
	UNNAMED_CIV_GROUP_94,
	UNNAMED_CIV_GROUP_95,
	UNNAMED_CIV_GROUP_96,
	UNNAMED_CIV_GROUP_97,
	UNNAMED_CIV_GROUP_98,
	UNNAMED_CIV_GROUP_99,
	UNNAMED_CIV_GROUP_100,
	UNNAMED_CIV_GROUP_101,
	UNNAMED_CIV_GROUP_102,
	UNNAMED_CIV_GROUP_103,
	UNNAMED_CIV_GROUP_104,
	UNNAMED_CIV_GROUP_105,
	UNNAMED_CIV_GROUP_106,
	UNNAMED_CIV_GROUP_107,
	UNNAMED_CIV_GROUP_108,
	UNNAMED_CIV_GROUP_109,
	UNNAMED_CIV_GROUP_110,
	UNNAMED_CIV_GROUP_111,
	UNNAMED_CIV_GROUP_112,
	UNNAMED_CIV_GROUP_113,
	UNNAMED_CIV_GROUP_114,
	UNNAMED_CIV_GROUP_115,
	UNNAMED_CIV_GROUP_116,
	UNNAMED_CIV_GROUP_117,
	UNNAMED_CIV_GROUP_118,
	UNNAMED_CIV_GROUP_119,
	UNNAMED_CIV_GROUP_120,
	UNNAMED_CIV_GROUP_121,
	UNNAMED_CIV_GROUP_122,
	UNNAMED_CIV_GROUP_123,
	UNNAMED_CIV_GROUP_124,
	UNNAMED_CIV_GROUP_125,
	UNNAMED_CIV_GROUP_126,
	UNNAMED_CIV_GROUP_127,
	UNNAMED_CIV_GROUP_128,
	UNNAMED_CIV_GROUP_129,
	UNNAMED_CIV_GROUP_130,
	UNNAMED_CIV_GROUP_131,
	UNNAMED_CIV_GROUP_132,
	UNNAMED_CIV_GROUP_133,
	UNNAMED_CIV_GROUP_134,
	UNNAMED_CIV_GROUP_135,
	UNNAMED_CIV_GROUP_136,
	UNNAMED_CIV_GROUP_137,
	UNNAMED_CIV_GROUP_138,
	UNNAMED_CIV_GROUP_139,
	UNNAMED_CIV_GROUP_140,
	
	UNNAMED_CIV_GROUP_141,
	UNNAMED_CIV_GROUP_142,
	UNNAMED_CIV_GROUP_143,
	UNNAMED_CIV_GROUP_144,
	UNNAMED_CIV_GROUP_145,
	UNNAMED_CIV_GROUP_146,
	UNNAMED_CIV_GROUP_147,
	UNNAMED_CIV_GROUP_148,
	UNNAMED_CIV_GROUP_149,
	UNNAMED_CIV_GROUP_150,
	UNNAMED_CIV_GROUP_151,
	UNNAMED_CIV_GROUP_152,
	UNNAMED_CIV_GROUP_153,
	UNNAMED_CIV_GROUP_154,
	UNNAMED_CIV_GROUP_155,
	UNNAMED_CIV_GROUP_156,
	UNNAMED_CIV_GROUP_157,
	UNNAMED_CIV_GROUP_158,
	UNNAMED_CIV_GROUP_159,
	UNNAMED_CIV_GROUP_160,
	UNNAMED_CIV_GROUP_161,
	UNNAMED_CIV_GROUP_162,
	UNNAMED_CIV_GROUP_163,
	UNNAMED_CIV_GROUP_164,
	UNNAMED_CIV_GROUP_165,
	UNNAMED_CIV_GROUP_166,
	UNNAMED_CIV_GROUP_167,
	UNNAMED_CIV_GROUP_168,
	UNNAMED_CIV_GROUP_169,
	UNNAMED_CIV_GROUP_170,
	
	UNNAMED_CIV_GROUP_171,
	UNNAMED_CIV_GROUP_172,
	UNNAMED_CIV_GROUP_173,
	UNNAMED_CIV_GROUP_174,
	UNNAMED_CIV_GROUP_175,
	UNNAMED_CIV_GROUP_176,
	UNNAMED_CIV_GROUP_177,
	UNNAMED_CIV_GROUP_178,
	UNNAMED_CIV_GROUP_179,
	UNNAMED_CIV_GROUP_180,
	UNNAMED_CIV_GROUP_181,
	UNNAMED_CIV_GROUP_182,
	UNNAMED_CIV_GROUP_183,
	UNNAMED_CIV_GROUP_184,
	UNNAMED_CIV_GROUP_185,
	UNNAMED_CIV_GROUP_186,
	UNNAMED_CIV_GROUP_187,
	UNNAMED_CIV_GROUP_188,
	UNNAMED_CIV_GROUP_189,
	UNNAMED_CIV_GROUP_190,
	UNNAMED_CIV_GROUP_191,
	UNNAMED_CIV_GROUP_192,
	UNNAMED_CIV_GROUP_193,
	UNNAMED_CIV_GROUP_194,
	UNNAMED_CIV_GROUP_195,
	UNNAMED_CIV_GROUP_196,
	UNNAMED_CIV_GROUP_197,
	UNNAMED_CIV_GROUP_198,
	UNNAMED_CIV_GROUP_199,
	UNNAMED_CIV_GROUP_200,
	
	UNNAMED_CIV_GROUP_201,
	UNNAMED_CIV_GROUP_202,
	UNNAMED_CIV_GROUP_203,
	UNNAMED_CIV_GROUP_204,
	UNNAMED_CIV_GROUP_205,
	UNNAMED_CIV_GROUP_206,
	UNNAMED_CIV_GROUP_207,
	UNNAMED_CIV_GROUP_208,
	UNNAMED_CIV_GROUP_209,
	UNNAMED_CIV_GROUP_210,
	UNNAMED_CIV_GROUP_211,
	UNNAMED_CIV_GROUP_212,
	UNNAMED_CIV_GROUP_213,
	UNNAMED_CIV_GROUP_214,
	UNNAMED_CIV_GROUP_215,
	UNNAMED_CIV_GROUP_216,
	UNNAMED_CIV_GROUP_217,
	UNNAMED_CIV_GROUP_218,
	UNNAMED_CIV_GROUP_219,
	UNNAMED_CIV_GROUP_220,
	UNNAMED_CIV_GROUP_221,
	UNNAMED_CIV_GROUP_222,
	UNNAMED_CIV_GROUP_223,
	UNNAMED_CIV_GROUP_224,
	UNNAMED_CIV_GROUP_225,
	UNNAMED_CIV_GROUP_226,
	UNNAMED_CIV_GROUP_227,
	UNNAMED_CIV_GROUP_228,
	UNNAMED_CIV_GROUP_229,
	UNNAMED_CIV_GROUP_230,
	
	UNNAMED_CIV_GROUP_231,
	UNNAMED_CIV_GROUP_232,
	UNNAMED_CIV_GROUP_233,
	UNNAMED_CIV_GROUP_234,
	UNNAMED_CIV_GROUP_235,
	UNNAMED_CIV_GROUP_236,
	UNNAMED_CIV_GROUP_237,
	UNNAMED_CIV_GROUP_238,
	UNNAMED_CIV_GROUP_239,
	UNNAMED_CIV_GROUP_240,
	UNNAMED_CIV_GROUP_241,
	UNNAMED_CIV_GROUP_242,
	UNNAMED_CIV_GROUP_243,
	UNNAMED_CIV_GROUP_244,
	UNNAMED_CIV_GROUP_245,
	UNNAMED_CIV_GROUP_246,
	UNNAMED_CIV_GROUP_247,
	UNNAMED_CIV_GROUP_248,
	UNNAMED_CIV_GROUP_249,
	UNNAMED_CIV_GROUP_250,
	
	UNNAMED_CIV_GROUP_251,
	UNNAMED_CIV_GROUP_252,
	UNNAMED_CIV_GROUP_253,
	UNNAMED_CIV_GROUP_254,
	NUM_CIV_GROUPS
};

#define CIV_GROUP_NEUTRAL 0
#define CIV_GROUP_WILL_EVENTUALLY_BECOME_HOSTILE 1
#define CIV_GROUP_WILL_BECOME_HOSTILE 2
#define CIV_GROUP_HOSTILE 3


// boxing state
enum BoxingStates
{
	NOT_BOXING = 0,
	BOXING_WAITING_FOR_PLAYER,
	PRE_BOXING,
	BOXING,
	DISQUALIFIED,
	WON_ROUND,
	LOST_ROUND
} ;

//NOTE:	The editor uses these enumerations, so please update the text as well if you modify or
//			add new groups.	Try to abbreviate the team name as much as possible.	The text is in 
//			EditorMercs.c
#ifdef JA2EDITOR
	extern CHAR16 gszCivGroupNames[ NUM_CIV_GROUPS ][ 128 ];
#endif
//
//-----------------------------------------------

// PALETTE SUBSITUTION TYPES
typedef struct 
{
	UINT8		ubStart;
	UINT8		ubEnd;

} PaletteSubRangeType;

typedef CHAR8 PaletteRepID[ 30 ];

typedef struct 
{
	UINT8					ubType;
	PaletteRepID	ID;
	UINT8					ubPaletteSize;
	UINT8					*r;
	UINT8					*g;
	UINT8					*b;

} PaletteReplacementType;

// MACROS
// This will set an animation ID
#define			SET_PALETTEREP_ID( a, b )				( strcpy( a, b ) )
																						// strcmp returns 0 if true!		
#define	 COMPARE_PALETTEREP_ID( a, b )		( strcmp( a, b ) ? FALSE : TRUE )

#endif
