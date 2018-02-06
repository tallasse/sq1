/// \file inventory.cpp
/// Code for the inventory class:
/// Updated:
/// Jeremy Smith 11/25/04

#include <string>
#include "object.h"
#include "defines.h" //essential defines
#include "timer.h" //game timer
#include "spriteman.h" //sprite manager
#include "random.h" // random numbers
#include "debug.h"
#include "sound.h" //for sound manager
#include "objman.h"
#include "inventory.h"
#include "player.h"
#include "LayerManager2d.h"
#include "d3dutil.h"
#include "d3dfont.h"
#include "dxutil.h"
#include <d3dx9.h>
//using namespace std;

extern CTimer g_cTimer;
extern CRandom g_cRandom; //random number generator
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager; //sound manager
extern ObjectManager g_cObjectManager; //object manager
extern LayerManager2d layerManager;


extern CD3DFont** Font; // font array 


//==================================== Inventory Object Implementation ====================================

inventoryObj::inventoryObj()
	:gameObj("nothing", nothing_icon, D3DXVECTOR3(0.0f, (float)g_nScreenHeight - 250.0f, 200.0f))	
	//:gameObj("nothing", nothing_icon, D3DXVECTOR3(0.0f, 
	//( layerManager.getInvOffset() * 0.35f), 
	//200.0f))
{

    m_numObjects = 0;
	m_maxObjects = 30;
	m_currentIndex = 0;
	inventoryList = new gameObj*[m_maxObjects];
	for(int i = 0; i < m_maxObjects; i++)
	{
		inventoryList[i] = NULL;
	}

	theMaterial.Emissive.r = 1.0f;
	theMaterial.Emissive.g = 1.0f;
	theMaterial.Emissive.b = 1.0f;
	theMaterial.Emissive.a = 1.0f;

	firstItemSelected = false;

	//gameObj* baseObj = new nothingObj();
	//addObject(baseObj);

	//loadAnimation(nothing_icon);

	//for screenshot mode
	//loadAnimation(bigrice);
} // inventoryObj constructor




void inventoryObj::move()
{
	D3DXVECTOR3 playerLoc = g_cObjectManager.PlayerLocation();
	location.x = playerLoc.x - 300.0f;
//	location.x = playerLoc.x + ((float)g_nScreenWidth * 0.20f);

    // move only the 5 objects being displayed in the inventory
    for (int i = 0; ((i < m_numObjects) && (i < 5)); i++)
    {
		// start at the currently selected object in inventory
		int j = i + m_currentIndex;

		// handle the case where j becomes larger then number of objects in the
		// inventory
		if ( j >= m_numObjects )
		{
			j = j - (m_numObjects);
		}


        if (inventoryList[j] != NULL)
        {
            if (inventoryList[j]->name() == "armor")
            {
                inventoryList[j]->setXLoc( playerLoc.x + ( (float)(i) * 100.0f * 1.01f ) );
                inventoryList[j]->setYLoc( layerManager.getInvOffset() * 0.4f - 350.0f);                

			}
			else if (inventoryList[j]->name() == "katana")
            {
                inventoryList[j]->setXLoc( playerLoc.x + ( (float)(i) * 100.0f * 1.01f ) );
                inventoryList[j]->setYLoc( layerManager.getInvOffset() * 0.4f - 350.0f);                
		
			}
            else
                inventoryList[j]->setXLoc( playerLoc.x + ( (float)(i) * 50.0f ) ); 

        }
    }

} // end inventoryObj move method





bool inventoryObj::addObject(gameObj* newObject)
{
	//DEBUGPRINTF("inventoryObj::addObject - begin\n");
	if(m_numObjects < m_maxObjects) //if room, create object
	{
		for(int i = 0; inventoryList[i] != NULL; i++); //find first free slot

        // change newObject's location so that it will appear correctly when we draw inventory
        if (newObject->name() == "armor")
        {
//            newObject->setXLoc( 0.0f + ( (float)i * 100.0f ) );
            newObject->setYLoc( layerManager.getInvOffset() * 0.4f );
            newObject->setZLoc( 1000.0f );
            
            inventoryList[i] = newObject;
        }
		else if (newObject->name() == "katana")
		{
            newObject->setYLoc( layerManager.getInvOffset() * 0.4f );
            newObject->setZLoc( 1000.0f );
            
            inventoryList[i] = newObject;
		}
        else // it is food
        {
//            newObject->setXLoc( 0.0f + ( (float)i * 100.0f ) );
            newObject->setYLoc( layerManager.getInvOffset() * 0.4f );
            newObject->setZLoc( 200.0f );

		    inventoryList[i] = newObject;

        }

		//DEBUGPRINTF("inventoryObj::addObject - item added to list at index %d.\n", i);
		m_numObjects++;

		if (!firstItemSelected)
		{
			firstItemSelected = true;
			scrollForward();
		}

		return true;
	} // end if room
	else
	{
		return false;
		//DEBUGPRINTF("inventoryObj:: no room available to add inventory objects\n");
	} // end else

} // end addObject

void inventoryObj::removeObject(int objIndex)
{
	//DEBUGPRINTF("inventoryObj::removingObject - removing object at index %d.\n", m_currentIndex);

    if (objIndex == m_numObjects)
    {
        delete inventoryList[objIndex];
	    inventoryList[objIndex] = NULL;
	    m_numObjects--;
	    scrollForward();
    }
    else
    {
        delete inventoryList[objIndex];
	    inventoryList[objIndex] = NULL;

        // move all the objects above objIndex down a slot
        // this allow inventory sliding and allows looping using
        // m_numObjects instead of maximum number of inventory objects

        for (int i = objIndex; i < m_numObjects; i++)
        {
            inventoryList[i] = inventoryList[i+1];
        }

        m_numObjects--;
	    scrollForward();
    }

	if (m_numObjects == 0)
	{
		loadAnimation(nothing_icon);
		firstItemSelected = false;
	}

} // end removeObject

