#include "builddefines.h"

#ifdef PRECOMPILEDHEADERS
	#include "TileEngine All.h"
#else
	#include "stdio.h"
	#include "sgp.h"
	#include "vobject.h"
	#include "tiledef.h"
	#include "wcheck.h"
	#include "utilities.h"
	#include "worlddef.h"
	#include "isometric utils.h"
	#include "renderworld.h"
	#include "worlddat.h"
	#include "vobject_blitters.h"
	#include "overhead map.h"
	#include "interface.h"
	#include "interface control.h"
	#include "overhead.h"
	#include "radar screen.h"
	#include "cursors.h"
	#include "Sys Globals.h"
	#include "render dirty.h"
	#include "soldier find.h"
	#include "font control.h"
	#include "Game Clock.h"
	#include "interface panels.h"
	#include "english.h"
	#include "line.h"
	#include "map information.h"
	#include "Tactical Placement GUI.h"
	#include "world items.h"
	#include "environment.h"
	#include "message.h"
	#include "faces.h"
	#include "Squads.h"
	#include "Interactive Tiles.h"
	#include "gameloop.h"
	#include "sysutil.h"
	#include "tile surface.h"
	#include <vector>
#endif

#include "connect.h"

// VENGEANCE
#include "GameSettings.h"	
// /VENEGANCE

#ifdef JA2EDITOR
#include "Soldier Init List.h"
extern SOLDIERINITNODE *gpSelected;
#endif


typedef struct
{
	HVOBJECT	vo;
	UINT32		fType;

} SMALL_TILE_SURF;

typedef struct
{
	HVOBJECT	vo;
	UINT16		usSubIndex;
	UINT32		fType;

} SMALL_TILE_DB;


SMALL_TILE_SURF		gSmTileSurf[ NUMBEROFTILETYPES ];
SMALL_TILE_DB			gSmTileDB[ NUMBEROFTILES ];
UINT8							gubSmTileNum	= 0;
BOOLEAN						gfSmTileLoaded = FALSE;
BOOLEAN						gfInOverheadMap = FALSE;
MOUSE_REGION			OverheadRegion;
MOUSE_REGION			OverheadBackgroundRegion;
UINT32						uiOVERMAP;
UINT32						uiPERSONS;
BOOLEAN						gfOverheadMapDirty = FALSE;
extern BOOLEAN		gfRadarCurrentGuyFlash;
INT16							gsStartRestrictedX, gsStartRestrictedY;
BOOLEAN						gfOverItemPool = FALSE;
INT32							gsOveritemPoolGridNo;

UINT16 iOffsetHorizontal;	// Horizontal start postion of the overview map
UINT16 iOffsetVertical;	// Vertical start position of the overview map	

//dnl ch45 021009 Current position of map displayed in overhead map, (A=TopLeft, B=BottomLeft, C=TopRight)
#define MAXSCROLL 4
INT32 giXA = 0, giYA = WORLD_ROWS/2;
INT32 giXB = (0 + OLD_WORLD_COLS/2), giYB = (WORLD_ROWS/2 + OLD_WORLD_ROWS/2);
INT32 giXC = (0 + OLD_WORLD_COLS/2), giYC = (WORLD_ROWS/2 - OLD_WORLD_ROWS/2);

extern BOOLEAN gfValidLocationsChanged;//dnl ch45 051009

void HandleOverheadUI( );
void ClickOverheadRegionCallback(MOUSE_REGION *reg,INT32 reason);
void MoveOverheadRegionCallback(MOUSE_REGION *reg,INT32 reason);
void DeleteOverheadDB( );
BOOLEAN GetOverheadMouseGridNoForFullSoldiersGridNo( INT32 *psGridNo );


extern BOOLEAN AnyItemsVisibleOnLevel( ITEM_POOL *pItemPool, INT8 bZLevel );
extern void HandleAnyMercInSquadHasCompatibleStuff( UINT8 ubSquad, OBJECTTYPE *pObject, BOOLEAN fReset );


//Isometric utilities (for overhead stuff only)
BOOLEAN GetOverheadMouseGridNo( INT32 *psGridNo );
BOOLEAN GetOverheadScreenXYFromGridNo(INT32 sGridNo, INT16* psScreenX, INT16* psScreenY);//dnl ch45 041009
void CopyOverheadDBShadetablesFromTileset( );

void RenderOverheadOverlays();

void InitNewOverheadDB( UINT8 ubTilesetID )
{
	UINT32					uiLoop;
	VOBJECT_DESC	VObjectDesc;
	HVOBJECT		hVObject;
	CHAR8	cFileBPP[128];
	CHAR8	cAdjustedFile[ 128 ];
	UINT32					cnt1, cnt2;
	SMALL_TILE_SURF	s;
	UINT32					NumRegions;
	UINT32					dbSize = 0;


	for (uiLoop = 0; uiLoop < NUMBEROFTILETYPES; uiLoop++)
	{

		// Create video object

		// Adjust for BPP
		FilenameForBPP( gTilesets[ ubTilesetID ].TileSurfaceFilenames[ uiLoop ], cFileBPP);

		// Adjust for tileset position
		sprintf( cAdjustedFile, "TILESETS\\%d\\T\\%s", ubTilesetID, cFileBPP );

		VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
		strcpy( VObjectDesc.ImageFile, cAdjustedFile);
		hVObject = CreateVideoObject( &VObjectDesc );

		if ( hVObject == NULL )
		{
			// TRY loading from default directory
			FilenameForBPP( gTilesets[ TLS_GENERIC_1 ].TileSurfaceFilenames[ uiLoop ], cFileBPP);
			// Adjust for tileset position
			sprintf( cAdjustedFile, "TILESETS\\0\\T\\%s", cFileBPP );

			// LOAD ONE WE KNOW ABOUT!
			VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
			strcpy( VObjectDesc.ImageFile, cAdjustedFile );
			hVObject = CreateVideoObject( &VObjectDesc );

			if ( hVObject == NULL )
			{
				// LOAD ONE WE KNOW ABOUT!
				VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
				strcpy( VObjectDesc.ImageFile, "TILESETS\\0\\T\\grass.sti");
				hVObject = CreateVideoObject( &VObjectDesc );
			}
		}

		gSmTileSurf[ uiLoop ].vo				= hVObject;
		gSmTileSurf[ uiLoop ].fType		=	uiLoop;
	}

	// NOW LOOP THROUGH AND CREATE DATABASE
	for( cnt1 = 0; cnt1 < NUMBEROFTILETYPES; cnt1++ )
	{
		// Get number of regions
		s = gSmTileSurf[ cnt1 ];

		NumRegions = s.vo->usNumberOfObjects;

		// Check for overflow
		if ( NumRegions > gNumTilesPerType[ cnt1 ] )
		{
				// Cutof
				NumRegions = gNumTilesPerType[ cnt1 ];
		}

		for( cnt2 = 0; cnt2 < NumRegions; cnt2++ )
		{

			gSmTileDB[ dbSize ].vo						= s.vo;
			gSmTileDB[ dbSize ].usSubIndex		= (UINT16)cnt2;
			gSmTileDB[ dbSize ].fType					= cnt1;

			dbSize++;
		}

		// Check if data matches what should be there
		if ( NumRegions < gNumTilesPerType[ cnt1 ] )
		{
				// Do underflows here
				for ( cnt2 = NumRegions; cnt2 < gNumTilesPerType[ cnt1 ]; cnt2++ )
				{
					gSmTileDB[ dbSize ].vo						= s.vo;
					gSmTileDB[ dbSize ].usSubIndex		= 0;
					gSmTileDB[ dbSize ].fType					= cnt1;
					dbSize++;
				}

		}
	}

	gsStartRestrictedX = 0;
	gsStartRestrictedY = 0;

	// Calculate Scale factors because of restricted map scroll regions
	if ( gMapInformation.ubRestrictedScrollID != 0 )
	{
		INT16 sX1, sY1, sX2, sY2;

		CalculateRestrictedMapCoords( NORTH, &sX1, &sY1, &sX2, &gsStartRestrictedY, iOffsetHorizontal + 640, iOffsetVertical + 320 );
		CalculateRestrictedMapCoords( WEST,	&sX1, &sY1, &gsStartRestrictedX, &sY2, iOffsetHorizontal + 640, iOffsetVertical + 320 );//dnl ch49 061009
	}

	// Copy over shade tables from main tileset
	CopyOverheadDBShadetablesFromTileset( );
}


void DeleteOverheadDB( )
{
	INT32 cnt;

	for( cnt = 0; cnt < NUMBEROFTILETYPES; cnt++ )
	{
		DeleteVideoObject( gSmTileSurf[ cnt ].vo );
	}

}


