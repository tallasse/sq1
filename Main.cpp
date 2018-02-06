/// /// \file main.cpp 
/// Main file for for Demo 9.
/// Copyright Ian Parberry, 2004.
/// Last updated October 22, 2004.

/// \mainpage Demo 9: The Joystick
/// CInputManager now manages joystick input using DirectInput. There's also a device
/// menu with radio buttons to let you pick the input device, accessible from the
/// main menu using the "Devices" button.

/// Last Updated 11/4/04 by Jeremy Smith

//system includes
#include <windows.h>
#include <windowsx.h>
#include <d3dx9.h>
#include <string>

#include "defines.h"
#include "object.h"
#include "abort.h"
#include "tinyxml.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "timer.h"
#include "sprite.h"
#include "object.h"
#include "spriteman.h"
#include "objman.h"
#include "random.h"
#include "sound.h"
#include "input.h"

/**************************************************************/

#include "LayerManager2d.h"
#include "d3dutil.h"
#include "d3dfont.h"
#include "dxutil.h"

/**************************************************************/





using namespace std;

//globals
BOOL g_bActiveApp;  ///< TRUE if this is the active application
HWND g_HwndApp; ///< Application window handle.
CImageFileNameList g_cImageFileName; ///< List of image file names.
BOOL g_bCameraDefaultMode=TRUE; ///< FALSE for eagle-eye camera mode.

//graphics settings
int g_nScreenWidth=1024; ///< Screen width.
int g_nScreenHeight=768; ///< Screen height.
BOOL g_bWindowed=FALSE; ///< TRUE to run in a window, FALSE for fullscreen.

//abort variables
extern char g_errMsg[]; ///< Error message if we had to abort.
extern BOOL g_errorExit; ///< True if we had to abort.

//XML settings
const char* g_xmlFileName="samurai.xml"; ///< Settings file name
TiXmlDocument g_xmlDocument(g_xmlFileName); ///< TinyXML document for settings.
TiXmlElement* g_xmlSettings = NULL; ///< TinyXML element for settings tag

//debug variables
#ifdef DEBUG_ON
  CDebugManager g_cDebugManager; //< The debug manager.
#endif //DEBUG_ON

//d3d9 variables
IDirect3D9* g_d3d=NULL; ///< D3D object.
IDirect3DDevice9* g_d3ddevice=NULL; ///< Graphics device.
D3DPRESENT_PARAMETERS g_d3dpp; ///< Device specifications.
LPDIRECT3DTEXTURE9 g_pBackgroundTexture=NULL; ///< Texture for background.
LPDIRECT3DTEXTURE9 g_pFloorTexture=NULL; ///< Texture for floor.
LPDIRECT3DVERTEXBUFFER9 g_pBackgroundVB;  ///< Vertex buffer for background. 
LPDIRECT3DVERTEXBUFFER9 g_pTerrainVB;  ///< Vertex buffer for terrain. 
LPDIRECT3DINDEXBUFFER9	g_pTerrainIB; ///< Index buffer for terrain.
IDirect3DSurface9* g_d3dbackbuffer=NULL; ///< Back buffer for rendering.
IDirect3DSurface9* g_pMainMenuSurface=NULL; ///< Surface for main menu animation.
IDirect3DSurface9* g_pCursorBitmap=NULL; ///< Surface for custom mouse cursor.
IDirect3DSurface9* g_pDeviceMenuSurface=NULL; ///< Surface for device menu.
int g_terrainVertices = (terrainXSize + 1) * (terrainZSize + 1); ///< total number of vertices in terrain
int g_terrainPrimitives = terrainXSize * terrainZSize * 2; ///< total number of primitives in terrain
D3DMATERIAL9 g_baseMaterial; ///< base material for sky and ground, etc
	
//other game variables
CTimer g_cTimer; ///< The game timer.
CSpriteManager g_cSpriteManager; ///< The sprite manager.
ObjectManager g_cObjectManager; ///< The object manager.
CRandom g_cRandom; ///< The random number generator.
CSoundManager* g_pSoundManager; //< The sound manager.
CInputManager* g_pInputManager; ///< The input manager.
int g_difficultyLevel = 1;  // difficulty level, increases HP
weatherType g_currentWeather = CLEAR_WEATHER; ///< what's the weather like?
timeOfDay g_currentTime = DAY_TIME; ///< what time of day is it?
groundType g_levelGround = GRASSY_GROUND; ///< what is the ground texture?
locationType g_levelLocation = MOUNTAIN_LOCATION; ///< where does the level take place?
int g_gameLevel = 0; ///< stage the player is on
int g_gTotalEnemyNumber = 0;

/*******************************/
const int g_nMaxNumberFonts = 20;
LayerManager2d layerManager; /// manages the 2d layers to draw on the screen
CD3DFont** Font;

/*******************************/


//game state variables
GameStateType g_nCurrentGameState; ///< Current game state.
GameStateType g_nNextGameState; ///< Next game state.
BOOL g_bEndGameState=FALSE; ///< Should we abort current state?
int g_nGameStateTime=0; ///< Time in state.
BOOL g_bDisplayCredits = FALSE; //< TRUE to display intro sequence.
int g_nLogoDisplayTime = 1; //< Number of seconds to display logo screen.
int g_nTitleDisplayTime = 1; //< Number of seconds to display title screen.
int g_nCreditsDisplayTime = 1; //< Number of seconds to display credits screen.
bool g_bWaitingForKeystroke = false; ///< are we waiting for a keystroke before continuing?

//prototypes for init functions in setup.cpp
HWND CreateDefaultWindow(char* name,HINSTANCE hInstance,int nCmdShow);
BOOL InitD3D(HINSTANCE hInstance,HWND hwnd); //initialize graphics
void ReleaseD3D(); //clean up graphics
void SetD3dStates(); //set D3D render states
BOOL DeviceLost(); //check for lost device
BOOL RestoreDevice(); //restore lost device and all surfaces
void SetWorldMatrix(float); //set world matrix
void SetViewMatrix(float, float, BOOL); //set view matrix

/// Create game objects. 
/// Creates a plane and a collection of crows at random positions,
/// and some cool farm stuff on the ground.

//+++++++++++++++++++++++++++++++++++++++++++++++++++ Prototypes +++++++++++++++++++++++++++++++++++++++++++++++++++++

void LoadEnvironmentTextures();
void setEnvironment();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//---------------------------------------------------------------------------------------------------------------------------------

