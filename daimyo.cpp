/// \file daimyo.cpp
/// Code for the daimyoObj class
/// Copyright Jeremy Smith, 2004.
/// Updated: 11/16/04

#include <string>
#include "daimyo.h"
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

daimyoObj::daimyoObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList) // constructor
	:enemyObj("daimyo", daimyo_stand, p, HP, theWeaponList, ENEMY_FACTION)
{
	xSpeedLimit = 3;
	zSpeedLimit = 2;
	currWeapon = weaponList[0];
	theReach = 150;
	currFaction = ENEMY_FACTION;

	CountedAlready = false;
	UseCriticalHit = false;
	//DEBUGPRINTF("daimyoObj::constructor - weaponlist is 0: %d, 1: %d, current is: %d\n", weaponList[0], weaponList[1], currWeapon);
}

int daimyoObj::damageAmount()
{
	return g_cRandom.number(20, 50);
}

void daimyoObj::doAction(actionType theAction)
{

}

// run AI thing
void daimyoObj::runAI(gameObj** objectList, int maxObjects)
{
	active = true;

	if (!dead && active) // if the item isn't dead
	{
		if (g_cTimer.elapsed(lastAITime, AIDelayTime)) // if enough time has passed, run AI again
		{
			running = false;
			gameObj* target;
			for (int i = 0; i < maxObjects; i++)
			{
				target = objectList[i];
				if (target != NULL) // make sure we're not looking at an empty space
				{
					if (target->faction() == PLAYER_FACTION) // if we're looking at a player
					{
						if (hitPoints < 300) // if we're going to die soon...
						{
							int decideToRun = g_cRandom.number(0, 2);
							switch(decideToRun)
							{
								case 0:
									doWait();
									break;
								default:
									if (!target->isDead()) // make sure target isn't dead
									{
										doAttack(target);
									} // end if target is not dead
							}
							
							
						} // end if hitPoints low
						else // else we're not about to die, so...
						{
							if (!target->isDead()) // make sure target isn't dead
							{
								doAttack(target);
							} // end if target is not dead
							else // otherwise target is probably dead
							{
								
							} // end else target is dead
			
						} // end else not dying

					} // end if we're looking at a player
				} // end if we're looking at null pointer
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

void daimyoObj::stop()
{
	// something goes here
}

void daimyoObj::move()
{

	if (dead)
		lifetime--;

	////DEBUGPRINTF("playerObj::move - begin\n");
	if ((location.z > UpperZBoundary) && active)
		location.z = UpperZBoundary;
	if (location.z < LowerZBoundary)
		location.z = LowerZBoundary;

	if (active && !dead)
	{
		if (location.x > g_cObjectManager.PlayerLocation().x + playerSnapBounds)
		{
			location.x = g_cObjectManager.PlayerLocation().x + playerSnapBounds - 2;
		}
		if (location.x < g_cObjectManager.PlayerLocation().x - playerSnapBounds)
		{
			location.x = g_cObjectManager.PlayerLocation().x - playerSnapBounds + 2;
		}
	}

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
		if (jumping)
		{
			animationEnded = true;
			jumping = false;
		}
	}
	if( (location.y > yFloor) && (active) )
	{
			//DEBUGPRINTF("playerObj::move - above yFloor, current y-pos is %f, yFloor is %d\n", (float)location.y, yFloor);
			if (!dead)
			{
				jumping = true;
				currAction = ENEMY_STALL;
				loadAnimation(daimyo_jump);
			}
			fallTime++;
			location.y = (float)(25 + (location.y - .981*(fallTime^2)));
	}

	
		
	if (!dead)
	{

		if (!busy())
		{
			if( (xSpeed == 0) && (zSpeed == 0) )
			{
				//loadAnimation(main_stand);
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
						//theObjectType = main_walk;
						//typeChange = true;
						//currentFrame = 0;
					}
					else
					{
						currAction = ENEMY_WAIT;
						//currentFrame = 0;
					}
					jumping = false;
				} // end if jumping
				else
				{
					currAction = ENEMY_WAIT;
					//currentFrame = 0;
				} // end else not jumping
			} // animation not ended
			else
			{
				//typeChange = false;
			}
			break;

		case ENEMY_WALK:
			if( abs(xSpeed) <= xSpeedLimit) // if we're moving slowly...
			{
				loadAnimation(daimyo_walk);
			}
			else
			{
				loadAnimation(daimyo_walk);
			}
			break;
/*
		case PLAYER_RUN:
			//{
				//theObjectType = main_run;
				loadAnimation(main_run);
			
			//typeChange = true;
			//}
			break;

        case PLAYER_JUMP:
			//typeChange = true;
			loadAnimation(main_jump);
			jumping = true;
			//DEBUGPRINTF("playerObj::move - PLAYER_JUMP\n");
			location.y = location.y+1;
			//theObjectType = main_jump;
			currAction = PLAYER_STALL;	
			break;
*/
		case ENEMY_ATTACK_SWORD:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - ATTACK\n");
			int i;
			
			xSpeed = 0;
			zSpeed = 0;
			i = g_cRandom.number(0,1);
			switch(i)
			{
				case 0:
					loadAnimation(daimyo_slash);
					g_pSoundManager->play(SWING1_SOUND); //sword swing noise
					break;
				case 1:
					loadAnimation(daimyo_stab);
					g_pSoundManager->play(SWING2_SOUND); //sword swing noise
					break;
			} // end case i
			currAction = ENEMY_STALL;
			break;


	/*	case ENEMY_ATTACK_BOW:
			xSpeed = 0;
			zSpeed = 0;
			loadAnimation(daimyo_throw);
			currAction = ENEMY_STALL;
			break;*/

		case ENEMY_WAIT:

			loadAnimation(daimyo_stand);
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

void daimyoObj::hurt(int damage)
{
	if (!isDead())
	{
		int i; 
		hitPoints = hitPoints - damage;
		if (hitPoints > 0)
		{
			i = g_cRandom.number(0,2);
			switch(i)
			{
				case 0:
					g_pSoundManager->play(HURT1_SOUND); 
					break;
				case 1:
					g_pSoundManager->play(HURT2_SOUND); 
					break;
				case 2:
					g_pSoundManager->play(HURT3_SOUND); 
					break;
			} // end case i
		}
		else
		{
			xSpeed = 0;
			ySpeed = 0;
			zSpeed = 0;
			jumping = false;
			currAction = ENEMY_STALL;
			loadAnimation(daimyo_die);
			deathTime = g_cTimer.time();
			D3DXVECTOR3 newLocation;
			newLocation.x = location.x;
			newLocation.y = 16.0f;
			newLocation.z = location.z;
			dead = true;
			lifetime = 1000;
			isCollidable = false;
			i = g_cRandom.number(0,3);
			g_pSoundManager->play(DAIMYODIE_SOUND); 
		} // end else
	} // end if not dead	
} // end daimyoObj::hurt

void daimyoObj::doAttack(gameObj* target)
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
				violence = g_cRandom.number(0, 9);
				switch(violence) //decide to either close the distance or attack
				{
					case 0:
						theAIState = JUMP_ABOUT;
						break;
					default:
						theAIState = CLOSE_TO_TARGET;
						
				} // end switch
			} // end else we're not in range
		} // end target's not dead
	} // end current weapon is sword or spear
	else // else it's not a sword or spear... what to do?
	{
		if (!target->isDead()) // if target's not dead...
		{
			if ( (location.x < target->loc().x - 400) || (location.x > target->loc().x + 400) ) 
			{
				  theAIState = ATTACK_RANGED;
			} // end if in range
			else // else we're in range
			{
				int violence = g_cRandom.number(0, 9);
				switch(violence) //decide to either get out of melee range or switch to melee
				{
					case 0:
						theAIState = JUMP_ABOUT;
						break;
					default:
						theAIState = CLOSE_TO_TARGET;
						currWeapon = SWORD;
				} // end switch
			} // end else we're not in range
		} // end if target's not dead
		else // else target is dead, so we don't care what happens
		{
			theAIState = WAIT;
		}
	} // end else weapon is not sword or spear

	int theDir = 0;

	switch(theAIState)
	{
		case ATTACK_MELEE:
          	if (location.x > target->loc().x)
				theDirection = FACE_LEFT;
			else
				theDirection = FACE_RIGHT;

				currAction = ENEMY_ATTACK_SWORD;
				if (location.x > target->loc().x)
					theDirection = FACE_LEFT;
				else
					theDirection = FACE_RIGHT;
				if (g_cObjectManager.inRange(this, target))
					g_cObjectManager.combatManager(this, target);
		
			break;
		case ATTACK_RANGED:
			
			currWeapon = SWORD;
			break;

		case JUMP_ABOUT:
			theDir = g_cRandom.number(0, 1);
			if (theDir == 0)
			{
				xSpeed = (float)(2.0f * xSpeedLimit);
				location.y = location.y + 1.0f;
			}
			else
			{
				xSpeed = (float)(2.0f * -xSpeedLimit);
				location.y = location.y + 1.0f;
			}
			break;

		case CLOSE_TO_TARGET:
			if (location.x > target->loc().x) // if we're to the right of the target
			{
				if (location.x > target->loc().x + theReach) // if our x-loc is higher than target's
				{
					currAction = ENEMY_WALK; // move left
					xSpeed = (float)-xSpeedLimit;
				}
				if (location.z < target->loc().z - 70) // if our z-loc is lower than target's
				{
					currAction = ENEMY_WALK; // move up
					zSpeed = (float)zSpeedLimit;
				}
				if (location.z > target->loc().z + 70)  //if our z-loc is higher than target's
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
				if (location.z < target->loc().z - 70) // if our z-loc is lower than target's
				{
					currAction = ENEMY_WALK; // move up
					zSpeed = (float)zSpeedLimit;
				}
				if (location.z > target->loc().z + 70)  //if our z-loc is higher than target's
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


void daimyoObj::doWait()
{
	xSpeed = 0;
	zSpeed = 0;

}

bool daimyoObj::busy()
{
	if ( (currAction == ENEMY_WALK) || (currAction == ENEMY_WAIT) )
		return false;
	else
		return true;
}