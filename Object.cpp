/// \file object.cpp
/// Code for the game object class CGameObject.
/// Copyright Ian Parberry, 2004.
/// Last updated August 25, 2004.

#include <string>
#include "object.h"
#include "defines.h" //essential defines
#include "timer.h" //game timer
#include "spriteman.h" //sprite manager
#include "random.h" // random numbers
#include "debug.h"
#include "sound.h" //for sound manager
#include "objman.h"

using namespace std;

extern CTimer g_cTimer; ///< game timer
extern CRandom g_cRandom; /// <random number generator
extern int g_nScreenWidth; ///< screen width
extern int g_nScreenHeight; ///< screen height
extern CSpriteManager g_cSpriteManager; ///< sprite manager
extern TiXmlElement* g_xmlSettings; ///< XML settings
extern CSoundManager* g_pSoundManager; ///< sound manager
extern ObjectManager g_cObjectManager; ///< object manager
extern IDirect3DDevice9* g_d3ddevice; ///< graphics device


/*************************************************************************************/

// prototypes
void setLength(double L);
void setHeight(double H);
void setWidth(double W);
		
double getLength();
double getHeight();
double getWidth();

/*************************************************************************************/







// ===============================================================================================================
// **										Constructor/Destructor Functions									**
// ===============================================================================================================

//==========================================
// gameObj default constructor
// shouldn't be used in game, but in case 
// something goes wrong, it's here
//==========================================
gameObj::gameObj()
{
	Animation = NULL;
	theObjectType = rock;
	animationFrameCount = 0;
	currentFrame = 0;
	lastFrameTime = g_cTimer.time();
	frameInterval = 150; 
	theName = "rock";
	location = D3DXVECTOR3(200, 200, 200);
	animationEnded = true;
	LoadSettings(theName);
	yFloor = (structSize.y() / 2);
	//DEBUGPRINTF("gameObj::constructor - yFloor set to %d, structsize y value is %d\n", yFloor, structSize.y());
	
	fallTime = 0;
	xSpeed = 0;
	ySpeed = 0;
	zSpeed = 0;
	xSpeedLimit = 0;
	zSpeedLimit = 0;
	dead = false;
	theDirection = FACE_RIGHT;
	currAction = nothing;
	currFaction = INANIMATE_OBJECT;
	loadAnimation(rock);
	move_xPos = true;
	move_yPos = true;
	move_zPos = true;
	move_xNeg = true;
	move_yNeg = true;
	move_zNeg = true;
	isDangerous = false;
	isParticle = false;
	theReach = 0;
	isDelicious = false;
	running = false;
	ducking = false;
	defending = false;
	origYsize = structSize.y();

	ZeroMemory(&theMaterial,sizeof(theMaterial));

	// color of ambient light reflected
	theMaterial.Ambient.r = 1.0f;
	theMaterial.Ambient.g = 1.0f; 
	theMaterial.Ambient.b = 1.0f; 
	theMaterial.Ambient.a = 1.0f; 

	// color of ambient light diffused
	theMaterial.Diffuse.r = 1.0f;
	theMaterial.Diffuse.g = 1.0f; 
	theMaterial.Diffuse.b = 1.0f; 
	theMaterial.Diffuse.a = 1.0f; 

	// color of specular highlights reflected
	theMaterial.Specular.r = 0.0f;
	theMaterial.Specular.g = 0.0f; 
	theMaterial.Specular.b = 0.0f; 
	theMaterial.Specular.a = 1.0f; 

	// color that it glows
	theMaterial.Emissive.r = 0.0f;
	theMaterial.Emissive.g = 0.0f; 
	theMaterial.Emissive.b = 0.0f; 
	theMaterial.Emissive.a = 0.0f; 

	lightIndex = -1; // index of light object maintains, if exists


///****************************************************************************/
//
//	// Use these default dimensions unless we specifically
//	// them later
//	length = 45.0;
//	height = 45.0;
//	width = 65.0;
//
///****************************************************************************/


	CountedAlready = true;
	UseCriticalHit = false;


} // end gameObj default constructor