void CreateObjects()
{
	D3DXVECTOR3 v; //location vector

	//DEBUGPRINTF("CreateObjects function\n");
	int i, chooser;

	v.x = 0;
	v.y = -512;
	v.z = 1500;
	g_cObjectManager.create(BACKGROUND_TYPE, "mountains", v);

	if (g_gameLevel == 0)
	{
		v.x = -1500.0f;
		v.y = 0.0f;
		v.z = 750.0f;
		g_cObjectManager.create(PLAYER_TYPE, "player", v);
		g_cObjectManager.create(PLAYER_TYPE, "inventory", v);
	}

	v.x = -2000.0f;
	v.y = 0.0f;
	v.z = 750.0f;
	g_cObjectManager.create(OBSTACLE_TYPE, "barrier", v);

	v.x = 11000.0f;
	v.y = 0.0f;
	v.z = 750.0f;
	g_cObjectManager.create(OBSTACLE_TYPE, "barrier", v);

	if (g_levelLocation == MOUNTAIN_LOCATION)
	{
		v.x = 1000;
		v.y = 0;
		v.z = 2300;
		g_cObjectManager.create(BACKGROUND_TYPE, "castle", v);
	}

	if( (g_levelLocation == MOUNTAIN_LOCATION) || (g_levelLocation == SEASHORE_LOCATION) )
	{
		v.x = 0;
		v.y = 128;
		v.z = 1000;
		g_cObjectManager.create(BACKGROUND_TYPE, "grass", v);
		
		v.x = (float)g_nScreenWidth;
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "grass", v);

		v.x = (float)(g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "grass", v);

		v.x = (float)-g_nScreenWidth;
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "grass", v);

		v.x = (float)(-g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "grass", v);

		//DEBUGPRINTF("CreateObjects - background grass done\n");

		v.x = 0.0f;
		v.y = 128.0f;
		v.z = 200.0f;
		g_cObjectManager.create(FOREGROUND_TYPE, "grass", v);
		
		v.x = (float)g_nScreenWidth;
		v.y = 128.0f;
		v.z = 200.0f;
		g_cObjectManager.create(FOREGROUND_TYPE, "grass", v);

		v.x = (float)(g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 200.0f;
		g_cObjectManager.create(FOREGROUND_TYPE, "grass", v);

		v.x = (float)-g_nScreenWidth;
		v.y = 128.0f;
		v.z = 200.0f;
		g_cObjectManager.create(FOREGROUND_TYPE, "grass", v);

		v.x = (float)(-g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 200.0f;
		g_cObjectManager.create(FOREGROUND_TYPE, "grass", v);
		//DEBUGPRINTF("CreateObjects - foreground grass done\n");

		int chooser;
		// some rocks
		for(i = 0; i < 12; i++)
		{
			v.x = (float)g_cRandom.number(0, 10000);
			v.y = 40.0f;
			v.z = (float)g_cRandom.number(LowerZBoundary, UpperZBoundary);
			chooser = g_cRandom.number(0, 2);
			switch(chooser)
			{
				case 0:
				case 1:
					g_cObjectManager.create(OBSTACLE_TYPE, "rock", v);
					break;
				case 2:
					g_cObjectManager.create(OBSTACLE_TYPE, "smallbush", v);
					break;
			} // end switch chooser
			//DEBUGPRINTF("CreateObjects - rock or bush created\n");
		} // end for
	} // end if mountain or beach
	else
	{
		v.x = 0;
		v.y = 128;
		v.z = 1000;
		g_cObjectManager.create(BACKGROUND_TYPE, "wall", v);
		
		v.x = (float)g_nScreenWidth;
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "wall", v);

		v.x = (float)(g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "wall", v);

		v.x = (float)-g_nScreenWidth;
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "wall", v);

		v.x = (float)(-g_nScreenWidth * 2);
		v.y = 128.0f;
		v.z = 1000.0f;
		g_cObjectManager.create(BACKGROUND_TYPE, "wall", v);

		//DEBUGPRINTF("CreateObjects - interior wall done\n");
	}

	if (g_levelLocation == MOUNTAIN_LOCATION)
	{
		// some trees
		for(i = 0; i < 10; i++)
		{
			v.x = (float)g_cRandom.number(-2000, 2000);
			v.y = 256.0f;
			v.z = (float)g_cRandom.number(1050, 1400);
			g_cObjectManager.create(BACKGROUND_TYPE, "tree", v);
			//DEBUGPRINTF("CreateObjects - tree created\n");
		} // end for

		for(i = 0; i < 6; i++)
		{
			v.x = (float)g_cRandom.number(-2000, 2000);
			v.y = 256.0f;
			v.z = (float)g_cRandom.number(1500, 1850);
			g_cObjectManager.create(BACKGROUND_TYPE, "smalltree", v);
			//DEBUGPRINTF("CreateObjects - tree created\n");
		} // end for

		for(i = 0; i < 3; i++)
		{
			v.x = (float)g_cRandom.number(-2000, 2000);
			v.y = 256.0f;
			v.z = (float)g_cRandom.number(1500, 1850);
			chooser = g_cRandom.number(1, 2);
			switch(chooser)
			{
				case 1:
					g_cObjectManager.create(BACKGROUND_TYPE, "cottage", v);
					break;
				case 2:
					int fireLoc = g_cObjectManager.create(BACKGROUND_TYPE, "burned_cottage", v);
					int fireChance = g_cRandom.number(0, 2);
					if( (fireChance == 1) && (g_currentWeather == CLEAR_WEATHER) )
					{
						g_cObjectManager.spawnFire(v, g_cRandom.number(40, 50), fireLoc);
					}
					break;
			} // end switch chooser
			//DEBUGPRINTF("CreateObjects - cottage created\n");
		} // end for
	}

	if (g_levelLocation == CITY_LOCATION)
	{
		for(i = -1; i < 9; i++)
		{
			v.x = i * 1500.0f;
			v.y = 450.0f;
			v.z = 750.0f;
			g_cObjectManager.create(OBSTACLE_TYPE, "lantern", v);
			g_cObjectManager.createLight(v, 1.0f, 0.6f, 0.7f, 1.0f, 500.0f);
		}
	}


	if( (g_levelLocation == MOUNTAIN_LOCATION) || (g_levelLocation == SEASHORE_LOCATION) )
	{
		for(i = 0; i < 1; i++)
		{
			v.x = (float)g_cRandom.number(0, 10000);
			v.y = 0.0f;
			v.z = (float)g_cRandom.number(LowerZBoundary, UpperZBoundary);
			if (g_currentWeather == CLEAR_WEATHER)
			{
				g_cObjectManager.create(FOREGROUND_TYPE, "campfire", v);
			}
		}
	} // end if mountain or beach


	if (g_currentWeather == RAINING_WEATHER)
	{
		g_cObjectManager.createRain(50);
	}

 

} //CreateObjects


