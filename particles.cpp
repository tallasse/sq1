/// \file particles.cpp
/// Code for the game object classes:
/// obstacleObj, projectileObj, 
/// magicObj, bloodObj
/// Copyright Jeremy Smith 2004
/// 11/10/04

#include <string>
#include "object.h"
#include "defines.h" //essential defines
#include "timer.h" //game timer
#include "spriteman.h" //sprite manager
#include "random.h" // random numbers
#include "debug.h"
#include "sound.h" //for sound manager
#include "objman.h"
#include "particles.h"

using namespace std;

extern CTimer g_cTimer;
extern CRandom g_cRandom; //random number generator
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager

//==================================== Obstacle Object Implementation ====================================

obstacleObj::obstacleObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP)
	:gameObj(objName, newType, p)
{
	hitPoints = HP;
	if(theName != "lantern")
	{
		location.y = (float) yFloor;	
	}
	
	// //DEBUGPRINTF("obstacleObj constructor\n");
}

int obstacleObj::getHP()
{
	return hitPoints;
} // end getHP method

void obstacleObj::hurt(int damage)
{
	//g_pSoundManager->play(THUMP_SOUND); 
	if (hitPoints != -1)
	{
		hitPoints = hitPoints - damage;
		if (theObjectType == smallbush)
		{
			g_cObjectManager.generateParticles(location, 5, SPARK_TYPE, LEAF_PARTICLE, 200, 0, -1, 0.0f, 0.0f, 0.0f, 0.0f);
			g_pSoundManager->play(RUSTLE_SOUND);
		}
		if (theObjectType == rock)
		{
			g_cObjectManager.generateParticles(location, 5, SPARK_TYPE, SHARD_PARTICLE, 200, 0, -1, 0.0f, 0.0f, 0.0f, 0.0f);
			g_pSoundManager->play(CLANG_SOUND);
		}


		int foodChance;

		if (hitPoints < 0)
		{
			lifetime = 0;
			foodChance = g_cRandom.number(0, 2);
			if (theObjectType == smallbush)
			{
				if (foodChance == 0)
				{
					g_cObjectManager.create(OBSTACLE_TYPE, "smallrice", location);
				}
				g_cObjectManager.generateParticles(location, 15, SPARK_TYPE, LEAF_PARTICLE, 200, 0, -1, 0.0f, 0.0f, 0.0f, 0.0f);

			}
			if (theObjectType == rock)
			{
				if (foodChance == 0)
				{
					g_cObjectManager.create(OBSTACLE_TYPE, "bigrice", location);
				}
				g_cObjectManager.generateParticles(location, 15, SPARK_TYPE, SHARD_PARTICLE, 200, 0, -1, 0.0f, 0.0f, 0.0f, 0.0f);
			}
		} // end if hitPoints
	} // if not invincible
} // end hurt method

void obstacleObj::move()
{
	// don't move
}


//==================================== Projectile Object Implementation ====================================

projectileObj::projectileObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, float newX, float newY, float newZ)
	:gameObj(objName, newType, p)
{
	xSpeed = newX;
	zSpeed = newZ;
	ySpeed = newY;
	isDangerous = causesDamage;
	damageAmt = theDamageAmt;
	if(theObjectType == shuriken)
	{
		theMaterial.Specular.r = 1.0f;
		theMaterial.Specular.g = 1.0f;
		theMaterial.Specular.b = 1.0f;
		theMaterial.Specular.a = 1.0f;
	}
	//DEBUGPRINTF("projectileObj constructor\n");
}

int projectileObj::damageAmount()
{
	int theDamage = damageAmt;
	isCollidable = false;
	damageAmt = 0;
	isDangerous = false;
	return theDamage;
}

