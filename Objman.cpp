/// \file objman.cpp
/// Code for the object manager class CObjectManager.
/// Copyright Ian Parberry, 2004.
/// Last updated August 25, 2004.

#include <ctime>
#include <string> // for strings
#include "objman.h"
#include "defines.h" //essential defines
#include "random.h" // random numbers
#include "timer.h" // game timer
#include "player.h" // for player objects
#include "samurai.h" // for samurai objects
#include "bosssamurai.h" // for samurai objects
#include "ninja.h" // for ninja objects
#include "bossninja.h" // for ninja objects
#include "majou.h" // for majou boss
#include "daimyo.h" // for daimyo boss
#include "onmyou.h" // for onmyou boss
#include "dragon.h" // for dragon boss
#include "characters.h" // for character objects
#include "particles.h" // for particle objects
#include "inanimates.h" // for background/foreground objects
#include "inventory.h" ///< for inventory objects
#include "object.h"
#include "debug.h"
#include "sound.h" //for sound manager


using namespace std;

extern CTimer g_cTimer; //game timer
extern CRandom g_cRandom; //random number generator
extern CSoundManager* g_pSoundManager; //sound manager
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern int g_difficultyLevel;
extern int g_gameLevel;
extern IDirect3DDevice9* g_d3ddevice; //graphics device 
extern weatherType g_currentWeather; ///< what's the weather like?
extern timeOfDay g_currentTime; ///< what time of day is it?
extern locationType g_levelLocation; ///< where does the level take place?

ObjectManager::ObjectManager() //constructor 
{
	////DEBUGPRINTF("ObjectManager:: constructor begin\n");
	m_nMaxCount = MAX_OBJECTS;
	m_nCount = 0;
	m_nPlayerIndex = -1;

	//-------------------------------------------------------------------------

	m_nBossIndex = -2;
	
	//-------------------------------------------------------------------------
	
	int i = 0;

	drawOrder = new int[m_nMaxCount];
	
	for(i = 0; i < m_nMaxCount; i++)
	{
		drawOrder[i] = NULL;
	}

	m_pObjectList = new gameObj*[m_nMaxCount]; //object list

	for(i = 0; i < m_nMaxCount; i++)
	{
		m_pObjectList[i] = NULL;
	}
	m_numLights = 1;

	nextSoundTime = 2000;
	lastSoundTime = -1;

	nextMusicTime = g_cRandom.number(0, 1500);
	lastMusicTime = -1;

	m_soundLoopStarted = false;
	m_lightningTimer = 0;
	m_lightningFlash = false;
	////DEBUGPRINTF("ObjectManager:: constructor - nextSoundTime: %d, lastSoundTime: %d\n", nextSoundTime, lastSoundTime);
} // end constructor


ObjectManager::~ObjectManager() //destructor
{
	// //DEBUGPRINTF("ObjectManager:: destructor begin\n");
	for(int i = 0; i < m_nMaxCount; i++) //for each object
	{
	    delete m_pObjectList[i]; //delete it
	} // end for
	
	delete[] drawOrder; //delete drawOrder;

	delete[]m_pObjectList; //delete object list

} // end destructor


