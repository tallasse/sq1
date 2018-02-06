/// \file bosssamurai.cpp
/// Code for the dragonObj class
/// Copyright Jeremy Smith, 2004.
/// Updated: 10/31/04

#include <string>
#include "dragon.h"
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

dragonObj::dragonObj(D3DXVECTOR3 p, int HP, weaponType* theWeaponList) // constructor
	:enemyObj("dragon", dragon_stand, p, HP, theWeaponList, ENEMY_FACTION)
{
	xSpeedLimit = 0;
	zSpeedLimit = 0;
	currWeapon = weaponList[0];
	theReach = 250;
	currFaction = ENEMY_FACTION;
	CountedAlready = false;
	
	float red = 1.0f;
	float blue = 1.0f;
	float green = 1.0f;
	float alpha = 1.0f;

	theMaterial.Ambient.r = red; // original color
	theMaterial.Ambient.g = green; // original color
	theMaterial.Ambient.b = blue; // original color
	theMaterial.Ambient.a = alpha; // original color
		
	theMaterial.Diffuse.r = red; // original color
	theMaterial.Diffuse.g = green; // original color
	theMaterial.Diffuse.b = blue; // original color
	theMaterial.Diffuse.a = alpha; // original color
	theDirection = FACE_LEFT;

	alreadyCalledForHelp = false;
	
	//DEBUGPRINTF("dragonObj::constructor - weaponlist is 0: %d, 1: %d, current is: %d\n", weaponList[0], weaponList[1], currWeapon);
}

int dragonObj::damageAmount()
{
	return g_cRandom.number(20, 40);
}

void dragonObj::doAction(actionType theAction)
{

}

// run AI thing
void dragonObj::runAI(gameObj** objectList, int maxObjects)
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
							if (!target->isDead()) // make sure target isn't dead
							{
								doAttack(target);
							} // end if target is not dead
							else // otherwise target is probably dead
							{
								doWait();
							} // end else target is dead

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

void dragonObj::stop()
{
	// something goes here
}

void dragonObj::move()
{

	if (dead)
		lifetime--;

	////DEBUGPRINTF("playerObj::move - begin\n");
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
			loadAnimation(dragon_stand);
			break;

		case ENEMY_ATTACK_SWORD:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - ATTACK\n");
			int i;
			i = 1 + g_cRandom.number(0,2);
			xSpeed = 0;
			zSpeed = 0;

			loadAnimation(dragon_attack);
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
			loadAnimation(dragon_shoot);
			currAction = ENEMY_STALL;
			break;

		case ENEMY_WAIT:
			loadAnimation(dragon_stand);
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

void dragonObj::hurt(int damage)
{
	if (!isDead())
	{
		int i; 
		hitPoints = hitPoints - damage;
		if (hitPoints > 0)
		{
			g_pSoundManager->play(DRAGONSCREAM_SOUND); 
		}
		else
		{
			xSpeed = 0;
			ySpeed = 0;
			zSpeed = 0;
			currAction = ENEMY_STALL;
			//typeChange = true;
			//theObjectType = sam_die;
			loadAnimation(dragon_die);
			D3DXVECTOR3 newLocation;
			newLocation = location;
			newLocation.y = 60.0f;
			newLocation.z = location.z - 50.0f;
			g_cObjectManager.create(OBSTACLE_TYPE, "monk", newLocation);
			newLocation = location;
			for (int b = 0; b < 50; b++)
			{
				newLocation.x = location.x + (float)g_cRandom.number(-150, 150);
				newLocation.y = location.y + (float)g_cRandom.number(-50, 0);
				newLocation.z = location.z + (float)g_cRandom.number(-75, -25);
				g_cObjectManager.generateParticles(newLocation, 1, FIRE_TYPE, COLOR_PARTICLE, 1000, 0, -1, 0.6f, 0.6f, 0.6f, 1.0f);
			}
			deathTime = g_cTimer.time();
			dead = true;
			lifetime = 1000;
			isCollidable = false;
			i = g_cRandom.number(0,3);
			g_pSoundManager->play(DRAGONDIE_SOUND); 
		} // end else
	} // end if not dead	
} // end dragonObj::hurt

void dragonObj::doAttack(gameObj* target)
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
				theAIState = ATTACK_RANGED;
				currWeapon = ARROW;
			} // end else we're not in range
		} // end target's not dead
	} // end current weapon is sword or spear
	else // else it's not a sword or spear... what to do?
	{
		if (!target->isDead()) // if target's not dead...
		{
			
			int violence = g_cRandom.number(0, 4);
			switch(violence) //decide to either get out of melee range or switch to melee
			{
				case 0:
					currWeapon = SWORD;
					break;
				default:
					theAIState = ATTACK_RANGED;
					
			} // end switch
			
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
					} // end else to right of target
			} // end switch violence
			break;
	} // end switch theAIState
	
} // end doAttack



void dragonObj::doWait()
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

bool dragonObj::busy()
{
	if ( (currAction == ENEMY_WALK) || (currAction == ENEMY_WAIT) )
		return false;
	else
		return true;
}