BOOLEAN GetClosestItemPool( INT32 sSweetGridNo, ITEM_POOL **ppReturnedItemPool, UINT8 ubRadius, INT8 bLevel )
{
	INT16	sTop, sBottom;
	INT16	sLeft, sRight;
	INT16	cnt1, cnt2;
	INT32 sGridNo;
	INT32		uiRange, uiLowestRange = 999999;
	INT32					leftmost;
	BOOLEAN	fFound = FALSE;
	ITEM_POOL	*pItemPool;

	//create dummy soldier, and use the pathing to determine which nearby slots are
	//reachable.

	sTop		= ubRadius;
	sBottom = -ubRadius;
	sLeft	= - ubRadius;
	sRight	= ubRadius;

	uiLowestRange = 999999;

	for( cnt1 = sBottom; cnt1 <= sTop; cnt1++ )
	{
		leftmost = ( ( sSweetGridNo + ( WORLD_COLS * cnt1 ) )/ WORLD_COLS ) * WORLD_COLS;

		for( cnt2 = sLeft; cnt2 <= sRight; cnt2++ )
		{
			sGridNo = sSweetGridNo + ( WORLD_COLS * cnt1 ) + cnt2;
			if( sGridNo >=0 && sGridNo < WORLD_MAX && sGridNo >= leftmost && sGridNo < ( leftmost + WORLD_COLS ) )
			{
				// Go on sweet stop
				if ( GetItemPool( sGridNo, &pItemPool, bLevel ) )
				{
					uiRange = GetRangeInCellCoordsFromGridNoDiff( sSweetGridNo, sGridNo );

					if ( uiRange < uiLowestRange )
					{
						(*ppReturnedItemPool) = pItemPool;
						uiLowestRange = uiRange;
						fFound = TRUE;
					}
				}
			}
		}
	}

	return( fFound );
}

BOOLEAN GetClosestMercInOverheadMap( INT32 sSweetGridNo, SOLDIERTYPE **ppReturnedSoldier, UINT8 ubRadius )
{
	INT16	sTop, sBottom;
	INT16	sLeft, sRight;
	INT16	cnt1, cnt2;
	INT32 sGridNo;
	INT32		uiRange, uiLowestRange = 999999;
	INT32					leftmost;
	BOOLEAN	fFound = FALSE;

	//create dummy soldier, and use the pathing to determine which nearby slots are
	//reachable.

	sTop		= ubRadius;
	sBottom = -ubRadius;
	sLeft	= - ubRadius;
	sRight	= ubRadius;

	uiLowestRange = 999999;

	for( cnt1 = sBottom; cnt1 <= sTop; cnt1++ )
	{
		leftmost = ( ( sSweetGridNo + ( WORLD_COLS * cnt1 ) )/ WORLD_COLS ) * WORLD_COLS;

		for( cnt2 = sLeft; cnt2 <= sRight; cnt2++ )
		{
			sGridNo = sSweetGridNo + ( WORLD_COLS * cnt1 ) + cnt2;

			if( sGridNo >=0 && sGridNo < WORLD_MAX && sGridNo >= leftmost && sGridNo < ( leftmost + WORLD_COLS ) )
			{
				// Go on sweet stop
		if ( gpWorldLevelData[ sGridNo ].pMercHead != NULL && gpWorldLevelData[ sGridNo ].pMercHead->pSoldier->bVisible != -1 )
				{
					uiRange = GetRangeInCellCoordsFromGridNoDiff( sSweetGridNo, sGridNo );

					if ( uiRange < uiLowestRange )
					{
						(*ppReturnedSoldier) = gpWorldLevelData[ sGridNo ].pMercHead->pSoldier;
						uiLowestRange = uiRange;
						fFound = TRUE;
					}
				}
			}
		}
	}

	return( fFound );
}

//dnl ch45 041009
void DisplayMercNameInOverhead(SOLDIERTYPE* pSoldier)
{
	INT16 sWorldScreenX, sX;
	INT16 sWorldScreenY, sY;

	// Get Screen position of guy.....
	sX = pSoldier->sX;
	sY = pSoldier->sY;

	sX -= ((giXA - 0) * CELL_X_SIZE);
	sY -= ((giYA - WORLD_ROWS/2) * CELL_Y_SIZE);
	GetWorldXYAbsoluteScreenXY((sX/CELL_X_SIZE), (sY/CELL_Y_SIZE), &sWorldScreenX, &sWorldScreenY);
	if(sWorldScreenX < 0 || sWorldScreenX > NORMAL_MAP_SCREEN_WIDTH || sWorldScreenY < 0 || sWorldScreenY > NORMAL_MAP_SCREEN_HEIGHT)
		return;

	sWorldScreenX = gsStartRestrictedX + (sWorldScreenX/5) + 5;
	sWorldScreenY = gsStartRestrictedY + (sWorldScreenY/5) + (pSoldier->sHeightAdjustment/5) + (gpWorldLevelData[pSoldier->sGridNo].sHeight/5) - 8;

	sWorldScreenY += (gsRenderHeight/5);

	// Display name
	SetFont(TINYFONT1);
	SetFontBackground(FONT_MCOLOR_BLACK);
	SetFontForeground(FONT_MCOLOR_WHITE);

	// Center here....
	FindFontCenterCoordinates(sWorldScreenX, sWorldScreenY, (INT16)(1), 1, pSoldier->name, TINYFONT1, &sX, &sY);

	// Full size maps
	if(gsStartRestrictedX == 0)
		sX += iOffsetHorizontal;

	// Full size maps
	if(gsStartRestrictedY == 0)
		sY += iOffsetVertical;

	// OK, selected guy is here...
	gprintfdirty(sX, sY, pSoldier->name);
	mprintf(sX, sY, pSoldier->name);
}

void HandleOverheadMap( )
{
	static BOOLEAN fFirst = TRUE;
	SOLDIERTYPE *pSoldier;

	if ( fFirst )
	{
		fFirst = FALSE;
	}

	gfInOverheadMap = TRUE;
	gfOverItemPool	= FALSE;


	// Check tileset numbers
	if ( gubSmTileNum != giCurrentTilesetID )
	{
		// If loaded, unload!
		if ( gfSmTileLoaded )
		{
			//Unload
			DeleteOverheadDB( );

			// Force load
			gfSmTileLoaded = FALSE;
		}
	}

	gubSmTileNum = (UINT8)giCurrentTilesetID;


	if ( gfSmTileLoaded == FALSE )
	{
		// LOAD LAND
		InitNewOverheadDB( gubSmTileNum );
		gfSmTileLoaded = TRUE;
	}

	// restore background rects
	RestoreBackgroundRects( );

	// Render the overhead map
	RenderOverheadMap(giXA, giYA, iOffsetHorizontal, iOffsetVertical, 640+iOffsetHorizontal, 320+iOffsetVertical, FALSE);//dnl ch45 011009

	HandleTalkingAutoFaces( );

	if( !gfEditMode )
	{
		// CHECK FOR UI
		if( gfTacticalPlacementGUIActive )
		{
			TacticalPlacementHandle();
			if( !gfTacticalPlacementGUIActive )
			{
				return;
			}
		}
		else
		{
			HandleOverheadUI();

			if ( !gfInOverheadMap )
			{
				return;
			}
			RenderTacticalInterface( );
			RenderRadarScreen( );

/* i didnt check when it was rendered so i commented it :P
 * any questions? joker
 */
//			RenderClock( CLOCK_X, CLOCK_Y );
//			RenderTownIDString( );

		HandleAutoFaces( );
		}
	}

	if( !gfEditMode && !gfTacticalPlacementGUIActive )
	{
		INT32 usMapPos;
		ITEM_POOL	*pItemPool;

		gfUIHandleSelectionAboveGuy			= FALSE;

		HandleAnyMercInSquadHasCompatibleStuff( (INT8) CurrentSquad( ), NULL, TRUE );

		if ( GetOverheadMouseGridNo( &usMapPos ) )
		{
			// ATE: Find the closest item pool within 5 tiles....
			if ( GetClosestItemPool( usMapPos, &pItemPool, 1, 0 ) )
			{
				STRUCTURE					*pStructure = NULL;
				INT32 sIntTileGridNo;
				INT8							bZLevel = 0;
				INT32 sActionGridNo = usMapPos;

				// Get interactive tile...
				if ( ConditionalGetCurInteractiveTileGridNoAndStructure( &sIntTileGridNo , &pStructure, FALSE ) )
				{
					sActionGridNo = sIntTileGridNo;
				}

				bZLevel = GetZLevelOfItemPoolGivenStructure( sActionGridNo, 0, pStructure );

				if ( AnyItemsVisibleOnLevel( pItemPool, bZLevel ) )
				{
					DrawItemPoolList( pItemPool, usMapPos	, ITEMLIST_DISPLAY, bZLevel, gusMouseXPos, gusMouseYPos );

					gfOverItemPool = TRUE;
					gsOveritemPoolGridNo = pItemPool->sGridNo;
				}
			}

			if ( GetClosestItemPool( usMapPos, &pItemPool, 1, 1 ) )
			{
				INT8							bZLevel = 0;

				if ( AnyItemsVisibleOnLevel( pItemPool, bZLevel ) )
				{
					DrawItemPoolList( pItemPool, usMapPos	, ITEMLIST_DISPLAY, bZLevel, gusMouseXPos, (UINT16)( gusMouseYPos - 5 ) );

					gfOverItemPool = TRUE;
					gsOveritemPoolGridNo = pItemPool->sGridNo;
				}
			}

	}

    if ( GetOverheadMouseGridNoForFullSoldiersGridNo( &usMapPos ) )
	{
		if ( GetClosestMercInOverheadMap( usMapPos, &pSoldier, 1 ) )
		{
			if ( pSoldier->bTeam == gbPlayerNum )
			{
				gfUIHandleSelectionAboveGuy = TRUE;
				gsSelectedGuy = pSoldier->ubID;
			}
			DisplayMercNameInOverhead( pSoldier );
		}
	}

	}

	// Soldier dummy and items ...
	RenderOverheadOverlays();


	if( !gfEditMode && !gfTacticalPlacementGUIActive && gusSelectedSoldier != NOBODY )
	{
		pSoldier = MercPtrs[ gusSelectedSoldier ];

	DisplayMercNameInOverhead( pSoldier );
	}

	RenderButtons( );
	StartFrameBufferRender( );

	// save background rects
	SaveBackgroundRects( );

	RenderButtonsFastHelp();

	ExecuteBaseDirtyRectQueue( );
	EndFrameBufferRender( );

	fInterfacePanelDirty = FALSE;

}