int ObjectManager::create(baseObjectType object, string name, D3DXVECTOR3 location)
{

	////DEBUGPRINTF("ObjectManager:: create begin\n");

	weaponType newWeaponList[2];
	
	if(m_nCount < m_nMaxCount) //if room, create object
	{

		for(int i = 0; m_pObjectList[i] != NULL; i++); //find first free slot

		// //DEBUGPRINTF("ObjectManager:: create - traversing to slot %d in \n ObjectList while looking for free spot for ne object\n", i);
		switch(object)
		{
			case PLAYER_TYPE:
				if (name == "player")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new playerObj(location, newWeaponList);
					m_nPlayerIndex = i;
					////DEBUGPRINTF("ObjectManager:: create - creating new PLAYER_TYPE object at index %d, referencing location %p\n", i, m_pObjectList[i]);					
					//m_pObjectList[i] = new playerObj("player", main_stand, location, (100 * (g_difficultyLevel)), newWeaponList, PLAYER_FACTION);
				}
				if (name == "inventory")
				{
					theInventory = new inventoryObj();
					////DEBUGPRINTF("ObjectManager:: create - created inventory object at index %d\n", i);					
					
				}
				break;

			case OBSTACLE_TYPE:
				if (name == "rock")
				{
					m_pObjectList[i] = new obstacleObj("rock", rock, location, 1000);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "smallbush")
				{
					m_pObjectList[i] = new obstacleObj("bush", smallbush, location, 100);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "bigrice")
				{
					m_pObjectList[i] = new deliciousObj("food", bigrice, location, false, -400);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "smallrice")
				{
					m_pObjectList[i] = new deliciousObj("food", smallrice, location, false, -200);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "armor")
				{
					m_pObjectList[i] = new deliciousObj("armor", armor, location, false, 10);
					// //DEBUGPRINTF("ObjectManager::create - armor created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "katana")
				{
					m_pObjectList[i] = new deliciousObj("katana", katana, location, false, 10);
					// //DEBUGPRINTF("ObjectManager::create - sword created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "lantern")
				{
					m_pObjectList[i] = new obstacleObj("lantern", lantern, location, 20);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "cave")
				{
					m_pObjectList[i] = new obstacleObj("cave", cave, location, -1);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "empress")
				{
					m_pObjectList[i] = new obstacleObj("empress", empress, location, -1);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "monk")
				{
					m_pObjectList[i] = new obstacleObj("monk", monk, location, -1);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "barrier")
				{
					m_pObjectList[i] = new obstacleObj("barrier", barrier, location, -1);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				break;

			case ENEMY_TYPE:
				if (name == "samurai")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new samuraiObj(location, (100 * (g_difficultyLevel)), newWeaponList);
					// //DEBUGPRINTF("ObjectManager:: create - creating new SAMURAI_TYPE object at location %p\n", m_pObjectList[i]);
					////DEBUGPRINTF("ObjectManager::create - samurai created\n");
				}
				if (name == "bosssamurai")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new bossSamuraiObj(location, (300 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
					// //DEBUGPRINTF("ObjectManager:: create - creating new SAMURAI_TYPE object at location %p\n", m_pObjectList[i]);
					////DEBUGPRINTF("ObjectManager::create - samurai created\n");
				}
				if (name == "bossninja")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new bossNinjaObj(location, (200 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
					// //DEBUGPRINTF("ObjectManager:: create - creating new SAMURAI_TYPE object at location %p\n", m_pObjectList[i]);
					////DEBUGPRINTF("ObjectManager::create - samurai created\n");
				}
				if (name == "ninja")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new ninjaObj(location, (150 * (g_difficultyLevel)), newWeaponList);
				}
				if (name == "daimyo")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new daimyoObj(location, (1500 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
				}
				if (name == "majou")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new majouObj(location, (500 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
				}
				if (name == "dragon")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new dragonObj(location, (500 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
				}
				if (name == "onmyou")
				{
					newWeaponList[0] = SWORD;
					newWeaponList[1] = ARROW;
					m_pObjectList[i] = new onmyouObj(location, (500 * (g_difficultyLevel)), newWeaponList);
					m_nBossIndex = i;
				}
				break;

			case BACKGROUND_TYPE:
				if (name == "mountains")
				{
					m_pObjectList[i] = new backgroundObj("mountains", mountains, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "castle")
				{
					m_pObjectList[i] = new backgroundObj("tree1", castle, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "grass")
				{
					m_pObjectList[i] = new nearBackgroundObj("grass", grass, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "wall")
				{
					m_pObjectList[i] = new nearBackgroundObj("wall", insidewall, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "cottage")
				{
					m_pObjectList[i] = new nearBackgroundObj("cottage", cottage, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "burned_cottage")
				{
					m_pObjectList[i] = new nearBackgroundObj("cottage", burned_cottage, location);
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "tree")
				{
					int pickOne = g_cRandom.number(0, 1);
					switch(pickOne)
					{
						case 0:
                            m_pObjectList[i] = new nearBackgroundObj("tree1", tree1, location);
							break;
						case 1:
							m_pObjectList[i] = new nearBackgroundObj("tree1", tree2, location);
							break;
					} // end switch
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "smalltree")
				{
					int pickOne = g_cRandom.number(0, 1);
					switch(pickOne)
					{
						case 0:
						case 1:
							m_pObjectList[i] = new nearBackgroundObj("smalltree", smalltree1, location);
							break;
					} // end switch
						////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				break;

			case FOREGROUND_TYPE:
				
				if (name == "grass")
				{
					m_pObjectList[i] = new foregroundObj("grass", grass, location);
					////DEBUGPRINTF("ObjectManager::create - mountains created at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
				}
				if (name == "campfire")
				{
					////DEBUGPRINTF("ObjectManager::create - spawning fire at location x= %d, y= %d, z= %d\n", (int)location.x, (int)location.y, (int)location.z);
					int randomFire = g_cRandom.number(40, 50);
					spawnFire(location, randomFire);
				}
				break;

		} // end switch

		// //DEBUGPRINTF("ObjectManager:: create ended succesfully, room was available, object index is %d\n", m_nCount);
		if (name != "campfire" && name != "inventory")
		{
            m_nCount++; //one more object
		}

		return i;

	} // end if room
	else
	{
		//DEBUGPRINTF("ObjectManager:: create ended unsuccesfully, no room available\n");
		return -1; //no room
	} // end else

} // end create method


void ObjectManager::killBoss()
{
	m_pObjectList[m_nBossIndex]->hurt(1000);
}

/// Move all game objects.
void ObjectManager::move()
{

	//	//DEBUGPRINTF("ObjectManager::ambientSounds - running ambientSounds\n");
	ambientSounds();

	////DEBUGPRINTF("ObjectManager::move - run Collision Detection\n");

	CollisionManager(); // fully functional collision detection
	//collisionDetection(); // demo mode collision detection

	////DEBUGPRINTF("ObjectManager::move - cull old objects\n");
	cull();
	////DEBUGPRINTF("ObjectManager::move - beginning\n");


    for(int i = 0; i < m_nMaxCount; i++) //for each slot
	{
		////DEBUGPRINTF("ObjectManager::move - looping through Object List, on object %d\n", i);
		if(m_pObjectList[i] != NULL) //if there's an object there
		{
			////DEBUGPRINTF("ObjectManager::move - Object %d\ isn't NULL\n", i);
			if(m_pObjectList[i]->NPC()) //if intelligent, tell it about objects
			{
				if( (m_pObjectList[i]->loc().x > (PlayerLocation().x - playerSnapBounds)) && 
					(m_pObjectList[i]->loc().x < (PlayerLocation().x + playerSnapBounds)) )  // if within range of game bounds
				{
					////DEBUGPRINTF("ObjectManager::move - telling object %d to run AI\n", i);
					m_pObjectList[i]->runAI(m_pObjectList, m_nMaxCount);
					m_pObjectList[i]->move(); //move it
				} // end if in bounds
				else // else out of game bounds
				{
					m_pObjectList[i]->move(); //move it
					////DEBUGPRINTF("ObjectManager::move - object out of bounds for now\n");
				} // end else out of bounds
			}
			else // else not intelligent
			{
				////DEBUGPRINTF("ObjectManager::move - object not intelligent, moving it anyway\n");
				m_pObjectList[i]->move(); //move it
			}
		
		} // end if object exists
    
	} // end for loop 


//============================================set draw order by z-location =================================
    delete[] drawOrder; //delete old draw order

	drawOrder = new int[m_nMaxCount]; // instantiate new draw order
	for (int i = 0; i < m_nMaxCount; i++)
	{
		drawOrder[i] = NULL;
	}
	
	int zLoc = 0;
	int indexNum = 0;

	int* zLocList = new int[m_nMaxCount];
	for (int i = 0; i < m_nMaxCount; i++)
	{
		if (m_pObjectList[i] != NULL)
		{
			zLocList[i] = (int)(m_pObjectList[i]->loc().z);
		}
		else
		{
			zLocList[i] = 0;
		}
	}

	for (int i = 0; i < m_nMaxCount; i++)
	{
		for (int j = 0; j < m_nMaxCount; j++)
		{
			if (zLocList[j] > zLoc)
			{
				zLoc = zLocList[j];
				indexNum = j;
			}
		}// end for j
		////DEBUGPRINTF("ObjectManager::move - drawOrder entry %d is object %d\n", i, indexNum);
		drawOrder[i] = indexNum;
		zLocList[indexNum] = 0;
		zLoc = 0;
		indexNum = 0;
	} // end for i

	delete[] zLocList;
	////DEBUGPRINTF("ObjectManager::move - end move method\n");
	
} // end move method


/// Draw all game objects

void ObjectManager::draw()
{ 
	for(int i = 0; i < m_nMaxCount; i++) //for each object slot
	{
		if (drawOrder[i] != NULL)
		{
			////DEBUGPRINTF("ObjectManager::draw - drawing object %d\n", i);
			m_pObjectList[drawOrder[i]]->draw(); //draw it
		} // end if not null
	} // end for
}// end draw method


/// Set the plane's index.
/// This lets the object manager know who the player is, so that the
/// camera can keep it in the center of the screen, and the NPCs
/// (Non Player Characters) can use their AI to avoid it.

void ObjectManager::SetPlayerIndex()
{ 
	//m_nPlayerIndex = m_nCount - 1;
	//DEBUGPRINTF("ObjectManager:: SetPlayerIndex - player index set to %d\n", m_nPlayerIndex);
}


void ObjectManager::resetPlayerLocation()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	if (thePlayer != NULL)
	{
		thePlayer->resetLocation();
	}
}

/// Euclidean distance function.
/// Given two points in 2D space, returns the distance between them.
/// \param x1 x-coordinate of point 1
/// \param y1 y-coordinate of point 1
/// \param x2 x-coordinate of point 2
/// \param y2 y-coordinate of point 2
/// \return Euclidean distance between (x1,y1) and (x2,y2)
float ObjectManager::distance(float x1, float y1, float x2, float y2)
{
  const float fWorldWidth = 2.0f * (float)g_nScreenWidth; //world width
  float x = (float)fabs(x1-x2), y = (float)fabs(y1-y2); //x and y distance

  return (float)sqrt(x*x + y*y);  //return result
}


/// Distance between objects.
/// \param first index of an object in the object list
/// \param second index of another object in the object list
/// \return distance between the two objects

float ObjectManager::distance(int first, int second){ //return distance between objects

  //bail if bad index
  if(first < 0 || first >= m_nMaxCount)return -1;
  if(second < 0 || second >= m_nMaxCount)return -1;

  //get coordinates of centers
  float x1, y1, x2, y2; //coordinates of objects

  x1 = m_pObjectList[first]->loc().x;
  y1 = m_pObjectList[first]->loc().y;
  x2 = m_pObjectList[second]->loc().x;
  y2 = m_pObjectList[second]->loc().y;

  //return distance between coordinates
  return distance(x1, y1, x2, y2);
}


/// Remove an object from the object list.
/// Assumes that there is an object there to be deleted.
/// \param index index of the object to be deleted.

void ObjectManager::kill(int index)
{ //remove object
	m_nCount--;
	delete m_pObjectList[index];
	m_pObjectList[index] = NULL;
	////DEBUGPRINTF("ObjectManager::kill - deleting object %d\n", index);
}


/// Cull old objects.
/// Run through the objects in the object list and compare their age to
/// their life span. Kill any that are too old. Immortal objects are
/// flagged with a negative life span, so ignore those.

void ObjectManager::cull() //cull old objects
{

  gameObj *object;

  for(int i = 0; i < m_nMaxCount; i++) //for each object
  {

    object = m_pObjectList[i]; //current object

    if(object != NULL) //if there's really an object there
	{
      //died of old age
      if((object->lifeRemaining() < 1) && (object->lifeRemaining() != -1))
        kill(i); //...then kill it 
	} // end if
	
  } // end for

} // end cull method

/*
void ObjectManager::collisionDetection()
{
	// empty for now
	gameObj* testObj;
	gameObj* tempObj;
	sizeMatrix testSize;
	sizeMatrix tempSize;
	float xDist, yDist, zDist;
	float xSize, ySize, zSize;

	for (int i = 0; i < m_nMaxCount; i++)
	{
		if(m_pObjectList[i] != NULL)
		{
			testObj = m_pObjectList[i];

			for (int j = 0; j < m_nMaxCount; j++)
			{
				if(m_pObjectList[j] != NULL)
				{
					tempObj = m_pObjectList[j];
					if( (tempObj->collidable()) && (testObj->collidable()) ) // if both objects are collidable
					{
						testSize = testObj->size();
						tempSize = tempObj->size();
						xSize = (float)(testSize.x() + tempSize.x()) / 2.0f;
						ySize = (float)(testSize.y() + tempSize.y()) / 2.0f;
						zSize = (float)(testSize.z() + tempSize.z()) / 2.0f;

						xDist = (float)abs(testObj->loc().x - tempObj->loc().x);
						yDist = (float)abs(testObj->loc().y - tempObj->loc().y);
						zDist = (float)abs(testObj->loc().z - tempObj->loc().z);

						if( (xDist < xSize) && (zDist < zSize) && (yDist < ySize) ) //if the distance of one object from the other is less than the width of both objects combined, it's a hit.
						{
							if (tempObj->dangerous()) // if temp object is dangerous
							{
								if (tempObj != testObj) // make sure not killing ourselves
								{
									if (tempObj->delicious()) // if it's food
									{  
										if (testObj->faction() == PLAYER_FACTION) // if it's a player on the food
										{
											if (addToInventory(tempObj))
											{
												m_pObjectList[j] = NULL;
												m_nCount--;
											}
											////DEBUGPRINTF("ObjectManager::combatManager - delicious object collision\n");
										} // end if player took food
										else // else it's not a player
										{
											////DEBUGPRINTF("ObjectManager::combatManager - attacker is faction %d, attackee is faction %d, no food for you!\n", attacker->faction(), attackee->faction());
											// no food for you!
										} // end else it's not food
									} // end if it's food
									else // else it's not food
									{
										combatManager(tempObj, testObj); // do the damage
									} // end else it's not food
						
								} // end if not killing ourselves
							} // end if dangerous
							else // else it's not dangerous
							{
								/*
								if (tempObj != testObj) // make sure not killing ourselves
								{
									if (tempObj->NPC() || tempObj->name() == "player")
									{
										if( (xDist < xSize) && (zDist < zSize) && (yDist < ySize) )
										{
											if (tempObj->loc().x > testObj->loc().x)
											{
												tempObj->adjustLocation((xSize - xDist), 0, 0);
											}
											else
											{
												tempObj->adjustLocation(-(xSize - xDist), 0, 0);
											}
											
										}

										if( (xDist < xSize) && (zDist < zSize) && (yDist < ySize) )
										{
											if (tempObj->loc().z > testObj->loc().z)
											{
												tempObj->adjustLocation(0, 0, (zSize - zDist));
											}
											else
											{
												tempObj->adjustLocation(0, 0, -(zSize - zDist));
											}											
										}
									}
								} // end if not killing self
								
							} // end else thing
						} // end if within range

					} // end if both objects are collidable

				} // end if object j not null

			} // end for j

		} // end if object i not null
	
	} // end for i

} // end COllision Detection
*/

/// Reset.
/// Resets the object manager to initial conditions. This code is needed to make the code
/// re-entrant so that we can re-enter the game engine from the menu screen.
void ObjectManager::Reset()
{

	//reset Object Manager member variables
	g_d3ddevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_nCount = 2;
	//m_nCount = 0;
	//m_nPlayerIndex = -1;
	//delete object list
	//-------------------------------------------------------------------------
  	m_nBossIndex = -2;
	//-------------------------------------------------------------------------
	for (int i = 0; i < m_nMaxCount; i++)
	{
		//for multiple level system
		if(i != m_nPlayerIndex)
		{
			delete m_pObjectList[i];
			m_pObjectList[i] = NULL;
		} // end if not player or inventory
		
		//delete m_pObjectList[i];
		//m_pObjectList[i] = NULL;
	} // end for i
	nextSoundTime = 2000;
	lastSoundTime = -1;
	m_soundLoopStarted = false;
	m_lightningTimer = 0;
	m_lightningFlash = false;
} // end reset method



void ObjectManager::ClearAll()
{
	g_d3ddevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	//reset Object Manager member variables
	m_nCount = 0;
	m_nPlayerIndex = -1;
	m_nBossIndex = -2;
	for (int i = 0; i < m_nMaxCount; i++)
	{
		delete m_pObjectList[i];
		m_pObjectList[i] = NULL;
	} // end for i
	delete theInventory;
	nextSoundTime = 2000;
	lastSoundTime = -1;
	m_soundLoopStarted = false;
	m_lightningTimer = 0;
	m_lightningFlash = false;
} // end reset method




void ObjectManager::Pause()
{
	// empty for now

} // end pause method

//-------------------------------------------------------------------------

int ObjectManager::DeadEnemy()
{
	int DeadObjects=0; //number of dead enemies

	for(int i = 0; i < m_nMaxCount; i++) //for each slot
	{
		if(m_pObjectList[i] != NULL) //if there's an object there
		{	
			if(m_pObjectList[i]->theObjectType == rock) //if rock type game object
			{
				//do nothing
			}
			else if(m_pObjectList[i]->theObjectType == blood) //if blood type game object
			{
				//do nothing
			}
			else if(m_pObjectList[i]->theObjectType == smallbush) //if blood type game object
			{
				//do nothing
			}
			else if(m_pObjectList[i]->theObjectType == smoke) //if blood type game object
			{
				//do nothing
			}
			else
			{
				if(m_pObjectList[i]->isDead() == true)
				{
					DeadObjects++; //count dead objects of enemy types
					////DEBUGPRINTF("ObjectManager::DeadEnemy - dead objects %d\n", DeadObjects);

				}
			}
		}
	}
	
return DeadObjects;
}

bool ObjectManager::PlayerWon(int MaxEnemies)
{
	int TotalDeadObjects=0; //number of dead enemies

	TotalDeadObjects = DeadEnemy();

	if(TotalDeadObjects >= MaxEnemies)
	{
		return true;
	}
	else
	{
		return false;
	}
} // end PlayerWon method

bool ObjectManager::PlayerLose()
{
	if (m_pObjectList[m_nPlayerIndex]->isDead() == true)
	{
		if (g_cTimer.elapsed(m_pObjectList[m_nPlayerIndex]->deathTime, 5000))
		{
			return true;
		}
	}
		return false;

} // end PlayerLose method


bool ObjectManager::BossDead()
{
	if (m_pObjectList[m_nBossIndex]->isDead() == true)
	{
		if (g_cTimer.elapsed(m_pObjectList[m_nBossIndex]->deathTime, 5000))
		{
			return true;
		} // end if timer
	} // end if dead
	
	return false;

	// end if player is dead
} // end PlayerWon method

//-------------------------------------------------------------------------

D3DXVECTOR3 ObjectManager::PlayerLocation()
{
	////DEBUGPRINTF("ObjectManager::PlayerLocation run\n");
	if (m_pObjectList[m_nPlayerIndex] != NULL)
	{
		return m_pObjectList[m_nPlayerIndex]->loc();
	}
	else
	{
		//DEBUGPRINTF("ObjectManager::PlayerLocation - referencing NULL object!\n");
		D3DXVECTOR3 zeroVector(0,0,0);
		return zeroVector;
	}
}

int ObjectManager::playerHealth()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	return thePlayer->getHP();
} 


int ObjectManager::playerPower()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	return thePlayer->GetCriticalPower;
} 

int ObjectManager::playerMaxPower()
{
	return 1000;
} 


int ObjectManager::playerMaxHealth()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	return thePlayer->getMaxHP();
} 

int ObjectManager::playerAS()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	return thePlayer->getAttack();
}

int ObjectManager::playerDS()
{
	playerObj* thePlayer = (playerObj*)m_pObjectList[m_nPlayerIndex];
	return thePlayer->getDefense();
}

const char* ObjectManager::itemDescription()
{
	inventoryObj* theInventory = (inventoryObj*)m_pObjectList[m_nInventoryIndex];
	const char* theDesc = new char;
	theDesc = theInventory->getCurrentItemDescription().c_str();
	return theDesc;
}




D3DXVECTOR3 ObjectManager::itemLocation(int theIndex)
{
	return m_pObjectList[theIndex]->loc();
}

void ObjectManager::MovePlayer(int xSpeed, int zSpeed)
{
	// nothing for now
} // end pause method

void ObjectManager::CriticalKeyHit()
{
	int Power = m_pObjectList[m_nPlayerIndex]->GetCriticalPower;
	
	//m_pObjectList[m_nPlayerIndex]->CriticalKey();
	//generateParticles(30, ROTATE_HORIZONTAL_TYPE, COLOR_PARTICLE, 200, 0, 1.0, 0.0, 0.0, 1.0);

	if(Power >= 1000)
	{
		m_pObjectList[m_nPlayerIndex]->CriticalKey();
		generateParticles(30, ROTATE_HORIZONTAL_TYPE, COLOR_PARTICLE, 200, 0, 1.0, 0.0, 0.0, 1.0);
	}
	else
	{
		////DEBUGPRINTF("ObjectManager::combatManager - Power is not more than 1000\n");
	}

}

void ObjectManager::combatManager(gameObj* attacker, gameObj* attackee)
{
	if( (attacker->name() == "food") || (attackee->name() == "food") )
	{
		////DEBUGPRINTF("ObjectManager::combatManager - attacker is faction %d, attackee is faction %d\n", attacker->faction(), attackee->faction());
	}

	int damage = 0;
	////DEBUGPRINTF("ObjectManager::combatManager - %d damage\n", damage);

	bool CriticalPowerUse = m_pObjectList[m_nPlayerIndex]->UseCriticalHit;
	////DEBUGPRINTF("ObjectManager::combatManager - %d CriticalPowerUse\n", CriticalPowerUse);

	int Power = m_pObjectList[m_nPlayerIndex]->GetCriticalPower;
	////DEBUGPRINTF("ObjectManager::combatManager - %d Power\n", Power);
	
	if (attackee->isDead())
	{
		////DEBUGPRINTF("ObjectManager::combatManager - Attackee is dead\n");
		//Player get power when the each enemy's death
		if(!attackee->particle()) // if attackee is not a particle
		{
			if (!attackee->AlreadyCounted())
			{
				attacker->Critical(g_cRandom.number(200,350));
				Power = m_pObjectList[m_nPlayerIndex]->GetCriticalPower;
				
				////DEBUGPRINTF("ObjectManager::combatManager - %d \n", Power);
				////DEBUGPRINTF("ObjectManager::combatManager - Attackee is dead\n");
			}
		}
		else
		{
			////DEBUGPRINTF("ObjectManager::combatManager - It's a particle\n");
		}
	}
	else
	{
		////DEBUGPRINTF("ObjectManager::combatManager - Attackee is not dead\n");
		if(!attackee->particle()) // if attackee is not a particle
		{
			if (attackee->faction() == ENEMY_FACTION)
			{
				attacker->Critical(g_cRandom.number(1, 3));
				Power = m_pObjectList[m_nPlayerIndex]->GetCriticalPower;
			} // end if enemy faction
				
			////DEBUGPRINTF("ObjectManager::combatManager - %d \n", Power);
		}		
		else
		{
			////DEBUGPRINTF("ObjectManager::combatManager - It's a particle\n");
		}
	}

	if((Power >= 1000) && (attacker == m_pObjectList[m_nPlayerIndex]) )
	{
		m_pObjectList[m_nPlayerIndex]->ResetPower();
		attacker->Critical(1000);
		
		if(!attackee->particle()) // if attackee is not a particle
		{
			if(CriticalPowerUse)
			{
				////DEBUGPRINTF("ObjectManager::combatManager - Critical Power Attack\n");
				//DEBUGPRINTF("ObjectManager::combatManager - UseCriticalHit is true\n");

				damage = attacker->damageAmount(); // get damage
				int CriticalDamage = damage * 10;
				////DEBUGPRINTF("ObjectManager::combatManager - %d CriticalDamage, Criticaldamage\n");

				generateParticles(30, ROTATE_VERTICAL_TYPE, COLOR_PARTICLE, 50, 0, 1.0, 0.0, 0.0, 1.0);
				g_pSoundManager->play(BIGCLICK_SOUND);

				attackee->hurt(CriticalDamage); // hurt target

				m_pObjectList[m_nPlayerIndex]->ResetPower();
				
			}
			else
			{
				////DEBUGPRINTF("ObjectManager::combatManager - UseCriticalHit is false\n");
				damage = attacker->damageAmount(); // get damage
				attackee->hurt(damage); // hurt target
			}
		}
		else	
		{
			////DEBUGPRINTF("ObjectManager::combatManager - It's a particle\n");
		}
	}
	
	else if((Power < 1000) && (attacker == m_pObjectList[m_nPlayerIndex]) )
	{
		if(!attackee->particle()) // if attackee is not a particle
		{
			m_pObjectList[m_nPlayerIndex]->ResetCriticalKey();
			damage = attacker->damageAmount(); // get damage
			attackee->hurt(damage); // hurt target
		}
		else	
		{
			////DEBUGPRINTF("ObjectManager::combatManager - It's a particle\n");
		}
	}
	
	else
	{
		if(!attackee->particle()) // if attackee is not a particle
		{			
			damage = attacker->damageAmount(); // get damage
			attackee->hurt(damage); // hurt target
		} // end if not a particle
		else	
		{
			////DEBUGPRINTF("ObjectManager::combatManager - It's a particle\n");
		}
	}
	////DEBUGPRINTF("ObjectManager::combatManager - %s did %d damage to %s\n", attacker->name().c_str(), attackee->name().c_str(), damage);
	if (attackee->intelligent())
	{
		////DEBUGPRINTF("ObjectManager::combatManager - calling splatter\n");
		if (damage > 0)
			bloodSpatter(attackee->loc(), damage);
	}
	else
	{
		////DEBUGPRINTF("ObjectManager::combatManager - object not intelligent\n");
	}
} // end if attackee not dead



void ObjectManager::bloodSpatter(D3DXVECTOR3 p, int damage)
{
	g_pSoundManager->play(SPLAT_SOUND);
	////DEBUGPRINTF("ObjectManager::bloodSplatter - splat!\n");
	int j = 0;
	int numSplats = (damage / 7) + 1;
	if (numSplats > 10)
		numSplats = 10;
	for (int i = 0; i < numSplats; i++)
	{
		if(m_nCount < m_nMaxCount) //if room, create object
		{
			////DEBUGPRINTF("ObjectManager::bloodSplatter - splat!\n");
			for(j = 0; m_pObjectList[j] != NULL; j++); //find first free slot
			m_pObjectList[j] = new bloodObj("blood", blood, p, damage);
			m_nCount++;
		} // end if
		
	} // end for number of splats

} // end bloodSpatter

void ObjectManager::fireArrow(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed)
{
	g_pSoundManager->play(ARROW_SOUND);
	////DEBUGPRINTF("ObjectManager::fireArrow - firing at xSpeed %d, ySpeed %d, zSpeed %d, from x=%d y= %d z=%d\n", xSpeed, ySpeed, zSpeed, (int)p.x, (int)p.y, (int)p.z);
	if(m_nCount < m_nMaxCount) //if room, create object
	{
		for(int i = 0; m_pObjectList[i] != NULL; i++); //find first free slot
		m_pObjectList[i] = new projectileObj("arrow", arrow, p, true, 15, xSpeed, ySpeed, zSpeed);
		m_nCount++;
	} // end if
} // end fireArrow



void ObjectManager::fireShuriken(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed)
{
	g_pSoundManager->play(SWING1_SOUND);
	////DEBUGPRINTF("ObjectManager::fireArrow - firing at xSpeed %d, ySpeed %d, zSpeed %d, from x=%d y= %d z=%d\n", xSpeed, ySpeed, zSpeed, (int)p.x, (int)p.y, (int)p.z);
	if(m_nCount < m_nMaxCount) //if room, create object
	{
		for(int i = 0; m_pObjectList[i] != NULL; i++); //find first free slot
		m_pObjectList[i] = new projectileObj("shuriken", shuriken, p, true, 15, xSpeed, 0, zSpeed);
		m_nCount++;
	} // end if

} // end fireShuriken

void ObjectManager::fireBall(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed)
{
	////DEBUGPRINTF("ObjectManager::fireBall - firing at xSpeed %d, ySpeed %d, zSpeed %d, from x=%d y= %d z=%d\n", xSpeed, ySpeed, zSpeed, (int)p.x, (int)p.y, (int)p.z);
	g_pSoundManager->play(FIREBALL_SOUND);
	int lightNum = createLight(p, 1.0f, 0.0f, 0.4f, 1.0f, 500.0f);
    float startChance, chance = 0;
	float newX, newY, newZ;
	D3DXVECTOR3 newP;
	int numFire = g_cRandom.number(30,40);
	for (int j = 0; j < numFire; j++)
	{
		if(m_nCount < m_nMaxCount) //if room, create object
		{
			for(int i = 0; m_pObjectList[i] != NULL; i++); //find first free slot
			int pickOne = g_cRandom.number(0, 1);
			////DEBUGPRINTF("ObjectManager::fireBall - chance: %f\n", chance);
			chance = (float)10 / (float)g_cRandom.number(11, 100);
			startChance = (float)10 / (float)g_cRandom.number(11, 100);
//			newY = startChance;
			startChance = startChance * 10;
			
			
			switch(pickOne)
			{
				case 0: 
					startChance = -startChance;
					break;
				//default:
			}

			
			newP.x = p.x + startChance;
			newP.y = p.y + startChance;
			
			if (xSpeed < 0)
			{
				//newP.x = p.x + startChance;
				
				newX = xSpeed + 2*chance;
				
			}
			else
			{
				//newP.x = p.x - startChance;
				
				newX = xSpeed - 2*chance;
			}
			
			

			//chance = (float)10 / (float)g_cRandom.number(11, 100);
			
			newP.z = (p.z - (startChance/2)) + startChance;
				
				//newZ = zSpeed + 2*chance;
			
			//newP.z = p.z;
			newZ = zSpeed;
			newY = ySpeed;

			pickOne = g_cRandom.number(0, 2);
			switch(pickOne)
			{
			case 0:
                m_pObjectList[i] = new magicObj("magic", fire1, newP, true, 1, newX, newY, newZ, lightNum);
				break;
			case 1:
                m_pObjectList[i] = new magicObj("magic", fire2, newP, true, 1, newX, newY, newZ, lightNum);
				break;
			case 2:
                m_pObjectList[i] = new magicObj("magic", fire3, newP, true, 1, newX, newY, newZ, lightNum);
				break;
			} // end switch
			////DEBUGPRINTF("ObjectManager::fireBall - made a fireball at X: %f, Y: %f, Z: %f\n", newP.x, newP.y, newP.z);
			m_nCount++;

		} // end if space available

	} // end for j

}

void ObjectManager::playerCombat()
{
	gameObj* thePlayer = m_pObjectList[m_nPlayerIndex];
	gameObj* theTarget = NULL;

	

	////DEBUGPRINTF("ObjectManager::playerCombat\n");

	for (int i = 0; i < m_nMaxCount; i++) // for all objects
	{
		if (m_pObjectList[i] != NULL) // grab object from list
		{
			////DEBUGPRINTF("ObjectManager::playerCombat - grabbed object %d from the list\n", i);
			theTarget = m_pObjectList[i];
			if ( inRange(thePlayer, theTarget) ) // if in range to be attacked
			{
				combatManager(thePlayer, theTarget); // do it
			} // end if in range
		} // end if not null
	} // end for
} // end playercombat

float ObjectManager::playerXSpeed()
{
	////DEBUGPRINTF("ObjectManager::playerXSpeed - getting xSpeed \n");
	return m_pObjectList[m_nPlayerIndex]->getXSpeed();

}

float ObjectManager::playerYSpeed()
{
	return m_pObjectList[m_nPlayerIndex]->getYSpeed();
}

float ObjectManager::playerZSpeed()
{
	return m_pObjectList[m_nPlayerIndex]->getZSpeed();
} // end playerZSpeed


void ObjectManager::Action(actionType theAction, int xSpeed, int zSpeed, bool fromJoystick)
{
	gameObj* thePlayer = m_pObjectList[m_nPlayerIndex];
	if (!thePlayer->isDead()) //if player is not dead
	{
		thePlayer->doAction(theAction, xSpeed, zSpeed, fromJoystick);
	} // end if not dead
} // end Action


void ObjectManager::stopPlayerX()
{
	m_pObjectList[m_nPlayerIndex]->stopX();
}

void ObjectManager::stopPlayerY()
{
	m_pObjectList[m_nPlayerIndex]->stopY();
}

void ObjectManager::stopPlayerZ()
{
	m_pObjectList[m_nPlayerIndex]->stopZ();
}

void ObjectManager::stopPlayerAll()
{
	m_pObjectList[m_nPlayerIndex]->stopAll();
}

bool ObjectManager::playerBusy()
{
	return m_pObjectList[m_nPlayerIndex]->busy();
}

void ObjectManager::playerRunning()
{
	m_pObjectList[m_nPlayerIndex]->running = true;
	//Action(PLAYER_RUN, 0, 0, false);
}

void ObjectManager::playerWalking()
{
	m_pObjectList[m_nPlayerIndex]->running = false;
	//Action(PLAYER_WALK, 0, 0, false);
}

void ObjectManager::playerDucking(bool ducking)
{
	if (ducking)
	{
		m_pObjectList[m_nPlayerIndex]->ducking = true;
	}
	else
	{
		m_pObjectList[m_nPlayerIndex]->ducking = false;
	}
}


void ObjectManager::inventoryForward()
{
	gameObj* thePlayer = m_pObjectList[m_nPlayerIndex];
	if (!thePlayer->isDead()) //if player is not dead
	{
		theInventory->scrollForward();
	} // end if not dead
} // end inventoryForward


void ObjectManager::inventoryBackward()
{
	gameObj* thePlayer = m_pObjectList[m_nPlayerIndex];
	if (!thePlayer->isDead()) //if player is not dead
	{
		theInventory->scrollBackward();
	} // end if not dead
} // end inventoryBackward


bool ObjectManager::addToInventory(gameObj* newObject)
{
	g_pSoundManager->play(HEAL_SOUND); //play healing sound
	if(theInventory->addObject(newObject))
		return true;
	else
		return false;
	// set pointer to food object to NULL, in order to remove it from list
} // end inventoryBackward


void ObjectManager::useInventoryObject()
{
	gameObj* thePlayer = m_pObjectList[m_nPlayerIndex];
	if (!thePlayer->isDead()) //if player is not dead
	{
		theInventory->useObject(m_pObjectList[m_nPlayerIndex]);
	} // end if not dead
} // end useInventoryObject


void ObjectManager::drawInventory()
{
	theInventory->move();
	theInventory->draw();
}

void ObjectManager::playerDefending(bool defending)
{
	if (defending)
	{
		m_pObjectList[m_nPlayerIndex]->defending = true;
	}
	else
	{
		m_pObjectList[m_nPlayerIndex]->defending = false;
	}
}

bool ObjectManager::inRange(gameObj* attacker, gameObj* attackee)
{

	sizeMatrix attackerSize = attacker->size();
	sizeMatrix attackeeSize = attackee->size();
	D3DXVECTOR3 attackerLoc = attacker->loc();
	D3DXVECTOR3 attackeeLoc = attackee->loc();
	int attackerReach = attacker->reach();

	if (attackee != attacker) // if not atacking self...
	{

		if (attacker->direction() == FACE_RIGHT)
		{
			
			if (	(attackeeLoc.x - (.5 * attackeeSize.x()) < attackerLoc.x + attackerReach) &&
					(attackeeLoc.x > attackerLoc.x + 1) &&
					(attackeeLoc.z - (.5 * attackeeSize.z()) < attackerLoc.z + attackerReach) &&
					(attackeeLoc.z + (.5 * attackeeSize.z()) > attackerLoc.z - attackerReach) &&
					(attackeeLoc.y < attackerLoc.y + attackerReach) &&
					(attackeeLoc.y > attackerLoc.y - attackerReach)		) // if within range to right
			{
			//	if (!attackee->isDead())
			//	{
					return true;
			//	}
			} // end if within range to right
		} // end if facing right
		else // else facing left
		{
			if (	(attackeeLoc.x + (.5 * attackeeSize.x()) > attackerLoc.x - attackerReach) &&
					(attackeeLoc.x < attackerLoc.x - 1) &&
					(attackeeLoc.z - (.5 * attackeeSize.x()) < attackerLoc.z + attackerReach) &&
					(attackeeLoc.z + (.5 * attackeeSize.x()) > attackerLoc.z - attackerReach) &&
					(attackeeLoc.y < attackerLoc.y + attackerReach) &&
					(attackeeLoc.y > attackerLoc.y - attackerReach)		) // if within range to right
			{
			//	if (!attackee->isDead())
			//	{
					return true;
			//	}
			} // end if within range to right
		} // end if facing left
	} // end if not attacking self
	
	return false; // Sorry, you missed everything.  Loser.

} // end ObjectManager::inRange


void ObjectManager::spawnFire(D3DXVECTOR3 p, int fireSize)
{
	spawnFire(p, fireSize, -1);
}


void ObjectManager::spawnFire(D3DXVECTOR3 p, int fireSize, int followObject)
{
	
	D3DXVECTOR3 newP;
	int pickOne;

	int fireBounds;
	fireBounds = fireSize / 2;
	int lightNum = createLight(p, 1.0f, 0.0f, 0.4f, 1.0f, 400.0f);
	for (int i = 0; i < fireSize; i++)
	{
		if(m_nCount < m_nMaxCount) //if room, create object
		{
			for(int j = 0; m_pObjectList[j] != NULL; j++); //find first free slot
			
			pickOne = g_cRandom.number(0, 2);
			switch(pickOne)
			{
				case 0:
					m_pObjectList[j] = new fireObj("fire", fire1, p, true, 1, followObject, lightNum);
					break;
				case 1:
					m_pObjectList[j] = new fireObj("fire", fire2, p, true, 1, followObject, lightNum);
					break;
				case 2:
					m_pObjectList[j] = new fireObj("fire", fire3, p, true, 1, followObject, lightNum);
					break;
			} // end switch
			////DEBUGPRINTF("ObjectManager::fireBall - made a fireball at X: %f, Y: %f, Z: %f\n", newP.x, newP.y, newP.z);
			m_nCount++;

		} // end if space available

	} // end for j

} // end spawnFire
	

void ObjectManager::createRain(int howHard)
{
	
	float fogStart;
	float fogEnd;
	float fogDensity;

	switch(g_currentTime)
	{
		case NIGHT_TIME:
			fogStart = 0.0f;
			fogEnd = 1.0f;
			fogDensity = 0.2f;
			break;

		default:
			fogStart = 0.0f;
			fogEnd = 1.0f;
			fogDensity = 0.6f;
			
	} // end time switch

	g_d3ddevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	g_d3ddevice->SetRenderState(D3DRS_FOGCOLOR, 0x00DDDDDD);
	g_d3ddevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
    g_d3ddevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
	g_d3ddevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*) (&fogStart)));
	g_d3ddevice->SetRenderState(D3DRS_FOGEND, *((DWORD*) (&fogEnd)));
	g_d3ddevice->SetRenderState(D3DRS_FOGDENSITY, *((DWORD*) (&fogDensity)));

	// then rain
	D3DXVECTOR3 newP;
	
	for (int i = 0; i < howHard; i++)
	{
		newP.x = (float)g_cRandom.number(-g_nScreenWidth, g_nScreenWidth);
		newP.y = (float)g_cRandom.number(0, g_nScreenHeight);
		newP.z = (float)g_cRandom.number(600, 1500);
		generateParticles(newP, 1, RAIN_TYPE, RAIN_PARTICLE, -1, 0, -1, 1.0f, 1.0f, 1.0f, 1.0f);

	} // end for loop

} // end createRain



void ObjectManager::generateParticles(D3DXVECTOR3 p, int numParticles, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, int followObject, float red, float green, float blue, float alpha)
{
	////DEBUGPRINTF("ObjectManager::generateParticles - made a particle\n");
	for (int i = 0; i < numParticles; i++)
	{
		if(m_nCount < m_nMaxCount) //if room, create object
		{
			for(int j = 0; m_pObjectList[j] != NULL; j++); //find first free slot
			m_pObjectList[j] = new particleObj(p, theType, theColor, theLifetime, damagePerParticle, followObject, red, green, blue, alpha);
			m_nCount++;
		} // end if space available
	} // end for numParticles
} // end generateParticle

void ObjectManager::generateParticles(int numParticles, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, float red, float green, float blue, float alpha)
{
	for (int i = 0; i < numParticles; i++)
	{
		if(m_nCount < m_nMaxCount) //if room, create object
		{
			for(int j = 0; m_pObjectList[j] != NULL; j++); //find first free slot
			m_pObjectList[j] = new particleObj(m_pObjectList[m_nPlayerIndex]->loc(), theType, theColor, theLifetime, damagePerParticle, m_nPlayerIndex, red, green, blue, alpha);
			m_nCount++;
		} // end if space available
	} // end for numParticles
} // end generateParticle


int ObjectManager::createLight(D3DXVECTOR3 p, float red, float blue, float green, float alpha, float range)
{
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type = D3DLIGHT_POINT;

	p.z = p.z - 5;
	p.y = p.y + 100;
	p.x = p.x + 600;
	light.Position = p;

	light.Ambient.r = red;
    light.Ambient.g = green;
    light.Ambient.b = blue;
	light.Ambient.a = 1.0f;

	light.Falloff = 1.0f;
    
	light.Diffuse.r = red;
    light.Diffuse.g = green;
    light.Diffuse.b = blue;
	light.Diffuse.a = 1.0f;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	light.Range = range;

    //D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
 
    g_d3ddevice->SetLight(m_numLights, &light);
	g_d3ddevice->LightEnable(m_numLights, TRUE);
    g_d3ddevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	////DEBUGPRINTF("ObjectManager::createLight - light created\n");
	m_numLights++;
	return m_numLights-1;
}

void ObjectManager::moveLight(int lightNumber, D3DXVECTOR3 p)
{

	
    D3DLIGHT9 light;
    g_d3ddevice->GetLight(lightNumber, &light);

	p.x = p.x + 600;
    light.Position = p;

	g_d3ddevice->SetLight(lightNumber, &light);
	g_d3ddevice->LightEnable(lightNumber, TRUE);
    g_d3ddevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


void ObjectManager::ambientMusic()
{
	nextMusicTime = g_cRandom.number(5000, 1500) + 2500;
	int musicChoice = g_cRandom.number(0, 14);
	switch (musicChoice)
	{
		case 0: 
			g_pSoundManager->play(SHAKU1_SOUND);
			break;
		case 1: 
			g_pSoundManager->play(SHAKU2_SOUND);
			break;
		case 2: 
			g_pSoundManager->play(SHAKU3_SOUND);
			break;
		case 3: 
			g_pSoundManager->play(SHAKU4_SOUND);
			break;
		case 4: 
			g_pSoundManager->play(SHAKU5_SOUND);
			break;
		case 5: 
			g_pSoundManager->play(SHAMI1_SOUND);
			break;
		case 6: 
			g_pSoundManager->play(SHAMI2_SOUND);
			break;
		case 7: 
			g_pSoundManager->play(SHAMI3_SOUND);
			break;
		case 8: 
			g_pSoundManager->play(SHAMI4_SOUND);
			break;
		case 9: 
			g_pSoundManager->play(SHAMI5_SOUND);
			break;
		case 10: 
			g_pSoundManager->play(KOTO1_SOUND);
			break;
		case 11: 
			g_pSoundManager->play(KOTO2_SOUND);
			break;
		case 12: 
			g_pSoundManager->play(KOTO3_SOUND);
			break;
		case 13: 
			g_pSoundManager->play(KOTO4_SOUND);
			break;
		case 14: 
			g_pSoundManager->play(KOTO5_SOUND);
			break;
		default:
			g_pSoundManager->play(MAJOUSCREAM_SOUND);
	} // end switch musicChoice
} // end AmbientMusic



void ObjectManager::ambientSounds()
{
	if (m_lightningFlash)
	{
		m_lightningTimer++;
		if (m_lightningTimer > 15)
		{
			g_d3ddevice->SetRenderState(D3DRS_AMBIENT,0x00111111);
			m_lightningFlash = false;
		}
	} // end if lightning

	int chooseSound;

	if (lastSoundTime == -1)
	{
		lastSoundTime = g_cTimer.time();
	} // end if lastSoundTime not initialized

	if (lastMusicTime == -1)
	{
		lastMusicTime = g_cTimer.time();
	} // end if lastSoundTime not initialized
	if (g_cTimer.elapsed(lastMusicTime, nextMusicTime))
	{
		ambientMusic();
	}

	if (!m_soundLoopStarted) //if we haven't started the sound loop for the level, do it
	{
		m_soundLoopStarted = true;
		if (g_currentWeather == RAINING_WEATHER)
		{
			g_pSoundManager->play(RAINLOOP_SOUND, TRUE);
		}
		else
		{
			switch(g_levelLocation)
			{
				case MOUNTAIN_LOCATION:
					switch(g_currentTime)
					{
						case DAWN_TIME:
						case DAY_TIME: 
							g_pSoundManager->play(WINDLOOP_SOUND, TRUE);
							break;

						case DUSK_TIME:
						case NIGHT_TIME:
							chooseSound = g_cRandom.number(0, 2);
							switch(chooseSound)
							{
								case 0:
									g_pSoundManager->play(CRICKETLOOP1_SOUND, TRUE);
									break;
								case 1:
									g_pSoundManager->play(CRICKETLOOP2_SOUND, TRUE);
									break;
								default:
									g_pSoundManager->play(CRICKETLOOP3_SOUND, TRUE);
							} // end chooseSound switch
							break;
					} // end switch current time
					break;

				case SEASHORE_LOCATION:
					g_pSoundManager->play(WAVELOOP_SOUND, TRUE);
					break;

				case CITY_LOCATION:
					g_pSoundManager->play(STEPS_SOUND, TRUE);
					break;
			} // end switch location
		} // end else it's not raining
	} // end if sound loop hasn't started

	if (g_cTimer.elapsed(lastSoundTime, nextSoundTime)) // if time has passed since last sound time
	{
		////DEBUGPRINTF("ObjectManager::ambientSounds - time has passed, level is %d, playing sound\n", g_gameLevel);
		
		if (g_currentWeather == RAINING_WEATHER) // if it's raining
		{
			chooseSound = g_cRandom.number(0, 4);
			nextSoundTime = g_cRandom.number(0, 5000) + 5000;
			////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
			////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
			switch(chooseSound) // pick a sound to play
			{
				case 0:
					g_pSoundManager->play(WIND_SOUND);
					break;
				case 1:
					m_lightningTimer = 0;
					m_lightningFlash = true;
					g_d3ddevice->SetRenderState(D3DRS_AMBIENT,0x00FFFFFF);
					g_pSoundManager->play(LIGHTNING1_SOUND);
					break;
				case 2:
					m_lightningTimer = 0;
					m_lightningFlash = true;
					g_d3ddevice->SetRenderState(D3DRS_AMBIENT,0x00FFFFFF);
					g_pSoundManager->play(LIGHTNING2_SOUND);
					break;
				case 3:
					m_lightningTimer = 0;
					m_lightningFlash = true;
					g_d3ddevice->SetRenderState(D3DRS_AMBIENT,0x00FFFFFF);
					g_pSoundManager->play(LIGHTNING3_SOUND);
					break;
				case 4:
					g_pSoundManager->play(WIND2_SOUND);
					break;
				default:
					g_pSoundManager->play(BOMB_SOUND);
			} // end switch choosesound
				
		} // end if raining
		else // else it's not raining
		{
			switch(g_levelLocation)
			{
				case MOUNTAIN_LOCATION:
					switch(g_currentTime)
					{
						case DAWN_TIME:
						case DAY_TIME: 
							chooseSound = g_cRandom.number(0, 6);
							nextSoundTime = g_cRandom.number(0, 2000) + 8000;
							////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
							////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
							switch(chooseSound) // pick a sound to play
							{
								case 0:
									g_pSoundManager->play(WIND_SOUND);
									break;
								case 1:
									g_pSoundManager->play(BIRD1_SOUND);
									break;
								case 2:
									g_pSoundManager->play(BIRD2_SOUND);
									break;
								case 3:
									g_pSoundManager->play(BIRD3_SOUND);
									break;
								case 4:
									g_pSoundManager->play(BIRD4_SOUND);
									break;
								case 5:
									g_pSoundManager->play(BIRD5_SOUND);
									break;
								case 6:
									g_pSoundManager->play(WIND2_SOUND);
									break;
								default:
									g_pSoundManager->play(BOMB_SOUND);
							} // end switch choosesound
							break;

						case DUSK_TIME:
							chooseSound = g_cRandom.number(0, 6);
							nextSoundTime = g_cRandom.number(0, 2000) + 8000;
							////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
							////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
							switch(chooseSound) // pick a sound to play
							{
								case 0:
									g_pSoundManager->play(WIND_SOUND);
									break;
								case 1:
									g_pSoundManager->play(CICADA1_SOUND);
									break;
								case 2:
									g_pSoundManager->play(CICADA2_SOUND);
									break;
								case 3:
									g_pSoundManager->play(FROG_SOUND);
									break;
								case 4:
									g_pSoundManager->play(OWL_SOUND);
									break;
								case 5:
									g_pSoundManager->play(INSECT_SOUND);
									break;
								case 6:
									g_pSoundManager->play(WIND2_SOUND);
									break;
								default:
									g_pSoundManager->play(BOMB_SOUND);
							} // end switch choosesound
							break;

						case NIGHT_TIME:
							chooseSound = g_cRandom.number(0, 4);
							nextSoundTime = g_cRandom.number(0, 2000) + 8000;
							////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
							////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
							switch(chooseSound) // pick a sound to play
							{
								case 0:
									g_pSoundManager->play(WIND_SOUND);
									break;
								case 1:
									g_pSoundManager->play(INSECT_SOUND);
									break;
								case 2:
									g_pSoundManager->play(FROG_SOUND);
									break;
								case 3:
									g_pSoundManager->play(OWL_SOUND);
									break;
								case 4:
									g_pSoundManager->play(WIND2_SOUND);
									break;
								default:
									g_pSoundManager->play(BOMB_SOUND);
							} // end switch choosesound
							break;
					} // end switch current time
					break;

				case SEASHORE_LOCATION:
							chooseSound = g_cRandom.number(0, 5);
							nextSoundTime = g_cRandom.number(0, 2000) + 8000;
							////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
							////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
							switch(chooseSound) // pick a sound to play
							{
								case 0:
									g_pSoundManager->play(WIND_SOUND);
									break;
								case 1:
									g_pSoundManager->play(SEAGULL1_SOUND);
									break;
								case 2:
									g_pSoundManager->play(SEAGULL2_SOUND);
									break;
								case 3:
									g_pSoundManager->play(WINDLOOP_SOUND);
									break;
								case 4:
									g_pSoundManager->play(WIND2_SOUND);
									break;
								case 5:
									g_pSoundManager->play(INSECT_SOUND);
									break;
								default:
									g_pSoundManager->play(BOMB_SOUND);
							} // end switch choosesound
							break;

				case CITY_LOCATION:
							chooseSound = g_cRandom.number(0, 6);
							nextSoundTime = g_cRandom.number(0, 2000) + 8000;
							////DEBUGPRINTF("ObjectManager::ambientSounds - chooseSound: %d\n", chooseSound);
							////DEBUGPRINTF("ObjectManager::ambientSounds - nextSoundTime: %d\n", nextSoundTime);
							switch(chooseSound) // pick a sound to play
							{
								case 0:
									g_pSoundManager->play(CITY1_SOUND);
									break;
								case 1:
									g_pSoundManager->play(CITY2_SOUND);
									break;
								case 2:
									g_pSoundManager->play(HORSE1_SOUND);
									break;
								case 3:
									g_pSoundManager->play(HORSE2_SOUND);
									break;
								case 4:
									g_pSoundManager->play(DOG_SOUND);
									break;
								case 5:
									g_pSoundManager->play(HORN1_SOUND);
									break;
								case 6:
									g_pSoundManager->play(HORN1_SOUND);
									break;
								default:
									g_pSoundManager->play(BOMB_SOUND);
							} // end switch choosesound
							break;
				default:
					g_pSoundManager->play(BOMB_SOUND);

			} // end switch location
		} // end else it's not raining
	} // end if time has passed
} // end ambient sound thing


// =====================================================================================
// =====================================================================================
// ***							Russell's Collision Detection						 ***
// =====================================================================================
// =====================================================================================



void ObjectManager::BounceBack(gameObj *Object1, gameObj *Object2, 
                               bool left, bool front, bool bottom)
{
    // bounce back of Object1
    if ( Object1->isMovable() )
    {    

        // this is the ratio of the height of the bottom of object1 divided the top of object2 at which
        // Object1 is put on top of Object in bounce back.  Same for Object1 being under Object2.
        float onTopRatio = 0.25f;
        float onBottomRatio = 0.75f;

        //this is the ratio of the xmax of object1 over the xmin of object2 at which
        //Object1 is pushed in the -x dir of Object in bounce back. Same for Object1 
        //xmin vs Object2 xmax.
        float onLeftRatio = 1.0f;
        float onRightRatio = 1.0f;

        // this is the ratio of the zmax of object1 over the zmin of object2 at which
        // Object1 is pushed in the -z dir of Object in bounce back. Same for Object1 
        // zmin vs Object2 zmax.
        float onFrontRatio = 1.0f;
        float onBackRatio = 1.0f;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

        // (case 1)
        if (left && front && bottom) 
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onBottomRatio )
            {
                Object1->setYLoc( Object2->getYLoc() -
                                    (float)(Object2->getHeight() / 2.0) - 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmax = Object1->getXLoc() + ( (float)Object1->getLength() / 2.0f );
                float Obj2xmin = Object2->getXLoc() - ( (float)Object2->getLength() / 2.0f );

                float Obj1zmax = Object1->getZLoc() + ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmin = Object2->getZLoc() - ( (float)Object2->getWidth() / 2.0f );


                // see if Object1 xmax is close to Object2's xmin
                if ( Obj1xmax <= Obj2xmin * onLeftRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );



/*************************************************************/
//DEBUGPRINTF("case 1A\n");
/*************************************************************/




                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {

                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );


/*************************************************************/
//DEBUGPRINTF("case 1B\n");
/*************************************************************/



                    }

                }  

                // Object1 is not close to Object's xmin  
                else
                {

                    // if so bounce in z direction
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        
                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );


                        /*************************************************************/
//DEBUGPRINTF("case 1C\n");
/*************************************************************/

                    }

                    else
                    {
                        if ( fabs(Obj1xmax - Obj2xmin) >= fabs(Obj1zmax - Obj2zmin) )
                        {
                            Object1->setZLoc( Object2->getZLoc() - 
                                        (float)(Object2->getWidth() / 2.0) -
                                        (float)(Object1->getWidth() / 2.0) - 1.0f);

                            /*************************************************************/
//DEBUGPRINTF("case 1D\n");
/*************************************************************/

                        }


                        else
                        {    
                            
                            Object1->setXLoc( Object2->getXLoc() - 
                                        (float)(Object2->getLength() / 2.0) -
                                        (float)(Object1->getLength() / 2.0) );
                            
                            

                        /*************************************************************/
//DEBUGPRINTF("case 1E\n");
/*************************************************************/

                        }



                    }

                }

            }

        }   //end case 1


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


        // (case 2)
        else if (left && !front && bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() -
                                    (float)(Object2->getHeight() / 2.0) - 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmax = Object1->getXLoc() + ( (float)Object1->getLength() / 2.0f );
                float Obj2xmin = Object2->getXLoc() - ( (float)Object2->getLength() / 2.0f );

                float Obj1zmin = Object1->getZLoc() - ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmax = Object2->getZLoc() + ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmax is close to Object2's xmin
                if ( Obj1xmax <= Obj2xmin * onLeftRatio )
                {


                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 2A\n");
/*************************************************************/


                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );


                        /*************************************************************/
//DEBUGPRINTF("case 2B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin 
                else
                {
                    
                    // if so bounce in z direction
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 2C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        if ( fabs( Obj1xmax - Obj2xmin ) >= fabs( Obj1zmin - Obj2zmax ) )
                        {
                            
                            Object1->setZLoc( Object2->getZLoc() + 
                                (float)(Object2->getWidth() / 2.0) +
                                (float)(Object1->getWidth() / 2.0) + 1.0f);

                            /*************************************************************/
//DEBUGPRINTF("case 2D\n");
/*************************************************************/
                        }
                        else
                        {   
                            
                            Object1->setXLoc( Object2->getXLoc() - 
                                (float)(Object2->getLength() / 2.0) -
                                (float)(Object1->getLength() / 2.0) );
                            
                            
                        /*************************************************************/
//DEBUGPRINTF("case 2E\n");
/*************************************************************/

                        }
                    }

                }

            }

        }  //end case 2


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



        // (case 3)
        else if (left && front && !bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );

            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() +
                                    (float)(Object2->getHeight() / 2.0) + 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmax = Object1->getXLoc() + ( (float)Object1->getLength() / 2.0f );
                float Obj2xmin = Object2->getXLoc() - ( (float)Object2->getLength() / 2.0f );

                float Obj1zmax = Object1->getZLoc() + ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmin = Object2->getZLoc() - ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmax is close to Object2's xmin
                if ( Obj1xmax <= Obj2xmin * onLeftRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 3A\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 3B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin 
                else
                {

                    // if so bounce in z direction
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {

                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 3C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        if ( fabs(Obj1xmax - Obj2xmin) >= fabs(Obj1zmax - Obj2zmin) )
                        {    
                            Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 3D\n");
/*************************************************************/
                        }
                        else
                        {     
                            Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );
                            
                        /*************************************************************/
//DEBUGPRINTF("case 3E\n");
/*************************************************************/
                        }
                    }

                }

            }

        }  //end case 3

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

        // (case 4)
        else if (left && !front && !bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() +
                                    (float)(Object2->getHeight() / 2.0) + 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmax = Object1->getXLoc() + ( (float)Object1->getLength() / 2.0f );
                float Obj2xmin = Object2->getXLoc() - ( (float)Object2->getLength() / 2.0f );

                float Obj1zmin = Object1->getZLoc() - ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmax = Object2->getZLoc() + ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmax is close to Object2's xmin
                if ( Obj1xmax <= Obj2xmin * onLeftRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 4A\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 4B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin
                else
                {
                    // if so bounce in z direction
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

 /*************************************************************/
//DEBUGPRINTF("case 4C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else 
                    {
                        if ( fabs(Obj1xmax - Obj2xmin) >= fabs(Obj1zmin - Obj2zmax) )
                        {
                            Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 4D\n");
/*************************************************************/
                        }
                        else
                        {
                            Object1->setXLoc( Object2->getXLoc() - 
                                            (float)(Object2->getLength() / 2.0) -
                                            (float)(Object1->getLength() / 2.0) );
/*************************************************************/
//DEBUGPRINTF("case 4E\n");
/*************************************************************/
                        }
                    }
                }

            }

        }  // end case4

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

        // (case 5)
        else if (!left && front && bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() -
                                    (float)(Object2->getHeight() / 2.0) - 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmin = Object1->getXLoc() - ( (float)Object1->getLength() / 2.0f );
                float Obj2xmax = Object2->getXLoc() + ( (float)Object2->getLength() / 2.0f );

                float Obj1zmax = Object1->getZLoc() + ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmin = Object2->getZLoc() - ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmin is close to Object2's xmax
                if ( Obj1xmin >= Obj2xmax * onRightRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

 /*************************************************************/
//DEBUGPRINTF("case 5A\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 5B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin
                else
                {
                    // if so bounce in z direction
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 5C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        if ( fabs(Obj1xmin - Obj2xmax) >= fabs(Obj1zmax - Obj2zmin) )
                        {
                            Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

///*************************************************************/
////DEBUGPRINTF("case 5D\n");
///*************************************************************/
                        }
                        else
                        {

                            Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +

                                            (float)(Object1->getLength() / 2.0) );
///*************************************************************/
////DEBUGPRINTF("case 5E\n");
///*************************************************************/
                        }
                    }

                }

            }

        }  // end case 5


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


        // (case 6)
        else if (!left && !front && bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() -
                                    (float)(Object2->getHeight() / 2.0) - 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmin = Object1->getXLoc() - ( (float)Object1->getLength() / 2.0f );
                float Obj2xmax = Object2->getXLoc() + ( (float)Object2->getLength() / 2.0f );

                float Obj1zmin = Object1->getZLoc() - ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmax = Object2->getZLoc() + ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmin is close to Object2's xmax
                if ( Obj1xmin >= Obj2xmax * onRightRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) + 1.0f);
                        /*************************************************************/