void projectileObj::move()
{
	lifetime--;
	location.x = location.x + xSpeed;
	location.z = location.z + zSpeed;

	if (location.y < yFloor || location.y == yFloor)
	{
		xSpeed = 0;
		zSpeed = 0;
		ySpeed = 0;
		location.y = (float) yFloor;
		fallTime = 0;
		isDangerous = false;
		cycleSprite = false;

/***************************************************/

		// don't projectiles on the ground to be colliable
		isCollidable = false;


/***************************************************/

	}

	if (theName == "shuriken") // if shuriken
	{
		int test;
		if (cycleSprite == true)
			test = 1;
		else test = 0;
		//DEBUGPRINTF("projectileObj::move - shuriken, cycleSprite: %d\n", test);
		if (location.y > yFloor) // if above floor
		{
			fallTime++;
			location.y = ySpeed + (location.y + -0.001f *(fallTime^2));  //fall slowly

		} // end if above floor
	} // end if is shuriken
	else // else not a shuriken
	{
		if (location.y > yFloor) //if above floor
		{
			fallTime++;
			location.y = ySpeed + (location.y + -0.481f *(fallTime^2));  //fall
		} // end if above floor
	} // end else not shuriken
	
	if (xSpeed > 0)
		theDirection = FACE_RIGHT;
	if (xSpeed < 0)
		theDirection = FACE_LEFT;

}

//==================================== Magical Projectile Object Implementation ====================================

magicObj::magicObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, float newX, float newY, float newZ, int lightNum)
	:projectileObj(objName, newType, p, causesDamage, theDamageAmt, newX, newY, newZ)
{
	isParticle = true;
	theMaterial.Emissive.r = 1.0f; // original color
	theMaterial.Emissive.g = 1.0f; // original color
	theMaterial.Emissive.b = 1.0f; // original color
	theMaterial.Emissive.a = 1.0f; // original color
	lightIndex = lightNum;
}

void magicObj::move()
{
	g_cObjectManager.moveLight(lightIndex, location);
	lifetime--;
	location.x = location.x + xSpeed;
	location.z = location.z + zSpeed;

	if (location.y < yFloor || location.y == yFloor)
	{
		ySpeed = 0;
		//xSpeed = 0;
		zSpeed = 0;
		location.y = (float) yFloor;
		fallTime = 0;
		if (lifetime > 10)
			lifetime = 10;
	}
	if (location.y > yFloor)
	{
			fallTime++;
			location.y = ySpeed + (location.y + -0.011f * (fallTime^2));  //gravity REDUCE!
	}

	int smokeChance = g_cRandom.number(0, 75);
	if (smokeChance == 0)
        g_cObjectManager.generateParticles(location, 1, SMOKE_TYPE, SMOKE_PARTICLE, 50, 0, -1, 0.0f, 0.0f, 0.0f, 0.0f);
	
	int facing = g_cRandom.number(0,1);
	switch(facing)
	{
		case 0 :		
            theDirection = FACE_RIGHT;
			break;
		case 1:
            theDirection = FACE_LEFT;
			break;
	} // end switch

} // end magicObj move method


int magicObj::damageAmount()
{
	//DEBUGPRINTF("magicObj::damageAmount - hit something\n");
	int theDamage = damageAmt;
	damageAmt = 0;
	isDangerous = false;
	kill();
	return theDamage;
	
} // end magic object damageAmount

//==================================== Blood Particle Object Implementation ====================================

bloodObj::bloodObj(string objName, ObjectType newType, D3DXVECTOR3 p, int damage)
	:gameObj(objName, newType, p)
{
	theMaterial.Specular.r = 1.0f; // original color
	theMaterial.Specular.g = 1.0f; // original color
	theMaterial.Specular.b = 1.0f; // original color
	theMaterial.Specular.a = 1.0f; // original color
	theMaterial.Power = 10.0f; // original color
	dead = true;
	float chance = (float)10 / (float)g_cRandom.number(11, 100);	
	xSpeed = g_cRandom.number(-4, 4) + chance;
	chance = (float)10 / (float)g_cRandom.number(11, 100);	
	ySpeed = g_cRandom.number(2, 5) + chance;
	if (ySpeed > 20)
		ySpeed = 20;
	chance = (float)10 / (float)g_cRandom.number(11, 100);	
	zSpeed = g_cRandom.number(-4, 4) + chance;
	lifetime = 200 + g_cRandom.number(0, 200);
	int facing = g_cRandom.number(0,1);
	switch(facing)
	{
		case 0 :		
            theDirection = FACE_RIGHT;
			break;
		case 1:
            theDirection = FACE_LEFT;
			break;
	} // end switch
	
} // end bloodObj constructor