void inventoryObj::useObject(gameObj* thePlayer)
{ 
	if (m_numObjects != 0)
	{
		//DEBUGPRINTF("inventoryObj::useObject - player is using object at index %d.\n", m_currentIndex);
		playerObj* thePlayerObject = (playerObj*)thePlayer;
		//if (m_currentIndex == 0)
		//{
			if (inventoryList[m_currentIndex]->name() == "food")
			{
				g_pSoundManager->play(HEAL_SOUND); //play healing sound
				g_cObjectManager.generateParticles(30, GLOW_TYPE, COLOR_PARTICLE, 50, 0, 1.0f, 1.0f, 1.0f, 1.0f);
				int damage = inventoryList[m_currentIndex]->damageAmount();
				removeObject(m_currentIndex);
				thePlayerObject->hurt(damage);
			} // end if food
			else 
			if (inventoryList[m_currentIndex]->name() == "armor")
			{
				g_pSoundManager->play(DEFEND_SOUND); //play defending sound
				int defenseAmount = inventoryList[m_currentIndex]->damageAmount();
				thePlayerObject->addDefense(defenseAmount);
				g_cObjectManager.generateParticles(30, ROTATE_VERTICAL_TYPE, COLOR_PARTICLE, 100, 0, 0.2f, 0.2f, 0.6f, 1.0f);
				removeObject(m_currentIndex);
			} // end if armor
			else
			if (inventoryList[m_currentIndex]->name() == "katana")
			{
				g_pSoundManager->play(SWING1_SOUND); //play swing sound
				int attackAmount = inventoryList[m_currentIndex]->damageAmount();
				thePlayerObject->addAttack(attackAmount);
				g_cObjectManager.generateParticles(30, ROTATE_HORIZONTAL_TYPE, COLOR_PARTICLE, 100, 0, 0.8f, 0.1f, 0.2f, 1.0f);
				removeObject(m_currentIndex);
			}

	}

	if (m_numObjects == 0)
	{
		loadAnimation(nothing_icon);
	}
}

void inventoryObj::scrollForward()
{
	if (m_numObjects != 0)
	{
		int i = 0;
		m_currentIndex++;
		if (m_currentIndex >= m_maxObjects)
			m_currentIndex = 0;

		while( (inventoryList[m_currentIndex] == NULL) && (i < m_maxObjects) )
		{
			m_currentIndex++;	
			//DEBUGPRINTF("inventoryObj::scrollForward - scrolling forward, current index is %d.\n", m_currentIndex);
			if (m_currentIndex >= m_maxObjects)
			{
				m_currentIndex = 0;
				//DEBUGPRINTF("inventoryObj::scrollForward - wrapping around to 0, current index is %d.\n", m_currentIndex);
			}
		}
		if (inventoryList[m_currentIndex] == NULL)
		{
			m_currentIndex = 0;
			//DEBUGPRINTF("inventoryObj::scrollForward - list is empty, index is %d.\n", m_currentIndex);
		}
		//DEBUGPRINTF("inventoryObj::scrollForward - loading animation from object at index %d.\n", m_currentIndex);
		if (inventoryList[m_currentIndex] != NULL)
			loadAnimation(inventoryList[m_currentIndex]->currentType());
	}
	else
	{
		m_currentIndex = 0;
	}
} // end scrollForward

void inventoryObj::scrollBackward()
{
	if (m_numObjects != 0)
	{

		int i = 0;
		m_currentIndex--;	
		if (m_currentIndex < 0)
			m_currentIndex = m_maxObjects - 1;

		while( (inventoryList[m_currentIndex] == NULL) && (i < m_maxObjects) )
		{
			m_currentIndex--;	
			if (m_currentIndex < 0)
			{
				m_currentIndex = m_maxObjects - 1;
				//DEBUGPRINTF("inventoryObj::scrollBackward - wrapping around to maxObjects, current index is %d.\n", m_currentIndex);
			}
		}
		if (inventoryList[m_currentIndex] == NULL)
		{
			m_currentIndex = 0;
			//DEBUGPRINTF("inventoryObj::scrollBackward - list is empty, index is %d.\n", m_currentIndex);
		}
		//DEBUGPRINTF("inventoryObj::scrollBackward - loading animation from object at index %d.\n", m_currentIndex);
		if (inventoryList[m_currentIndex] != NULL)
			loadAnimation(inventoryList[m_currentIndex]->currentType());
	}
	else
	{
		m_currentIndex = 0;
	}
} // end scrollBackward

string inventoryObj::getCurrentItemDescription()
{
	deliciousObj* theItem = (deliciousObj*)inventoryList[m_currentIndex];
	string theString = theItem->getDescription();
	return theString;
} // end getCurrentItemDescription1

//==================================== Nothing Object Implementation ====================================

nothingObj::nothingObj()
	:gameObj("nothing", nothing_icon, D3DXVECTOR3(0.0f, 0.0f, 0.0f))
{

} // end nothing object


int inventoryObj::get_numObjects()
{
	return m_numObjects;

}


/****************************************************************/

gameObj** inventoryObj::getInventoryList()
{
    return inventoryList;
}



// return the current index of the item selected in 
// inventory
int	inventoryObj::getCurrentIndex()
{
	return m_currentIndex;
}




/****************************************************************/