//DEBUGPRINTF("case 6A\n");
/*************************************************************/



                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );


                        /*************************************************************/
//DEBUGPRINTF("case 6B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin 
                else
                {
                    // if so bounce in z direction
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 6C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        if ( fabs(Obj1xmin - Obj2xmax) >= fabs(Obj1zmin - Obj2zmax) )
                        {   
                            Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );
                            

/*************************************************************/
//DEBUGPRINTF("case 6D\n");
/*************************************************************/
                        }
                        else
                        {   
                            Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );
                            

/*************************************************************/
//DEBUGPRINTF("case 6E\n");
/*************************************************************/
                        }
                    }
                    

                }

            }

        }  // end case 6


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


        // (case 7)
        else if (!left && front && !bottom)
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() +
                                    (float)(Object2->getHeight() / 2.0) + 
                                    (float)(Object1->getHeight() / 2.0) );

            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmin = Object1->getXLoc() - ( (float)Object1->getLength() / 2.0f );
                float Obj2xmax = Object2->getXLoc() + ( (float)Object2->getLength() / 2.0f );

                float Obj1zmax = Object1->getZLoc() + ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmin = Object2->getZLoc() - ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmin is close to Object2's xmax
                if ( Obj1xmin >= Obj2xmax * onRightRatio )
                {

                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 7A\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 7B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin
                else
                {
                    // if so bounce z direction
                    if ( Obj1zmax <= Obj2zmin * onFrontRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );



                        /*************************************************************/
//DEBUGPRINTF("case 7C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else  
                    {
                        if ( fabs(Obj1xmin - Obj2xmax) >= fabs(Obj1zmax - Obj2zmin) )
                        {

                            Object1->setZLoc( Object2->getZLoc() - 
                                            (float)(Object2->getWidth() / 2.0) -
                                            (float)(Object1->getWidth() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 7D\n");
/*************************************************************/
                        }
                        else
                        {

                            Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );


/*************************************************************/
//DEBUGPRINTF("case 7E\n");
/*************************************************************/
                        }
                    }

                }

            }

        } // end case7


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


        // (case 8)
        else
        {
            float Obj1ybottom = Object1->getYLoc() - ( (float)Object1->getHeight() / 2.0f );
            float Obj2ytop = Object2->getYLoc() + ( (float)Object2->getHeight() / 2.0f );
            
            //if Object1 should be on bottom of Object2 so reset to the top
            if ( Obj1ybottom >= Obj2ytop * onTopRatio )
            {
                Object1->setYLoc( Object2->getYLoc() +
                                    (float)(Object2->getHeight() / 2.0) + 
                                    (float)(Object1->getHeight() / 2.0) );
            }

            // Object1 is on relatively the same Y as Object2 so we just
            // push Object1 back.
            else  
            {

                float Obj1xmin = Object1->getXLoc() - ( (float)Object1->getLength() / 2.0f );
                float Obj2xmax = Object2->getXLoc() + ( (float)Object2->getLength() / 2.0f );

                float Obj1zmin = Object1->getZLoc() - ( (float)Object1->getWidth() / 2.0f );
                float Obj2zmax = Object2->getZLoc() + ( (float)Object2->getWidth() / 2.0f );

                // see if Object1 xmin is close to Object2's xmax
                if ( Obj1xmin >= Obj2xmax * onRightRatio )
                {


                    // see if we are close to the corner of Objec2's bounding box
                    // if so bounce in both x and z directions
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 8A\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else // 
                    {
                        Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

                        /*************************************************************/
//DEBUGPRINTF("case 8B\n");
/*************************************************************/

                    }

                }  

                // Object1 is not close to Object's xmin
                else
                {
                    // if so bounce in z direction
                    if ( Obj1zmin >= Obj2zmax * onBackRatio )
                    {
                        Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );


                        /*************************************************************/
//DEBUGPRINTF("case 8C\n");
/*************************************************************/

                    }

                    // since we are not close to object2 bounding box corner
                    // bounce in only x
                    else
                    {
                        if ( fabs(Obj1xmin - Obj2xmax) >= fabs(Obj1zmin - Obj2zmax) )
                        {
                            Object1->setZLoc( Object2->getZLoc() + 
                                            (float)(Object2->getWidth() / 2.0) +
                                            (float)(Object1->getWidth() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 8D\n");
/*************************************************************/
                        }
                        else
                        {

                            Object1->setXLoc( Object2->getXLoc() + 
                                            (float)(Object2->getLength() / 2.0) +
                                            (float)(Object1->getLength() / 2.0) );

/*************************************************************/
//DEBUGPRINTF("case 8E\n");
/*************************************************************/
                        }
                    }

                }

            }

        }  // end case8



    } // end if Object1 ismovable

}  // end BounceBack



	
void ObjectManager::StopMovement( gameObj *Object1, gameObj *Object2,
    double *Obj1vsObj2XMin,
	double *Obj1vsObj2XMax,
    double *Obj1vsObj2YMin,
    double *Obj1vsObj2YMax,
    double *Obj1vsObj2ZMin,
    double *Obj1vsObj2ZMax, 
	BOOL *canMoveXpos,
	BOOL *canMoveYpos,
	BOOL *canMoveZpos,
	BOOL *canMoveXneg,
	BOOL *canMoveYneg,
	BOOL *canMoveZneg,
	double *largerObjLength,
	double *largerObjHeight,
	double *largerObjWidth)
{

    // Now we know there is a collision of some sort and it
    // is not a projectile, so stop movement in appropriate directions.
	// Test to see if gameobject in INSIDE gameobject x boundaries
    // use 0.0001 to account for imperfect floating point numbers

    // use fabs( Obj1vsObj2___ ) < 0.0001 to test to see if the Object1 and is AT the boundary of Object2
    // instead of ((Obj1vsObj2XMax > -0.0001) && (Obj1vsObj2XMax < 0.0001))

	////DEBUGPRINTF("ObjectManager::StopMovement - begin\n");

    StopObj1LeftObj2(Object1, Object2,
                    Obj1vsObj2XMin,
	                Obj1vsObj2XMax,
                    Obj1vsObj2YMin,
                    Obj1vsObj2YMax,
                    Obj1vsObj2ZMin,
                    Obj1vsObj2ZMax, 
	                canMoveXpos,
	                canMoveYpos,
	                canMoveZpos,
	                canMoveXneg,
	                canMoveYneg,
	                canMoveZneg,
	                largerObjLength,
	                largerObjHeight,
	                largerObjWidth);


    StopObj1RightObj2(Object1, Object2,
                    Obj1vsObj2XMin,
	                Obj1vsObj2XMax,
                    Obj1vsObj2YMin,
                    Obj1vsObj2YMax,
                    Obj1vsObj2ZMin,
                    Obj1vsObj2ZMax, 
	                canMoveXpos,
	                canMoveYpos,
	                canMoveZpos,
	                canMoveXneg,
	                canMoveYneg,
	                canMoveZneg,
	                largerObjLength,
	                largerObjHeight,
	                largerObjWidth);

}