BOOLEAN InOverheadMap( )
{
	return( gfInOverheadMap );
}

void GoIntoOverheadMap( )
	{
	VOBJECT_DESC	VObjectDesc;
	HVOBJECT				hVObject;

	gfInOverheadMap = TRUE;

	//dnl??? ch45 021009 Add here moving overhead map cords to your current position on big map

	//RestoreExternBackgroundRect( INTERFACE_START_X, INTERFACE_START_Y, SCREEN_WIDTH, INTERFACE_HEIGHT );

	// Overview map should be centered in the middle of the tactical screen.
	iOffsetHorizontal = (SCREEN_WIDTH / 2) - (640 / 2);		// Horizontal start postion of the overview map
	iOffsetVertical = (SCREEN_HEIGHT - 160) / 2 - 160;		// Vertical start position of the overview map

	MSYS_DefineRegion( &OverheadBackgroundRegion, 0, 0 , SCREEN_WIDTH, SCREEN_HEIGHT, MSYS_PRIORITY_HIGH,
						CURSOR_NORMAL, MSYS_NO_CALLBACK, MSYS_NO_CALLBACK );

	//Add region
	MSYS_AddRegion( &OverheadBackgroundRegion );

	MSYS_DefineRegion( &OverheadRegion, 0, 0 , SCREEN_WIDTH, SCREEN_HEIGHT, MSYS_PRIORITY_HIGH,
						CURSOR_NORMAL, MoveOverheadRegionCallback, ClickOverheadRegionCallback );

	// Add region
	MSYS_AddRegion( &OverheadRegion );


	// LOAD CLOSE ANIM
	if (iResolution == 0)
	{
	VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
	FilenameForBPP("INTERFACE\\MAP_BORD.sti", VObjectDesc.ImageFile);
	if( !AddVideoObject( &VObjectDesc, &uiOVERMAP ) )
		AssertMsg(0, "Missing INTERFACE\\MAP_BORD.sti" );
	}
	else if (iResolution == 1)
	{
		VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
		FilenameForBPP("INTERFACE\\MAP_BORD_800x600.sti", VObjectDesc.ImageFile);
		if( !AddVideoObject( &VObjectDesc, &uiOVERMAP ) )
			AssertMsg(0, "Missing INTERFACE\\MAP_BORD_800x600.sti" );
	}
	else if (iResolution == 2)
	{
		VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
		FilenameForBPP("INTERFACE\\MAP_BORD_1024x768.sti", VObjectDesc.ImageFile);
		if( !AddVideoObject( &VObjectDesc, &uiOVERMAP ) )
			AssertMsg(0, "Missing INTERFACE\\MAP_BORD_1024x768.sti" );
	}

	// LOAD PERSONS
	VObjectDesc.fCreateFlags = VOBJECT_CREATE_FROMFILE;
	FilenameForBPP("INTERFACE\\PERSONS.sti", VObjectDesc.ImageFile);
	if( !AddVideoObject( &VObjectDesc, &uiPERSONS ) )
		AssertMsg(0, "Missing INTERFACE\\PERSONS.sti" );

	// Add shades to persons....
	GetVideoObject( &hVObject, uiPERSONS );
	hVObject->pShades[ 0 ]		= Create16BPPPaletteShaded( hVObject->pPaletteEntry, 256, 256, 256, FALSE );
	hVObject->pShades[ 1 ]		= Create16BPPPaletteShaded( hVObject->pPaletteEntry, 310, 310, 310, FALSE );
	hVObject->pShades[ 2 ]		= Create16BPPPaletteShaded( hVObject->pPaletteEntry, 0, 0, 0, FALSE );

	gfOverheadMapDirty = TRUE;

	if( !gfEditMode )
	{
		// Make sure we are in team panel mode...
		gfSwitchPanel = TRUE;
		gbNewPanel = TEAM_PANEL;
		gubNewPanelParam = (UINT8)gusSelectedSoldier;
		fInterfacePanelDirty = DIRTYLEVEL2;

		// Disable tactical buttons......
		if( !gfEnterTacticalPlacementGUI )
		{
			// Handle switch of panel....
			HandleTacticalPanelSwitch( );
			DisableTacticalTeamPanelButtons( TRUE );
		}

	EmptyBackgroundRects( );
	}

}

//dnl ch45 021009
void HandleOverheadUI(void)
{
	INT32 sMousePos = 0;
	InputAtom InputEvent;
	UINT8 ubID;

	// CHECK FOR MOUSE OVER REGIONS...
	if(GetOverheadMouseGridNo(&sMousePos))
	{
		// Look quickly for a soldier....
		ubID = QuickFindSoldier(sMousePos);
		if(ubID != NOBODY)
		{
			// OK, selected guy is here...

			// WANNE: Commented these lines out.
			//gprintfdirty( gusMouseXPos, gusMouseYPos, MercPtrs[ ubID ]->name );
			//mprintf( gusMouseXPos, gusMouseYPos, MercPtrs[ ubID ]->name );
		}
	}

	ScrollOverheadMap();

	while(DequeueEvent(&InputEvent) == TRUE)
	{
		if(InputEvent.usEvent == KEY_DOWN || InputEvent.usEvent == KEY_REPEAT)
		{
			INT32 i = 1;
			switch(InputEvent.usParam)
			{
			case ESC:
			case INSERT:
				KillOverheadMap();
				break;
			case 'x':
				if(InputEvent.usKeyState & ALT_DOWN)
					HandleShortCutExitState();
				break;
			}
		}
	}
}