void LevelSystem(int Level)
{

	CreateObjects();

	//DEBUGPRINTF("LevelSystem - Creating Objects for level %d\n", Level);


	D3DXVECTOR3 v; //location vector

	int i;
	int Ninja, Samurai;

	if(Level == 0)
	{
		Ninja = 4;
		Samurai = 0;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 0\n");
		
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bossninja", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 1)
	{
		Ninja = 4;
		Samurai = 2;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 1\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bossninja", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 2)
	{
		Ninja = 4;
		Samurai = 4;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bossninja", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 3)
	{
		Ninja = 4;
		Samurai = 6;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bosssamurai", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 4)
	{
		Ninja = 4;
		Samurai = 8;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bosssamurai", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 5)
	{
		Ninja = 5;
		Samurai = 10;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "bosssamurai", v);
		
		v.x = 9100;
		v.y = 60.0f;
		v.z = 999.0f;
		g_cObjectManager.create(OBSTACLE_TYPE, "monk", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 6)
	{
		Ninja = 2;
		Samurai = 5;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 999;
		g_cObjectManager.create(OBSTACLE_TYPE, "cave", v);
		v.z = 998;
		v.x = 8900;
		g_cObjectManager.create(ENEMY_TYPE, "dragon", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}
	else if (Level == 7)
	{
		Ninja = 10;
		Samurai = 14;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "onmyou", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}

	else if (Level == 8)
	{
		Ninja = 10;
		Samurai = 14;
		g_gTotalEnemyNumber = Ninja + Samurai;
		//DEBUGPRINTF("CreateObjects - Level 2\n");
		v.x = 9000;
		v.y = 100;
		v.z = 750;
		g_cObjectManager.create(ENEMY_TYPE, "daimyo", v);
		v.x = 9000.0f;
		v.y = 78.0f;
		v.z = 999.0f;
		g_cObjectManager.create(OBSTACLE_TYPE, "empress", v);
		//DEBUGPRINTF("CreateObjects - majou created\n");
	}

	// some badguys
	for(i = 0; i < Samurai; i++)
	//for(i = 0; i < 1; i++)
	{
		v.x = (float)g_cRandom.number(0, 8000);
		v.y = (float)78;
		v.z = (float)g_cRandom.number(LowerZBoundary, UpperZBoundary);
		g_cObjectManager.create(ENEMY_TYPE, "samurai", v);
		//DEBUGPRINTF("CreateObjects - samurai created\n");
	} // end for


	for(i = 0; i < Ninja; i++)
	//for(i = 0; i < 1; i++)
	{
		v.x = (float)g_cRandom.number(0, 8000);
		if (g_levelLocation == CITY_LOCATION)
		{
			v.y = 700.0f;
			v.z = 900.0f;
		}
		else
		{
			v.y = 78.0f;
			v.z = 1005.0f;
		}
		
		g_cObjectManager.create(ENEMY_TYPE, "ninja", v);
		//DEBUGPRINTF("CreateObjects - ninja created\n");
	} // end for

	for(i = 0; i < g_cRandom.number(0, 2); i++)
	{
		v.x = (float)g_cRandom.number(0, 8000);
		v.y = 32.0f;
		v.z = (float)g_cRandom.number(LowerZBoundary, UpperZBoundary);
		g_cObjectManager.create(OBSTACLE_TYPE, "smallrice", v);
			//DEBUGPRINTF("CreateObjects - small rice created\n");
	}

	for(i = 0; i < g_cRandom.number(0, 1); i++)
	{
		v.x = (float)g_cRandom.number(0, 8000);
		v.y = 32.0f;
		v.z = (float)g_cRandom.number(LowerZBoundary, UpperZBoundary);
		g_cObjectManager.create(OBSTACLE_TYPE, "bigrice", v);
		//DEBUGPRINTF("CreateObjects - big rice created\n");
	}

}

//---------------------------------------------------------------------------------------------------------------------------------





/// Show the back buffer.
/// If we are fullscreen, flip back buffer to front to show next frame of animation.
/// If we are windowed, blit the back buffer to the front instead. D3D takes care
/// of the details.
///  \return TRUE if it succeeded

BOOL PageFlip() //show back buffer
{
	//DEBUGPRINTF("PageFlip - start\n");
	return SUCCEEDED(g_d3ddevice->Present(NULL,NULL,NULL,NULL));
} //PageFlip

/// Draw the game background.
/// \param x x coordinate of camera

void DrawBackground(float x)
{

	//DEBUGPRINTF("DrawBackground function\n");

	const float delta = 2.0f * g_nScreenWidth;
	float fQuantizeX = delta * (int)(x/delta-1.0f) + g_nScreenWidth; ///< Quantized x coordinate

	//set vertex buffer to background
	


//=====================Draw Sky ==========================
	g_baseMaterial.Emissive.r = 1.0f;
	g_baseMaterial.Emissive.g = 1.0f;
	g_baseMaterial.Emissive.b = 1.0f;
	g_baseMaterial.Emissive.a = 1.0f;
	g_d3ddevice->SetMaterial(&g_baseMaterial);	

	g_d3ddevice->SetStreamSource(0,g_pBackgroundVB,0,sizeof(BILLBOARDVERTEX));
	g_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

	g_d3ddevice->SetTexture(0,g_pBackgroundTexture); //set cloud texture
	SetWorldMatrix(x);
	g_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
//=====================Done Drawing Sky ==========================

//=====================Draw Ground ==========================

	g_baseMaterial.Emissive.r = 0.0f;
	g_baseMaterial.Emissive.g = 0.0f;
	g_baseMaterial.Emissive.b = 0.0f;
	g_baseMaterial.Emissive.a = 0.0f;
	g_d3ddevice->SetMaterial(&g_baseMaterial);	

	g_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format
	g_d3ddevice->SetStreamSource(0, g_pTerrainVB, 0, sizeof(BILLBOARDVERTEX));
	g_d3ddevice->SetIndices(g_pTerrainIB);
	g_d3ddevice->SetTexture(0,g_pFloorTexture); //set floor texture

	SetWorldMatrix(fQuantizeX);
	g_d3ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_terrainVertices, 0, g_terrainPrimitives);

	SetWorldMatrix(fQuantizeX-delta);
	g_d3ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_terrainVertices, 0, g_terrainPrimitives);

	SetWorldMatrix(fQuantizeX+delta);
	g_d3ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_terrainVertices, 0, g_terrainPrimitives);

	SetWorldMatrix(fQuantizeX+2*delta);
	g_d3ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, g_terrainVertices, 0, g_terrainPrimitives);

//=====================Done Drawing Ground==========================
	
} //DrawBackground


/// Create offscreen surface.
/// Creates a surface in system memory.
/// \param lplpSurface pointer to the place where a pointer to the new surface is to be put
/// \param width desired surface width
/// \param height desired surface height
/// \param format desired surface format
/// \return TRUE if it succeeds

BOOL CreateOffscreenSurface(IDirect3DSurface9** lplpSurface,
                            int width, int height, D3DFORMAT format){
  HRESULT hr = g_d3ddevice->CreateOffscreenPlainSurface(
    width, height, format, D3DPOOL_SYSTEMMEM, lplpSurface, NULL);
  return SUCCEEDED(hr);
} //CreateOffscreenSurface


/// Create offscreen surface.
/// Create a plain offscreen surface from pooled system memory. Primarily
/// intended for the display of screen images in the game shell.
/// \param lplpSurface address of pointer to the created surface

BOOL CreateOffscreenSurface(IDirect3DSurface9** lplpSurface){
  return CreateOffscreenSurface(lplpSurface, 
    g_d3dpp.BackBufferWidth, g_d3dpp.BackBufferHeight, g_d3dpp.BackBufferFormat);
} //CreateOffscreenSurface

/// Display screen image.
/// Display an image from a file to the screen.
/// \param filename name of image file
/// \return TRUE if the load from file succeeded

BOOL DisplayScreen(char *filename) //display image file
{
	//DEBUGPRINTF("DisplayScreen\n");

	IDirect3DSurface9* pSurface = NULL; //surface for image

	//get surface for image
	CreateOffscreenSurface(&pSurface);

	//load image
	if(FAILED(D3DXLoadSurfaceFromFile(pSurface, NULL, NULL, //copy to surface
									filename, NULL, D3DX_DEFAULT, 0, NULL))) //using default settings
	{
		return FALSE;
	}

	//draw image to back buffer
	g_d3ddevice->BeginScene();
	g_d3ddevice->UpdateSurface(pSurface, NULL, g_d3dbackbuffer, NULL);
	g_d3ddevice->EndScene(); 
	PageFlip(); //flip to front

	//release image surface and return
	if(pSurface)
	{
		pSurface->Release(); //release
	}

	//DEBUGPRINTF("DisplayScreen - finishing\n");
	return TRUE;

} //DisplayScreen


void waitForKeystroke()
{
	g_bWaitingForKeystroke = true;
	while (g_bWaitingForKeystroke)
	{
		Sleep(100);
	} // end while
}



/// Display intro screen.
/// Display the intro screen for the current state.

void DisplayIntroScreen()
{ 
	//DEBUGPRINTF("DisplayIntroScreen - function begin\n");
	switch(g_nCurrentGameState)
	{
		case LOGO_GAMESTATE: 
			g_pSoundManager->play(LOGO_SOUND); //signature chord
			DisplayScreen(g_cImageFileName[LOGO_IMAGEFILE]); 
			break;
		case TITLE_GAMESTATE: 
			g_pSoundManager->stop(); //silence sounds from previous game state
      		g_pSoundManager->play(TITLE_SOUND); //title sound
            DisplayScreen(g_cImageFileName[TITLE_IMAGEFILE]);
			break;
	    case CREDITS_GAMESTATE: 
			g_pSoundManager->stop(); //silence sounds from previous game state
      		g_pSoundManager->play(CREDITS_SOUND); //credits sound
            DisplayScreen(g_cImageFileName[CREDITS_IMAGEFILE]);
			break;
		case CREDITS1_GAMESTATE: 
			//g_pSoundManager->stop(); //silence sounds from previous game state
      		//g_pSoundManager->play(CREDITS_SOUND); //credits sound
            DisplayScreen(g_cImageFileName[CREDITS1_IMAGEFILE]);
			break;
		case CREDITS2_GAMESTATE: 
			//g_pSoundManager->stop(); //silence sounds from previous game state
      		//g_pSoundManager->play(CREDITS_SOUND); //credits sound
            DisplayScreen(g_cImageFileName[CREDITS2_IMAGEFILE]);
			break;
		case PLAYING_GAMESTATE: break; //do nothing
	} // end switch
} //DisplayIntroScreen

/// Change game state.
/// Changes to a new game state and performs all the necessary
/// processing for entering that state.
/// \param newstate new state to enter

void ChangeGameState(GameStateType newstate) //enter new state
{
	// DEBUGPRINTF("ChangeGameState function - start\n");

	//change to new state
	GameStateType oldgamestate = g_nCurrentGameState; //save old game state
	g_nCurrentGameState = newstate; g_nGameStateTime = g_cTimer.time();
	g_bEndGameState=FALSE; //we hopefully came here because this was TRUE
	g_pInputManager->ChangeState(newstate); //input changes state too

	//start-of-state housekeeping
	switch(g_nCurrentGameState)
	{
		case LOGO_GAMESTATE:
      		g_pSoundManager->LoadSounds(0); //load sounds for intro sequence
      		DisplayIntroScreen();
      		break;
    	case TITLE_GAMESTATE:
    	case CREDITS_GAMESTATE:
		case CREDITS1_GAMESTATE:
		case CREDITS2_GAMESTATE:
		      DisplayIntroScreen();
      		break;
    	case MENU_GAMESTATE:
      		if(oldgamestate == PLAYING_GAMESTATE)
        		g_pSoundManager->stop(); //silence playing phase
      		else if(oldgamestate == CREDITS2_GAMESTATE) //if coming in from intro
			{
        		g_pSoundManager->clear(); //clear out old sounds
        		g_pSoundManager->LoadSounds(1); //load game sounds
      		}

      		g_pInputManager->SetupButtons(MENU_GAMESTATE);
      		break;   
    	case DEVICEMENU_GAMESTATE:
      		g_pInputManager->SetupButtons(DEVICEMENU_GAMESTATE);
      		break;

//---------------------------------------------------------------------------------------------------------------------------------
		case PLAYING_GAMESTATE: //prepare the game engine
		
			switch(oldgamestate)	//depending on previous game state
			{
				case MENU_GAMESTATE:
					g_gameLevel = 0; //start at stage 1
					DisplayScreen(g_cImageFileName[GAMESTART_IMAGEFILE]);
					g_cObjectManager.ambientMusic();
					g_bWaitingForKeystroke = true;

					break;

				case NEXTLEVEL_GAMESTATE:
					g_pSoundManager->stop(); //silence success phase
					g_gameLevel++; //go to next level
					g_cObjectManager.resetPlayerLocation();
					break;	
			}
			
			//create objects in game engine
			setEnvironment();
			LoadEnvironmentTextures();
			if (g_gameLevel == 0)
			{
				g_cObjectManager.ClearAll();
			}
			else
			{
				g_cObjectManager.Reset(); //clear object manager
			}
			//CreateObjects(); //create new objects
			//DEBUGPRINTF("Creating Objects for level %d\n", g_gameLevel);
			LevelSystem(g_gameLevel);
			break;

		case NEXTLEVEL_GAMESTATE:
			//DEBUGPRINTF("NextLevel\n");
			g_pSoundManager->stop(); //stop sounds from previous phase
			g_cObjectManager.ambientMusic();
			if(g_gameLevel == 0)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL1_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 1)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL2_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 2)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL3_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 3)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL4_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 4)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL5_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 5)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL6_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 6)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL7_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 7)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL8_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			if(g_gameLevel == 8)
			{
				DisplayScreen(g_cImageFileName[NEXTLEVEL9_IMAGEFILE]); //display success screen
				g_bWaitingForKeystroke = true;
			}
			break;