void ObjectManager::StopObj1LeftObj2( gameObj *Object1, gameObj *Object2,
                        double *Obj1vsObj2XMin,
	                    double *Obj1vsObj2XMax,
                        double *Obj1vsObj2YMin,
                        double *Obj1vsObj2YMax,
                        double *Obj1vsObj2ZMin,
                        double *Obj1vsObj2ZMax, 
	                    BOOL *canMoveXpos,
	                    BOOL *canMoveYpos,
	                    BOOL *canMoveZpos,
	                    BOOL *canMoveXneg,
	                    BOOL *canMoveYneg,
	                    BOOL *canMoveZneg,
	                    double *largerObjLength,
	                    double *largerObjHeight,
	                    double *largerObjWidth)
{

    // this is the ratio of the height of the bottom of object1 divided the top of object2 at which
    // Object1 is put on top of Object in bounce back.  Same for Object1 being under Object2.
    float onTopRatio = 0.25f;
    float onBottomRatio = 0.25f;

    // this is the ratio of the xmax of object1 over the xmin of object2 at which
    // Object1 is pushed in the -x dir of Object in bounce back. Same for Object1 
    // xmin vs Object2 xmax.
    float onLeftRatio = 0.5f;
    float onRightRatio = 0.5f;

    // this is the ratio of the zmax of object1 over the zmin of object2 at which
    // Object1 is pushed in the -z dir of Object in bounce back. Same for Object1 
    // zmin vs Object2 zmax.
    float onFrontRatio = 0.5f;
    float onBackRatio = 0.5f;


    // first do Object1 xmax INSIDE object2
    if ( (*Obj1vsObj2XMin < -0.0001) && ( fabs( *Obj1vsObj2XMin ) <= *largerObjLength ) )
    {

        // test to see if Object1 yMAX is INSIDE object2 ymin
        if ( (*Obj1vsObj2YMin < -0.0001) && ( fabs( *Obj1vsObj2YMin ) <= *largerObjHeight ) )
        {
			
			// INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{

				//Object1 cannot move positive X dir
			    //Object1 cannot move positive Y dir
			    //Object1 cannot move positive Z dir

			    Object1->setXPos(FALSE);
                Object1->setYPos(FALSE);
			    Object1->setZPos(FALSE);

				*canMoveXpos = FALSE;
				*canMoveYpos = FALSE;
				*canMoveZpos = FALSE;
				
                // bounce Object1 back
                BounceBack(Object1, Object2, TRUE, TRUE, TRUE);

			
			} 

			// AT zmin
			else if	( fabs( *Obj1vsObj2ZMin ) < 0.0001 )
			{
                // Object1 cannot move positive Z dir
				Object1->setZPos(FALSE);

				*canMoveZpos = FALSE;


			}

			// INSIDE zmax
			if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                // Object1 cannot move positive X dir
				// Object1 cannot move positive Y dir
				// Object1 cannot move -Z dir

				Object1->setXPos(FALSE);
				Object1->setYPos(FALSE);
				Object1->setZNeg(FALSE);

				*canMoveXpos = FALSE;
				*canMoveYpos = FALSE;
				*canMoveZneg = FALSE;

                // bounce Object1 back
                BounceBack(Object1, Object2, TRUE, FALSE, TRUE);

			}

			// AT zmax
			else if	( fabs( *Obj1vsObj2ZMax ) < 0.0001 ) 
			{
                // Object1 cannot move -Z dir
				Object1->setZNeg(FALSE);

				*canMoveZneg = FALSE;


			}
                            
		} // end gameobjec yMAX test INSIDE  
							

