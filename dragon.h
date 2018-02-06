/// \file dragon.h
/// Interface for dragon enemy object class dragonObj
/// Copyright Jeremy Smith, 2004
/// Updated: 11/15/04

#pragma once

#include <string>
#include "object.h"
#include "characters.h"

using namespace std;

/// dragon enemy object

class dragonObj: public enemyObj
{

	protected:

		void doAttack(gameObj* target);
		void doWait();
		bool alreadyCalledForHelp;

	public:

		dragonObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList); // constructor
		int damageAmount(); // return damageAmount
		void doAction(actionType theAction);
		void runAI(gameObj** objectList, int maxObjects); // tells AI where objects are to let it react
		void stop(); // stuck on something
		void move();
		void hurt(int damage); // ow
		bool busy();
	
}; // end dragonObj class
