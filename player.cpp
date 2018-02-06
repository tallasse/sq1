/// \file player.cpp
/// Code for the playerObj game object
/// Copyright Jeremy Smith 2004
/// Last updated 11/7/04

#include <string>
#include "object.h"
#include "player.h"
#include "defines.h" //essential defines
#include "timer.h" //game timer
#include "spriteman.h" //sprite manager
#include "random.h" // random numbers
#include "debug.h"
#include "sound.h" //for sound manager
#include "objman.h"
#include "characters.h"

using namespace std;

extern CTimer g_cTimer;
extern CRandom g_cRandom; //random number generator
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager

const int PLAYER_HEALTH = 500;

//==================================== Player Object Implementation ====================================

// Modification: 11/1/04
playerObj::playerObj(D3DXVECTOR3 p, weaponType* theWeaponList)
	:intelligentObj("player", main_stand, p, PLAYER_HEALTH, theWeaponList, PLAYER_FACTION)

//playerObj::playerObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP, weaponType* theWeaponList, factionType theCurrFaction) // constructor
//	:intelligentObj(objName, newType, p, HP, theWeaponList, theCurrFaction)
{
	attackPower = 100.0f;
	defensePower = 100.0f;
	xSpeedLimit = 4;
	zSpeedLimit = 3;
	currAction = PLAYER_STALL;
	lastAction = PLAYER_STALL;
	lastActionTime = 0;
	jumping = false;
	running = false;
	theReach = 130;
	maxHealth = PLAYER_HEALTH;
	GetCriticalPower = 0;
	UseCriticalHit = false;
	//DEBUGPRINTF("playerObj constructor\n");
}