/******************/
		// test to see if Object1 yMAX is AT Object2 ymin
		else if ( fabs( *Obj1vsObj2YMin ) < 0.0001)
        {

			// inside zmin
            if (*Obj1vsObj2ZMin < -0.0001 && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
                // Object1 cannot move positive Y dir
				Object1->setYPos(FALSE);

				*canMoveYpos = FALSE;
			} 
						
            // inside zmax
            if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                // Object1 cannot move positive Y dir
				Object1->setYPos(FALSE);

				*canMoveYpos = FALSE;

			} 


		}	// end gameobjec yMAX test AT

/******************/

		// test to see if Object1 yMIN is INSIDE Object2 ymax
		if ((*Obj1vsObj2YMax < -0.0001) && ( fabs( *Obj1vsObj2YMax ) <= *largerObjHeight ) )
        {
			// INSIDE zmin
            if (*Obj1vsObj2ZMin < -0.0001 && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
    		{
			    //Object1 cannot move positive X dir
			    //Object1 cannot move -Y dir
			    //Object1 cannot move positive Z dir

			    Object1->setXPos(FALSE);
			    Object1->setYNeg(FALSE);
                Object1->setZPos(FALSE);
                
				*canMoveXpos = FALSE;
				*canMoveYneg = FALSE;
				*canMoveZpos = FALSE;
                
                // bounce Object 1 back
                BounceBack(Object1, Object2, TRUE, TRUE, FALSE);

            } 
		    
            // AT zmin
		    else if	( fabs( *Obj1vsObj2ZMin ) < 0.0001 )
		    {
				//Object1 cannot move positive Z dir
				Object1->setZPos(FALSE);

				*canMoveZpos = FALSE;

			}

			// INSIDE zmax
			if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{										
                //Object1 cannot move positive X dir
				//Object1 cannot move -Y dir
				//Object1 cannot move -Z dir

				Object1->setXPos(FALSE);
				Object1->setYNeg(FALSE);
				Object1->setZNeg(FALSE);

				*canMoveXpos = FALSE;
				*canMoveYneg = FALSE;
				*canMoveZneg = FALSE;

                // bounce Object1 back
                BounceBack(Object1, Object2, TRUE, FALSE, FALSE);

			}

			// AT zmax
			else if	( fabs( *Obj1vsObj2ZMax ) < 0.0001 )  
			{
				//Object1 cannot move -Z dir
				Object1->setZNeg(FALSE);

				*canMoveZneg = FALSE;

			}
                            
		} // end gameobjec yMIN test INSIDE  




/******************/
		// test to see if Object1 yMIN is AT Object2 ymax
		else if ( fabs( *Obj1vsObj2YMax ) < 0.0001 )
        {

			// inside zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
               	//Object1 cannot move -Y dir	
				Object1->setYNeg(FALSE);

				*canMoveYneg = FALSE;
            } 


            // inside zmax
            if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                // Object1 cannot move -Y dir
				Object1->setYNeg(FALSE);

				*canMoveYneg = FALSE;
			} 

																		
		}	// end gameobjec yMAX test AT

