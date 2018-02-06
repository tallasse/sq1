/// \file player.h
/// Interface for player object
/// Copyright Jeremy Smith 2004
/// Last Modified 11/7/04

#pragma once

#include <string>
#include "object.h"
#include "characters.h"

using namespace std;


class playerObj: public intelligentObj
{
	
	protected:

		int lastActionTime;
		actionType lastAction;
		int maxHealth;
		float attackPower;
		float defensePower;

	public:

		playerObj(D3DXVECTOR3 p, weaponType* theWeaponList);
		bool busy(); ///< too busy to listen to stupid joystick?
		void move(); // moves player
		void doAction(actionType theAction, int newXSpeed, int newZSpeed, bool fromJoystick=false); // take input and do something
		int damageAmount(); // return damageAmount
		int getMaxHP(); // return max health
		void hurt(int damage);
		void addAttack(int attackAdd);
		void addDefense(int defenseAdd);
		int getDefense();
		int getAttack();
		void stopX();
		void stopY();
		void stopZ();
		void stopAll();
		void resetLocation();
		//void move(directionType direction, int speed) // moves player in the direction indicated.
		//void jump() // starts jump count and affects y motion
		//•	etc, etc, for all player abilities

}; // end playerObj class