//==========================================
// gameObj implicit constructor
// will actually be used in game
//==========================================
gameObj::gameObj(string objName, ObjectType newType, D3DXVECTOR3 p)
{
	Animation = NULL;
	if (newType == rock)
		theObjectType = blood;
	else
		theObjectType = rock;

	animationFrameCount = 0;
	currentFrame = 0;
	lastFrameTime = g_cTimer.time();
	frameInterval = 150; 
	theName = objName;
	location = p;
	animationEnded = true;
	LoadSettings(theName);
	yFloor = (structSize.y() / 2);
	//DEBUGPRINTF("gameObj::constructor - yFloor set to %d, structsize y value is %d\n", yFloor, structSize.y());
	fallTime = 0;
	xSpeed = 0;
	ySpeed = 0;
	zSpeed = 0;
	xSpeedLimit = 0;
	zSpeedLimit = 0;
	dead = false;
	theDirection = FACE_RIGHT;
	currAction = nothing;
	currFaction = INANIMATE_OBJECT;
	loadAnimation(newType);
	move_xPos = true;
	move_yPos = true;
	move_zPos = true;
	move_xNeg = true;
	move_yNeg = true;
	move_zNeg = true;
	isDangerous = false;
	isParticle = false;
	theReach = 0;
	isDelicious = false;
	running = false;
	ducking = false;
	defending = false;
	origYsize = structSize.y();
	ZeroMemory(&theMaterial,sizeof(theMaterial));

	// color of ambient light reflected
	theMaterial.Ambient.r = 1.0f;
	theMaterial.Ambient.g = 1.0f; 
	theMaterial.Ambient.b = 1.0f; 
	theMaterial.Ambient.a = 1.0f; 

	// color of ambient light diffused
	theMaterial.Diffuse.r = 1.0f;
	theMaterial.Diffuse.g = 1.0f; 
	theMaterial.Diffuse.b = 1.0f; 
	theMaterial.Diffuse.a = 1.0f; 

	// color of specular highlights reflected
	theMaterial.Specular.r = 0.0f;
	theMaterial.Specular.g = 0.0f; 
	theMaterial.Specular.b = 0.0f; 
	theMaterial.Specular.a = 1.0f; 

	// color that it glows
	theMaterial.Emissive.r = 0.0f;
	theMaterial.Emissive.g = 0.0f; 
	theMaterial.Emissive.b = 0.0f; 
	theMaterial.Emissive.a = 0.0f; 

	lightIndex = -1; // index of light object maintains, if exists

	CountedAlready = true;
	UseCriticalHit = false;


///****************************************************************************/
//
//	// Use these default dimensions unless we specifically
//	// them later
//	length = 45.0;
//	height = 45.0;
//	width = 65.0;
//
///****************************************************************************/
//


} // end gameObj implicit constructor


//==========================================
// gameObj destructor
// deletes animation and disbles lights
//==========================================
gameObj::~gameObj()
{
	//DEBUGPRINTF("gameObj destructor\n");
	if (lightIndex != -1)
	{
		g_d3ddevice->LightEnable(lightIndex, FALSE);
	}
	delete [] Animation;
} // end gameObj destructor


// ===============================================================================================================
// **											Protected Functions												**
// ===============================================================================================================