/******************/
	} // end Object1 xmax INSIDE Object2 xmin

/******************/

	// Now test Object1 xmax AT Object2 xmin
	else if ( fabs( *Obj1vsObj2XMin ) < 0.0001 )
    {
        // INSIDE ymin
        if ( (*Obj1vsObj2YMin < -0.0001) && ( fabs( *Obj1vsObj2YMin ) <= *largerObjHeight ) ) 
        {
            // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
                //Object1 cannot move positive X dir
				Object1->setXPos(FALSE);

				*canMoveXpos = FALSE;
			} 

			// INSIDE zmax
			else if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                //Object1 cannot move positive X dir
				Object1->setXPos(FALSE);

				*canMoveXpos = FALSE;
			}

        }  // end ymax INSIDE ymin

        // INSIDE ymax
        else if ( (*Obj1vsObj2YMax < -0.0001) && ( fabs( *Obj1vsObj2YMax ) <= *largerObjHeight ) )
        {
            // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
		    {
                //Object1 cannot move positive X dir								
				Object1->setXPos(FALSE);

				*canMoveXpos = FALSE;
			} 

			// INSIDE zmax
			else if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
            {
                //Object1 cannot move positive X dir
				Object1->setXPos(FALSE);

				*canMoveXpos = FALSE;
			}

        }  // end ymin INSIDE ymax

    }  // end test Object1 xmax AT Object2 xmin


}