//---------------------------------------------------------------------------------------------------------------------------------

  } // end switch
} //ChangeGameState

/// Compose a frame of animation.
/// Draws the next frame of animation to the back buffer and flips it to the front.
/// \return TRUE if it succeeded

BOOL ComposeFrame() //compose a frame of animation 
{
	//DEBUGPRINTF("ComposeFrame function begin\n\n");
	g_d3ddevice->Clear(0L,NULL,D3DCLEAR_TARGET,0,1.0f,0L); //clear render buffer

	//DEBUGPRINTF("ComposeFrame - render buffer cleared\n");

	if(SUCCEEDED(g_d3ddevice->BeginScene())) //can start rendering
	{
		//move objects
		//DEBUGPRINTF("ComposeFrame - tell Object Manager to move objects\n");
		g_cObjectManager.move();
		//DEBUGPRINTF("ComposeFrame - Object Manager finished moving objects\n");
	} // end if succeeded
	else
	{
		DEBUGPRINTF("ComposeFrame - failed g_d3ddevice->BeginScene\n");
	}
    //set camera location
	
	//DEBUGPRINTF("ComposeFrame - grabbing Player's current location\n");

    D3DXVECTOR3 vecPlayerLocation;

	vecPlayerLocation = g_cObjectManager.PlayerLocation(); //player's current location

    //DEBUGPRINTF("ComposeFrame - Player's current location grabbed\n");
    float y = vecPlayerLocation.y;
    if(y > g_nScreenHeight-400)
	{
		y = (float)g_nScreenHeight-400.0f;
	}
	//DEBUGPRINTF("ComposeFrame - setting ViewMatrix\n");

    SetViewMatrix(vecPlayerLocation.x + g_nScreenWidth/2.0f, y, g_bCameraDefaultMode);
	
	//DEBUGPRINTF("ComposeFrame - ViewMatrix set\n");
    //draw background 
	//DEBUGPRINTF("ComposeFrame - drawing background\n");
    DrawBackground(vecPlayerLocation.x-g_nScreenWidth/2.0f);
	//DrawBackground(vecPlayerLocation.x);
	//DEBUGPRINTF("ComposeFrame - background drawn\n");
    //draw objects
	//DEBUGPRINTF("ComposeFrame - tell Object Manager to draw objects\n");
    g_cObjectManager.draw();
	//DEBUGPRINTF("ComposeFrame - Object Manager drew objects\n");
/**********************************************************************************/
// Use spritemanager begin and end so the sprites will be efficiently 
// added to the pipeline.

	layerManager.draw2dLayer();


/*********************************************************************************/
	g_cObjectManager.drawInventory();

    g_d3ddevice->EndScene(); //done rendering
	//DEBUGPRINTF("ComposeFrame function end\n\n");
	return true;
} //ComposeFrame