//==========================================
// gameObj::loadAnimation method
// loads animation from XML file and sets
// object to new type specified as parameter
//==========================================
void gameObj::loadAnimation(ObjectType newObject)
{
	if (newObject != theObjectType) // if object type has changed
	{
		//DEBUGPRINTF("gameObj::loadAnimation - object %s changed from type %d to %d\n", theName.c_str(), theObjectType, newObject);
		TiXmlElement* animationSettings = g_xmlSettings->FirstChildElement("sprites"); //look for <sprites> tag
		currentFrame = 0; // start at frame 0

		if(animationSettings) // if we found the <sprites> tag
		{
			animationEnded = false; // animation isn't at end
			animationFrameCount = 0; // reset frame count
			cycleSprite = false; // reset cycle setting
			delete[] Animation; // free up memory used by Animation
			Animation = NULL; // reset Animation pointer

			//DEBUGPRINTF("gameObj::draw - animationSettings found\n");

			TiXmlElement* anim = animationSettings->FirstChildElement("sprite"); //set anim to the first "sprite" tag with the correct name
			int objectNum = -1;
			anim->Attribute("ObjectType", &objectNum);
			while(anim && !(objectNum == newObject)) // cycle through looking for correct <sprite> tag
			{
				//DEBUGPRINTF("gameObj::loadAnimation - objectType we want is %d, one we are looking at in XML is %d\n", theObjectType, objectNum);
				anim = anim->NextSiblingElement("sprite");
				anim->Attribute("ObjectType", &objectNum);
				
			} // end while

			if(anim) // if we found right <sprite> tag
			{
				//DEBUGPRINTF("gameObj::loadAnimation - sprite tag with correct name found\n");

				//load cycle setting
				int testCycle = 0;
				anim->Attribute("cycle", &testCycle);
				anim->Attribute("interval", &frameInterval);

				if (testCycle == 1)
				{
					cycleSprite = true;
					//DEBUGPRINTF("gameObj::loadAnimation - cycleSprite = true\n");
				}
				else
				{
					cycleSprite = false;
					//DEBUGPRINTF("gameObj::loadAnimation - cycleSprite = false\n");
				}
				

				//get sequence length
				int length = strlen(anim->Attribute("sequence"));
				animationFrameCount = 1; //one more than number of commas

				for (int i = 0; i < length; i++) //for each character
				{
					if(anim->Attribute("sequence")[i] == ',') // if it's a comma
					{
						animationFrameCount++; // increase frame count
					} // end if comma

					//DEBUGPRINTF("gameObj::loadAnimation - animationFrameCount:%d\n", animationFrameCount);

				} // end for i

				//DEBUGPRINTF("gameObj::loadAnimation - total frame count for new animation: %d\n", animationFrameCount);
				
				Animation = new int[animationFrameCount]; //memory for animation sequence
				i = 0; //character index
				int count = 0; //number of frame numbers input
				int num; //frame number
				char c = anim->Attribute("sequence")[i]; //character in sequence string

				while(i < length) // while less than length, get next frame number
				{ 
					num = 0;
					while(i < length && c >= '0' && c <= '9')
					{
						num = num*10 + c - '0';
						c = anim->Attribute("sequence")[++i];
					} // end while
					//process frame number
					c = anim->Attribute("sequence")[++i]; //skip over comma
					//DEBUGPRINTF("gameObj::loadAnimation - Animation[%d] = %d \n", (count + 1), num);
					Animation[count++] = num; //record frame number
				} // end while

			} // end if we found correct <sprite> tag
			else // else we didn't find right <sprite> tag
			{
				//DEBUGPRINTF("gameObj::loadAnimation - correct sprite not found\n");
			} // end else <sprite> tag not found

		} // end if we found <sprites> tag
		else // else we didnt find <sprites> tag... serious problem here
		{
			//DEBUGPRINTF("gameObj::loadAnimation - animationSettings NOT found\n");
		} // end else we didn't find <sprites> tag

		theObjectType = newObject; // set object to new type

	} // end if object type has changed
	else //else it's the same type, no need to reload animation
	{
		//DEBUGPRINTF("gameObj::loadAnimation - no change, no setting will be loaded\n");
	} // end else it's same type

} // end method gameObj::loadAnimation


