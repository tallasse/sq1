/// \file daimyo.h
/// Interface for daimyo enemy object class daimyoObj
/// Copyright Jeremy Smith, 2004
/// Updated: 11/16/04

#pragma once

#include <string>
#include "object.h"
#include "characters.h"

using namespace std;

/// daimyo enemy object

class daimyoObj: public enemyObj
{

	protected:

		void doAttack(gameObj* target);
		void doWait();

	public:

		daimyoObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList); // constructor
		int damageAmount(); // return damageAmount
		void doAction(actionType theAction);
		void runAI(gameObj** objectList, int maxObjects); // tells AI where objects are to let it react
		void stop(); // stuck on something
		void move();
		void hurt(int damage); // ow
		bool busy();
	
}; // end daimyoObj class
