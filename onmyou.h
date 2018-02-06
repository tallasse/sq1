/// \file onmyou.h
/// Interface for onmyou enemy object class onmyouObj
/// Copyright Jeremy Smith, 2004
/// Updated: 10/31/04

#pragma once

#include <string>
#include "object.h"
#include "characters.h"

using namespace std;

/// Onmyou enemy object

class onmyouObj: public enemyObj
{

	protected:

		void doAttack(gameObj* target);
		void doWait();

	public:

		onmyouObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList); // constructor
		int damageAmount(); // return damageAmount
		void doAction(actionType theAction);
		void runAI(gameObj** objectList, int maxObjects); // tells AI where objects are to let it react
		void stop(); // stuck on something
		void move();
		void hurt(int damage); // ow
		bool busy();
	
}; // end onmyouObj class
