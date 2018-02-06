/// \file onmyou.cpp
/// Code for the onmyouObj class
/// Copyright Jeremy Smith, 2004.
/// Updated: 11/14/04

#include <string>
#include "onmyou.h"
#include "timer.h" //game timer
#include "random.h" //for random number generator
#include "DEBUG.h"
#include "sound.h" //for sound manager
#include "objman.h"
#include "characters.h"

using namespace std;

extern CTimer g_cTimer;  //game timer
extern CRandom g_cRandom; //random number generator
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager
extern int g_difficultyLevel; // game difficulty

onmyouObj::onmyouObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList) // constructor
	:enemyObj("onmyou", onmyou_stand, p, HP, theWeaponList, ENEMY_FACTION)
{
	xSpeedLimit = 4;
	zSpeedLimit = 4;
	currWeapon = weaponList[0];
	theReach = 250;
	AIDelayTime = 1000 / g_difficultyLevel;

	CountedAlready = false;
	UseCriticalHit = false;
	// for screenshot mode
	//AIDelayTime = 500;
} // end onmyouObj constructor

int onmyouObj::damageAmount()
{
	return g_cRandom.number(15, 50);
} // end method onmyouObj::damageAmount


void onmyouObj::doAction(actionType theAction)
{

}


void onmyouObj::runAI(gameObj** objectList, int maxObjects)
{

	active = true;

	if (!dead) // if the item isn't dead
	{
		if (g_cTimer.elapsed(lastAITime, AIDelayTime)) // if enough time has passed, run AI again
		{
			gameObj* target;
			for (int i = 0; i < maxObjects; i++)
			{
				target = objectList[i];
				if (target != NULL) // make sure we're not looking at an empty space
				{
					if (target->faction() == PLAYER_FACTION) // if we're looking at a player
					{
						if (!target->isDead()) // make sure target isn't dead
						{
							doAttack(target);
						} // end if target is not dead
						else // otherwise target is probably dead
						{
							doWait();
						} // end else target is dead
			
					} // end if we're looking at a player
				} // end if we're not looking at null pointer
			} // end for i
		} // end if time has elapsed
		else // not ready for another pass
		{
			//do nothing when it's not your turn yet
		} // end else timer not up
	} // end if dead
	else // else item is dead
	{
		//no action if dead for this thing
	} // end else item is dead


}

void onmyouObj::stop()
{
	// something goes here
}

void onmyouObj::move()
{

	if (dead)
		lifetime--;

	if(lifetime == 920) // if dead, and fell to ground
	{
		D3DXVECTOR3 shatterLoc;
		shatterLoc.x = location.x;
		shatterLoc.z = location.z;
		shatterLoc.y = 10;
		g_pSoundManager->play(SHATTER_SOUND); //shattering noise
		g_cObjectManager.generateParticles(shatterLoc, 30, SPARK_TYPE, COLOR_PARTICLE, 500, 0, -1, 0.43f, 0.49f, 1.0f, 0.8f);
	} // end if dead and fell to ground

	if (active && !dead)
	{
		if (location.x >= g_cObjectManager.PlayerLocation().x + playerSnapBounds)
		{
			location.x = g_cObjectManager.PlayerLocation().x + playerSnapBounds - 2;
		}
		if (location.x <= g_cObjectManager.PlayerLocation().x - playerSnapBounds)
		{
			location.x = g_cObjectManager.PlayerLocation().x - playerSnapBounds + 2;
		}
	}
	//DEBUGPRINTF("onmyouObj::move - begin\n");
	if (location.z > UpperZBoundary)
		location.z = (float)UpperZBoundary;
	if (location.z < LowerZBoundary)
		location.z = (float)LowerZBoundary;

	if (!running) // if we're walking
	{
		if (xSpeed > xSpeedLimit)
			xSpeed = (float)xSpeedLimit;
		if (xSpeed < -xSpeedLimit)
			xSpeed = (float)-xSpeedLimit;

		if (zSpeed > zSpeedLimit)
			zSpeed = (float)zSpeedLimit;
		if (zSpeed < -zSpeedLimit)
			zSpeed = (float)-zSpeedLimit;
	} // end if we're not walking
	else // else we're running
	{
		if (xSpeed > 0)
			xSpeed = (float)(2 * xSpeedLimit);
		if (xSpeed < 0)
			xSpeed = (float)(-2 * xSpeedLimit);

		if (zSpeed > 0)
			zSpeed = (float)(2 * zSpeedLimit);
		if (zSpeed < 0)
			zSpeed = (float)(-2 * zSpeedLimit);
	} // end else we're running

	location.x = location.x + xSpeed;
	location.z = location.z + zSpeed;

	if (location.y < yFloor)
	{
		location.y = (float)yFloor;
		fallTime = 0;
	}
	if (location.y > yFloor)
	{
			//DEBUGPRINTF("playerObj::move - above yFloor, current y-pos is %f, yFloor is %d\n", (float)location.y, yFloor);
			fallTime++;
			location.y = (float)(20 + (location.y - .981*(fallTime^2)));
	}

	
		
	if (!dead)
	{

		if (!busy())
		{
			if( (xSpeed == 0) && (zSpeed == 0) )
			{
				currAction = ENEMY_WAIT;
			}
		}

		switch(currAction)
		{
		case ENEMY_STALL:
			if (animationEnded)
			{
				if (jumping)
				{
					if ( (xSpeed != 0) || (zSpeed != 0) )
					{
						currAction = ENEMY_WALK;
					}
					else
					{
						currAction = ENEMY_WAIT;
					}
					jumping = false;
				} // end if jumping
				else
				{
					currAction = ENEMY_WAIT;					
				} // end else not jumping
			} // animation not ended
			else
			{
				// nothing
			}
			break;

		case ENEMY_WALK:
			if( abs(xSpeed) <= xSpeedLimit) // if we're moving slowly...
			{
				loadAnimation(onmyou_stand);
			}
			else
			{
				loadAnimation(onmyou_stand);
			}
			break;

		case ENEMY_ATTACK_SWORD:
			//DEBUGPRINTF("onmyouObj::move - ATTACK\n");
			xSpeed = 0;
			zSpeed = 0;
			loadAnimation(onmyou_attack);
			g_pSoundManager->play(SPARKS_SOUND); //sword swing noise
			currAction = ENEMY_STALL;
			break;


		case ENEMY_ATTACK_BOW:
			xSpeed = 0;
			zSpeed = 0;
			loadAnimation(onmyou_cast);
			currAction = ENEMY_STALL;
			break;

		case ENEMY_WAIT:
			loadAnimation(onmyou_stand);
			xSpeed = 0;
			zSpeed = 0;
			break;
		
		} // end switch
	} // end if not dead
	else
	{
			xSpeed = 0;
			zSpeed = 0;
	}

	if (xSpeed > 0)
		theDirection = FACE_RIGHT;
	if (xSpeed < 0)
		theDirection = FACE_LEFT;

} // end move