//==========================================
// gameObj::LoadSettings method
// loads object settings from XML file 
//==========================================
void gameObj::LoadSettings(string name)
{

	int newX, newY, newZ;

	if(g_xmlSettings) // if we found settings
	{

		TiXmlElement* objSettings = g_xmlSettings->FirstChildElement("objects"); //get <objects> tag

		if(objSettings) // if we found <objects> tag
		{
			
			TiXmlElement* obj = objSettings->FirstChildElement("object"); //set obj to the first "object" tag with the correct name
      
			while(obj && name != obj->Attribute("name")) // while current <object> tag is not correct <object> tag
            {
				obj = obj->NextSiblingElement("object");
			} // end while

			if(obj) // if we found the <object> tag with the right name
			{
				// load settings from tag
				obj->Attribute("length",&newX);
				obj->Attribute("height",&newY);
				obj->Attribute("width",&newZ);


				length = (double) newX;
				height = (double) newY;
				width = (double) newZ; 

				structSize.setX(newX);
				//DEBUGPRINTF("gameObj::LoadSettings - grabbed length for object %s, newX = %d, actual = %d\n", name.c_str(), newX, structSize.x());
				structSize.setY(newY);
				//DEBUGPRINTF("gameObj::LoadSettings - grabbed height for object %s, newY = %d, actual = %d\n", name.c_str(), newY, structSize.y());
				structSize.setZ(newZ);
				//DEBUGPRINTF("gameObj::LoadSettings - grabbed width for object %s, newZ = %d, actual = %d\n", name.c_str(), newZ, structSize.z());

				int testIntelligent, testCollidable, testNPC, testMovable;

				obj->Attribute("intelligent", &testIntelligent);
				//DEBUGPRINTF("gameObj::LoadSettings - grabbed intelligence for object %s, intelligent = %d\n", name.c_str(), testIntelligent);
				if (testIntelligent == 1)
					isIntelligent = true;
				else
					isIntelligent = false;
		
				obj->Attribute("collidable", &testCollidable);
				if (testCollidable == 1)
					isCollidable = true;
				else
					isCollidable = false;

		        obj->Attribute("lifetime", &lifetime);
 	
				obj->Attribute("npc", &testNPC);
				if (testNPC == 1)
					isNPC = true;
				else
					isNPC = false;


				obj->Attribute("movable", &testMovable);
				if (testMovable == 1)
					movable = TRUE;
				else
					movable = FALSE;
                


			} // end if we found right <object> tag
		} // end if we found the <objects> tag
	} // end if we found settings

} // end method gameObj::LoadSettings

// ===============================================================================================================
// **											Data Access Functions											**
// ===============================================================================================================

bool gameObj::AlreadyCounted()
{
	if(!CountedAlready)
	{
		//DEBUGPRINTF("gameObj::AlreadyCounted - COUNTED\n");
		CountedAlready = true;
		return false;
	}
	else
	{
		return true;
		//DEBUGPRINTF("gameObj::AlreadyCounted - ALREADY COUNTED!!!!\n");
	}
}

void gameObj::Critical(int DeadEnemy)
{
	GetCriticalPower = GetCriticalPower + DeadEnemy;
}

void gameObj::ResetPower()
{
	GetCriticalPower = 0;
}

void gameObj::CriticalKey()
{
	UseCriticalHit = true;
}
void gameObj::ResetCriticalKey()
{
	UseCriticalHit = false;
}

//==========================================
// gameObj::loc method
// returns location of object as D3DXVECTOR3
//==========================================
D3DXVECTOR3 gameObj::loc()
{
	return location;
} // end method gameObj::loc


//==========================================
// gameObj::size method
// returns size of object as sizeMatrix
//==========================================
sizeMatrix gameObj::size()
{
	return structSize;
} // end method gameObj::size


//==========================================
// gameObj::direction method
// returns direction of object 
// as faceDirection type
//==========================================
faceDirection gameObj::direction()
{
	return theDirection;
} // end method gameObj::direction


//==========================================
// gameObj::faction method
// returns faction of object as factionType
//==========================================
factionType gameObj::faction()
{
	return currFaction;
} // end method gameObj::faction


//==========================================
// gameObj::currentType method
// returns type of object as ObjectType
//==========================================
ObjectType gameObj::currentType()
{
	return theObjectType;
} // end method gameObj::currentType


