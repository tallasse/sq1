/// \file bosssamurai.cpp
/// Code for the bossSamuraiObj class
/// Copyright Jeremy Smith, 2004.
/// Updated: 10/31/04

#include <string>
#include "bosssamurai.h"
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

bossSamuraiObj::bossSamuraiObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList) // constructor
	:enemyObj("samurai", sam_stand, p, HP, theWeaponList, ENEMY_FACTION)
{
	xSpeedLimit = 3;
	zSpeedLimit = 3;
	currWeapon = weaponList[0];
	theReach = 130;
	currFaction = ENEMY_FACTION;
	CountedAlready = false;
	
	float red = 1.0f;
	float blue = 0.5f;
	float green = 0.5f;
	float alpha = 1.0f;

	theMaterial.Ambient.r = red; // original color
	theMaterial.Ambient.g = green; // original color
	theMaterial.Ambient.b = blue; // original color
	theMaterial.Ambient.a = alpha; // original color
		
	theMaterial.Diffuse.r = red; // original color
	theMaterial.Diffuse.g = green; // original color
	theMaterial.Diffuse.b = blue; // original color
	theMaterial.Diffuse.a = alpha; // original color

	alreadyCalledForHelp = false;
	
	//DEBUGPRINTF("bossSamuraiObj::constructor - weaponlist is 0: %d, 1: %d, current is: %d\n", weaponList[0], weaponList[1], currWeapon);
}

int bossSamuraiObj::damageAmount()
{
	return g_cRandom.number(10, 20);
}

void bossSamuraiObj::doAction(actionType theAction)
{

}

// run AI thing
void bossSamuraiObj::runAI(gameObj** objectList, int maxObjects)
{
	active = true;
	if (!dead) // if the item isn't dead
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
						if (hitPoints < 150) // if we're going to die soon...
						{
							int decideToRun = g_cRandom.number(0, 3);
							switch(decideToRun)
							{
								case 0:
									if (!target->isDead()) // make sure target isn't dead
									{
										doAttack(target);
									} // end if target is not dead
									break;
								case 1:
									doWait();
									break;
								default:
									doEscape(target);
							}
							
							
						} // end if hitPoints low
						else // else we're not about to die, so...
						{
							int decideToRun = g_cRandom.number(0, 3);
							switch(decideToRun)
							{
								case 0:
								case 1:
									doEscape(target);
								default:
									if (!target->isDead()) // make sure target isn't dead
									{
										doAttack(target);
									} // end if target is not dead
									break;
									
							}
			
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

void bossSamuraiObj::stop()
{
	// something goes here
}

void bossSamuraiObj::move()
{

	if (dead)
		lifetime--;

	////DEBUGPRINTF("playerObj::move - begin\n");
	if (location.z > UpperZBoundary)
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
				//theObjectType = main_walk;
				if (currWeapon == SWORD)
				{
					//theObjectType = sam_walk;
					loadAnimation(sam_walk);
				}
				else
				{
					//theObjectType = bow_walk;
					loadAnimation(bow_walk);
				}
			}
			else
			{
				if (currWeapon == SWORD)
				{
					//theObjectType = sam_walk;
					loadAnimation(sam_walk);
				}
				else
				{
					//theObjectType = bow_walk;
					loadAnimation(bow_walk);
				}
				//loadAnimation(main_run);
			}
			break;

		case ENEMY_ATTACK_SWORD:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - ATTACK\n");
			int i;
			i = 1 + g_cRandom.number(0,2);
			xSpeed = 0;
			zSpeed = 0;

			loadAnimation(sam_slash);
			switch(i)
			{
				case 1:
					g_pSoundManager->play(SWING1_SOUND); //sword swing noise
					break;
				case 2:
					g_pSoundManager->play(SWING2_SOUND); //sword swing noise
					break;
				case 3:
    				g_pSoundManager->play(SWING3_SOUND); //sword swing noise
					break;
			} // end case i
			currAction = ENEMY_STALL;
			break;


		case ENEMY_ATTACK_BOW:
			xSpeed = 0;
			zSpeed = 0;
			loadAnimation(bow_shoot);
			currAction = ENEMY_STALL;
			break;

		case ENEMY_WAIT:

			if (currWeapon == SWORD)
			{
				loadAnimation(sam_stand);
			}
			else
			{
				loadAnimation(bow_stand);
			}
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

void bossSamuraiObj::hurt(int damage)
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
			currAction = ENEMY_STALL;
			//typeChange = true;
			//theObjectType = sam_die;
			loadAnimation(sam_die);
			deathTime = g_cTimer.time();
			dead = true;
			lifetime = 1000;
			isCollidable = false;
			i = g_cRandom.number(0,3);
			switch(i)
			{
				case 0:
					g_pSoundManager->play(DEATH1_SOUND); 
					break;
				case 1:
					g_pSoundManager->play(DEATH2_SOUND); 
					break;
				case 2:
					g_pSoundManager->play(DEATH3_SOUND); 
					break;
				case 3:
					g_pSoundManager->play(DEATH4_SOUND); 
					break;
			} // end case i
		} // end else
	} // end if not dead	
} // end bossSamuraiObj::hurt