void ScrollOverheadMap(void)
{
	if(WORLD_MAX == OLD_WORLD_MAX)
		return;
	UINT32 uiFlags = 0;
	INT32 i;
	if(_KeyDown(UPARROW))
		uiFlags |= SCROLL_UP;
	if(_KeyDown(DNARROW))
		uiFlags |= SCROLL_DOWN;
	if(_KeyDown(RIGHTARROW))
		uiFlags |= SCROLL_RIGHT;
	if(_KeyDown(LEFTARROW))
		uiFlags |= SCROLL_LEFT;
	if(uiFlags)
	{
		gfOverheadMapDirty = TRUE;
		gfValidLocationsChanged = TRUE;
		gfTacticalPlacementGUIDirty  = TRUE;
	}
	if(uiFlags & SCROLL_LEFT)// Scroll Left  { Y = X + (3*WORLD_ROWS-WORLD_COLS)/4; --> p3 }
	{
		i = 1;
		if(_KeyDown(SHIFT))
			i = MAXSCROLL;
		while(i--)
		{
			if(giYA == (giXA + (3*WORLD_ROWS-WORLD_COLS)/4))
				break;
			--giXA, ++giYA;
			--giXB, ++giYB;
			--giXC, ++giYC;
		}
	}
	if(uiFlags & SCROLL_RIGHT)// Scroll Right  { Y = X + (WORLD_ROWS-3*WORLD_COLS)/4; --> p3 }
	{
		i = 1;
		if(_KeyDown(SHIFT))
			i = MAXSCROLL;
		while(i--)
		{
			if(giYC == (giXC + (WORLD_ROWS-3*WORLD_COLS)/4))
				break;
			++giXA, --giYA;
			++giXB, --giYB;
			++giXC, --giYC;
		}
	}
	if(uiFlags & SCROLL_UP)// Scroll Up  { Y = -X + (WORLD_ROWS+WORLD_COLS)/4; --> p4 }
	{
		i = 1;
		if(_KeyDown(SHIFT))
			i = MAXSCROLL;
		while(i--)
		{
			if(giYA == (-giXA + (WORLD_ROWS+WORLD_COLS)/4))
				break;
			--giXA, --giYA;
			--giXB, --giYB;
			--giXC, --giYC;
		}
	}
	if(uiFlags & SCROLL_DOWN)// Scroll Down  { Y = -X + 3*(WORLD_ROWS+WORLD_COLS)/4; --> p2 }
	{
		i = 1;
		if(_KeyDown(SHIFT))
			i = MAXSCROLL;
		while(i--)
		{
			if(giYB == (-giXB + 3*(WORLD_ROWS+WORLD_COLS)/4))
				break;
			++giXA, ++giYA;
			++giXB, ++giYB;
			++giXC, ++giYC;
		}
	}
}

void ResetScrollOverheadMap(void)
{
	giXA = 0, giYA = WORLD_ROWS/2;
	giXB = (0 + OLD_WORLD_COLS/2), giYB = (WORLD_ROWS/2 + OLD_WORLD_ROWS/2);
	giXC = (0 + OLD_WORLD_COLS/2), giYC = (WORLD_ROWS/2 - OLD_WORLD_ROWS/2);
}

void KillOverheadMap()
{
	gfInOverheadMap = FALSE;
	SetRenderFlags( RENDER_FLAG_FULL );
	RenderWorld( );

	MSYS_RemoveRegion(&OverheadRegion );
	MSYS_RemoveRegion(&OverheadBackgroundRegion );

	DeleteVideoObjectFromIndex( uiOVERMAP );
	DeleteVideoObjectFromIndex( uiPERSONS );

	HandleTacticalPanelSwitch( );
	DisableTacticalTeamPanelButtons( FALSE );

}


INT16 GetOffsetLandHeight( INT32 sGridNo )
{
	INT16 sTileHeight;

	sTileHeight = gpWorldLevelData[ sGridNo ].sHeight;

	return( sTileHeight );
}

INT16 GetModifiedOffsetLandHeight( INT32 sGridNo )
{
	INT16 sTileHeight;
	INT16 sModifiedTileHeight;

	sTileHeight = gpWorldLevelData[ sGridNo ].sHeight;

	sModifiedTileHeight = ( ( ( sTileHeight / 80 ) - 1 ) * 80 );

	if ( sModifiedTileHeight < 0 )
	{
		sModifiedTileHeight = 0;
	}

	return( sModifiedTileHeight );
}