//==========================================
// gameObj::name method
// returns name of object as string
//==========================================
string gameObj::name()
{
	return theName;
} // end method gameObj::name


//==========================================
// gameObj::getXSpeed method
// returns xSpeed of object as float
//==========================================
float gameObj::getXSpeed()
{
	return xSpeed;
} // end method gameObj::getXSpeed


//==========================================
// gameObj::getYSpeed method
// returns ySpeed of object as float
//==========================================
float gameObj::getYSpeed()
{
	return ySpeed;
} // end method gameObj::getYSpeed


//==========================================
// gameObj::getZSpeed method
// returns zSpeed of object as float
//==========================================
float gameObj::getZSpeed()
{
	return zSpeed;
} // end method gameObj::getZSpeed


//==========================================
// gameObj::collidable method
// returns true if object is collidable
//==========================================
bool gameObj::collidable()
{
	return isCollidable;
} // end method gameObj::collidable


//==========================================
// gameObj::isDead method
// returns true if object is dead
//==========================================
bool gameObj::isDead()
{
	return dead;
} // end method gameObj::isDead


//==========================================
// gameObj::dangerous method
// returns true if object causes damage
//==========================================
bool gameObj::dangerous()
{
	return isDangerous;
} // end method gameObj::dangerous


//==========================================
// gameObj::particle method
// returns true if object is a particle
//==========================================
bool gameObj::particle()
{
	return isParticle;
} // end method gameObj::particle


//==========================================
// gameObj::intelligent method
// returns true if object is intelligent
//==========================================
bool gameObj::intelligent()
{
	return isIntelligent;
} // end method gameObj::intelligent


//==========================================
// gameObj::NPC method
// returns true if object is an NPC
//==========================================
bool gameObj::NPC()
{
	return isNPC;
}  // end method gameObj::NPC


//==========================================
// gameObj::delicious method
// returns true if object is 
// usable in inventory
//==========================================
bool gameObj::delicious()
{
	return isDelicious;
} // end method gameObj::delicious


//==========================================
// gameObj::reach method
// returns reach of object as int
//==========================================
int gameObj::reach()
{
	return theReach;
} // end method gameObj::reach


//==========================================
// gameObj::lifeRemaining method
// returns lifetime of object as int
//==========================================
int gameObj::lifeRemaining()
{
	return lifetime;
} // end method gameObj::lifeRemaining()


//==========================================
// gameObj::damageAmount method
// not used by base object
// allows successors to return amount
// of damage they do as an int
//==========================================
int gameObj::damageAmount()
{
	return 0;
} // end method gameObj::damageAmount


// ===============================================================================================================
// **											Utility Functions												**
// ===============================================================================================================


//==========================================
// gameObj::kill method
// destroys object by setting lifetime to 0
//==========================================
void gameObj::kill()
{
	lifetime = 0;
} // end method gameObj::kill