void bloodObj::move()
{
	lifetime--;
	location.x = location.x + xSpeed;
	location.z = location.z + zSpeed;

	if (location.y < yFloor)
	{
		ySpeed = 0;
		xSpeed = 0;
		zSpeed = 0;
		location.y = (float) yFloor;
		fallTime = 0;
	}
	if (location.y > yFloor)
	{
			fallTime++;
			location.y = ySpeed + (location.y - 0.281f * (fallTime^2));
	}
} // end bloodObj move

//==================================== Fire Object Implementation ====================================

fireObj::fireObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, int followObject, int lightNum)
	:projectileObj(objName, newType, p, causesDamage, theDamageAmt, 0, 0, 0)
{
	//DEBUGPRINTF("fireObj constructor - spawned a fire particle\n");
	followingObject = followObject;
	origLife = lifetime;
	lifetime = origLife + g_cRandom.number(0, 25);
	origObject = newType;
	origLoc = p;
	xLocation = -15 + g_cRandom.number(0, 30);
	yLocation = 0;
	zLocation = 0;
	lifetime = lifetime - g_cRandom.number(0, 195);
	theMaterial.Emissive.r = 1.0f; // original color
	theMaterial.Emissive.g = 1.0f; // original color
	theMaterial.Emissive.b = 1.0f; // original color
	theMaterial.Emissive.a = 1.0f; // original color
	lightIndex = lightNum;
}

void fireObj::move()
{
	//DEBUGPRINTF("fireObj::move - moving a fire particle\n");
	//lifetime--;
	g_cObjectManager.moveLight(lightIndex, location);
	if (theObjectType == origObject)
	{
		lifetime = lifetime - 1 - (abs(xLocation) / 5);
	}
	else
	{
		lifetime--;
	}

	if (followingObject != -1) // if we're following something
	{
		origLoc = g_cObjectManager.itemLocation(followingObject); // get its location
		origLoc.y = origLoc.y + 100;
	} // end if following

	if (theObjectType == smoke)
	{
		xSpeed = 0;
		zSpeed = 0;
	}
	else
	{
		xSpeed = (float) g_cRandom.number(-2, 2);
		zSpeed = (float) g_cRandom.number(-2, 2);	
	}

	xLocation = xLocation + (int) xSpeed;
	yLocation = yLocation + 1;
	zLocation = zLocation + (int) zSpeed;
	location.x = origLoc.x + xLocation;
	location.y = origLoc.y + yLocation;
	location.z = origLoc.z + zLocation;

	if (lifetime < 150)
	{
		loadAnimation(smoke);
		//lifetime = lifetime + g_cRandom.number(0, 50);
	}

	if (lifetime < 5)
	{
		loadAnimation(origObject);
		lifetime = origLife + g_cRandom.number(0, 25);
		location = origLoc;
		xLocation = -15 + g_cRandom.number(0, 30);
		yLocation = 0;
		zLocation = 0;
	} // end if lifetime = 1

	int facing = g_cRandom.number(0,1);
	switch(facing)
	{
		case 0 :		
            theDirection = FACE_RIGHT;
			break;
		case 1:
            theDirection = FACE_LEFT;
			break;
	} // end switch

} // end fireObj move

//==================================== Particle Object Implementation ====================================