/// Compose menu frame.
/// Compose a frame of menu screen animation. This is just the buttons right
/// now, but we could add a glitzy animation if we wished.

BOOL ComposeMenuFrame(IDirect3DSurface9* pMenuSurface){ //display menu screen
  g_d3ddevice->BeginScene();  
  g_d3ddevice->UpdateSurface(pMenuSurface, NULL, g_d3dbackbuffer, NULL);
  g_pInputManager->DrawButtons(); //draw buttons on screen
  g_d3ddevice->EndScene();
  return TRUE;
} //ComposeMenuFrame
/// Process a frame of animation. Compose a frame, present it, then check for 
/// lost device and restore it if necessary.

void ProcessFrame() //process a frame of animation
{
	//DEBUGPRINTF("ProcessFrame - function start\n");
  BOOL bGameOver = FALSE; //TRUE if the game is over
  bGameOver = bGameOver || 
    g_pInputManager->ProcessKeyboardInput(); //process keyboard input
  bGameOver = bGameOver ||
    g_pInputManager->ProcessMouseInput(); //process mouse input
  g_pInputManager->ProcessJoystickInput(); //process joystick input
  if(bGameOver){ //if game over
    DestroyWindow(g_HwndApp); //end of game so destroy window
    return; //bail out
  }

	//check for lost graphics device
	if(DeviceLost()) //if device lost
	{
		//DEBUGPRINTF("ProcessFrame - lost control of graphics device, restoring\n");
		RestoreDevice(); //restore it
	//if we're showing the main menu
    if(g_nCurrentGameState == MENU_GAMESTATE ) 
      //if we're showing the main menu
      g_d3ddevice->ShowCursor(TRUE); //show d3d cursor, it was lost too
	    DisplayIntroScreen(); //will failsoft if not in the intro
	}

  
	//process a frame of animation
	
	//reset sound manager to avoid duplicate sounds starting simultaneously (loud)
	g_pSoundManager->beginframe();

	const int SUCCESS_DISPLAY_TIME=4500; //duration of success
	
	switch(g_nCurrentGameState) //what state are we in?
	{

		case LOGO_GAMESTATE: //displaying logo screen
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, g_nLogoDisplayTime * 1000))
			ChangeGameState(TITLE_GAMESTATE); //go to title screen
			Sleep(100); //to prevent using too much CPU time in a busy/wait
			break;

		case TITLE_GAMESTATE: //displaying title screen
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, g_nTitleDisplayTime * 1000))
			ChangeGameState(CREDITS_GAMESTATE); //go to credits
			Sleep(100); //to prevent using too much CPU time in a busy/wait
			break;

		case CREDITS_GAMESTATE: //displaying credits screen
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, g_nCreditsDisplayTime * 1000))
			ChangeGameState(CREDITS1_GAMESTATE); //go to menu
			Sleep(100); //to prevent using too much CPU time in a busy/wait
			break;

		case CREDITS1_GAMESTATE: //displaying credits screen
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, g_nCreditsDisplayTime * 1000))
			ChangeGameState(CREDITS2_GAMESTATE); //go to menu
			Sleep(100); //to prevent using too much CPU time in a busy/wait
			break;

		case CREDITS2_GAMESTATE: //displaying credits screen
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, g_nCreditsDisplayTime * 1000))
			ChangeGameState(MENU_GAMESTATE); //go to menu
			Sleep(100); //to prevent using too much CPU time in a busy/wait
			break;

		case MENU_GAMESTATE: //main menu
      		ComposeMenuFrame(g_pMainMenuSurface); //menu screen animation
      		PageFlip(); //flip video memory surfaces
      		if(g_bEndGameState)ChangeGameState(g_nNextGameState); //device menu or game play
      		Sleep(10); //to prevent using too much CPU time in a busy/wait
      		break;

    	case DEVICEMENU_GAMESTATE:
      		ComposeMenuFrame(g_pDeviceMenuSurface); //device menu screen animation
      		PageFlip(); //flip video memory surfaces
      		if(g_bEndGameState)ChangeGameState(MENU_GAMESTATE); //main menu
      		break;

//---------------------------------------------------------------------------------------------------------------------------------

		case PLAYING_GAMESTATE: //game engine
			if (!g_bWaitingForKeystroke)
			{
				ComposeFrame(); //compose a frame in back surface
				PageFlip(); //flip video memory surfaces

				//DEBUGPRINTF("%d DeadObjects\n",g_cObjectManager.DeadEnemy());

				if(g_bEndGameState || g_cObjectManager.PlayerLose() || (g_gameLevel >= 9)) //if end of state
				{
					//DEBUGPRINTF("ProcessFrame - Player is Dead\n");
					ChangeGameState(MENU_GAMESTATE); //go to menu				
				}
				else if(g_cObjectManager.BossDead() /*|| g_cObjectManager.PlayerWon(g_gTotalEnemyNumber) */ )				
				{	
					//DEBUGPRINTF("ProcessFrame - Player wins and go to the next level\n");		
					ChangeGameState(NEXTLEVEL_GAMESTATE); //go to next level				
				}
			}
			break;
		case NEXTLEVEL_GAMESTATE:
			if(g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, SUCCESS_DISPLAY_TIME)) //long enough
				ChangeGameState(PLAYING_GAMESTATE); //go to next level
			break;

//---------------------------------------------------------------------------------------------------------------------------------
			
	} // end switch
} //ProcessFrame

/// Create offscreen surfaces.
/// Creates the offscreen surfaces required by the game.
/// \return TRUE if it succeeds

