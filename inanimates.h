/// \file inanimates.h
/// interface for the game object classes:
/// environmentObj, backgroundObj, 
/// nearBackgroundObj, foregroundObj
/// Copyright Jeremy Smith 2004
/// 11/10/04

#pragma once

#include <string>
#include "object.h"

using namespace std;

class environmentObj: public gameObj // for game environment, background, foreground, etc.
{
    
 	public:

		environmentObj(string objName, ObjectType newType, D3DXVECTOR3 p);
		void move(); // sets z-location to int value

}; // end environmentObj class

class backgroundObj: public gameObj // for game environment, background, foreground, etc.
{
    
	protected:
		int origX;

 	public:

		backgroundObj(string objName, ObjectType newType, D3DXVECTOR3 p);
		virtual void move(); // sets z-location to int value

}; // end backgroundObj class

class nearBackgroundObj: public backgroundObj // for game environment, background, foreground, etc.
{

	protected:
		float distance; ///< simulate distance;

 	public:

		nearBackgroundObj(string objName, ObjectType newType, D3DXVECTOR3 p);
		void move(); // sets z-location to int value

}; // end nearBackgroundObj class

class foregroundObj: public backgroundObj // for game environment, background, foreground, etc.
{

 	public:

		foregroundObj(string objName, ObjectType newType, D3DXVECTOR3 p);
		void move(); // wraps it to keep up with player

}; // end nearBackgroundObj class