particleObj::particleObj(D3DXVECTOR3 p, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, int followObject, float red, float green, float blue, float alpha)
	:projectileObj("particle", particle_p, p, true, damagePerParticle, 0, 0, 0)
{
	isParticle = true;
	theParticle = theType;

	
	if (damagePerParticle != 0) // does it cause damage or heal you?
	{
		isDangerous = true; // causes damage
		isCollidable = true; // collidable
	} // end if causes/heals damage

	tempLifetime = 100 + g_cRandom.number(0, 25); // give some randomness
	if (theParticle == SMOKE_TYPE)
	{
		lifetime = lifetime + g_cRandom.number(0, 50); // give some randomness
	}
	
	switch(theColor)
	{
		case COLOR_PARTICLE:
			loadAnimation(particle_p);
			theMaterial.Emissive.r = red; // original color
			theMaterial.Emissive.g = green; // original color
			theMaterial.Emissive.b = blue; // original color
			theMaterial.Emissive.a = alpha; // original color

			theMaterial.Ambient.r = red; // original color
			theMaterial.Ambient.g = green; // original color
			theMaterial.Ambient.b = blue; // original color
			theMaterial.Ambient.a = alpha; // original color
		
			theMaterial.Diffuse.r = red; // original color
			theMaterial.Diffuse.g = green; // original color
			theMaterial.Diffuse.b = blue; // original color
			theMaterial.Diffuse.a = alpha; // original color
			break;
		
		case SMOKE_PARTICLE:
			loadAnimation(smoke);
			theMaterial.Emissive.r = 0.0f; // original color
			theMaterial.Emissive.g = 0.0f; // original color
			theMaterial.Emissive.b = 0.0f; // original color
			theMaterial.Emissive.a = 0.0f; // original color
			break;
		case SHARD_PARTICLE:
			theMaterial.Emissive.r = 0.0f; // original color
			theMaterial.Emissive.g = 0.0f; // original color
			theMaterial.Emissive.b = 0.0f; // original color
			theMaterial.Emissive.a = 0.0f; // original color
			loadAnimation(shard_p);
			break;
		case LEAF_PARTICLE:
			theMaterial.Emissive.r = 0.0f; // original color
			theMaterial.Emissive.g = 0.0f; // original color
			theMaterial.Emissive.b = 0.0f; // original color
			theMaterial.Emissive.a = 0.0f; // original color
			loadAnimation(leaf_p);
			break;
		case RAIN_PARTICLE:
			isCollidable = false;
			theMaterial.Emissive.r = 0.0f; // original color
			theMaterial.Emissive.g = 0.0f; // original color
			theMaterial.Emissive.b = 0.0f; // original color
			theMaterial.Emissive.a = 0.0f; // original color
			theMaterial.Specular.r = 1.0f; // original color
			theMaterial.Specular.g = 1.0f; // original color
			theMaterial.Specular.b = 1.0f; // original color
			theMaterial.Specular.a = 1.0f; // original color
			loadAnimation(rain_p);
			break;
	} // end switch color


	int facing = g_cRandom.number(0,1); // generate random facing
	switch(facing) // pick a random facing
	{
		case 0 :		
            theDirection = FACE_RIGHT;
			break;
		case 1:
            theDirection = FACE_LEFT;
			break;
	} // end switch

	origLoc = p;
	location = p;
	followingObject = followObject;
	setSpeeds = false;

	xBoundary = 100;
	yBoundary = 100;
	zBoundary = 100;
	
	if (theLifetime != -1)
	{
		lifetime = theLifetime + g_cRandom.number(0, 20);
	}


	t = g_cRandom.number(0, 100) + ((float)g_cRandom.number(0, 10) / 10);

}