//==========================================
// gameObj::draw method
// draw object at correct frame
// advance to next frame if neccessary
//==========================================
void gameObj::draw() //draw
{
	////DEBUGPRINTF("gameObj::draw - begin\n");
	
	g_d3ddevice->SetMaterial(&theMaterial); //set object's material

	int t = frameInterval; //frame interval
	
	if (Animation == NULL) // if there is no animation loaded...
	{
		
		//DEBUGPRINTF("gameObj::draw - drawing object %s as static sprite facing direction %d\n", theName.c_str(), theDirection);
		g_cSpriteManager.Draw(theObjectType, location, 0, theDirection); // draw as static sprite
		
	} // end if there is no animation
	else // else there is an animation
	{
		//DEBUGPRINTF("gameObj::draw - drawing object %s as type %d, animation frame is %d, currentframe = %d, animationFrameCount = %d \n", theName.c_str(), theObjectType, Animation[currentFrame], currentFrame, animationFrameCount);
		//DEBUGPRINTF("gameObj::draw - drawing animated sprite facing direction %d\n", theDirection);
		g_cSpriteManager.Draw(theObjectType, location, Animation[currentFrame], theDirection);
		if(g_cTimer.elapsed(lastFrameTime, t)) //if enough time passed
		{
			//DEBUGPRINTF("gameObj::draw - time elapsed\n");
			//increment and loop if necessary
			//currentFrame++;

			if( ++currentFrame >= animationFrameCount) // if next frame is higher than total frames
			{
				//DEBUGPRINTF("gameObj::draw - current frame %d is bigger than frame count %d\n", currentFrame, animationFrameCount);

				if (cycleSprite) // if it is a cycling sprite
				{
					//DEBUGPRINTF("gameObj::draw - cycling sprite\n");
					currentFrame = 0; // reset frame number to 0
				} // end if it is a cycling sprite
				else // else it doesn't cycle
				{
					//DEBUGPRINTF("gameObj::draw - animationEnded = TRUE, not a cycling sprite\n");
					animationEnded = true; // animation is ended
					currentFrame--; // go back a frame so we don't go over the buffer
				} // end else it doesn't cycle
			} // end if ++currentFrame
			else // else we're not at last frame
			{
				//DEBUGPRINTF("gameObj::draw - not at last frame yet\n");
				animationEnded = false;
			} // end else not at last frame
		} // end if g_cTimer...
	} // end else Animation == null
} // end method gameObj::draw


//==========================================
// gameObj::setXPos method
// sets whether or not item can move in
// positive-X direction
//==========================================
void gameObj::setXPos(bool canMove) ///< set X negative movement
{
	move_xPos = canMove;
} // end method gameObj::setXPos


//==========================================
// gameObj::setYPos method
// sets whether or not item can move in
// positive-Y direction
//==========================================
void gameObj::setYPos(bool canMove) ///< set Y negative movement
{
	move_yPos = canMove;
} // end method gameObj::setYPos


//==========================================
// gameObj::setZPos method
// sets whether or not item can move in
// positive-Z direction
//==========================================
void gameObj::setZPos(bool canMove) ///< set Z negative movement
{
	move_zPos = canMove;
} // end method gameObj::setZPos


//==========================================
// gameObj::setXNeg method
// sets whether or not item can move in
// negative-X direction
//==========================================
void gameObj::setXNeg(bool canMove) ///< set X negative movement
{
	move_xNeg = canMove;
} // end method gameObj::setXNeg


//==========================================
// gameObj::setYNeg method
// sets whether or not item can move in
// negative-Y direction
//==========================================
void gameObj::setYNeg(bool canMove) ///< set Y negative movement
{
	move_yNeg = canMove;
} // end method gameObj::setYNeg


//==========================================
// gameObj::setZNeg method
// sets whether or not item can move in
// negative-Z direction
//==========================================
void gameObj::setZNeg(bool canMove) ///< set Z negative movement
{
	move_zNeg = canMove;
} // end method gameObj::setZNeg


//==========================================
// gameObj::setXLoc method
// sets x location
//==========================================
void gameObj::setXLoc(float newX) ///< set Z negative movement
{
	location.x = newX;
} // end method gameObj::setXLoc


//==========================================
// gameObj::setYLoc method
// sets y location
//==========================================
void gameObj::setYLoc(float newY) ///< set Z negative movement
{
	location.y = newY;
} // end method gameObj::setYLoc


//==========================================
// gameObj::setZLoc method
// sets z location
//==========================================
void gameObj::setZLoc(float newZ) ///< set Z negative movement
{
	location.z = newZ;
} // end method gameObj::setZLoc


//==========================================
// gameObj::adjustLocation method
// adjuct object's location to avoid
// getting stuck inside objects
//==========================================
void gameObj::adjustLocation(int xAdj, int yAdj, int zAdj)
{
	location.x = location.x + xAdj;
	location.y = location.y + yAdj;
	location.z = location.z + zAdj;
	/*
	if (xAdj != 0)
        location.x = (float)xAdj;
	if (yAdj != 0)
		location.y = (float)yAdj;
	if (zAdj != 0)
		location.z = (float)zAdj;
	*/
}// end method gameObj::adjustLocation



