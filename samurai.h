/// \file samurai.h
/// Interface for samurai enemy object class samuraiObj
/// Copyright Jeremy Smith, 2004
/// Updated: 11/15/04

#pragma once

#include <string>
#include "object.h"
#include "characters.h"

using namespace std;

/// Samurai enemy object

class samuraiObj: public enemyObj
{

	protected:

		void doEscape(gameObj* target);
		void doAttack(gameObj* target);
		void doWait();

	public:

		samuraiObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList); // constructor
		int damageAmount(); // return damageAmount
		void doAction(actionType theAction);
		void runAI(gameObj** objectList, int maxObjects); // tells AI where objects are to let it react
		void stop(); // stuck on something
		void move();
		void hurt(int damage); // ow
		bool busy();
	
}; // end samuraiObj class