void RenderOverheadMap( INT16 sStartPointX_M, INT16 sStartPointY_M, INT16 sStartPointX_S, INT16 sStartPointY_S, INT16 sEndXS, INT16 sEndYS, BOOLEAN fFromMapUtility )
{
	INT8				bXOddFlag = 0;
	INT16				sModifiedHeight = 0;
	INT16				sAnchorPosX_M, sAnchorPosY_M;
	INT16				sAnchorPosX_S, sAnchorPosY_S;
	INT16				sTempPosX_M, sTempPosY_M;
	INT16				sTempPosX_S, sTempPosY_S;
	BOOLEAN			fEndRenderRow = FALSE, fEndRenderCol = FALSE;
	INT32			usTileIndex;
	INT16				sX, sY;
	UINT32			uiDestPitchBYTES;
	UINT8				*pDestBuf;
	LEVELNODE		*pNode;
	SMALL_TILE_DB	*pTile;
	INT16				sHeight;
	HVOBJECT hVObject;
	INT16				sX1, sX2, sY1, sY2;

	// Get video object for persons...
	if ( !fFromMapUtility )
	{
		GetVideoObject( &hVObject, uiPERSONS );
	}

	if ( gfOverheadMapDirty )
	{
		//sStartPointX_S += 50;
		//sStartPointY_S += 20;

		// Black color for the background!
		//ColorFillVideoSurfaceArea( FRAME_BUFFER, sStartPointX_S, sStartPointY_S, sEndXS,	sEndYS, 0 );

		if(gfTacticalPlacementGUIActive)//dnl ch45 021009 Skip overwrite buttons area which is not refresh during scroll
			ColorFillVideoSurfaceArea(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-160, 0);
		else
			ColorFillVideoSurfaceArea(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT-120, 0);
		fInterfacePanelDirty = DIRTYLEVEL2;

		InvalidateScreen();
		gfOverheadMapDirty = FALSE;

		// Begin Render Loop
		sAnchorPosX_M = sStartPointX_M;
		sAnchorPosY_M = sStartPointY_M;

		// sStartPointX_S ..... Start point of the overhead map
		sAnchorPosX_S = sStartPointX_S;
		sAnchorPosY_S = sStartPointY_S;

		// Zero out area!
		//ColorFillVideoSurfaceArea( FRAME_BUFFER, 0, 0, (INT16)(640), (INT16)(gsVIEWPORT_WINDOW_END_Y), Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );

		pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );

		// Nur Karte und position der geb�ude
		do
		{
			fEndRenderRow = FALSE;

			sTempPosX_M = sAnchorPosX_M;
			sTempPosY_M = sAnchorPosY_M;
			sTempPosX_S = sAnchorPosX_S;
			sTempPosY_S = sAnchorPosY_S;

			if(bXOddFlag > 0)
				sTempPosX_S += 4;


			do
			{
				// / 5->/ 3

				usTileIndex=FASTMAPROWCOLTOPOS( sTempPosY_M, sTempPosX_M );

				if ( usTileIndex < GRIDSIZE )
				{
					sHeight=( GetOffsetLandHeight(usTileIndex) /5);

					pNode = gpWorldLevelData[ usTileIndex ].pLandStart;
					while( pNode != NULL )
					{

						pTile = &( gSmTileDB[ pNode->usIndex ] );

						sX = sTempPosX_S;
						sY = sTempPosY_S - sHeight + ( gsRenderHeight / 5 );

						pTile->vo->pShadeCurrent= gSmTileSurf[ pTile->fType ].vo->pShades[pNode->ubShadeLevel];

						// RENDER!
						//BltVideoObjectFromIndex(	FRAME_BUFFER, SGR1, gSmallTileDatabase[ gpWorldLevelData[ usTileIndex ].pLandHead->usIndex ], sX, sY, VO_BLT_SRCTRANSPARENCY, NULL );
						//BltVideoObjectFromIndex(	FRAME_BUFFER, SGR1, 0, sX, sY, VO_BLT_SRCTRANSPARENCY, NULL );
						Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, pTile->vo, sX, sY, pTile->usSubIndex );
						pNode = pNode->pPrevNode;
					}

				}

				sTempPosX_S += 8;
				sTempPosX_M ++;
				sTempPosY_M --;

				if ( sTempPosX_S >= sEndXS )
				{
					fEndRenderRow = TRUE;
				}

			} while( !fEndRenderRow );

			if ( bXOddFlag > 0 )
			{
				sAnchorPosY_M ++;
			}
			else
			{
				sAnchorPosX_M ++;
			}


			bXOddFlag = !bXOddFlag;
			sAnchorPosY_S += 2;

			if ( sAnchorPosY_S >= sEndYS )
			{
				fEndRenderCol = TRUE;
			}

		}
		while( !fEndRenderCol );

		// Begin Render Loop
		sAnchorPosX_M = sStartPointX_M;
		sAnchorPosY_M = sStartPointY_M;
		sAnchorPosX_S = sStartPointX_S;
		sAnchorPosY_S = sStartPointY_S;
		bXOddFlag = 0;
		fEndRenderRow = FALSE;
		fEndRenderCol = FALSE;

		do
		{

			fEndRenderRow = FALSE;
			sTempPosX_M = sAnchorPosX_M;
			sTempPosY_M = sAnchorPosY_M;
			sTempPosX_S = sAnchorPosX_S;
			sTempPosY_S = sAnchorPosY_S;

			if(bXOddFlag > 0)
				sTempPosX_S += 4;


			do
			{

				usTileIndex=FASTMAPROWCOLTOPOS( sTempPosY_M, sTempPosX_M );

				if ( usTileIndex < GRIDSIZE )
				{
					sHeight=( GetOffsetLandHeight(usTileIndex) /5);
					sModifiedHeight = ( GetModifiedOffsetLandHeight( usTileIndex ) / 5 );

					pNode = gpWorldLevelData[ usTileIndex ].pObjectHead;
					while( pNode != NULL )
					{

						if ( pNode->usIndex < NUMBEROFTILES )
						{
							// Don't render itempools!
							if ( !( pNode->uiFlags & LEVELNODE_ITEM ) )
							{
								pTile = &( gSmTileDB[ pNode->usIndex ] );

								sX = sTempPosX_S;
								sY = sTempPosY_S;

								if( gTileDatabase[ pNode->usIndex ].uiFlags & IGNORE_WORLD_HEIGHT )
								{
									sY -= sModifiedHeight;
								}
								else
								{
									sY -= sHeight;
								}

								sY += ( gsRenderHeight / 5 );

								pTile->vo->pShadeCurrent= gSmTileSurf[ pTile->fType ].vo->pShades[pNode->ubShadeLevel];

								// RENDER!
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, pTile->vo, sX, sY, pTile->usSubIndex );
							}
						}

						pNode = pNode->pNext;
					}


					pNode = gpWorldLevelData[ usTileIndex ].pShadowHead;
					while( pNode != NULL )
					{
						if ( pNode->usIndex < NUMBEROFTILES )
						{
							pTile = &( gSmTileDB[ pNode->usIndex ] );
							sX = sTempPosX_S;
							sY = sTempPosY_S - sHeight;

							sY += ( gsRenderHeight / 5 );

							pTile->vo->pShadeCurrent= gSmTileSurf[ pTile->fType ].vo->pShades[pNode->ubShadeLevel];

							// RENDER!
							Blt8BPPDataTo16BPPBufferShadow((UINT16*)pDestBuf, uiDestPitchBYTES, pTile->vo, sX, sY, pTile->usSubIndex );
						}

						pNode = pNode->pNext;
					}

					pNode = gpWorldLevelData[ usTileIndex ].pStructHead;

					while( pNode != NULL )
					{
						if ( pNode->usIndex < NUMBEROFTILES )
						{
							// Don't render itempools!
							if ( !( pNode->uiFlags & LEVELNODE_ITEM ) )
							{
								pTile = &( gSmTileDB[ pNode->usIndex ] );

								sX = sTempPosX_S;
								sY = sTempPosY_S - (gTileDatabase[ pNode->usIndex ].sOffsetHeight/5);

								if( gTileDatabase[ pNode->usIndex ].uiFlags & IGNORE_WORLD_HEIGHT )
								{
									sY -= sModifiedHeight;
								}
								else
								{
									sY -= sHeight;
								}

								sY += ( gsRenderHeight / 5 );

								pTile->vo->pShadeCurrent= gSmTileSurf[ pTile->fType ].vo->pShades[pNode->ubShadeLevel];

								// RENDER!
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, pTile->vo, sX, sY, pTile->usSubIndex );
							}
						}

						pNode = pNode->pNext;
					}
				}

				sTempPosX_S += 8;
				sTempPosX_M ++;
				sTempPosY_M --;

				if ( sTempPosX_S >= sEndXS )
				{
					fEndRenderRow = TRUE;
				}

			} while( !fEndRenderRow );

			if ( bXOddFlag > 0 )
			{
				sAnchorPosY_M ++;
			}
			else
			{
				sAnchorPosX_M ++;
			}


			bXOddFlag = !bXOddFlag;
			sAnchorPosY_S += 2;

			if ( sAnchorPosY_S >= sEndYS )
			{
				fEndRenderCol = TRUE;
			}

		}
		while( !fEndRenderCol );

		//if ( !fFromMapUtility && !gfEditMode )
		{

			// ROOF RENDR LOOP
			// Begin Render Loop
			sAnchorPosX_M = sStartPointX_M;
			sAnchorPosY_M = sStartPointY_M;
			sAnchorPosX_S = sStartPointX_S;
			sAnchorPosY_S = sStartPointY_S;
			bXOddFlag = 0;
			fEndRenderRow = FALSE;
			fEndRenderCol = FALSE;

			do
			{

				fEndRenderRow = FALSE;
				sTempPosX_M = sAnchorPosX_M;
				sTempPosY_M = sAnchorPosY_M;
				sTempPosX_S = sAnchorPosX_S;
				sTempPosY_S = sAnchorPosY_S;

				if(bXOddFlag > 0)
					sTempPosX_S += 4;

				do
				{
					usTileIndex=FASTMAPROWCOLTOPOS( sTempPosY_M, sTempPosX_M );

					if ( usTileIndex < GRIDSIZE )
					{
						sHeight=( GetOffsetLandHeight(usTileIndex) /5);

					pNode = gpWorldLevelData[ usTileIndex ].pRoofHead;
					while( pNode != NULL )
					{
						if ( pNode->usIndex < NUMBEROFTILES )
						{
							if ( !( pNode->uiFlags & LEVELNODE_HIDDEN ) )
							{
								pTile = &( gSmTileDB[ pNode->usIndex ] );

								sX = sTempPosX_S;
								sY = sTempPosY_S - (gTileDatabase[ pNode->usIndex ].sOffsetHeight/5) -sHeight;

								sY -= ( WALL_HEIGHT/5 );

								sY += ( gsRenderHeight / 5 );

								pTile->vo->pShadeCurrent= gSmTileSurf[ pTile->fType ].vo->pShades[pNode->ubShadeLevel];

								// RENDER!
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, pTile->vo, sX, sY, pTile->usSubIndex );
							}
						}
						pNode = pNode->pNext;
					}
					}

					sTempPosX_S += 8;
					sTempPosX_M ++;
					sTempPosY_M --;

					if ( sTempPosX_S >= sEndXS )
					{
						fEndRenderRow = TRUE;
					}

				} while( !fEndRenderRow );

				if ( bXOddFlag > 0 )
				{
					sAnchorPosY_M ++;
				}
				else
				{
					sAnchorPosX_M ++;
				}


				bXOddFlag = !bXOddFlag;
				sAnchorPosY_S += 2;

				if ( sAnchorPosY_S >= sEndYS )
				{
					fEndRenderCol = TRUE;
				}

			}
			while( !fEndRenderCol );
		}

		UnLockVideoSurface( FRAME_BUFFER );

		// OK, blacken out edges of smaller maps...
		if ( gMapInformation.ubRestrictedScrollID != 0 )
		{
			CalculateRestrictedMapCoords( NORTH, &sX1, &sY1, &sX2, &sY2, sEndXS, sEndYS );
			ColorFillVideoSurfaceArea( FRAME_BUFFER, sX1, sY1, sX2, sY2, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );

			CalculateRestrictedMapCoords( EAST, &sX1, &sY1, &sX2, &sY2, sEndXS, sEndYS );
			ColorFillVideoSurfaceArea( FRAME_BUFFER, sX1, sY1, sX2, sY2, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );

			CalculateRestrictedMapCoords( SOUTH, &sX1, &sY1, &sX2, &sY2, sEndXS, sEndYS );
			ColorFillVideoSurfaceArea( FRAME_BUFFER, sX1, sY1, sX2, sY2, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );

			CalculateRestrictedMapCoords( WEST, &sX1, &sY1, &sX2, &sY2, sEndXS, sEndYS );
			ColorFillVideoSurfaceArea( FRAME_BUFFER, sX1, sY1, sX2, sY2, Get16BPPColor( FROMRGB( 0, 0, 0 ) ) );
		}

		if(!fFromMapUtility)
			BltVideoObjectFromIndex(FRAME_BUFFER, uiOVERMAP, 0, 0, 0, VO_BLT_SRCTRANSPARENCY, NULL);// Render border!

		// Update the save buffer
		UINT32 uiDestPitchBYTES, uiSrcPitchBYTES;
		UINT8	*pDestBuf, *pSrcBuf;
		UINT16 usWidth, usHeight;
		UINT8	ubBitDepth;
		// Update saved buffer - do for the viewport size ony!
		GetCurrentVideoSettings( &usWidth, &usHeight, &ubBitDepth );
		pSrcBuf = LockVideoSurface(guiRENDERBUFFER, &uiSrcPitchBYTES);
		pDestBuf = LockVideoSurface(guiSAVEBUFFER, &uiDestPitchBYTES);
		if(gbPixelDepth == 16)// BLIT HERE
			Blt16BPPTo16BPP((UINT16 *)pDestBuf, uiDestPitchBYTES, (UINT16 *)pSrcBuf, uiSrcPitchBYTES, 0, 0, 0, 0, usWidth, usHeight );
		UnLockVideoSurface(guiRENDERBUFFER);
		UnLockVideoSurface(guiSAVEBUFFER);
	}
}