// ===============================================================================================================
// **											Virtual Functions												**
// ===============================================================================================================


//==========================================
// gameObj::busy method
// returns true if object is busy
//==========================================
bool gameObj::busy()
{
	return false;
} // end method gameObj::busy


//==========================================
// gameObj::move method
// moves objects
//==========================================
void gameObj::move()
{

	//if moving in a positive x dir, check if possible
	if (xSpeed > 0.000001)
	{
		if (move_xPos)
		location.x = location.x + xSpeed;
	}

	// if moving in a negative x dir, check if possible
	else if (xSpeed < -0.000001)
	{
		if (move_xNeg)
		location.x = location.x + xSpeed;
	}

	//if moving in a positive z dir, check if possible
	if (zSpeed > 0.000001)
	{
		if (move_zPos)
		location.z = location.z + zSpeed;
	}

	//if moving in a negative z dir, check if possible
	else if (zSpeed < -0.000001)
	{
		if (move_zNeg)
		location.z = location.z + zSpeed;
	}


//	location.x = location.x + xSpeed;
//	location.z = location.z + zSpeed;

	if (location.y < yFloor || location.y == yFloor)
	{
		xSpeed = 0;
		zSpeed = 0;
		ySpeed = 0;
		location.y = (float)yFloor;
		fallTime = 0;
		
	}
	if (location.y > yFloor)
	{
			fallTime++;
			location.y = (float)(ySpeed + (location.y + -.481*(fallTime^2)));  //gravity REDUCE!
	}
	
	if (xSpeed > 0)
		theDirection = FACE_RIGHT;
	if (xSpeed < 0)
		theDirection = FACE_LEFT;
} // end gameObj::move


//==========================================
// gameObj::runAI method
// runs AI
//==========================================
void gameObj::runAI(gameObj**, int maxObjects)
{
	// stupid whiny Linker made me put this here...
} // end method gameObj::runAI


//==========================================
// gameObj::doAction method
// sets action based on input
//==========================================
void gameObj::doAction(actionType theAction, int newXSpeed, int newZSpeed, bool fromJoystick)
{
	//DEBUGPRINTF("gameObj::doAction - should never be here...\n");
} // end method gameObj::doAction


//==========================================
// gameObj::hurt method
// reduced object's HP by parameter
//==========================================
void gameObj::hurt(int damage)
{
	// nothing
} // end method gameObj::hurt


//==========================================
// gameObj::stopX method
// stops X motion
//==========================================
void gameObj::stopX()
{
	xSpeed = 0;
} // end method gameObj::stopX


//==========================================
// gameObj::stopY method
// stops YX motion
//==========================================
void gameObj::stopY()
{
	ySpeed = 0;
} // end method gameObj::stopY


//==========================================
// gameObj::stopZ method
// stops Z motion
//==========================================
void gameObj::stopZ()
{
	zSpeed = 0;
} // end method gameObj::stopZ


//==========================================
// gameObj::stopAll method
// stops all motion
//==========================================
void gameObj::stopAll()
{
	xSpeed = 0;
	ySpeed = 0;
	zSpeed = 0;
} // end method gameObj::stopAll



/*********************************************************************************/


// Sets the dimensions of the object
void gameObj::setLength(double L)
{
	length = L;
}
void gameObj::setHeight(double H)
{
	height = H;
}

void gameObj::setWidth(double W)
{
	width = W;
}



// Gets the dimensions of the object
double gameObj::getLength()
{
	return length;
}

double gameObj::getHeight()
{
	return height;
}

double gameObj::getWidth()
{
	return width;
}


BOOL gameObj::isMovable()
{
	return movable;
}

//< get X location
float gameObj::getXLoc() 
{
	return location.x;
}
//< get Y location
float gameObj::getYLoc() 
{
	return location.y;
}
//< get Z location
float gameObj::getZLoc() 
{
	return location.z;
}
/*********************************************************************************/