BOOL CreateOffscreenSurfaces(){ //create surfaces for image storage
  if(!CreateOffscreenSurface(&g_pMainMenuSurface))return FALSE; //main menu
  if(!CreateOffscreenSurface(&g_pDeviceMenuSurface))return FALSE; //device menu
  if(!CreateOffscreenSurface(&g_pCursorBitmap, 16, 16, D3DFMT_A8R8G8B8))
    return FALSE; //mouse cursor
  return TRUE; //succeeded
} //CreateOffscreenSurfaces

/// Load a surface from a file.
/// Loads an image from a file into a D3D surface. Aborts if the file doesn't exist.
///  \param surface pointer to D3D surface to receive the image
///  \param filename name of the fle containing the image

void LoadSurface(IDirect3DSurface9* surface,char* filename){
  HRESULT hres = D3DXLoadSurfaceFromFile(surface, NULL, NULL, //copy to surface
    filename, NULL, D3DX_DEFAULT, 0, NULL);
  if(FAILED(hres)) //fail and bail
    ABORT("Image file %s not found",filename);
} //LoadSurface

/// Load offscreen surfaces.
/// Loads the offscreen surfaces required by the game.

void LoadSurfaces(){
  LoadSurface(g_pMainMenuSurface, g_cImageFileName[MENU_IMAGEFILE]);
  LoadSurface(g_pDeviceMenuSurface, g_cImageFileName[DEVICEMENU_IMAGEFILE]);
  LoadSurface(g_pCursorBitmap, g_cImageFileName[CURSOR_IMAGEFILE]);
} //LoadSurfaces

/// Load a texture from a file.
/// Loads an image from a file into a D3D texture. Aborts if the file doesn't exist.
///  \param texture pointer to D3D texture to receive the image
///  \param filename name of the fle containing the texture

void LoadTexture(LPDIRECT3DTEXTURE9& texture, char* filename)
{
	//DEBUGPRINTF("LoadTexture begin \n");
	D3DXIMAGE_INFO structImageInfo; //image information
	HRESULT hres=D3DXCreateTextureFromFileEx(g_d3ddevice,filename,
    0,0,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_FILTER_NONE,
    D3DX_DEFAULT,0,&structImageInfo,NULL,&texture);

	if(FAILED(hres)) //fail and bail
		ABORT("Texture file %s not found",filename);
} //LoadTexture



void setEnvironment()
{
// =================================== set time, location, weather ================================
	switch(g_gameLevel)
	{
		case 0:
			// clear weather, seashore, daytime, sand
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = DAY_TIME;
			g_levelGround = SAND_GROUND;
			g_levelLocation = SEASHORE_LOCATION;
			break;

		case 1:
			// clear weather, seashore, sunset, sand
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = DUSK_TIME;
			g_levelGround = SAND_GROUND;
			g_levelLocation = SEASHORE_LOCATION;
			break;

		
		case 2:
			// rainy weather, mountains, day time, stone
			g_currentWeather = RAINING_WEATHER;
			g_currentTime = DAY_TIME;
			g_levelGround = STONE_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
			break;
		

		case 3:
			// clear weather, mountains, day time, grass
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = DAY_TIME;
			g_levelGround = GRASSY_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
			break;

		case 4:
			// clear weather, mountains, dusk, grass
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = DUSK_TIME;
			g_levelGround = GRASSY_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
			break;

		case 5:
			// clear weather, mountains, night time, stone
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = NIGHT_TIME;
			g_levelGround = STONE_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
			break;

		case 6:
			// rainy weather, mountains, night time, grass
			g_currentWeather = RAINING_WEATHER;
			g_currentTime = NIGHT_TIME;
			g_levelGround = GRASSY_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
			break;

		case 7:
			// clear weather, city, night, wood floor
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = NIGHT_TIME;
			g_levelGround = WOOD_GROUND;
			g_levelLocation = CITY_LOCATION;
			break;

		case 8:
			// clear weather, city, night, wood floor
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = NIGHT_TIME;
			g_levelGround = WOOD_GROUND;
			g_levelLocation = CITY_LOCATION;
			break;
		

		default:
			g_currentWeather = CLEAR_WEATHER;
			g_currentTime = DAY_TIME;
			g_levelGround = GRASSY_GROUND;
			g_levelLocation = MOUNTAIN_LOCATION;
	} // end case g_gameLevel

// ============================= environment lighting =========================

	D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );

	switch(g_currentTime)
	{
		case DAWN_TIME:
			vecDir.x = 1.0f;
			vecDir.y = 0.2f;
			vecDir.z = 0.5f;
			light.Ambient.r  = 0.52f;
			light.Ambient.g  = 0.44f;
			light.Ambient.b  = 0.56f;
			light.Ambient.a  = 1.0f;
			break;
		case DAY_TIME:
			vecDir.x = 1.0f;
			vecDir.y = 0.2f;
			vecDir.z = 0.5f;
			light.Ambient.r  = 0.83f;
			light.Ambient.g  = 0.82f;
			light.Ambient.b  = 0.8f;
			light.Ambient.a  = 1.0f;
			break;
		case DUSK_TIME:
			vecDir.x = 0.2f;
			vecDir.y = 0.2f;
			vecDir.z = 0.5f;
			light.Ambient.r  = 0.7f;
			light.Ambient.g  = 0.53f;
			light.Ambient.b  = 0.31f;
			light.Ambient.a  = 1.0f;
			break;
		case NIGHT_TIME:
			vecDir.x = 1.0f;
			vecDir.y = 0.2f;
			vecDir.z = 0.5f;
			light.Ambient.r  = 0.2f;
			light.Ambient.g  = 0.2f;
			light.Ambient.b  = 0.2f;
			light.Ambient.a  = 1.0f;
			break;
	} // end switch current time
    
    light.Type = D3DLIGHT_DIRECTIONAL;
	light.Direction = vecDir;

	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	light.Specular.a = 1.0f;

    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
	light.Diffuse.a  = 1.0f;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
    
    g_d3ddevice->SetLight(0, &light);
	g_d3ddevice->LightEnable(0, TRUE);
    g_d3ddevice->SetRenderState(D3DRS_LIGHTING, TRUE); 

} // end setEnvironment




/// Load the textures required for the game. 
/// Calls function LoadTexture to load in the textures needed for the game.
/// The texture names are hard-coded.