void onmyouObj::hurt(int damage)
{
	if (!isDead())
	{
		hitPoints = hitPoints - damage;
		if (hitPoints > 0)
		{
			g_pSoundManager->play(ONMYOUHIT_SOUND); 
		}
		else
		{
			xSpeed = 0;
			ySpeed = 0;
			zSpeed = 0;
			currAction = ENEMY_STALL;
			//typeChange = true;
			//theObjectType = sam_die;
			loadAnimation(onmyou_die);
			deathTime = g_cTimer.time();
			dead = true;
			lifetime = 1000;
			isCollidable = false;
			g_cObjectManager.generateParticles(location, 30, GLOW_TYPE, COLOR_PARTICLE, 80, 0, -1, 0.0f, 0.0f, 0.0f, 0.8f);
			g_pSoundManager->play(ONMYOUDIE_SOUND);
		} // end else
	} // end if not dead
} // end onmyouObj::hurt

void onmyouObj::doAttack(gameObj* target)
{
	int violence;

	if(currWeapon == SWORD || currWeapon == SPEAR) // if the current weapon is a sword or spear...
	{
		if (!target->isDead()) // if target's not dead...
		{
			if( g_cObjectManager.inRange(this, target) )  // if we're in range of the player, attack
			{
				theAIState = ATTACK_MELEE;
			} // end if in range
			else // else we're not in range
			{
				violence = g_cRandom.number(0, 5);
				switch(violence) //decide to either close the distance or attack
				{
					case 0:
					case 1:
					case 2:
						theAIState = CLOSE_TO_TARGET;
						break;
					default:
						theAIState = ATTACK_RANGED;
						currWeapon = ARROW;
				} // end switch
			} // end else we're not in range
		} // end target's not dead
	} // end current weapon is sword or spear
	else // else it's not a sword or spear... what to do?
	{
		if (!target->isDead()) // if target's not dead...
		{
			if( !g_cObjectManager.inRange(this, target) )  // if we're NOT in range of the player, attack
			{
                theAIState = ATTACK_RANGED;
				//g_cObjectManager.combatManager(this, objectList[i]);
			} // end if in range
			else // else we're in range
			{
				int violence = g_cRandom.number(0, 5);
				switch(violence) //decide to either get out of melee range or switch to melee
				{
					case 0:
					case 1:
					case 2:
					case 3:
						theAIState = CLOSE_TO_TARGET;
						currWeapon = SWORD;
						break;						

					default:
						theAIState = GET_OUT_OF_RANGE;
				} // end switch
			} // end else we're not in range
		} // end if target's not dead
		else // else target is dead, so we don't care what happens
		{
			theAIState = WAIT;
		}
	} // end else weapon is not sword or spear


	switch(theAIState)
	{
		case ATTACK_MELEE:
          	if (location.x > target->loc().x)
				theDirection = FACE_LEFT;
			else
				theDirection = FACE_RIGHT;

			violence = g_cRandom.number(0, 4);
			switch(violence)
			{
				case 0:
					currWeapon = ARROW;
					break;
				default:
					currAction = ENEMY_ATTACK_SWORD;
					if (location.x > target->loc().x)
						theDirection = FACE_LEFT;
					else
						theDirection = FACE_RIGHT;
					if (g_cObjectManager.inRange(this, target))
						g_cObjectManager.combatManager(this, target);
			}
			

			break;
		case ATTACK_RANGED:
			if (location.x > target->loc().x)
				theDirection = FACE_LEFT;
			else
				theDirection = FACE_RIGHT;
			violence = g_cRandom.number(0, 8);
			switch(violence)
			{
				case 0:
					currWeapon = SWORD;
					break;
				case 1:
				case 2:
				case 3:
					currWeapon = ARROW;
					break;
				default:
					currAction = ENEMY_ATTACK_BOW;		
					if (location.x < target->loc().x) // if we're to the left of the target
					{
						int zDir;
						if( (location.z > target->loc().z - 100) && (location.z < target->loc().z + 100) ) // if within z-range
						{
							zDir = 0; // no z-change
						} // end if within z range
						else // else at extreme angle
						{
							if (location.z > target->loc().z) // if we're above target
							{
								zDir = g_cRandom.number(-1, -10); // shoot down
							} // end if above target
							else // else we're below target
							{
								zDir = g_cRandom.number(1, 10); // shoot up
							} // end else we're above target
						} // end else at extreme angle
						int chance = g_cRandom.number(0, 300);
						chance = chance - 150;
						int xDistance = (int)((target->loc().x + chance) - location.x); // give randomness to guess
						float newYSpeed = 10.0f / (float)g_cRandom.number(11, 100);
						D3DXVECTOR3 newLoc = location;
						newLoc.x = location.x + 75.0f;
						g_cObjectManager.fireBall(newLoc, 15, newYSpeed, (float)zDir);
					} // end if we're to left of target
					else // else we're to right of target
					{
						int zDir;
						if( (location.z > target->loc().z - 100) && (location.z < target->loc().z + 100) ) // if within z-range
						{
							zDir = 0; // no z-change
						} // end if within z range
						else // else at extreme angle
						{
							if (location.z > target->loc().z) // if we're above target
							{
								zDir = g_cRandom.number(-1, -10); // shoot down
							} // end if above target
							else // else we're below target
							{
								zDir = g_cRandom.number(1, 10); // shoot up
							} // end else we're above target
						} // end else at extreme angle

						int chance = g_cRandom.number(0, 300);
						chance = chance - 150;
						int xDistance = (int)((target->loc().x + chance) - location.x); // give randomness to guess
						float newYSpeed = 10.0f / (float)g_cRandom.number(11, 100);
						D3DXVECTOR3 newLoc = location;
						newLoc.x = location.x - 75.0f;
						g_cObjectManager.fireBall(newLoc, -15, newYSpeed, (float)zDir);
					}
					//nothing
			} // end switch violence
			break;
		case GET_OUT_OF_RANGE:
			if (location.x > target->loc().x)  // if we're to the right of the target
			{
				if (location.x < target->loc().x + 200) // if we're within 300 px to the right
				{
					currAction = ENEMY_WALK;
					xSpeed = (float)xSpeedLimit;
				} // end if we're within 300 px to the right
			} // end if we're to the right of the target
			else // else we're to the left of the target
			{
				if (location.x > target->loc().x - 200) // if we're within 300 px to the left
				{
					currAction = ENEMY_WALK;
					xSpeed = (float)-xSpeedLimit;
				} // end if we're within 300px to the left
			} // end else we're to the left
			break;

		case CLOSE_TO_TARGET:
			if (location.x > target->loc().x) // if we're to the right of the target
			{
				if (location.x > target->loc().x + theReach) // if our x-loc is higher than target's
				{
					currAction = ENEMY_WALK; // move left
					xSpeed = (float)-xSpeedLimit;
				}
				if (location.z < target->loc().z - theReach) // if our z-loc is lower than target's
				{
					currAction = ENEMY_WALK; // move up
					zSpeed = (float)zSpeedLimit;
				}
				if (location.z > target->loc().z + theReach)  //if our z-loc is higher than target's
				{
					currAction = ENEMY_WALK; // move down
					zSpeed = (float)-zSpeedLimit;
				}
			} // end if we're to the right
			else // else we're to the left of the target
			{
				if (location.x < target->loc().x - theReach) // if our x-loc is lower than target's
				{
					currAction = ENEMY_WALK; // move right
					xSpeed = (float)xSpeedLimit;
				}
				if (location.z < target->loc().z - theReach) // if our z-loc is lower than target's
				{
					currAction = ENEMY_WALK; // move up
					zSpeed = (float)zSpeedLimit;
				}
				if (location.z > target->loc().z + theReach)  //if our z-loc is higher than target's
				{
					currAction = ENEMY_WALK; // move down
					zSpeed = (float)-zSpeedLimit;
				}
			} // end if we're to the right
			break;
//		default:
			//nothing
	} // end switch theAIState
	
} // end doAttack

void onmyouObj::doWait()
{
	xSpeed = 0;
	zSpeed = 0;

}

bool onmyouObj::busy()
{
	if ( (currAction == ENEMY_WALK) || (currAction == ENEMY_WAIT) )
		return false;
	else
		return true;
}