void ObjectManager::StopObj1RightObj2( gameObj *Object1, gameObj *Object2,
    double *Obj1vsObj2XMin,
	double *Obj1vsObj2XMax,
    double *Obj1vsObj2YMin,
    double *Obj1vsObj2YMax,
    double *Obj1vsObj2ZMin,
    double *Obj1vsObj2ZMax, 
	BOOL *canMoveXpos,
	BOOL *canMoveYpos,
	BOOL *canMoveZpos,
	BOOL *canMoveXneg,
	BOOL *canMoveYneg,
	BOOL *canMoveZneg,
	double *largerObjLength,
	double *largerObjHeight,
	double *largerObjWidth)
{

/*************************************************************************************************/

    // this is the ratio of the height of the bottom of object1 divided the top of object2 at which
    // Object1 is put on top of Object in bounce back.  Same for Object1 being under Object2.
    float onTopRatio = 0.25f;
    float onBottomRatio = 0.25f;

    // this is the ratio of the xmax of object1 over the xmin of object2 at which
    // Object1 is pushed in the -x dir of Object in bounce back. Same for Object1 
    // xmin vs Object2 xmax.
    float onLeftRatio = 0.5f;
    float onRightRatio = 0.5f;

    // this is the ratio of the zmax of object1 over the zmin of object2 at which
    // Object1 is pushed in the -z dir of Object in bounce back. Same for Object1 
    // zmin vs Object2 zmax.
    float onFrontRatio = 0.5f;
    float onBackRatio = 0.5f;


	// now perform all the same tests above but for the left
    // hand side of the bounding box of Object1

    // test Object1 xmin is INSIDE Object2 xmax

    if ( (*Obj1vsObj2XMax < -0.0001) && ( fabs( *Obj1vsObj2XMax ) <= *largerObjLength ) )
	{

	    // test to see if Object1 yMAX is INSIDE Object2 ymin
        if ( (*Obj1vsObj2YMin < -0.0001) && ( fabs( *Obj1vsObj2YMin ) <= *largerObjHeight ) )
        {
		    // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
				//Object1 cannot move -X dir
				//Object1 cannot move positive Y dir
				//Object1 cannot move positive Z dir
	
				Object1->setXNeg(FALSE);
				Object1->setYPos(FALSE);
				Object1->setZPos(FALSE);

				*canMoveXneg = FALSE;
				*canMoveYpos = FALSE;
				*canMoveZpos = FALSE;

                // bounce Object1 back
                BounceBack(Object1, Object2, FALSE, TRUE, TRUE);

			} 

			// AT zmin
			else if	( fabs( *Obj1vsObj2ZMin ) < 0.0001 )
			{
				//Object1 cannot move positive Z dir
				Object1->setZPos(FALSE);

				*canMoveZpos = FALSE;

			}

			// INSIDE zmax
			if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
				//Object1 cannot move -X dir
				//Object1 cannot move positive Y dir
				//Object1 cannot move -Z dir
	
				Object1->setXNeg(FALSE);
				Object1->setYPos(FALSE);
				Object1->setZNeg(FALSE);

				*canMoveXneg = FALSE;
				*canMoveYpos = FALSE;
				*canMoveZneg = FALSE;

                // bounce Object1 back
                BounceBack(Object1, Object2, FALSE, FALSE, TRUE);
			}

			// AT zmax
			else if	( fabs( *Obj1vsObj2ZMax ) < 0.0001 ) 
			{
				//Object1 cannot move -Z dir
				Object1->setZNeg(FALSE);

				*canMoveZneg = FALSE;

			}
                            
		} // end gameobjec yMAX test INSIDE  
/******************/

		// test to see if Object1 yMAX is AT Object2 ymin
		else if ( fabs( *Obj1vsObj2YMin ) < 0.0001)
        {
			// inside zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
                //Object1 cannot move positive Y dir	
				Object1->setYPos(FALSE);

				*canMoveYpos = FALSE;
			} 

            // inside zmax
            if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                // Object1 cannot move positive Y dir
				Object1->setYPos(FALSE);

				*canMoveYpos = FALSE;

			} 
																		
		}	// end gameobjec yMAX test AT

/******************/

        // test to see if Object1 yMIN is INSIDE Object2 ymax
        if ( (*Obj1vsObj2YMax < -0.0001) && ( fabs( *Obj1vsObj2YMax ) <= *largerObjHeight ) )
        {
	        // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
		    {
		        //Object1 cannot move -X dir
			    //Object1 cannot move -Y dir
			    //Object1 cannot move positive Z dir

			    Object1->setXNeg(FALSE);
			    Object1->setYNeg(FALSE);
			    Object1->setZPos(FALSE);

				*canMoveXneg = FALSE;
				*canMoveYneg = FALSE;
				*canMoveZpos = FALSE;

                // bounce Object1 back
                BounceBack(Object1, Object2, FALSE, TRUE, FALSE);

		    } 

		    // AT zmin
		    else if	( fabs( *Obj1vsObj2ZMin ) < 0.0001 )
		    {
		        //Object1 cannot move positive Z dir
			    Object1->setZPos(FALSE);

				*canMoveZpos = FALSE;

		    }

		    // INSIDE zmax
		    if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
		    {
                //Object1 cannot move -Z dir
			    //Object1 cannot move -X dir
			    //Object1 cannot move -Y dir

			    Object1->setXNeg(FALSE);
			    Object1->setYNeg(FALSE);
			    Object1->setZNeg(FALSE);

				*canMoveXneg = FALSE;
				*canMoveYneg = FALSE;
				*canMoveZneg = FALSE;


                // bounce Object1 back
                BounceBack(Object1, Object2, FALSE, FALSE, FALSE);

		    }

		    // AT zmax
		    else if	( fabs( *Obj1vsObj2ZMax ) < 0.0001 )
		    {
			    //Object1 cannot move -Z dir
			    Object1->setZNeg(FALSE);

				*canMoveZneg = FALSE;

		    }
                            
        } // end gameobjec yMIN test INSIDE  

/******************/

	    // test to see if Object1 yMIN is AT Object2 ymax
	    else if ( fabs( *Obj1vsObj2YMax ) < 0.0001 )
        {
		    // inside zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
		    {
                //Object1 cannot move -Y dir	
			    Object1->setYNeg(FALSE);
	
				*canMoveYneg = FALSE;
		    } 
    								
            // inside zmax
            if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                // Object1 cannot move positive Y dir
				Object1->setYNeg(FALSE);

				*canMoveYneg = FALSE;

			} 

	    }	// end gameobjec yMAX test AT

/******************/
    } // end Object1 xmin INSIDE Object2 xmax


/******************/
    // Now test Object1 xmin AT Object2 xmax
	else if ( fabs( *Obj1vsObj2XMax ) < 0.0001 ) 
    {
	    // INSIDE ymin
        if ( (*Obj1vsObj2YMin < -0.0001) && ( fabs( *Obj1vsObj2YMin ) <= *largerObjHeight ) ) 
        {
            // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
                //Object1 cannot move -X dir								
				Object1->setXNeg(FALSE);
				
				*canMoveXneg = FALSE;
			} 

			// INSIDE zmax
			else if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) ) 
			{
                //Object1 cannot move -X dir
				Object1->setXNeg(FALSE);

				*canMoveXneg = FALSE;
			}

        }  // end ymax INSIDE ymin

/******************/ 
        // INSIDE ymax               
        else if ( (*Obj1vsObj2YMax < -0.0001) && ( fabs( *Obj1vsObj2YMax ) <= *largerObjHeight ) )
        {
            // INSIDE zmin
            if ( (*Obj1vsObj2ZMin < -0.0001) && ( fabs( *Obj1vsObj2ZMin ) <= *largerObjWidth ) )
			{
                //Object1 cannot move -X dir								
				Object1->setXNeg(FALSE);

				*canMoveXneg = FALSE;
			} 

			// INSIDE zmax
			else if ( (*Obj1vsObj2ZMax < -0.0001) && ( fabs( *Obj1vsObj2ZMax ) <= *largerObjWidth ) )
			{
                //Object1 cannot move -X dir
				Object1->setXNeg(FALSE);
				
				*canMoveXneg = FALSE;
				
			}

        }  // end ymin INSIDE ymax

    }  // end test Object1 xmin AT Object2 xmax

} // end StopObj1RightObj2





// CollisionDetection test if there is a collision between the 
// bounding boxes of the 2 objects provided


