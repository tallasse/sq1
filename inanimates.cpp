/// \file inanimates.cpp
/// Code for the game object classes:
/// environmentObj, backgroundObj, 
/// nearBackgroundObj, foregroundObj
/// Copyright Jeremy Smith 2004
/// 11/10/04

#include <string>
#include "object.h"
#include "defines.h" //essential defines
#include "timer.h" //game timer
#include "spriteman.h" //sprite manager
#include "random.h" // random numbers
#include "debug.h"
#include "sound.h" //for sound manager
#include "objman.h"
#include "inanimates.h"

using namespace std;

extern CTimer g_cTimer;
extern CRandom g_cRandom; //random number generator
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager


//==================================== Environment Object Implementation ====================================

environmentObj::environmentObj(string objName, ObjectType newType, D3DXVECTOR3 p)
	:gameObj(objName, newType, p)
{
	// //DEBUGPRINTF("environmentObj constructor\n");
} // environmentObj constructor

void environmentObj::move() // sets z-location to int value
{
	
} // end environmentObj move method


//==================================== Background Object Implementation ====================================

backgroundObj::backgroundObj(string objName, ObjectType newType, D3DXVECTOR3 p)
	:gameObj(objName, newType, p)
{
	//location.y = yFloor;
	origX = location.x;
	theMaterial.Emissive.r = 0.0f; // original color
	theMaterial.Emissive.g = 0.0f; // original color
	theMaterial.Emissive.b = 0.0f; // original color
	theMaterial.Emissive.a = 1.0f; // original color

	theMaterial.Specular.r = 0.0f; // original color
	theMaterial.Specular.g = 0.0f; // original color
	theMaterial.Specular.b = 0.0f; // original color
	theMaterial.Specular.a = 1.0f; // original color
	
	//DEBUGPRINTF("backgroundObj x location: %d\n", (int)p.x);
} // backgroundObj constructor

void backgroundObj::move() // sets z-location to int value
{

	D3DXVECTOR3 playerLoc;
	playerLoc = g_cObjectManager.PlayerLocation();

	location.x = origX + playerLoc.x;
	
} // end backgroundObj move method

//==================================== near-Background Object Implementation ====================================

nearBackgroundObj::nearBackgroundObj(string objName, ObjectType newType, D3DXVECTOR3 p)
	:backgroundObj(objName, newType, p)
{
	origX = location.x;
	distance = fabs(1000.0f - location.z);
	distance = distance / 100.0f;
	if (distance > 9.0f) 
		distance = 9.0f; // keep things from moving faster than you
	if( (distance < 1.0f) && (distance > 0.0f) )
		distance = 1.0f;
	float tempYFloor;
	tempYFloor = (float)yFloor - (distance * 20.0);
	location.y = tempYFloor;

//	yFloor = (structSize.y() / 2);
//	location.y = yFloor;
	//DEBUGPRINTF("nearBackgroundObj structsize.y : %d, location.y: %f \n", structSize.y(), location.y);

	if (theName != "wall")
	{
		int pickDir = g_cRandom.number(0, 1);
		switch(pickDir)
		{
			case 1:
				theDirection = FACE_LEFT;
				break;
			default:
				theDirection = FACE_RIGHT;
		}	
	} // end if not a wall
	else // else a wall
	{
		theDirection = FACE_RIGHT;
	}
	//DEBUGPRINTF("nearBackgroundObj distance is %f / 100 = %f \n", location.z, distance);
} // nearBackgroundObj constructor

void nearBackgroundObj::move() // moves thing
{
	//DEBUGPRINTF("nearBackgroundObj moving\n");
	D3DXVECTOR3 playerLoc;
	playerLoc = g_cObjectManager.PlayerLocation();
	//DEBUGPRINTF("nearBackgroundObj::move - got Player Location\n");
	float playerX;
	playerX = g_cObjectManager.playerXSpeed();

	
	if (distance == 0.0f) //if it is on the closest layer (z = 1000), don't move it
	{
		xSpeed = 0;
	}
	else
	{
        xSpeed = playerX / (10 - distance);
	}
	//DEBUGPRINTF("nearBackgroundObj xSpeed is playerX: %f / distance: %f = xSpeed: %f\n", playerX, distance, xSpeed);

	location.x = location.x + xSpeed;

	if (location.x > playerLoc.x + (2 * g_nScreenWidth))
	{
		location.x = location.x - (4 * g_nScreenWidth);
		if (theName != "wall")
		{
			int pickDir = g_cRandom.number(0, 1);
			switch(pickDir)
			{
				case 1:
					theDirection = FACE_LEFT;
					break;
				default:
					theDirection = FACE_RIGHT;
			}	
		} // end if not a wall
		else // else a wall
		{
			theDirection = FACE_RIGHT;
		}
		//DEBUGPRINTF("wrapping background to left by %d\n", (2 * g_nScreenWidth));
	}
	if (location.x < playerLoc.x - (2 * g_nScreenWidth))
	{
		location.x = location.x + (4 * g_nScreenWidth);
		if (theName != "wall")
		{
			int pickDir = g_cRandom.number(0, 1);
			switch(pickDir)
			{
				case 1:
					theDirection = FACE_LEFT;
					break;
				default:
					theDirection = FACE_RIGHT;
			}	
		} // end if not a wall
		else // else a wall
		{
			theDirection = FACE_RIGHT;
		}
		//DEBUGPRINTF("wrapping background to right by %d\n", (2 * g_nScreenWidth));
	}
	//DEBUGPRINTF("nearBackgroundObj done moving\n");
} // end nearBackgroundObj move

//==================================== Foreground Object Implementation ====================================

foregroundObj::foregroundObj(string objName, ObjectType newType, D3DXVECTOR3 p)
	:backgroundObj(objName, newType, p)
{
	location.y = yFloor;
	origX = location.x;
	int pickDir = g_cRandom.number(0, 1);
	switch(pickDir)
	{
		case 1:
			theDirection = FACE_LEFT;
			break;
		default:
			theDirection = FACE_RIGHT;
	}
	//distance = (2000.0 - location.z) / 100.0;
	//location.y = yFloor - (100 - (distance * 10));
	
	//DEBUGPRINTF("nearBackgroundObj distance is %f / 100 = %f \n", location.z, distance);
} // foregroundObj constructor

void foregroundObj::move() // moves thing
{
	D3DXVECTOR3 playerLoc;
	playerLoc = g_cObjectManager.PlayerLocation();

	if (location.x > playerLoc.x + (2 * g_nScreenWidth))
	{
		location.x = location.x - (4 * g_nScreenWidth);
		int pickDir = g_cRandom.number(0, 1);
		switch(pickDir)
		{
			case 1:
				theDirection = FACE_LEFT;
				break;
			default:
				theDirection = FACE_RIGHT;
		}
		//DEBUGPRINTF("wrapping background to left by %d\n", (2 * g_nScreenWidth));
	}
	if (location.x < playerLoc.x - (2 * g_nScreenWidth))
	{
		location.x = location.x + (4 * g_nScreenWidth);
		int pickDir = g_cRandom.number(0, 1);
		switch(pickDir)
		{
			case 1:
				theDirection = FACE_LEFT;
				break;
			default:
				theDirection = FACE_RIGHT;
		}
		//DEBUGPRINTF("wrapping background to right by %d\n", (2 * g_nScreenWidth));
	}
} // end foregroundObj move