void LoadEnvironmentTextures()
{
	switch(g_levelGround)
	{
		case GRASSY_GROUND:
			LoadTexture(g_pFloorTexture,g_cImageFileName[GRASSY_GROUND_IMAGEFILE]);
			break;

		case WOOD_GROUND:
			LoadTexture(g_pFloorTexture,g_cImageFileName[WOOD_GROUND_IMAGEFILE]);
			break;

		case STONE_GROUND:
			LoadTexture(g_pFloorTexture,g_cImageFileName[STONE_GROUND_IMAGEFILE]);
			break;

		case SAND_GROUND:
			LoadTexture(g_pFloorTexture,g_cImageFileName[SAND_GROUND_IMAGEFILE]);
			break;
	} // end switch ground
		

	switch(g_levelLocation)
	{
		case MOUNTAIN_LOCATION:
			if (g_currentTime == DAY_TIME) // if it's daytime
			{
				if (g_currentWeather == CLEAR_WEATHER) // if weather is clear
				{
					LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLEARDAY_BACKGROUND_IMAGEFILE]);
				} // end if weather is clear
				else // else it's raining
				{
					LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLOUDYDAY_BACKGROUND_IMAGEFILE]);
				} // end else it's raining	
			} // end if day time
			
			if (g_currentTime == NIGHT_TIME) // if it's nighttime
			{
				if (g_currentWeather == CLEAR_WEATHER) // if weather is clear
				{
					LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLEARNIGHT_BACKGROUND_IMAGEFILE]);
				} // end if weather is clear
				else // else it's raining
				{
					LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLOUDYNIGHT_BACKGROUND_IMAGEFILE]);
				} // end else it's raining	
			} // end if day time

			if( (g_currentTime == DAWN_TIME) || (g_currentTime == DUSK_TIME) )// if it's dawn or dusk
			{
				LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLEARSUNSET_BACKGROUND_IMAGEFILE]);
			} // end if day time
			break;

		case SEASHORE_LOCATION:
			if (g_currentTime == DAY_TIME) // if it's daytime
			{
				LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLEARSEASHORE_BACKGROUND_IMAGEFILE]);
			} // end if day time

			if( (g_currentTime == DAWN_TIME) || (g_currentTime == DUSK_TIME) )// if it's dawn or dusk
			{
				LoadTexture(g_pBackgroundTexture,g_cImageFileName[SUNSETSEASHORE_BACKGROUND_IMAGEFILE]);
			} // end if day time

			break;

		case CITY_LOCATION:
			LoadTexture(g_pBackgroundTexture,g_cImageFileName[CLEARNIGHTCITY_BACKGROUND_IMAGEFILE]);
			break;

		//default:
			// do nothing
		
	} // end switch level location
} // end thingy


void LoadTextures() //create textures for image storage
{
	//DEBUGPRINTF("LoadTextures - function begin\n");
	
	//sprites
	g_cSpriteManager.Load(main_stand,"main_stand");
	g_cSpriteManager.Load(main_walk,"main_walk");
	g_cSpriteManager.Load(main_run,"main_run");
	g_cSpriteManager.Load(main_slash,"main_slash");
	g_cSpriteManager.Load(main_stab,"main_stab");
	g_cSpriteManager.Load(main_cut,"main_cut");
	g_cSpriteManager.Load(main_defend,"main_defend");
	g_cSpriteManager.Load(main_jump,"main_jump");
	g_cSpriteManager.Load(main_die,"main_die");
	g_cSpriteManager.Load(main_duck,"main_duck");

	g_cSpriteManager.Load(sam_stand,"sam_stand");
	g_cSpriteManager.Load(sam_walk,"sam_walk");
	g_cSpriteManager.Load(sam_slash,"sam_slash");
	g_cSpriteManager.Load(spear_stand,"spear_stand");
	g_cSpriteManager.Load(spear_walk,"spear_walk");
	g_cSpriteManager.Load(spear_stab,"spear_stab");
	g_cSpriteManager.Load(bow_stand,"bow_stand");
	g_cSpriteManager.Load(bow_walk,"bow_walk");
	g_cSpriteManager.Load(bow_shoot,"bow_shoot");
	g_cSpriteManager.Load(sam_die,"sam_die");
	g_cSpriteManager.Load(rock,"rock");
	g_cSpriteManager.Load(BUTTON_OBJECT,"button", FALSE);
	g_cSpriteManager.Load(blood,"blood");
	g_cSpriteManager.Load(arrow,"arrow");
	g_cSpriteManager.Load(mountains,"mountains");
	g_cSpriteManager.Load(fire1,"fire1");
	g_cSpriteManager.Load(fire2,"fire2");
	g_cSpriteManager.Load(fire3,"fire3");
	g_cSpriteManager.Load(grass,"grass");
	g_cSpriteManager.Load(tree1,"tree1");
	g_cSpriteManager.Load(tree2,"tree2");
	g_cSpriteManager.Load(smalltree1,"smalltree1");
	g_cSpriteManager.Load(smoke,"smoke");
	g_cSpriteManager.Load(particle_p,"particle_p");
	g_cSpriteManager.Load(shard_p,"shard_p");
	g_cSpriteManager.Load(leaf_p,"leaf_p");
	g_cSpriteManager.Load(rain_p,"rain_p");
	g_cSpriteManager.Load(majou_stand,"majou_stand");
	g_cSpriteManager.Load(majou_attack,"majou_attack");
	g_cSpriteManager.Load(majou_die,"majou_die");
	g_cSpriteManager.Load(bigrice,"bigrice");
	g_cSpriteManager.Load(smallrice,"smallrice");
	g_cSpriteManager.Load(cottage,"cottage");
	g_cSpriteManager.Load(burned_cottage,"burned_cottage");
	g_cSpriteManager.Load(smallbush,"smallbush");
	g_cSpriteManager.Load(castle,"castle");
	g_cSpriteManager.Load(cave,"cave");
	g_cSpriteManager.Load(shuriken,"shuriken");
	g_cSpriteManager.Load(ninja_stand,"ninja_stand");
	g_cSpriteManager.Load(ninja_walk,"ninja_walk");
	g_cSpriteManager.Load(ninja_slash,"ninja_slash");
	g_cSpriteManager.Load(ninja_throw,"ninja_throw");
	g_cSpriteManager.Load(ninja_die,"ninja_die");
	g_cSpriteManager.Load(ninja_flip,"ninja_flip");
	g_cSpriteManager.Load(main_hi_blow,"main_hi_blow");
	g_cSpriteManager.Load(main_lo_blow,"main_lo_blow");
	g_cSpriteManager.Load(nothing_icon,"nothing");
	g_cSpriteManager.Load(armor,"armor");
	g_cSpriteManager.Load(insidewall,"insidewall");
	g_cSpriteManager.Load(lantern,"lantern");

	g_cSpriteManager.Load(daimyo_stand,"daimyo_stand");
	g_cSpriteManager.Load(daimyo_walk,"daimyo_walk");
	g_cSpriteManager.Load(daimyo_stab,"daimyo_stab");
	g_cSpriteManager.Load(daimyo_slash,"daimyo_slash");
	g_cSpriteManager.Load(daimyo_die,"daimyo_die");
	g_cSpriteManager.Load(daimyo_jump,"daimyo_jump");

	g_cSpriteManager.Load(onmyou_stand,"onmyou_stand");
	g_cSpriteManager.Load(onmyou_cast,"onmyou_cast");
	g_cSpriteManager.Load(onmyou_attack,"onmyou_attack");
	g_cSpriteManager.Load(onmyou_die,"onmyou_die");

	g_cSpriteManager.Load(dragon_stand,"dragon_stand");
	g_cSpriteManager.Load(dragon_attack,"dragon_attack");
	g_cSpriteManager.Load(dragon_shoot,"dragon_shoot");
	g_cSpriteManager.Load(dragon_die,"dragon_die");

	g_cSpriteManager.Load(barrier,"barrier");
	g_cSpriteManager.Load(katana,"katana");

	g_cSpriteManager.Load(empress,"empress");
	g_cSpriteManager.Load(monk,"monk");

	//DEBUGPRINTF("LoadTextures - function end\n");

} //LoadTextures