void RenderOverheadOverlays()
{
	UINT32			uiDestPitchBYTES;
	WORLDITEM		*pWorldItem;
	UINT32				i;
	SOLDIERTYPE	*pSoldier;
	HVOBJECT		hVObject;
	INT16				sX, sY;
	UINT16			end;
	UINT16			usLineColor=0;
	UINT8				*pDestBuf;
	UINT8				ubPassengers = 0;

	pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
	GetVideoObject( &hVObject, uiPERSONS );


	//SOLDIER OVERLAY
	if( gfTacticalPlacementGUIActive )
	{ //loop through only the player soldiers
		end = gTacticalStatus.Team[ OUR_TEAM ].bLastID;
	}
	else
	{ //loop through all soldiers.
		end = MAX_NUM_SOLDIERS;
	}
	if(is_networked)end = MAX_NUM_SOLDIERS;

	for( i = 0; i < end; i++ )
	{
		//First, check to see if the soldier exists and is in the sector.
		pSoldier = MercPtrs[ i ];
		if( !pSoldier->bActive || !pSoldier->bInSector )
			continue;
		//Soldier is here.	Calculate his screen position based on his current gridno.

		if(!GetOverheadScreenXYFromGridNo(pSoldier->sGridNo, &sX, &sY))//dnl ch45 041009
			continue;

		//Now, draw his "doll"

		//adjust for position.
		sX += 2;
		sY -= 5;
		//sScreenY -= 7;	//height of doll

		if( !gfTacticalPlacementGUIActive && pSoldier->bLastRenderVisibleValue == -1 && !(gTacticalStatus.uiFlags&SHOW_ALL_MERCS) )
		{
					//hayden
					if(is_networked && pSoldier->bSide==0)
					{
					}
					else
					{
					continue;// ie dont render
					}
		}
		
		if (TileIsOutOfBounds(pSoldier->sGridNo))
		{
			continue;
		}

		sY -= ( GetOffsetLandHeight( pSoldier->sGridNo ) /5);

		// Adjust for height...
		sY -= ( pSoldier->sHeightAdjustment / 5 );

		sY += ( gsRenderHeight / 5 );

		// Adjust shade a bit...
		SetObjectShade( hVObject, 0 );

		// If on roof....
		if ( pSoldier->sHeightAdjustment )
		{
			SetObjectShade( hVObject, 1 );
		}

		if ( pSoldier->ubID == gusSelectedSoldier )
		{
			if( gfRadarCurrentGuyFlash && !gfTacticalPlacementGUIActive )
			{
				SetObjectShade( hVObject, 2 );
			}
		}

		// Smaller maps
		if (gsStartRestrictedX > 0)
		{
			sX += gsStartRestrictedX;
		}
		// Full size maps
		else
		{
			sX += iOffsetHorizontal;
		}

		// Smaller maps
		if (gsStartRestrictedY > 0)
		{
			sY += gsStartRestrictedY;
		}
		// Full size maps
		else
		{
			sY += iOffsetVertical;
		}

		#ifdef JA2EDITOR
		if( gfEditMode && gpSelected && gpSelected->pSoldier == pSoldier )
		{ //editor:	show the selected edited merc as the yellow one.
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 0 );
		}
		else
		#endif
		if( !gfTacticalPlacementGUIActive )
		{ //normal
			if(is_networked)
			{
				if(pSoldier->bTeam!=0)
				{
					if(pSoldier->bSide==1)
					{
						// Civ (white)
						if (pSoldier->bTeam == 4)
							Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 4 );
						// Enemy (red)
						else
							Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 1 );
					}

					// Other clients
					if(pSoldier->bSide==0)
					{
						int personIndex = pSoldier->bTeam + 4;
						Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, personIndex );
					}
				}
				else 
					// Color depends on the bTeam
					Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, pSoldier->bTeam );
			}
			else
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, pSoldier->bTeam );
			RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, sX, sY, (INT16)(sX + 3), (INT16)(sY + 9));
		}
		else if( pSoldier->flags.uiStatusFlags & SOLDIER_VEHICLE )
		{ //vehicle
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 9 );
			RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, (INT16)(sX-6), (INT16)(sY), (INT16)(sX + 9), (INT16)(sY + 10));
		}
		//else if( pSoldier->flags.uiStatusFlags & (SOLDIER_PASSENGER | SOLDIER_DRIVER) )
		//{// //don't draw person, because they are inside the vehicle.
		//	ubPassengers++;
		//}
		else if( gpTacticalPlacementSelectedSoldier == pSoldier )
		{ //tactical placement selected merc
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 7 );
			RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, (INT16)(sX-2), (INT16)(sY-2), (INT16)(sX + 5), (INT16)(sY + 11));
		}
		else if( gpTacticalPlacementHilightedSoldier == pSoldier && pSoldier->flags.uiStatusFlags )
		{ //tactical placement hilighted merc
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 8 );
			RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, (INT16)(sX-2), (INT16)(sY-2), (INT16)(sX + 5), (INT16)(sY + 11));
		}
		else
		{ //normal
			if(is_networked)
			{
				if(pSoldier->bTeam!=0)
				{
					if(pSoldier->bSide==1)
						continue;//dont render enemy

					// Green
					// Other clients
					if(pSoldier->bSide==0)
					{
						int personIndex = pSoldier->bTeam + 4;
						Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, personIndex );
					}
				}
				else 
					// Color depends on the bTeam
					Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, pSoldier->bTeam );
			}
			else
			Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, pSoldier->bTeam );
			RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, sX, sY, (INT16)(sX + 3), (INT16)(sY + 9));
		}
		if( ubPassengers )
		{
			SetFont( SMALLCOMPFONT );
			SetFontForeground( FONT_WHITE );
			gprintfdirty( (INT16)(sX - 3), sY, L"%d", ubPassengers );
			mprintf_buffer( pDestBuf, uiDestPitchBYTES, SMALLCOMPFONT, sX - 3,	sY , L"%d", ubPassengers );
		}
	}


	//ITEMS OVERLAY
	if( !gfTacticalPlacementGUIActive )
	{
		for( i = 0 ; i < guiNumWorldItems; i++	)
		{
			pWorldItem = &gWorldItems[ i ];
			if( !pWorldItem || !pWorldItem->fExists || pWorldItem->bVisible != VISIBLE && !(gTacticalStatus.uiFlags & SHOW_ALL_ITEMS) )
			{
				continue;
			}

			if(!GetOverheadScreenXYFromGridNo(pWorldItem->sGridNo, &sX, &sY))//dnl ch45 041009
				continue;

			//adjust for position.
			//sX += 2;
			sY += 6;
			sY -= ( GetOffsetLandHeight( pWorldItem->sGridNo ) /5);

			sY += ( gsRenderHeight / 5 );

			// Smaller maps
			if (gsStartRestrictedX > 0)
			{
				sX += gsStartRestrictedX;
			}
			// Full size maps
			else
			{
				sX += iOffsetHorizontal;
			}

			// Smaller maps
			if (gsStartRestrictedY > 0)
			{
				sY += gsStartRestrictedY;
			}
			// Full size maps
			else
			{
				sY += iOffsetVertical;
			}

			/*sX += iOffsetHorizontal + gsStartRestrictedX;
			sY += iOffsetVertical + gsStartRestrictedY;*/

			if ( gfRadarCurrentGuyFlash )
			{
				usLineColor = Get16BPPColor( FROMRGB( 0, 0, 0 ) );
			}
			else switch( pWorldItem->bVisible )
			{
				case HIDDEN_ITEM:				usLineColor = Get16BPPColor( FROMRGB(	0,	0, 255 ) );	break;
				case BURIED:						usLineColor = Get16BPPColor( FROMRGB( 255,	0,	0 ) );	break;
				case HIDDEN_IN_OBJECT:	usLineColor = Get16BPPColor( FROMRGB(	0,	0, 255 ) );	break;
				case INVISIBLE:					usLineColor = Get16BPPColor( FROMRGB(	0, 255,	0 ) );	break;
				case VISIBLE:						usLineColor = Get16BPPColor( FROMRGB( 255, 255, 255 ) );	break;
			}

			if ( gfOverItemPool && gsOveritemPoolGridNo == pWorldItem->sGridNo )
			{
				usLineColor = Get16BPPColor( FROMRGB( 255,	0,	0 ) );
			}

			PixelDraw( FALSE, sX, sY, usLineColor, pDestBuf );

			InvalidateRegion( sX, sY, (INT16)( sX + 1 ), (INT16)( sY + 1 ) );

		}
	}

	UnLockVideoSurface( FRAME_BUFFER );
}