BOOL ObjectManager::CollisionDetection( gameObj *Object1, gameObj *Object2,
    double &Obj1vsObj2XMin,
	double &Obj1vsObj2XMax,
    double &Obj1vsObj2YMin,
    double &Obj1vsObj2YMax,
    double &Obj1vsObj2ZMin,
    double &Obj1vsObj2ZMax)
{
	////DEBUGPRINTF("ObjectManager::CollisionDetection - begin\n");
    // Determine Object1 x boundaries
    double Object1XMin = ( Object1->loc().x - (Object1->getLength() / 2.0) );
	double Object1XMax = ( Object1->loc().x + (Object1->getLength() / 2.0) );

    // Determine Object2 x boundaries
    double Object2XMin = ( Object2->loc().x - (Object2->getLength() / 2.0) );
	double Object2XMax = ( Object2->loc().x + (Object2->getLength() / 2.0) );

    // see if Object1 and Object2 x boundaries overlap
    Obj1vsObj2XMin = Object2XMin - Object1XMax;
	Obj1vsObj2XMax = Object1XMin - Object2XMax;

    // use fabs to make sure that Object1 is not TOO far left or right of Object2
    double AbsObj1vsObj2XMin = fabs( Obj1vsObj2XMin );
    double AbsObj1vsObj2XMax = fabs( Obj1vsObj2XMax );
  
    BOOL XMinoverlap = FALSE; 
    BOOL XMaxoverlap = FALSE;



	// Find out the larger object length between Object1 and Object2
	double largerObjLength;

	if ( Object1->getLength() >= Object2->getLength() )
		largerObjLength = Object1->getLength();
	else
		largerObjLength = Object2->getLength();


    //test to see if Object1 is left or right of Object2 but use AbsObj to make sure that is 
    //not too far left or right
    if ( (Obj1vsObj2XMin <= 0.0000001) && (AbsObj1vsObj2XMin <= largerObjLength ) )
            XMinoverlap = TRUE;

    if ( (Obj1vsObj2XMax <= 0.0000001) && (AbsObj1vsObj2XMax <= largerObjLength ) )
            XMaxoverlap = TRUE;

    // only test Y boundaries if there is overlap in the X boundaries
	if ( XMinoverlap || XMaxoverlap )
    {			

	    // Determine Object1 y boundaries
        double Object1YMin = ( Object1->location.y - (Object1->getHeight() / 2.0) );
	    double Object1YMax = ( Object1->location.y + (Object1->getHeight() / 2.0) );

        // Determine Object2 y boundaries
        double Object2YMin = ( Object2->location.y - (Object2->getHeight() / 2.0) );
	    double Object2YMax = ( Object2->location.y + (Object2->getHeight() / 2.0) );

        // see if Object1 and Object2 y boundaries overlap
        Obj1vsObj2YMin = (Object2YMin - Object1YMax);
	    Obj1vsObj2YMax = (Object1YMin - Object2YMax);


        // use fabs to make sure that Object1 is not TOO far above or below of Object2
        double AbsObj1vsObj2YMin = fabs( Obj1vsObj2YMin );
        double AbsObj1vsObj2YMax = fabs( Obj1vsObj2YMax );
      
        BOOL YMinoverlap = FALSE; 
        BOOL YMaxoverlap = FALSE;


		// Find out the larger object height between Object1 and Object2
		double largerObjHeight;

		if ( Object1->getHeight() >= Object2->getHeight() )
			largerObjHeight = Object1->getHeight();
		else
			largerObjHeight = Object2->getHeight();


        //test to see if Object1 is above or below of Object2 but use AbsObj to make sure that is 
        //not too far above or below
        if ( (Obj1vsObj2YMin <= 0.0000001) && (AbsObj1vsObj2YMin <= largerObjHeight ) )
                YMinoverlap = TRUE;

        if ( (Obj1vsObj2YMax <= 0.0000001) && (AbsObj1vsObj2YMax <= largerObjHeight ) )
                YMaxoverlap = TRUE;


        // only test Z boundaries if there is overlap in the Y boundaries
	    if ( YMinoverlap || YMaxoverlap )
        {			
		
            // Determine Object1 z boundaries
            double Object1ZMin = ( Object1->loc().z - (Object1->getWidth() / 2.0) );
	        double Object1ZMax = ( Object1->loc().z + (Object1->getWidth() / 2.0) );

            // Determine Object2 z boundaries
            double Object2ZMin = ( Object2->loc().z - (Object2->getWidth() / 2.0) );
	        double Object2ZMax = ( Object2->loc().z + (Object2->getWidth() / 2.0) );

            // see if Object1 and Object2 z boundaries overlap
            Obj1vsObj2ZMin = (Object2ZMin - Object1ZMax);
	        Obj1vsObj2ZMax = (Object1ZMin - Object2ZMax);


            // use fabs to make sure that Object1 is not TOO far left or right of Object2
            double AbsObj1vsObj2ZMin = fabs( Obj1vsObj2ZMin );
            double AbsObj1vsObj2ZMax = fabs( Obj1vsObj2ZMax );
              
            BOOL ZMinoverlap = FALSE; 
            BOOL ZMaxoverlap = FALSE;

	
			// Find out the larger object width between Object1 and Object2
			double largerObjWidth;

			if ( Object1->getWidth() >= Object2->getWidth() )
				largerObjWidth = Object1->getWidth();
			else
				largerObjWidth = Object2->getWidth();


            //test to see if Object1 is in front of or behind Object2 but use AbsObj to make sure that is 
            //not too far in front of or behind
            if ( (Obj1vsObj2ZMin <= 0.0000001) && (AbsObj1vsObj2ZMin <= largerObjWidth ) )
                ZMinoverlap = TRUE;

            if ( (Obj1vsObj2ZMax <= 0.0000001) && (AbsObj1vsObj2ZMax <= largerObjWidth ) )
                ZMaxoverlap = TRUE;


            if ( ZMinoverlap || ZMaxoverlap )
            {



///**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**/
//		if (Object1 == m_pObjectList[m_nPlayerIndex])
//		{
//			//char tempbuff[100];
//			//sprintf(tempbuff,"%s hit by %s \n", m_pObjectList[i]->theName.c_str(), m_pObjectList[j]->theName.c_str() );
//			////DEBUGPRINTF(tempbuff);
//			
//			generateParticles(1, ROTATE_VERTICAL_TYPE, COLOR_PARTICLE, 50, 0, 1.0, 1.0, 0.0, 1.0);
//		}
///**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**/


                    // //DEBUGPRINTF("there was a CollisionDetection.\n");
                    return TRUE;
            }
            
        } // end z boundary tests

    } // end Y boundary tests

    return FALSE;

} // end CollisionDetection





// return false if we don't want to test these objects for collision
// each other
BOOL ObjectManager::BypassCertainObjects(gameObj *Object1, gameObj *Object2)
{
    // dynamic cast Object1 and Object2
	// so that we can test to see if they are projectileObj
	projectileObj *projectile1 = dynamic_cast< projectileObj * >( Object1 );
	projectileObj *projectile2 = dynamic_cast< projectileObj * >( Object2 );												
	// if both are projectile set noCollision b/c we don't
	// want collision between projectiles
	if ((projectile1 != 0) && (projectile2 != 0))
		return TRUE;
    else
        return FALSE;

}




// tests collision and sets appropriate flags for all objects in the object list
void ObjectManager::CollisionManager()
{
	////DEBUGPRINTF("ObjectManager::CollisionManager - begin\n");
    
    for (int i = 0; i < m_nCount ; i++)  // test all objects in list
    {

        if (m_pObjectList[i] != NULL)
        {
			if ( m_pObjectList[i]->collidable() )
			{
				// flag to reset movement of objects after collisions
				BOOL collision = FALSE;
				BOOL canMoveXpos = TRUE;
				BOOL canMoveYpos = TRUE;
				BOOL canMoveZpos = TRUE;
				BOOL canMoveXneg = TRUE;
				BOOL canMoveYneg = TRUE;
				BOOL canMoveZneg = TRUE;

				for (int j = 0; j < m_nCount ; j++) // against all the other objects in the list
				{
					if (m_pObjectList[j] != NULL)
					{

						////DEBUGPRINTF("ObjectManager::CollisionManager - loop j, j:%d\n", j);
						if ( j != i ) // don't want to test an object against itself
						{

							// as long as both object exists and can be run into
							// and at least 1 of them is capable of movement
							if ( (m_pObjectList[j]->collidable()) &&
								(m_pObjectList[i]->movable || m_pObjectList[j]->movable) &&
								(!BypassCertainObjects(m_pObjectList[i], m_pObjectList[j]))
								)
							{
								double XMinRelation = 1.0;
								double XMaxRelation = 1.0;
								double YMinRelation = 1.0;
								double YMaxRelation = 1.0;
								double ZMinRelation = 1.0;
								double ZMaxRelation = 1.0;

								// call Bool CollisionDetection to see if there is a collision
								// it modifies XMinRelation, XMaxRelation, YMinRelation, YMaxRelation,
								// ZMinRelation, and ZMinRelation so they can be used
								// in Stopmovement

								
								collision = CollisionDetection(m_pObjectList[i], m_pObjectList[j], 
									XMinRelation,
									XMaxRelation,
									YMinRelation,
									YMaxRelation,
									ZMinRelation,
									ZMaxRelation);
		            
								if (collision) 
								{     

									//if (m_pObjectList[i] == m_pObjectList[m_nPlayerIndex])
									//{
									//	char tempbuff[100];

									//	sprintf(tempbuff,"%s position is %f, %f, %f \n", 
									//		m_pObjectList[i]->theName.c_str(), 
									//		m_pObjectList[i]->getXLoc(),
									//		m_pObjectList[i]->getYLoc(),
									//		m_pObjectList[i]->getZLoc()
									//		);

									//	//DEBUGPRINTF(tempbuff);

									//	sprintf(tempbuff,"%s position is %f, %f, %f \n", 
									//		m_pObjectList[j]->theName.c_str(), 
									//		m_pObjectList[j]->getXLoc(),
									//		m_pObjectList[j]->getYLoc(),
									//		m_pObjectList[j]->getZLoc()
									//		);

									//	//DEBUGPRINTF(tempbuff);
									//	
									//}


				               		// check if it food				        
									if ( m_pObjectList[j]->delicious() || m_pObjectList[i]->delicious() ) 
									{  
										
										// test to see if is a player colliding with the food or armor
										if ( (m_pObjectList[i]->faction() == PLAYER_FACTION) || (m_pObjectList[j]->faction() == PLAYER_FACTION) )
										{

											if (addToInventory(m_pObjectList[j]))
											{
												m_pObjectList[j] = NULL;
												m_nCount--;
											}

										} // end if player took food
									} // end if it's food

									// it is not food, can either causedamage?
									else if ( m_pObjectList[i]->dangerous() ||
											m_pObjectList[j]->dangerous() )
									{

										//if (m_pObjectList[i] == m_pObjectList[m_nPlayerIndex])
										//{
										//	char tempbuff[100];
										//	sprintf(tempbuff,"%s hit by %s \n", m_pObjectList[i]->theName.c_str(), m_pObjectList[j]->theName.c_str() );
										//	//DEBUGPRINTF(tempbuff);

										////	generateParticles(3, ROTATE_VERTICAL_TYPE, COLOR_PARTICLE, 50, 0, 1.0, 1.0, 0.0, 1.0);

										//}

										//if (m_pObjectList[i]->dangerous()

										combatManager(m_pObjectList[j], m_pObjectList[i]);

									}
									else
									{

										//if (m_pObjectList[i] == m_pObjectList[m_nPlayerIndex])
										//{
										//	char tempbuff[100];

										//	sprintf(tempbuff,"%s position is %f, %f, %f \n", 
										//		m_pObjectList[i]->theName.c_str(), 
										//		m_pObjectList[i]->getXLoc(),
										//		m_pObjectList[i]->getYLoc(),
										//		m_pObjectList[i]->getZLoc()
										//		);

										//	//DEBUGPRINTF(tempbuff);

										//	//generateParticles(3, ROTATE_VERTICAL_TYPE, 
										//	//					COLOR_PARTICLE, 50, 0, 
										//	//					1.0, 1.0, 0.0, 1.0);

										//}
										


										// we need to stop m_pObjectList[i] b/c it
										// hit something and it wasn't food 
										// or a projectile

										// determine the larger dimension for the calculation
										double largerObjLength;
										double largerObjHeight;
										double largerObjWidth;


										if ( m_pObjectList[i]->getLength() >= m_pObjectList[j]->getLength() )
											largerObjLength = m_pObjectList[i]->getLength();
										else
											largerObjLength = m_pObjectList[j]->getLength();


										if ( m_pObjectList[i]->getHeight() >= m_pObjectList[j]->getHeight() )
											largerObjHeight = m_pObjectList[i]->getHeight();
										else
											largerObjHeight = m_pObjectList[j]->getHeight();


										if ( m_pObjectList[i]->getWidth() >= m_pObjectList[j]->getWidth() )
											largerObjWidth = m_pObjectList[i]->getWidth();
										else
											largerObjWidth = m_pObjectList[j]->getWidth();


										// actually stop m_pObjectList[i] now
										StopMovement(m_pObjectList[i], m_pObjectList[j],
											&XMinRelation,
											&XMaxRelation,
											&YMinRelation,
											&YMaxRelation,
											&ZMinRelation,
											&ZMaxRelation,
											&canMoveXpos,
											&canMoveYpos,
											&canMoveZpos,
											&canMoveXneg,
											&canMoveYneg,
											&canMoveZneg,
											&largerObjLength,
											&largerObjHeight,
											&largerObjWidth);
										
									}  // end else {StopMovement}
									
									
									
									// dynamic cast m_pObjectList[i] so that we can test to see if it 
									// a projectileObj
									
									//projectileObj *projectile = dynamic_cast< projectileObj * >( m_pObjectList[i] );
													
									/*if (projectile != 0)
									{
										if (projectile->dangerous()) // it is a projectile, call combat manager to handle it
											combatManager(m_pObjectList[i], m_pObjectList[j]);
			
										// don't run anymore collision b/c projectile can only hit 1 object
										j = m_nMaxCount;
									}
									else 
									{
										// stop the movement of m_pObjectList[i] in the appropriate directions
										StopMovement(m_pObjectList[i],
											XMinRelation,
											XMaxRelation,
											YMinRelation,
											YMaxRelation,
											ZMinRelation,
											ZMaxRelation);
									} // end else */

								} // end if collision

							} // !null and colliable

						} // end if (i != j)

					} // end if [j] not null

				} // end j for loop

				// reset collisions back to false so
				// that the object can move again
				// if the StopMovement function has not already 
				// set the flag

				if (canMoveXpos)
					m_pObjectList[i]->setXPos(TRUE); 

				if (canMoveYpos)
					m_pObjectList[i]->setYPos(TRUE); 

				if (canMoveZpos)
					m_pObjectList[i]->setZPos(TRUE); 

				if (canMoveXneg)
					m_pObjectList[i]->setXNeg(TRUE); 

				if (canMoveYneg)
					m_pObjectList[i]->setYNeg(TRUE); 

				if (canMoveZneg)
						m_pObjectList[i]->setZNeg(TRUE); 


			}  // end if [i] is colliable

		} // end if [i] not null

	} // end i for loop

} // end CollisionManager
/*********************************************************************/

inventoryObj* ObjectManager::get_pInventory()
{
	return theInventory;
}


int ObjectManager::getCurrentInventoryIndex()
{
	 
	inventoryObj *invObj = dynamic_cast< inventoryObj * >( theInventory );
	if (invObj != NULL)
		return  invObj->getCurrentIndex();
	else
		return 0;


}
/*********************************************************************/