void particleObj::move()
{
	t = t + 0.02f;
	
	if (followingObject != -1) // if we're following something
	{
		origLoc = g_cObjectManager.itemLocation(followingObject); // get its location
	} // end if following

	if (lifetime != -1) // if not immortal
	{
		lifetime--; // subtract life
	} // end if not immortal


	// possible particle types: 
	// SPARK, FIRE, ROTATE_HORIZONTAL, ROTATE_VERTICAL, GLOW

	switch(theParticle) // move based on object type
	{
		case FIRE_TYPE:
			tempLifetime--;
			xSpeed = (float)g_cRandom.number(-2, 2);
			zSpeed = (float)g_cRandom.number(-2, 2);	
			location.x = location.x + xSpeed;
			location.z = location.z + zSpeed;
			location.y = location.y + 1;
			
			if (tempLifetime < 5)
			{
				tempLifetime = 100 + g_cRandom.number(0, 25);
				location = origLoc;
			} // end if tempLifetime < 5
			break;

		case SMOKE_TYPE:
			
			location.y = location.y + 1;
			break;


		case SPARK_TYPE:
			if (!setSpeeds) //if we haven't set speed yet
			{
				float chance = (float)10 / (float)g_cRandom.number(11, 100);	
				xSpeed = g_cRandom.number(-4, 4) + chance;
				chance = (float)10 / (float)g_cRandom.number(11, 100);	
				ySpeed = g_cRandom.number(2, 5) + chance;
				if (ySpeed > 20)
				ySpeed = 20;
				chance = (float)10 / (float)g_cRandom.number(11, 100);	
				zSpeed = g_cRandom.number(-4, 4) + chance;
				setSpeeds = true; // we've set speeds
			} // end if not setspeeds
			
			location.x = location.x + xSpeed;
			location.z = location.z + zSpeed;

			if (location.y < yFloor)
			{
				ySpeed = 0;
				xSpeed = 0;
				zSpeed = 0;
				location.y = (float) yFloor;
				fallTime = 0;
			}
			if (location.y > yFloor)
			{
					fallTime++;
					location.y = ySpeed + (location.y - 0.281f *(fallTime^2));
			}
			break;

		case GLOW_TYPE:
			tempLifetime--;
			if (!setSpeeds) //if we haven't set speed yet
			{
				xSpeed = (float)g_cRandom.number(-2, 2); // set x speed
				ySpeed = (float)g_cRandom.number(-2, 2); // set y speed
				zSpeed = (float)g_cRandom.number(-2, 2); // set z speed
				setSpeeds = true; // we've set speeds
			} // end if not setspeeds

			location.x = location.x + xSpeed;
			location.z = location.z + zSpeed;
			location.y = location.y + ySpeed;
	
			if (tempLifetime < 5)
			{
				tempLifetime = 100 + g_cRandom.number(0, 25);
				location = origLoc;
			} // end if tempLifetime < 5
			break;

		case ROTATE_HORIZONTAL_TYPE:
		
			location.x = origLoc.x + 100.0f * cos( 1.88495559215 * t);
			location.y = origLoc.y;
			location.z = origLoc.z + 100.0f * sin( 1.88495559215 * t);
		
			break; // end if horizontal rotation

		case ROTATE_VERTICAL_TYPE:
				
			location.x = origLoc.x + 100.0f * cos( 1.88495559215 * t);
			location.y = origLoc.y + 100.0f * sin( 1.88495559215 * t);
			location.z = origLoc.z;
		
			break; // end if horizontal rotation

		case RAIN_TYPE:

			D3DXVECTOR3 playerLoc = g_cObjectManager.PlayerLocation();
			if (location.y <= 0)
			{
				location.y = (float)g_nScreenHeight + g_cRandom.number(0, 200);
				location.z = (float)g_cRandom.number(600, 1500);
				location.x = playerLoc.x + (float)g_cRandom.number(-g_nScreenWidth, g_nScreenWidth);
			}
			if (location.x > playerLoc.x + (g_nScreenWidth))
			{
				location.x = playerLoc.x - (g_nScreenWidth);
			}
			if (location.x < playerLoc.x - (g_nScreenWidth))
			{
				location.x = playerLoc.x + (g_nScreenWidth);
			}

			location.y = location.y - 15;
		
			break; // end if horizontal rotation


	} // end switch theParticle
	
} // end particleObj move

int particleObj::damageAmount()
{
	int theDamage = damageAmt;
	damageAmt = 0;
	isDangerous = false;
	kill();
	return theDamage;
	
}

//==================================== Delicious Object Implementation ====================================

deliciousObj::deliciousObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt)
	:projectileObj(objName, newType, p, causesDamage, theDamageAmt, 0, 0, 0)
{
	if ( (objName == "armor") || (objName == "katana") )
	{
		theMaterial.Specular.r = 1.0f; // original color
		theMaterial.Specular.g = 1.0f; // original color
		theMaterial.Specular.b = 1.0f; // original color
		theMaterial.Emissive.a = 1.0f; // original color
	}
	isDangerous = causesDamage;
	damageAmt = theDamageAmt;
	isDelicious = true;
	switch(theObjectType)
	{
		case nothing_icon:
			theDescription = "no item";
			break;
		case smallrice:
			theDescription = "+200 HP";
			break;
		case bigrice:
			theDescription = "+400 HP";
			break;
		case armor:
			theDescription = "+10% defense";
			break;
		case katana:
			theDescription = "+10% attack";
			break;
		default:
			theDescription = "i am error";
	} // end switch
	//DEBUGPRINTF("projectileObj constructor\n");
}

string deliciousObj::getDescription()
{
	return theDescription;
}

const char* deliciousObj::getCharDescription()
{
    //char itemDescript[60];
    //sprintf( itemDescript,"%s", theDescription.c_str() );
	const char *itemDescript = new char;
	itemDescript = theDescription.c_str();
    return itemDescript;

}

int deliciousObj::damageAmount()
{
	int theDamage = damageAmt;
	damageAmt = 0;
	isDangerous = false;
	kill();
	return theDamage;
}

void deliciousObj::move()
{
	// doesn't move, fools!
}