/*//Render the soldiers and items on top of the pristine overhead map.
void RenderOverheadOverlays( INT16 sStartPointX_M, INT16 sStartPointY_M, INT16 sStartPointX_S, INT16 sStartPointY_S, INT16 sEndXS, INT16 sEndYS )
{
	INT8				bXOddFlag = 0;
	INT16				sAnchorPosX_M, sAnchorPosY_M;
	INT16				sAnchorPosX_S, sAnchorPosY_S;
	INT16				sTempPosX_M, sTempPosY_M;
	INT16				sTempPosX_S, sTempPosY_S;
	BOOLEAN			fEndRenderRow = FALSE, fEndRenderCol = FALSE;
	UINT32			usTileIndex;
	INT16				sX, sY;
	UINT32			uiDestPitchBYTES;
	UINT8				*pDestBuf;
	LEVELNODE		*pNode;
	UINT16			usLineColor;
	INT16				sHeight;
	SOLDIERTYPE	*pSoldier;
	HVOBJECT hVObject;
	pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
	// Begin Render Loop
	sAnchorPosX_M = sStartPointX_M;
	sAnchorPosY_M = sStartPointY_M;
	sAnchorPosX_S = sStartPointX_S;
	sAnchorPosY_S = sStartPointY_S;
	bXOddFlag = 0;
	fEndRenderRow = FALSE;
	fEndRenderCol = FALSE;

	GetVideoObject( &hVObject, uiPERSONS );
	do
	{

		fEndRenderRow = FALSE;
		sTempPosX_M = sAnchorPosX_M;
		sTempPosY_M = sAnchorPosY_M;
		sTempPosX_S = sAnchorPosX_S;
		sTempPosY_S = sAnchorPosY_S;

		if(bXOddFlag > 0)
			sTempPosX_S += 4;


		do
		{

			usTileIndex=FASTMAPROWCOLTOPOS( sTempPosY_M, sTempPosX_M );

			if ( usTileIndex < GRIDSIZE )
			{
				sHeight=(gpWorldLevelData[usTileIndex].sHeight/5);

				pNode = gpWorldLevelData[ usTileIndex ].pStructHead;
				while( pNode != NULL )
				{
					// Render itempools!
					if ( ( pNode->uiFlags & LEVELNODE_ITEM ) )
					{
						sX = sTempPosX_S;
						sY = sTempPosY_S - sHeight;
						// RENDER!
						if ( pNode->pItemPool->bVisible == -1 && !(gTacticalStatus.uiFlags & SHOW_ALL_ITEMS)	)
						{

						}
						else
						{
							if ( gfRadarCurrentGuyFlash )
							{
								usLineColor = Get16BPPColor( FROMRGB( 0, 0, 0 ) );
							}
							else
							{
								usLineColor = Get16BPPColor( FROMRGB( 255, 255, 255 ) );
							}
							RectangleDraw( TRUE, sX, sY, sX + 1, sY + 1, usLineColor, pDestBuf );

							InvalidateRegion( sX, sY, (INT16)( sX + 2 ), (INT16)( sY + 2 ) );

						}
						break;
					}

					pNode = pNode->pNext;
				}


				pNode = gpWorldLevelData[ usTileIndex ].pMercHead;
				while( pNode != NULL )
				{
						pSoldier = pNode->pSoldier;

						sX = sTempPosX_S;
						sY = sTempPosY_S - sHeight - 8; // 8 height of doll guy

						// RENDER!
						if ( pSoldier->bLastRenderVisibleValue == -1 && !(gTacticalStatus.uiFlags&SHOW_ALL_MERCS)	)
						{

						}
						else
						{
							// Adjust for height...
							sY -= ( pSoldier->sHeightAdjustment / 5 );

							// Adjust shade a bit...
							SetObjectShade( hVObject, 0 );

							// If on roof....
							if ( pSoldier->sHeightAdjustment )
							{
								SetObjectShade( hVObject, 1 );
							}

							if ( pSoldier->ubID == gusSelectedSoldier )
							{
								if( gfRadarCurrentGuyFlash && !gfTacticalPlacementGUIActive )
								{
									SetObjectShade( hVObject, 2 );
								}
							}
							#ifdef JA2EDITOR
							if( gfEditMode && gpSelected && gpSelected->pSoldier == pSoldier )
							{ //editor:	show the selected edited merc as the yellow one.
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 0 );
							}
							else
							#endif
							if( gfTacticalPlacementGUIActive && gpTacticalPlacementSelectedSoldier == pSoldier )
							{ //tactical placement selected merc
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 7 );
							}
							else if( gfTacticalPlacementGUIActive && gpTacticalPlacementHilightedSoldier == pSoldier )
							{ //tactical placement selected merc
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, 8 );
							}
							else
							{ //normal
								Blt8BPPDataTo16BPPBufferTransparent((UINT16*)pDestBuf, uiDestPitchBYTES, hVObject, sX, sY, pSoldier->bTeam );
							}
							RegisterBackgroundRect(BGND_FLAG_SINGLE, NULL, (INT16)(sX-2), (INT16)(sY-2), (INT16)(sX + 5), (INT16)(sY + 11));
						}

						pNode = pNode->pNext;
				}
			}

			sTempPosX_S += 8;
			sTempPosX_M ++;
			sTempPosY_M --;

			if ( sTempPosX_S >= sEndXS )
			{
				fEndRenderRow = TRUE;
			}

		} while( !fEndRenderRow );

		if ( bXOddFlag > 0 )
		{
			sAnchorPosY_M ++;
		}
		else
		{
			sAnchorPosX_M ++;
		}


		bXOddFlag = !bXOddFlag;
		sAnchorPosY_S += 2;

		if ( sAnchorPosY_S >= sEndYS )
		{
			fEndRenderCol = TRUE;
		}

	}
	while( !fEndRenderCol );
	UnLockVideoSurface( FRAME_BUFFER );
}
*/

void MoveInOverheadRegionCallback(MOUSE_REGION *reg, INT32 reason)
{
	//Calculate the cursor...
}

void ClickOverheadRegionCallback(MOUSE_REGION *reg,INT32 reason)
{
	UINT32 uiCellX, uiCellY;
	INT16	sWorldScreenX, sWorldScreenY;

	if( gfTacticalPlacementGUIActive )
	{
		HandleTacticalPlacementClicksInOverheadMap( reg, reason );
		return;
	}

	if (!(reg->uiFlags & BUTTON_ENABLED))
		return;

	if(reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		reg->uiFlags |= BUTTON_CLICKED_ON;
	}
	else if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		reg->uiFlags &= (~BUTTON_CLICKED_ON );

		// Smaller maps
		if (gsStartRestrictedX > 0)
		{
		sWorldScreenX = ( gusMouseXPos - gsStartRestrictedX ) * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenX = (gusMouseXPos - iOffsetHorizontal) * 5;
		}

		// Smaller maps
		if (gsStartRestrictedY > 0)
		{
		sWorldScreenY = ( gusMouseYPos - gsStartRestrictedY ) * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenY = (gusMouseYPos - iOffsetVertical) * 5;
		}

		// Get new proposed center location.
		GetFromAbsoluteScreenXYWorldXY( (INT32 *)&uiCellX, (INT32 *)&uiCellY, sWorldScreenX, sWorldScreenY );

		//dnl ch45 021009
		uiCellX += ((giXA - 0) * CELL_X_SIZE);
		uiCellY += ((giYA - WORLD_ROWS/2) * CELL_Y_SIZE);

		SetRenderCenter( (INT16)uiCellX, (INT16)uiCellY );

		KillOverheadMap();

	}
	else if(reason & MSYS_CALLBACK_REASON_RBUTTON_DWN)
	{
		KillOverheadMap();
	}
}


void MoveOverheadRegionCallback(MOUSE_REGION *reg,INT32 reason)
{
	;
}


