/// \file inventory.h
/// interface for the inventory objects:
/// Updated:
/// Jeremy Smith 11/25/04

#pragma once

#include <string>
#include "object.h"
#include "inanimates.h"
#include "particles.h"

using namespace std;

class inventoryObj: public gameObj
{
	
	protected:

		gameObj** inventoryList;
		int m_numObjects;
		int m_maxObjects;
		int m_currentIndex;
		bool firstItemSelected;

	public:

		inventoryObj();
		void move();
		bool addObject(gameObj* newObject);
		void removeObject(int objIndex);
		void useObject(gameObj* thePlayer);
		void scrollForward();
		void scrollBackward();
//		void drawAll();
		string getCurrentItemDescription();
        gameObj** getInventoryList();
		int	getCurrentIndex();


/***************************************************************/
		int get_numObjects();
/***************************************************************/


}; // end inventoryObj class

class nothingObj: public gameObj
{
	
	public:

		nothingObj();
};