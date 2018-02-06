/// \file particles.h
/// interface for the game object classes:
/// obstacleObj, projectileObj, 
/// magicObj, bloodObj
/// Copyright Jeremy Smith 2004
/// 11/10/04

#pragma once

#include <string>
#include "object.h"

using namespace std;

class obstacleObj: public gameObj
{
	
	protected:

		int hitPoints;

	public:

        obstacleObj(string objName, ObjectType newType, D3DXVECTOR3 p, int HP); // for immobile obstacles such as boxes, rocks, etc.
		int getHP(); // returns remaining HP as an int
		void hurt(int damage); // subtracts int value from current HP
		virtual void move();

}; // end obstacleObj class

class projectileObj: public gameObj  // for projectiles
{

	protected:

		int damageAmt; // amount of damage done when projectile strikes

	public:

		projectileObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, float newX, float newY, float newZ);
		virtual int damageAmount(); // return damageAmount
		virtual void move();
}; // end projectileObj

class magicObj: public projectileObj
{
	public:

		magicObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, float newX, float newY, float newZ, int lightNum);
		void move();
		int damageAmount();
};


class bloodObj: public gameObj
{

	public:

		bloodObj(string objName, ObjectType newType, D3DXVECTOR3 p, int damage);
		void move();
}; // end bloodObj

class fireObj: public projectileObj
{
	protected:
		
		int origLife;
		D3DXVECTOR3 origLoc;
		ObjectType origObject;
		int followingObject;
		int xLocation, yLocation, zLocation;
		
	public:

		fireObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt, int followObject, int lightNum);
		void move();
};

class particleObj: public projectileObj
{
	protected:
		
		D3DXVECTOR3 origLoc;
		particleType theParticle;
		int followingObject;
		int tempLifetime;
		int xLocation, yLocation, zLocation;
		int xBoundary, yBoundary, zBoundary;
		bool setSpeeds;
		float t;

	public:

		particleObj(D3DXVECTOR3 p, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, int followObject, float red, float green, float blue, float alpha);
		int damageAmount();
		void move();
};

class deliciousObj: public projectileObj
{

	private:
		
		string theDescription;


	public:

		deliciousObj(string objName, ObjectType newType, D3DXVECTOR3 p, bool causesDamage, int theDamageAmt);
		string getDescription();
		const char* getCharDescription();
		int damageAmount();
		void move();
};