//dnl ch45 041009
BOOLEAN GetOverheadScreenXYFromGridNo(INT32 sGridNo, INT16* psScreenX, INT16* psScreenY)
{
	INT16 sWorldScreenX, sX;
	INT16 sWorldScreenY, sY;

	ConvertGridNoToXY(sGridNo, &sX, &sY);
	sX *= CELL_X_SIZE;
	sY *= CELL_Y_SIZE;

	sX -= ((giXA - 0) * CELL_X_SIZE);
	sY -= ((giYA - WORLD_ROWS/2) * CELL_Y_SIZE);
	GetWorldXYAbsoluteScreenXY((sX/CELL_X_SIZE), (sY/CELL_Y_SIZE), &sWorldScreenX, &sWorldScreenY);
	if(sWorldScreenX < 0 || sWorldScreenX > NORMAL_MAP_SCREEN_WIDTH || sWorldScreenY < 0 || sWorldScreenY > NORMAL_MAP_SCREEN_HEIGHT)
		return(FALSE);

	*psScreenX = sWorldScreenX;
	*psScreenY = sWorldScreenY;

	*psScreenX /= 5;
	*psScreenY /= 5;
	*psScreenX += 5;
	*psScreenY += 5;

	//Subtract the height....
	//*psScreenY -= gpWorldLevelData[sGridNo].sHeight / 5;

	return(TRUE);
}

// WANNE: Fixed bug from sir tech, which occured on smaller maps ;-)
BOOLEAN GetOverheadMouseGridNo( INT32 *psGridNo )
{
	INT32 uiCellX, uiCellY;
	INT16	sWorldScreenX, sWorldScreenY;

	if ( ( OverheadRegion.uiFlags & MSYS_MOUSE_IN_AREA ) )
	{
		// ATE: Adjust alogrithm values a tad to reflect map positioning
		// Smaller maps
		if (gsStartRestrictedX > 0)
		{
			sWorldScreenX = (gusMouseXPos - 5) * 5 - gsStartRestrictedX * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenX = (gusMouseXPos - 5) * 5 - iOffsetHorizontal * 5;
		}

		// Smaller maps
		if (gsStartRestrictedY > 0)
		{
			sWorldScreenY = (gusMouseYPos - 8) * 5 - gsStartRestrictedY * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenY = (gusMouseYPos - 8) * 5 - iOffsetVertical * 5;
		}

		// Get new proposed center location.
		GetFromAbsoluteScreenXYWorldXY( (INT32 *)&uiCellX, (INT32 *)&uiCellY, sWorldScreenX, sWorldScreenY );

		// Get gridNo
		(*psGridNo ) = MAPROWCOLTOPOS( ( uiCellY / CELL_Y_SIZE ), ( uiCellX / CELL_X_SIZE ) );

		// Adjust for height.....
		sWorldScreenY = sWorldScreenY + gpWorldLevelData[ (*psGridNo) ].sHeight;

		GetFromAbsoluteScreenXYWorldXY( (INT32 *)&uiCellX, (INT32 *)&uiCellY, sWorldScreenX, sWorldScreenY );

		//dnl ch45 021009
		uiCellX += ((giXA - 0) * CELL_X_SIZE);
		uiCellY += ((giYA - WORLD_ROWS/2) * CELL_Y_SIZE);

		// Get gridNo
		(*psGridNo ) = MAPROWCOLTOPOS( ( uiCellY / CELL_Y_SIZE ), ( uiCellX / CELL_X_SIZE ) );

		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}


// WANNE: Fixed bug from sir tech which occured on smaller maps ;-)
BOOLEAN GetOverheadMouseGridNoForFullSoldiersGridNo( INT32 *psGridNo )
{
	INT32 uiCellX, uiCellY;
	INT16	sWorldScreenX, sWorldScreenY;

	if ( ( OverheadRegion.uiFlags & MSYS_MOUSE_IN_AREA ) )
	{
		// ATE: Adjust alogrithm values a tad to reflect map positioning
		// Smaller maps
		if (gsStartRestrictedX > 0)
		{
			sWorldScreenX = (gusMouseXPos - 5) * 5 - gsStartRestrictedX * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenX = ( gusMouseXPos - 5 ) * 5 - iOffsetHorizontal * 5;
		}

		// Smaller maps
		if (gsStartRestrictedY > 0)
		{
			sWorldScreenY = ( gusMouseYPos ) * 5 - gsStartRestrictedY * 5;
		}
		// Full size maps
		else
		{
			sWorldScreenY = ( gusMouseYPos ) * 5 - iOffsetVertical * 5;
		}

		// Get new proposed center location.
		GetFromAbsoluteScreenXYWorldXY( (INT32 *)&uiCellX, (INT32 *)&uiCellY, sWorldScreenX, sWorldScreenY );

		// Get gridNo
		(*psGridNo ) = MAPROWCOLTOPOS( ( uiCellY / CELL_Y_SIZE ), ( uiCellX / CELL_X_SIZE ) );

		// Adjust for height.....
		sWorldScreenY = sWorldScreenY + gpWorldLevelData[ (*psGridNo) ].sHeight;

		GetFromAbsoluteScreenXYWorldXY( (INT32 *)&uiCellX, (INT32 *)&uiCellY, sWorldScreenX, sWorldScreenY );

		//dnl ch45 021009
		uiCellX += ((giXA - 0) * CELL_X_SIZE);
		uiCellY += ((giYA - WORLD_ROWS/2) * CELL_Y_SIZE);

		// Get gridNo
		(*psGridNo ) = MAPROWCOLTOPOS( ( uiCellY / CELL_Y_SIZE ), ( uiCellX / CELL_X_SIZE ) );

		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}


// This method is used for smaller overhead maps, to calculate the non visible borders to make them black
// It is also used to get the starting (x and y coordinate)
void CalculateRestrictedMapCoords(INT8 bDirection, INT16 *psX1, INT16 *psY1, INT16 *psX2, INT16 *psY2, INT16 sEndXS, INT16 sEndYS)//dnl ch49 061009
{
	switch(bDirection)
	{
	case NORTH:
		*psX1 = iOffsetHorizontal;
		*psX2 = sEndXS;
		*psY1 = iOffsetVertical;
		*psY2 = (abs(NORMAL_MAP_SCREEN_TY - gsTLY) / 5) + iOffsetVertical;
		break;
	case WEST:
		*psX1 = iOffsetHorizontal;
		*psX2 = (abs(-NORMAL_MAP_SCREEN_X - gsTLX) / 5) + iOffsetHorizontal;
		*psY1 = iOffsetVertical;
		*psY2 = sEndYS;
		break;
	case SOUTH:
		*psX1 = iOffsetHorizontal;
		*psX2 = sEndXS;
		*psY1 = ((NORMAL_MAP_SCREEN_HEIGHT - abs(NORMAL_MAP_SCREEN_BY - gsBLY)) / 5) + iOffsetVertical;
		*psY2 = sEndYS;
		break;
	case EAST:
		*psX1 = ((NORMAL_MAP_SCREEN_WIDTH - abs(NORMAL_MAP_SCREEN_X - gsTRX)) / 5) + iOffsetHorizontal;
		*psX2 = sEndXS;
		*psY1 = iOffsetVertical;
		*psY2 = sEndYS;
		break;
	}
}

void CopyOverheadDBShadetablesFromTileset( )
{
	UINT32					uiLoop, uiLoop2;
	PTILE_IMAGERY	pTileSurf;


	// Loop through tileset
	for (uiLoop = 0; uiLoop < NUMBEROFTILETYPES; uiLoop++)
	{
		pTileSurf = ( gTileSurfaceArray[ uiLoop ] );

		gSmTileSurf[ uiLoop ].vo->fFlags |= VOBJECT_FLAG_SHADETABLE_SHARED;

		for (uiLoop2 = 0; uiLoop2 < HVOBJECT_SHADE_TABLES; uiLoop2++)
		{
			gSmTileSurf[ uiLoop ].vo->pShades[ uiLoop2 ] = pTileSurf->vo->pShades[ uiLoop2 ];
			// VENGEANCE
			if( gGameExternalOptions.fMonochromaticOverheadMap	== TRUE )
			{
				gSmTileSurf[ uiLoop ].vo->pShades[ uiLoop2 ] = Create16BPPPaletteShaded( gSmTileSurf[ uiLoop ].vo->pPaletteEntry, 352, 352, 352, TRUE );
			}
			// /VENGEANCE
		}
	}

}

void TrashOverheadMap( )
{
	// If loaded, unload!
	if ( gfSmTileLoaded )
	{
		//Unload
		DeleteOverheadDB( );

		// Force load
		gfSmTileLoaded = FALSE;
	}
}