void bossSamuraiObj::doAttack(gameObj* target)
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
						theAIState = ATTACK_RANGED;
						currWeapon = ARROW;
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
			if ( (location.x < target->loc().x - 600) || (location.x > target->loc().x + 600) ) 
			{
				  theAIState = ATTACK_RANGED;
			} // end if in range
			else // else we're in range
			{
				int violence = g_cRandom.number(0, 7);
				switch(violence) //decide to either get out of melee range or switch to melee
				{
					case 0:
					case 1:
					case 2:
					case 3:
						theAIState = GET_OUT_OF_RANGE;
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


	switch(theAIState)
	{
		case ATTACK_MELEE:
          	if (location.x > target->loc().x)
				theDirection = FACE_LEFT;
			else
				theDirection = FACE_RIGHT;

			violence = g_cRandom.number(0, 9);
			switch(violence)
			{
				case 0:
					currWeapon = ARROW;
					break;
				case 1:
				case 2:
				case 3:
					currWeapon = SWORD;
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
			violence = g_cRandom.number(0, 9);
			switch(violence)
			{
				case 0:
				case 1:
				case 2:
					currWeapon = SWORD;
					break;
				case 3:
				case 4:
				case 5:
					currWeapon = ARROW;
					break;
				default:
					if (location.x > target->loc().x)
						theDirection = FACE_LEFT;
					else
						theDirection = FACE_RIGHT;

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
						int xDistance = ((int)target->loc().x + chance) - (int)location.x; // give randomness to guess
						float newYSpeed = (float)(abs(xDistance) / 100);
						float newXSpeed = (float)(xDistance / 50);
						D3DXVECTOR3 newLoc = location;
						newLoc.x = location.x + 85;
						g_cObjectManager.fireArrow(newLoc, newXSpeed, newYSpeed, (float)zDir);
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
						int xDistance = ((int)target->loc().x + chance) - (int)location.x; // give randomness to guess
						float newYSpeed = (float)(abs(xDistance) / 100);
						float newXSpeed = (float)(xDistance / 50);
						D3DXVECTOR3 newLoc = location;
						newLoc.x = location.x - 85;
						g_cObjectManager.fireArrow(newLoc, newXSpeed, newYSpeed, (float)zDir);
					} // end else to right of target
			} // end switch violence
			break;
		case GET_OUT_OF_RANGE:
			if (location.x > target->loc().x)  // if we're to the right of the target
			{
				if (location.x < target->loc().x + 600) // if we're within 600 px to the right
				{
					currAction = ENEMY_WALK;
					xSpeed = (float)xSpeedLimit;
				} // end if we're within 600 px to the right
			} // end if we're to the right of the target
			else // else we're to the left of the target
			{
				if (location.x > target->loc().x - 600) // if we're within 600 px to the left
				{
					currAction = ENEMY_WALK;
					xSpeed = (float)-xSpeedLimit;
				} // end if we're within 600px to the left
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

void bossSamuraiObj::doEscape(gameObj* target)
{
	running = true;
	if (location.x > target->loc().x) // if we're to the right
	{
		currAction = ENEMY_WALK; // run right
		xSpeed = 2.0f * (float)xSpeedLimit;
	} // end if we're to the right
	if (location.x < target->loc().x) // if we're to the left
	{
		currAction = ENEMY_WALK; // run left
		xSpeed = -2.0f * (float)xSpeedLimit;
	} // end if we're to the left
} // end bossSamuraiObj::doEscape


void bossSamuraiObj::doWait()
{
	xSpeed = 0;
	zSpeed = 0;

	if (!alreadyCalledForHelp)
	{
		g_pSoundManager->play(HORN2_SOUND);
		alreadyCalledForHelp = true;
		for (int i = 0; i < g_cRandom.number(2, 6); i++)
		{
			D3DXVECTOR3 p = location;
			p.x = p.x + 500.0f;
			p.z = p.z + g_cRandom.number(-250, 250);
			p.y = (float)78;
			g_cObjectManager.create(ENEMY_TYPE, "samurai", p);
		}


	}
}

bool bossSamuraiObj::busy()
{
	if ( (currAction == ENEMY_WALK) || (currAction == ENEMY_WAIT) )
		return false;
	else
		return true;
}