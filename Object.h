/// \file object.h
/// Interface for the game object class definitions.
/// Copyright Ian Parberry, 2004
/// Last updated August 24, 2004

#pragma once

#include <string>
#include "sprite.h"
#include "defines.h"
#include "tinyxml.h"
#include "helpers.h"

using namespace std;

class gameObj
{

	friend class ObjectManager;

	protected:
		
		D3DXVECTOR3 location; ///< object's current location
		D3DMATERIAL9 theMaterial; ///< object material
		sizeMatrix structSize; ///< size of object's bounding box
		ObjectType theObjectType; ///< ObjectType, for sprite manager
		faceDirection theDirection; ///< Direction currently facing.
		actionType currAction; ///< object's action
		factionType currFaction; ///< object's faction (it rhymes!)
		string theName; ///< object's name
		
		float xSpeed, ySpeed, zSpeed; ///< Speed

		int yFloor; /// < minimum y value, half of sprite size
		int xSpeedLimit, zSpeedLimit; ///< Speed Limits
		int origYsize; ///< original y-size
		int fallTime; ///< Time falling
		int currentFrame; ///< Frame to be displayed.
		int frameCount; ///< Number of frames in animation.
		int lastFrameTime; ///< Last time the frame was changed.
		int frameInterval; ///< Interval between frames.
		int* Animation; ///< Sequence of frame numbers to be repeated
		int animationFrameCount; ///< Number of entries in Animation
		int lifetime; ///< lifetime remaining, -1 for immortal objects
		int deathTime; ///< records time of death
		int lightIndex; ///< index of light object maintains, if exists
		int theReach; ///< How far can it reach?

		bool cycleSprite; ///< TRUE to cycle sprite frames, otherwise play once
		bool isCollidable; ///< true if object is solid
		bool isIntelligent; ///< intelligence flag
		bool isNPC; ///< NPC flag
		bool animationEnded; ///< currently in an animation?
		bool dead; ///< is it dead yet?
		bool running; ///< is it running?
		bool ducking; ///< is it ducking?
		bool defending; ///< is it defending?
		bool move_xPos, move_yPos, move_zPos, move_xNeg, move_yNeg, move_zNeg; ///< limit movement
		bool isDangerous; ///< does the projectile cause damage?
        bool isParticle; ///< is it a particle?
		bool isDelicious; ///< is it tasty?  is it sweet?

		
        void loadAnimation(ObjectType newObject); ///< loads animation for new object state
		void LoadSettings(string name); ///< Load object-dependent settings from XML element.
	

/***************************************************************************************/
		// dimensions of the object
		double length;
		double height;
		double width;

		BOOL movable; // True if the object is capable of movement

/***************************************************************************************/




	public:

		gameObj(); ///< generic game object, default constructor
		gameObj(string objName, ObjectType newType, D3DXVECTOR3 p); ///< generic game object, explicit constructor
		~gameObj(); ///< destructor

		D3DXVECTOR3 loc(); ///< returns a D3DXVECTOR3 with the current location
		sizeMatrix size(); ///< returns a sizeMatrix object of this object’s size
		faceDirection direction(); ///< returns direction currently facing
		factionType faction(); ///< returns faction of object
		ObjectType currentType(); ///< returns current object type
		string name(); ///< returns name of object

		float getXSpeed(); ///< returns xSpeed
		float getYSpeed(); ///< returns ySpeed
		float getZSpeed(); ///< returns zSpeed

		bool collidable(); ///< returns true if object is solid
		bool isDead(); ///< returns true if dead
		bool dangerous(); ///< returns true if this can cause damage
		bool particle(); ///< is it a particle?
		bool intelligent(); ///< returns true if object is intelligent
		bool NPC(); ///< returns true if it is a character
		bool delicious(); ///< is it crunchable, precious?
		
//===============================================================	
		bool AlreadyCounted(); ///< check whether already counted or not
		void Critical(int DeadEnemy); ///< add power for critical hit
		void ResetPower();///< reset critical power
		void CriticalKey();
		void ResetCriticalKey();
//===============================================================		
		int reach(); ///< how far can this thing reach?
		int lifeRemaining(); ///< returns lifetime as an int
		virtual int damageAmount(); ///< returns damage that this object does as an int
		
		void kill(); ///< sets lifetime to 0, killing object
		void draw(); ///< Draw at current location.
		void setXPos(bool canMove); ///< set X negative movement
		void setYPos(bool canMove); ///< set Y negative movement
		void setZPos(bool canMove); ///< set Z negative movement
		void setXNeg(bool canMove); ///< set X negative movement
		void setYNeg(bool canMove); ///< set Y negative movement
		void setZNeg(bool canMove); ///< set Z negative movement
		void setXLoc(float newX); ///< set new X location
		void setYLoc(float newY); ///< set new Y location
		void setZLoc(float newZ); ///< set new Z location
		void adjustLocation(int xAdj, int yAdj, int zAdj); ///< adjust location to compensate for overlapped collisions
//		void setMovement(bool xPos, bool yPos, bool zPos, bool xNeg, bool yNeg, bool zNeg); ///< set movement directions allowed

		
		virtual bool busy(); ///< is object too busy to listen to joystick?
		virtual void move(); ///< virtual function for successors to move
		virtual void runAI(gameObj** objectList, int maxObjects); ///< tells AI where objects are to let it react	
		virtual void doAction(actionType theAction, int newXSpeed, int newZSpeed, bool fromJoystick = false); ///< virtual function for successor to act on input
		virtual void hurt(int damage);  ///< virtual function for successor to take damage
		virtual void stopX(); ///< stop x motion
		virtual void stopY(); ///< stop y motion
		virtual void stopZ(); ///< stop z motion
		virtual void stopAll(); ///< stop all motion
			



/***************************************************************************************/
		
		// Sets the dimensions of the object
		void setLength(double L);
		void setHeight(double H);
		void setWidth(double W);

		
		// Get the dimensions of the object
		double getLength();
		double getHeight();
		double getWidth();

		BOOL isMovable();

		float getXLoc(); ///< get X location
		float getYLoc(); ///< get Y location
		float getZLoc(); ///< get Z location


/***************************************************************************************/

		int GetCriticalPower; ///<Power Gage
		bool CountedAlready; ///<TRUE if a enemy is counted already
		bool UseCriticalHit; ///<TRUE if Player can use Critical Hit

}; // end gameObj class