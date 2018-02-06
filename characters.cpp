/// \file characters.cpp
/// code for the game object classes:
/// intelligentObj, enemyObj 
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
#include "characters.h"

using namespace std;

extern CTimer g_cTimer;
extern CRandom g_cRandom; //random number generator
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager

// initializes a base game object.  All it has is a location and a collision flag.

//==================================== Intelligent Object Implementation ====================================

intelligentObj::intelligentObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP, weaponType* theWeaponList, factionType theCurrFaction)
	:gameObj(objName, newType, p)
{
	hitPoints = HP;
	weaponList = theWeaponList;
	currFaction = theCurrFaction;
	currWeapon = weaponList[0];
	jumping = false;
	jumpCount = 0;
	// //DEBUGPRINTF("intelligentObj constructor\n");
} // end intelligentObj constructor

int intelligentObj::getHP()
{
	return hitPoints;
}

weaponType intelligentObj::weapon()
{
	return currWeapon;
}

void intelligentObj::hurt(int damage) // subtracts int value from current HP
{
	hitPoints = hitPoints - damage;
}

int intelligentObj::damageAmount()
{
	return 0;
}


//==================================== Enemy Object Implementation ====================================

enemyObj::enemyObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP, weaponType* theWeaponList, factionType theCurrFaction) // constructor
	:intelligentObj(objName, newType, p, HP, theWeaponList, theCurrFaction)
{
	theAIState = WAIT;
	lastAITime = g_cTimer.time();
	AIDelayTime = 500;
	currAction = ENEMY_WAIT;
	isIntelligent = true;
	active = false;
	// //DEBUGPRINTF("enemyObj constructor\n");
}

void enemyObj::setAIState(aiState newState)
{
	theAIState = newState;
}

int enemyObj::lastAIrun()
{
	return lastAITime;
}

void enemyObj::stop()
{
	// nothing
}