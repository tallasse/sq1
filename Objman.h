/// \file objman.h
/// Interface for the object manager class CObjectManager.
/// Copyright Ian Parberry, 2004
/// Last updated August 24, 2004

#pragma once

#define MAX_OBJECTS 1024 //max number of objects in game

#include "object.h"
#include "inventory.h"

/// The object manager. The object manager is responsible for managing
/// the game objects. The plane object is a special object that gets
/// special treatment because it represents the player.

class ObjectManager
{

  protected:
    gameObj **m_pObjectList; ///< List of game objects.
    int m_nCount; ///< How many objects in list.
	int m_nMaxCount; ///< max number of objects   
    int m_nPlayerIndex; ///< Index of player
	int m_nInventoryIndex; ///< Index of inventory
	int m_nBossIndex; ///< index of boss
	int* drawOrder; ///< order to draw in;
	int m_numLights; ///< number of lights currently active
	int lastSoundTime; ///< last time an ambient sound played
	int nextSoundTime; ///< next time for an ambient sound to play
	int m_lightningTimer; ///< timer for lightning flash
	bool m_soundLoopStarted; ///< has background sound loop started?
	bool m_lightningFlash; ///< lightning flash activation
	inventoryObj* theInventory; ///< inventory pointer
    
	int lastMusicTime; ///< last time music played
	int nextMusicTime; ///< next time for music to play

	//distance functions
    float distance(float x1,float y1,float x2,float y2); ///< Euclidean distance.
    float distance(int first,int second); ///< Distance between objects.

           
	//managing dead objects
    
	void cull(); ///< Cull dead objects
    void kill(int index); ///< Remove dead object from list.
    //void replace(int index); ///< Replace object by next in series.

  public:
    ObjectManager(); ///< Constructor
    ~ObjectManager(); ///< Destructor
    int create(baseObjectType object, string name, D3DXVECTOR3 location); ///< Create new object
    void move(); ///< Move all objects
    void draw(); ///< Draw all objects
    void SetPlayerIndex(); ///< Set player index.
    void Action(actionType theAction, int newXSpeed, int newZSpeed, bool fromJoystick = false); ///< Player Action
    void Reset(); ///< Reset to original conditions plus player and inventory
	void ClearAll(); ///< Reset to original conditions.
	void Pause(); ///< Game paused
	//-------------------------------------------------------------------------
	int DeadEnemy();
	bool PlayerWon(int MaxEnemies); ///< TRUE if some amount of enemies are dead.
	bool PlayerLose(); ///<TRUE if player is dead.
	bool BossDead(); ///< TRUE if Boss is dead.
	void CriticalKeyHit();///<Change UseCriticalHit to TRUE

	void killBoss();
	//-------------------------------------------------------------------------
	//void collisionDetection(); ///< Process all collisions.
	
	BOOL CollisionDetection(gameObj *Object1, gameObj *Object2, 
							double &Obj1vsObj2XMin, double &Obj1vsObj2XMax,
                            double &Obj1vsObj2YMin, double &Obj1vsObj2YMax,
                            double &Obj1vsObj2ZMin, double &Obj1vsObj2ZMax);


/****************************************************************/
	void StopMovement(gameObj *Object1,  gameObj *Object2,
						double *Obj1vsObj2XMin, double *Obj1vsObj2XMax,
                        double *Obj1vsObj2YMin, double *Obj1vsObj2YMax,
                        double *Obj1vsObj2ZMin, double *Obj1vsObj2ZMax,	
						BOOL *canMoveXpos, BOOL *canMoveYpos,
						BOOL *canMoveZpos, BOOL *canMoveXneg,
						BOOL *canMoveYneg, BOOL *canMoveZneg,
						double *largerObjLength,
						double *largerObjHeight,
						double *largerObjWidth
						);

    void    StopObj1LeftObj2( gameObj *Object1, gameObj *Object2,
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
	                        double *largerObjWidth);


    void    StopObj1RightObj2( gameObj *Object1, gameObj *Object2,
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
	                        double *largerObjWidth);

    BOOL BypassCertainObjects(gameObj *Object1, gameObj *Object2);

    void BounceBack(gameObj *Object1, gameObj *Object2, bool left, bool front, bool bottom);

/****************************************************************/

	void CollisionManager(); ///< manages collsions

	D3DXVECTOR3 PlayerLocation(); ///< Get player location for camera.
	D3DXVECTOR3 itemLocation(int theIndex); ///< Get item location for following an object
	void MovePlayer(int xSpeed, int zSpeed); ///< Move player around
	void combatManager(gameObj* attacker, gameObj* attackee);
	void bloodSpatter(D3DXVECTOR3 p, int damage);
	void spawnFire(D3DXVECTOR3 p, int fireSize);
	void spawnFire(D3DXVECTOR3 p, int fireSize, int followObject);
	void fireArrow(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed);
	void fireShuriken(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed);
	void fireBall(D3DXVECTOR3 p, float xSpeed, float ySpeed, float zSpeed);
	void createRain(int howHard);
	void generateParticles(D3DXVECTOR3 p, int numParticles, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, int followObject, float red, float green, float blue, float alpha);
	void generateParticles(int numParticles, particleType theType, particleColor theColor, int theLifetime, int damagePerParticle, float red, float green, float blue, float alpha);
	int createLight(D3DXVECTOR3 p, float red, float blue, float green, float alpha, float range);
	void moveLight(int lightNumber, D3DXVECTOR3 p);
	void ambientSounds();
	void ambientMusic();
	void inventoryForward();
	void inventoryBackward();
	bool addToInventory(gameObj* newObject);
	void useInventoryObject();
	void drawInventory(); ///< draw the inventory
	void playerCombat();
	float playerXSpeed(); ///< get player's xspeed
	float playerYSpeed(); ///< get player's yspeed
	float playerZSpeed(); ///< get player's zspeed
	void stopPlayerX(); ///< stop player x motion
	void stopPlayerY(); ///< stop player y motion
	void stopPlayerZ(); ///< stop player z motion
	void stopPlayerAll(); ///< stop player motion all axes
	bool playerBusy(); ///< tell joystick if player is busy
	void playerRunning();
	void playerWalking();
	int playerHealth();
	int playerPower();
	int playerMaxPower();
	int playerMaxHealth();
	int playerAS();
	int playerDS();
	const char* itemDescription();
	void resetPlayerLocation();
	void playerDucking(bool ducking);
	void playerDefending(bool defending);
	bool inRange(gameObj* attacker, gameObj* attackee);
	
	inventoryObj* get_pInventory();

	int getCurrentInventoryIndex();

};