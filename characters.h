/// \file characters.h
/// interface for the game object classes:
/// intelligentObj, enemyObj 
/// Copyright Jeremy Smith 2004
/// 11/10/04

#pragma once

#include <string>
#include "object.h"

using namespace std;

class intelligentObj: public gameObj
{

	protected:
		
		int hitPoints; // hp
		weaponType* weaponList; // list of weapons
		weaponType currWeapon; // weapon currently in use
		bool jumping;
		int jumpCount;

	public:

        intelligentObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP, weaponType* theWeaponList, factionType theCurrFaction); // constructor
		int getHP(); // returns remaining HP as an int		
		weaponType weapon(); // returns weapon currently in use	
		void hurt(int damage); // subtracts int value from current HP
		virtual int damageAmount(); // return damageAmount

}; // end intelligentObj class


class enemyObj: public intelligentObj
{

	protected:
		
		aiState theAIState;
		
		void setAIState(aiState newState);
		int lastAITime; ///< Last time AI was used.
		int AIDelayTime; ///< Time until AI next used.
		bool active;


	public:

        enemyObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP, weaponType* theWeaponList, factionType theCurrFaction); // constructor
		int lastAIrun(); // returns last time AI was run
		virtual void stop(); // indicates that object is stuck, reads in which direction the obstacle is in to make choice of how to move

}; // end enemyObj class