void playerObj::move()
{
	
	if (xSpeed > 0)
		theDirection = FACE_RIGHT;
	if (xSpeed < 0)
		theDirection = FACE_LEFT;

	if (ducking)
	{
		structSize.setY(origYsize / 4);
	}
	else
	{
		structSize.setY(origYsize);
	}

	////DEBUGPRINTF("playerObj::move - begin\n");
	if (location.z > UpperZBoundary)
		location.z = UpperZBoundary;
	if (location.z < LowerZBoundary)
		location.z = LowerZBoundary;

	if (!running) // if we're walking
	{
		if (xSpeed > xSpeedLimit)
			xSpeed = (float) xSpeedLimit;
		if (xSpeed < -xSpeedLimit)
			xSpeed = (float) -xSpeedLimit;

		if (zSpeed > zSpeedLimit)
			zSpeed = (float) zSpeedLimit;
		if (zSpeed < -zSpeedLimit)
			zSpeed = (float) -zSpeedLimit;
	} // end if we're not walking
	else // else we're running
	{
		if (xSpeed > 0)
			xSpeed = 2 * (float) xSpeedLimit;
		if (xSpeed < 0)
			xSpeed = -2 * (float) xSpeedLimit;

		if (zSpeed > 0)
			zSpeed = 2 * (float) zSpeedLimit;
		if (zSpeed < 0)
			zSpeed = -2 * (float) zSpeedLimit;
	} // end else we're running



/**********************************************************************/
	
	//if moving in a positive x dir, check if possible
	if (xSpeed > 0.0f)
	{
		if (move_xPos)
			location.x = location.x + xSpeed;
		else
			xSpeed = 0.0f;
	}

	// if moving in a negative x dir, check if possible
	else if (xSpeed < 0.0f)
	{
		if (move_xNeg)
			location.x = location.x + xSpeed;
		else
			xSpeed = 0.0f;
	}

	//if moving in a positive z dir, check if possible
	if (zSpeed > 0.0f)
	{
		if (move_zPos)
			location.z = location.z + zSpeed;
		else
			zSpeed = 0.0f;
	}

	//if moving in a negative z dir, check if possible
	else if (zSpeed < 0.0f)
	{
		if (move_zNeg)
			location.z = location.z + zSpeed;
		else
			zSpeed = 0.0f;
	}

/**********************************************************************/




	//location.x = location.x + xSpeed;
	//location.z = location.z + zSpeed;

	if ( (location.y < yFloor) || !move_yNeg )
	{
		if (location.y < yFloor)
			location.y = (float) yFloor;

		fallTime = 0;
	}

	if (location.y > yFloor)
	{
			//DEBUGPRINTF("playerObj::move - above yFloor, current y-pos is %f, yFloor is %d\n", (float)location.y, yFloor);
			if (move_yNeg)
			{
				fallTime++;
				location.y = 20.0f + (location.y - 0.981f * ( fallTime^2) );
			}
	}
/*    else
	{
	    if (move_yPos)
        {   
			DEBUGPRINTF("move_yPos case\n");
          fallTime++;
          location.y = 20.0f + (location.y - 0.981f * ( fallTime^2) );
        }
	}
*/	
		
	if (!dead)
	{

		if (!busy())
		{
			if( (xSpeed == 0) && (zSpeed == 0) )
			{
				//loadAnimation(main_stand);
				currAction = PLAYER_WAIT;
			}
		}

	switch(currAction)
	{
		case PLAYER_STALL:
			if (animationEnded)
			{
				if (jumping)
				{
					if ( (xSpeed != 0) || (zSpeed != 0) )
					{
						currAction = PLAYER_WALK;
						//theObjectType = main_walk;
						//typeChange = true;
						//currentFrame = 0;
					}
					else
					{
						currAction = PLAYER_WAIT;
						//currentFrame = 0;
					}
					jumping = false;
				} // end if jumping
				else
				{
					currAction = PLAYER_WAIT;
					//currentFrame = 0;
				} // end else not jumping
				defending = false;
				ducking = false;
			} // animation not ended
			else
			{
				//typeChange = false;
			}
			break;

		case PLAYER_WALK:
			if( abs(xSpeed) < 5) // if we're moving slowly...
			{
				//theObjectType = main_walk;
				loadAnimation(main_walk);
			}
			else
			{
				loadAnimation(main_run);
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
*/
        case PLAYER_JUMP:
			//typeChange = true;
			loadAnimation(main_jump);
			jumping = true;
			//DEBUGPRINTF("playerObj::move - PLAYER_JUMP\n");
			location.y = location.y + 1.0f;
			//theObjectType = main_jump;
			currAction = PLAYER_STALL;	
			break;

		case PLAYER_ATTACK:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - ATTACK\n");
			g_cObjectManager.playerCombat();
			int i;
			i = 1 + g_cRandom.number(0,2);
			xSpeed = 0;
			zSpeed = 0;

			switch(i)
			{
				case 1:
                    //theObjectType = main_slash;
					loadAnimation(main_slash);
					g_pSoundManager->play(SWING1_SOUND); //sword swing noise
					break;
				case 2:
                    //theObjectType = main_stab;
					loadAnimation(main_stab);
					g_pSoundManager->play(SWING2_SOUND); //sword swing noise
					break;
				case 3:
                    //theObjectType = main_cut;
					loadAnimation(main_cut);
					g_pSoundManager->play(SWING3_SOUND); //sword swing noise
					break;
			} // end case i
			// do combat manager here
			currAction = PLAYER_STALL;
			break;

		case PLAYER_DEFEND:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - PLAYER_DEFEND\n");
			//theObjectType = main_defend;
			loadAnimation(main_defend);
			xSpeed = 0;
			zSpeed = 0;
			currAction = PLAYER_STALL;
			break;

		case PLAYER_DUCK:
			//typeChange = true;
			//DEBUGPRINTF("playerObj::move - PLAYER_DUCK\n");
			//theObjectType = main_duck;
			loadAnimation(main_duck);
			currAction = PLAYER_STALL;
			break;

		case SWITCH_WEAPON:
			//doAction(PLAYER_WAIT, 0, 0);
			break;

		case PLAYER_WAIT:
			//if (currAction != lastAction)
			//{
            //    typeChange = true;
			//	lastAction = currAction;
			//}
			//DEBUGPRINTF("playerObj::move - PLAYER_WAIT\n");
			//theObjectType = main_stand;
			if( (theObjectType == main_hi_blow) || (theObjectType == main_lo_blow) )
			{
				if (animationEnded)
				{
					loadAnimation(main_stand);
				}
			}
			else
			{
				loadAnimation(main_stand);
			}
			xSpeed = 0;
			zSpeed = 0;
//			currAction = PLAYER_STALL;
			break;
		
	} // end case

	} // end if not dead
	else
	{
			xSpeed = 0;
			zSpeed = 0;
	}

}

void playerObj::doAction(actionType theAction, int newXSpeed, int newZSpeed, bool fromJoystick)
{

	lastAction = currAction;

	if ( (theAction != currAction) || (theAction == PLAYER_WALK) ) // if the action is different, or if we're walking along...
	{
		//DEBUGPRINTF("doAction - doing it\n");
		if (!dead) // if we're not dead
		{
			if(theAction != PLAYER_WALK) // if we're doing something other than walking
			{
				
				if(g_cTimer.time() > lastActionTime + 350) // if enough time has elapsed...
                {
		            lastActionTime = g_cTimer.time(); // last action time = now
					if (fromJoystick) // if the input is from the joystick...
					{
						//DEBUGPRINTF("doAction - action coming in from Joystick\n");
						//if( (currAction != PLAYER_STALL) )  // if action isn't stalling to finish animation
						//{
							currAction = theAction; // change actions
							//typeChange = true; // load new settings for new animation
						//}
						//else
						//{
							//DEBUGPRINTF("doAction - state is PLAYER_STALL, not changing animation now.\n");
						//}
						//DEBUGPRINTF("doAction - timer is available, doing action %d from joystick\n", theAction);
					} // end if from Joystick
					else // otherwise the input is from the keyboard
					{
						//typeChange = true; // load new animation
						currAction = theAction; // change action
						//DEBUGPRINTF("doAction - different action coming from keyboard\n");
					} // end else from keyboard
				} // end if timer
				else
				{
					//DEBUGPRINTF("doAction - input coming in too fast\n");
				}
			} // end if the action isn't walking
			else
			{
				if( (currAction != PLAYER_WALK) )  // if action isn't stalling to finish animation
				{
					currAction = theAction; // change actions
					//typeChange = true; // load new settings for new animation
					//DEBUGPRINTF("doAction - starting walking animation\n");
				}
				else
				{
					//DEBUGPRINTF("doAction - already in walking animation.\n");
				}
				//DEBUGPRINTF("doAction - action is walking\n");
			} // else the action is walking
			
			//DEBUGPRINTF("doAction - applied x: %f and z: %f speed changes, new speeds are x: %f, z: %f\n", newXSpeed, newZSpeed, xSpeed, zSpeed);
		} // end if we're not dead
		else
		{
			//DEBUGPRINTF("doAction - dead people don't move\n");
		}
	} // end if action different
	else
	{
		if(theAction == PLAYER_WALK)
		{
			currAction = theAction;
		}
		//DEBUGPRINTF("doAction - action same, no changes\n");
	}


	//	if (!(abs((xSpeed + newXSpeed)) > xSpeedLimit)) // if we're not going to go above the speed limit...
//	{
	xSpeed = xSpeed + newXSpeed; // increase the x speed
//	}
//	if (!(abs((zSpeed + newZSpeed)) > zSpeedLimit)) // if we're not going to go above the speed limit...
//	{
	zSpeed = zSpeed + newZSpeed; // increase the z speed
//	}
	
	//DEBUGPRINTF("doAction - done with this doAction\n");
}

int playerObj::damageAmount()
{
	int yourDamage = (int)(g_cRandom.number(10,30) * (attackPower / 100.0f));
	return yourDamage;// something
}


int playerObj::getMaxHP()
{
	return maxHealth;
}


void playerObj::hurt(int damage)
{
	if (!isDead())
	{
			int i; 
			if (defending)
			{
				if (damage > 1)
				{
					damage = damage / 10;
					g_pSoundManager->play(DEFEND_SOUND); //loop plane sound
				}
			}
			if (damage > 1)
			{
				float thePercentage = 100.0f / defensePower;
				damage = (int)(damage * thePercentage);
				if (damage < 1)
				{
					damage = 1;
				}	
			}
			hitPoints = hitPoints - damage;
			if (hitPoints > maxHealth) // if over limit
			{
				hitPoints = maxHealth; // reduce to max
			}
			if (hitPoints > 0)
			{
				if  (damage >= 0)
				{
					i = g_cRandom.number(0,2);
					switch(i)
					{
						case 0:
							g_pSoundManager->play(HURT1_SOUND); //loop plane sound
							break;
						case 1:
							g_pSoundManager->play(HURT2_SOUND); //loop plane sound
							break;
						case 2:
							g_pSoundManager->play(HURT3_SOUND); //loop plane sound
							break;
					} // end case i

					i = g_cRandom.number(0,1);
					if( (!defending) && (!ducking) )
					{
						switch(i)
						{
							case 0:
								loadAnimation(main_hi_blow);
								break;
							case 1:
								loadAnimation(main_lo_blow);
								break;
						} // end case i
					} // end if not defending or ducking
				} // end if damage > 0
			}
			else
			{
				xSpeed = 0;
				ySpeed = 0;
				zSpeed = 0;
				currAction = PLAYER_STALL;
				//typeChange = true;
				//theObjectType = main_die;
				loadAnimation(main_die);
				deathTime = g_cTimer.time();
				dead = true;
				i = g_cRandom.number(0,3);
				switch(i)
				{
					case 0:
						g_pSoundManager->play(DEATH1_SOUND); //loop plane sound
						break;
					case 1:
						g_pSoundManager->play(DEATH2_SOUND); //loop plane sound
						break;
					case 2:
						g_pSoundManager->play(DEATH3_SOUND); //loop plane sound
						break;
					case 3:
						g_pSoundManager->play(DEATH4_SOUND); //loop plane sound
						break;
				} // end case i
			} // end else
	} // end if not dead
} // end playerObj hurt


void playerObj::addAttack(int attackAdd)
{
	attackPower = attackPower + (float)attackAdd;
} //end addAttack


void playerObj::addDefense(int defenseAdd)
{
	defensePower = defensePower + (float)defenseAdd;
	if( (defensePower > 100.0f) && (defensePower < 120.0f) )
	{
		theMaterial.Ambient.r = 0.8f;
		theMaterial.Ambient.g = 0.8f;
		theMaterial.Ambient.b = 1.0f;

		theMaterial.Diffuse.r = 0.8f;
		theMaterial.Diffuse.g = 0.8f;
		theMaterial.Diffuse.b = 1.0f;
	}
	if(defensePower > 120.0f)
	{
		theMaterial.Ambient.r = 1.0f;
		theMaterial.Ambient.g = 0.7f;
		theMaterial.Ambient.b = 0.7f;

		theMaterial.Diffuse.r = 1.0f;
		theMaterial.Diffuse.g = 0.7f;
		theMaterial.Diffuse.b = 0.7f;
	}
} //end addDefense

int playerObj::getAttack()
{
	//int theAttackPower = (int)(attackPower * 100.0f);
	return (int)attackPower;
}

int playerObj::getDefense()
{	
	/*int theDefensePower = (int)(defensePower * 100.0f);
	int offset = 100 - theDefensePower;
	theDefensePower = theDefensePower + (2 * offset);*/
	return (int)defensePower;
}

void playerObj::stopX()
{
	xSpeed = 0; // stop!
}

void playerObj::stopY()
{
	ySpeed = 0; // stop!
}

void playerObj::stopZ()
{
	zSpeed = 0; // stop!
}

void playerObj::stopAll()
{
	stopX();
	stopY();
	stopZ();
}

bool playerObj::busy()
{
	if ( (currAction == PLAYER_WALK) || (currAction == PLAYER_WALK) || (currAction == PLAYER_WAIT) )
		return false;
	else
		return true;
}

void playerObj::resetLocation()
{
	location.x = -500.0f;
	currAction = PLAYER_STALL;
	lastAction = PLAYER_STALL;
	loadAnimation(main_stand);
	stopAll();
}