/// Release game textures.
/// All textures used in the game are released - the release function is kind
/// of like a destructor for DirectX entities, which are COM objects.

void ReleaseTextures() //release offscreen images 
{
	//DEBUGPRINTF("ReleaseTextures - begin\n");
	if(g_pBackgroundTexture)
	{
		g_pBackgroundTexture->Release();
	}
	if(g_pFloorTexture)
	{
		g_pFloorTexture->Release(); 
	}
	
	g_cSpriteManager.Release(); //release sprites 

} //ReleaseTextures

void ReleaseOffscreenSurfaces(){ //release offscreen images 
  if(g_pCursorBitmap)g_pCursorBitmap->Release(); //release cursor
  if(g_pMainMenuSurface)g_pMainMenuSurface->Release(); //release main menu
  if(g_pDeviceMenuSurface)g_pDeviceMenuSurface->Release(); //release device menu
} //ReleaseOffscreenSurfaces

/// Initialize XML settings.
/// Opens an XML file and prepares to read settings from it. Settings
/// tag is loaded to XML element g_xmlSettings for later processing. Aborts if it
/// cannot load the file or cannot find settings tag in loaded file.

void InitXMLSettings()
{
  
  //open and load XML file
	if(!g_xmlDocument.LoadFile())
    ABORT("Cannot load settings file %s.", g_xmlFileName);

  //get settings tag
  g_xmlSettings = g_xmlDocument.FirstChildElement("settings"); //settings tag
  if(!g_xmlSettings) //abort if tag not found
    ABORT("Cannot find <settings> tag in %s.", g_xmlFileName);

} //InitXMLSettings


/// Load game settings from XML element.
/// Reads settings from g_xmlSettings. Settings are loaded into
/// global variables and member variables of classes. 

void LoadGameSettings()
{

  if(!g_xmlSettings)return; //bail and fail

  //get renderer settings
	TiXmlElement* renderSettings = 
    g_xmlSettings->FirstChildElement("renderer"); //renderer tag
  if(renderSettings){ //read renderer tag attributes
    renderSettings->Attribute("width",&g_nScreenWidth);
    renderSettings->Attribute("height",&g_nScreenHeight);
    renderSettings->Attribute("windowed",&g_bWindowed);
  }

  //get intro settings
	TiXmlElement* s = 
    g_xmlSettings->FirstChildElement("intro"); //renderer tag
  if(s){ //read renderer tag attributes
    s->Attribute("display",&g_bDisplayCredits);
    s->Attribute("logo",&g_nLogoDisplayTime);
    s->Attribute("title",&g_nTitleDisplayTime);
    s->Attribute("credits",&g_nCreditsDisplayTime);
  }

  //get image file names
  g_cImageFileName.GetImageFileNames(g_xmlSettings);

  //get debug settings
  #ifdef DEBUG_ON
    g_cDebugManager.GetDebugSettings(g_xmlSettings);
  #endif //DEBUG_ON

} //LoadGameSettings

//windows functions: window procedure and winmain

/// Window procedure.
/// Handler for messages from the Windows API. 
///  \param hwnd window handle
///  \param message message code
///  \param wParam parameter for message 
///  \param lParam second parameter for message
///  \return 0 if message is handled

LRESULT CALLBACK WindowProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{

  //DEBUGPRINTF("WindowProc begin\n");

  switch(message){ //handle message

    case WM_ACTIVATEAPP: g_bActiveApp=(BOOL)wParam; break; //iconize

    case WM_DESTROY: //on exit
	  delete g_pSoundManager; //kill sound manager
      delete g_pInputManager; //ditto for input manager
      ReleaseTextures(); //release textures
	  ReleaseOffscreenSurfaces(); //release offscreen surfaces
      ReleaseD3D(); //release Direct3D
      //display error message if we aborted
      if(g_errorExit)
        MessageBox(NULL,g_errMsg,"Error",MB_OK|MB_ICONSTOP);
      //finally, quit
      PostQuitMessage(0); //this is the last thing to do on exit
      break;

    default: //default window procedure
      return DefWindowProc(hwnd,message,wParam,lParam);

  } //switch(message)

  return 0L;
} //WindowProc
                         
/************************************************************/

bool Setup2d()
{
	//
	// Create a font object and initialize it.
	//

//	Font = new CD3DFont("Times New Roman", 18, 0);

Font = new CD3DFont* [g_nMaxNumberFonts];

for (int i = 0; i < g_nMaxNumberFonts; i++)
{
	Font[i] = new CD3DFont("Times New Roman", (i + 5), 0);
	Font[i]->InitDeviceObjects( g_d3ddevice );
	Font[i]->RestoreDeviceObjects();

}
	
	return true;
}


/************************************************************/
/// Winmain. 
/// Main entry point for this application. 
///  \param hInstance handle to the current instance of this application
///  \param hPrevInstance unused
///  \param lpCmdLine unused 
///  \param nCmdShow specifies how the window is to be shown
///  \return TRUE if application terminates correctly

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,int nCmdShow){
  MSG msg; //current message
  HWND hwnd; //handle to fullscreen window

  InitXMLSettings(); //initialize XML settings reader
  LoadGameSettings(); //load settings from XML
  #ifdef DEBUG_ON
    g_cDebugManager.open(); //open debug streams, settings came from XML file
  #endif //DEBUG_ON

  //DEBUGPRINTF("WinMain - creating Window\n");
  //create fullscreen window
  hwnd=CreateDefaultWindow("Samurai",hInstance,nCmdShow);
  if(!hwnd)return FALSE; //bail if problem creating window
  g_HwndApp=hwnd; //save window handle

  //create sound manager
  g_pSoundManager = new CSoundManager(hwnd);
  g_pSoundManager->LoadSettings(); //load general settings

  //initialize graphics
  if(!InitD3D(hInstance,hwnd))
    ABORT("Unable to select graphics display mode.");
  SetD3dStates(); //set D3D states
  LoadEnvironmentTextures(); ///< set textures for ground and sky
  LoadTextures(); //create textures for images
  CreateOffscreenSurfaces(); //create surfaces
  LoadSurfaces(); //load surfaces
/********************************************************/

if(!Setup2d())
	ABORT("Unable to setup 2d font.");


/********************************************************/
  g_pInputManager = new CInputManager(hInstance, hwnd); //create input manager (after graphics)
  g_cTimer.start(); //start game timer
  if(g_bDisplayCredits)
    ChangeGameState(LOGO_GAMESTATE); //start out displaying the logo
  else ChangeGameState(PLAYING_GAMESTATE); //go right to game

  //message loop
  while(TRUE)
    if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)){ //if message waiting
      if(!GetMessage(&msg,NULL,0,0))return (int)msg.wParam; //get it
      TranslateMessage(&msg); DispatchMessage(&msg); //translate it
    }
    else if(g_bActiveApp)ProcessFrame(); else WaitMessage(); //process frame

} //WinMain

