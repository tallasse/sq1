// Updated by Russell Whitmire 12/14/2004

#include "LayerManager2d.h"
#include "Objman.h"
#include "d3dutil.h"
#include "d3dfont.h"
#include "dxutil.h"
#include <d3dx9.h>

extern IDirect3DDevice9* g_d3ddevice; ///< graphics device 
extern ObjectManager g_cObjectManager; ///< The object manager.
extern CD3DFont** Font; // font array 
extern int g_nScreenWidth; ///< Screen width.
extern int g_nScreenHeight; ///< Screen height.



LayerManager2d::LayerManager2d()
: invOffset ((float)g_nScreenHeight / 9.0f)
{
	
}



void LayerManager2d::draw2dLayer()
{
	float health = (float) g_cObjectManager.playerHealth();
	float maxhealth = (float) g_cObjectManager.playerMaxHealth();
	
	// setting for health bar and health number
	// determine the color based on how much avatar's health
	float healthBarRatio = health / maxhealth;	
		
	DWORD healthBarColor;
	if (healthBarRatio < 0.33f)
		healthBarColor = 0xffff0000;
	else if (healthBarRatio < 0.66)
		healthBarColor = 0xffffff00;
	else
		healthBarColor = 0xff00ff30;

	// settings for the power bar
	// determine the color based on power for avatar
	float powerBarRatio = (float) (g_cObjectManager.playerPower()) / (float)(g_cObjectManager.playerMaxPower());
		

	DWORD powerBarColor;
	if (powerBarRatio >= 1.0f)
		powerBarColor = 0xffff0000;
	else if (powerBarRatio > 0.66)
		powerBarColor = 0xffffff00;
	else
		powerBarColor = 0xff00ff30;



	// Draw inventory bar
	drawInventory();	

	//Draw "Health" text
	if( Font )
		Font[8]->DrawText( ((float)g_nScreenWidth * 0.015f), ((float)g_nScreenHeight - invOffset * 0.9f), healthBarColor, "Health" );	

	// Draw "Power" text
	if( Font )
		Font[8]->DrawText( ((float)g_nScreenWidth * 0.015f), ((float)g_nScreenHeight - invOffset * 0.45f), healthBarColor, "Power" );	


	// Draw health number
	drawHealthNumber(healthBarColor);

	// Draw power number
	drawPowerNumber(powerBarColor);
	
	// Draw health bar
	drawHealthBar(healthBarColor, healthBarRatio);

	// Draw power bar
	drawPowerBar(powerBarColor, powerBarRatio);

	//Draw player attack and defense stats
	drawStats();

	drawItemInventory();
	
	// Draw "Inventory:" text
	if( Font )
		Font[6]->DrawText( ((float)g_nScreenWidth * 0.5f), ((float)g_nScreenHeight - invOffset * 0.9f), 0xffffffff, "Inventory: #2 to cycle, space bar to use" );	

}


void LayerManager2d::drawHealthNumber(const DWORD &healthBarColor)
{
	int nhealth = g_cObjectManager.playerHealth();

	//don't want to display a negative health
	if (nhealth < 0)
		nhealth = 0;

	char tempbuff[10];
	sprintf(tempbuff,"%d", nhealth);

	if( Font )
		Font[8]->DrawText( ((float)g_nScreenWidth * 0.40f), ((float)g_nScreenHeight - invOffset * 0.9f), healthBarColor, tempbuff );	
			
}




void LayerManager2d::drawPowerNumber(const DWORD &powerBarColor)
{
	int nPower = g_cObjectManager.playerPower();

	//don't want to display a negative health
	if (nPower < 0)
		nPower = 0;

	char tempbuff[10];
	sprintf(tempbuff,"%d", nPower);

	if( Font )
		Font[8]->DrawText( ((float)g_nScreenWidth * 0.40f), ((float)g_nScreenHeight - invOffset * 0.45f), powerBarColor, tempbuff );	
			
}





void LayerManager2d::drawBar(BarVertex *pVertices)
{
	if (g_d3ddevice)
	{	
		// do this so it doesn't screw up the coloring
		g_d3ddevice->SetTexture(0, 0);

		//draw
		g_d3ddevice->SetFVF(BarVertex::FVF);
		g_d3ddevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, pVertices, sizeof(BarVertex) );
		
	}

}



void LayerManager2d::drawHealthBar(const DWORD &barColor, float barRatio)
{
	BarVertex healthVertices[4];
	    
	if (g_d3ddevice)
	{	
		// vertices for health bar
		healthVertices[0] = BarVertex( (float)g_nScreenWidth * 0.08f, ((float)g_nScreenHeight - invOffset * 0.85f), 0.5f, 1.0f, barColor );
		healthVertices[1] = BarVertex( ((float)g_nScreenWidth * 0.08f) + ((float)g_nScreenWidth * 0.30f * barRatio), ((float)g_nScreenHeight - invOffset * 0.85f), 0.5f, 1.0f, barColor );
		healthVertices[2] = BarVertex( ((float)g_nScreenWidth * 0.08f) + ((float)g_nScreenWidth * 0.30f * barRatio), ((float)g_nScreenHeight - invOffset * 0.65f), 0.5f, 1.0f, barColor );
		healthVertices[3] = BarVertex( (float)g_nScreenWidth * 0.08f, ((float)g_nScreenHeight - invOffset * 0.65f), 0.5f, 1.0f, barColor );
		
		// draw the healthbar now
		drawBar(healthVertices);

	}

}


void LayerManager2d::drawPowerBar(const DWORD &powerBarColor, float barRatio)
{
	BarVertex powerVertices[4];
	    
	if (g_d3ddevice)
	{	
		// vertices for health bar
		powerVertices[0] = BarVertex( (float)g_nScreenWidth * 0.08f, ((float)g_nScreenHeight - invOffset * 0.4f), 0.5f, 1.0f, powerBarColor );
		powerVertices[1] = BarVertex( ((float)g_nScreenWidth * 0.08f) + ((float)g_nScreenWidth * 0.30f * barRatio), ((float)g_nScreenHeight - invOffset * 0.4f), 0.5f, 1.0f, powerBarColor );
		powerVertices[2] = BarVertex( ((float)g_nScreenWidth * 0.08f) + ((float)g_nScreenWidth * 0.30f * barRatio), ((float)g_nScreenHeight - invOffset * 0.2f), 0.5f, 1.0f, powerBarColor );
		powerVertices[3] = BarVertex( (float)g_nScreenWidth * 0.08f, ((float)g_nScreenHeight - invOffset * 0.2f), 0.5f, 1.0f, powerBarColor );
		
		// draw the powerbar now
		drawBar(powerVertices);
				
	}

}



void LayerManager2d::drawInventory()
{
	BarVertex inventoryVertices[4];
	    
	if (g_d3ddevice)
	{	
		// vertices for inventory bar
		inventoryVertices[0] = BarVertex( 0.0f, (float)g_nScreenHeight - invOffset, 0.5f, 1.0f, 0x00000000 );
		inventoryVertices[1] = BarVertex( (float)g_nScreenWidth, (float)g_nScreenHeight - invOffset, 0.5f, 1.0f, 0x00000000 );
		inventoryVertices[2] = BarVertex( (float)g_nScreenWidth, (float)g_nScreenHeight, 0.5f, 1.0f, 0x00000000 );
		inventoryVertices[3] = BarVertex( 0.0f, (float)g_nScreenHeight, 0.5f, 1.0f, 0x00000000 );
		
		// draw the inventory bar now
		drawBar(inventoryVertices);

	}

}

void LayerManager2d::drawStats()
{
	int nPlayerAttack = g_cObjectManager.playerAS();
	int nPlayerDefense = g_cObjectManager.playerDS();

	// Draw "Attack" text
	if( Font )
		Font[7]->DrawText( ((float)g_nScreenWidth * 0.88f), ((float)g_nScreenHeight - invOffset * 0.9f), 0xffffffff, "Attack:" );	

	// Draw Attack number
	char tempbuff[20];
	sprintf(tempbuff,"%d", nPlayerAttack);

	if( Font )
		Font[6]->DrawText( ((float)g_nScreenWidth * 0.96f), ((float)g_nScreenHeight - invOffset * 0.905f), 0xffffffff, tempbuff );	

	// Draw "Defense" text
	if( Font )
		Font[7]->DrawText( ((float)g_nScreenWidth * 0.88f), ((float)g_nScreenHeight - invOffset * 0.45f), 0xffffffff, "Defense:" );	

	// Draw Defense number
	sprintf(tempbuff,"%d", nPlayerDefense);

	if( Font )
		Font[6]->DrawText( ((float)g_nScreenWidth * 0.96f), ((float)g_nScreenHeight - invOffset * 0.4505f), 0xffffffff, tempbuff );	


}


void LayerManager2d::drawItemInventory()
{
	// get local copy of the pointer to the inventory in ObjectManager
	inventoryObj *plocalTheInventory = g_cObjectManager.get_pInventory();

    //get local copy of the number of Objects in the inventory
	int numObjects = plocalTheInventory->get_numObjects();

    gameObj **pplocalTheInvList = plocalTheInventory->getInventoryList();

    // move all the objects
	plocalTheInventory->move();

	//get the index currently pointed to in the inventory
	int currentIndex = g_cObjectManager.getCurrentInventoryIndex();

	// draw 5 items from the inventory based on currently selected item
//    for (int i = currentIndex; i < currentIndex + 5; i++)
    for (int i = 0; ((i < numObjects) && (i < 5)); i++)
    {

		// start at the currently selected object in inventory
		int j = i + currentIndex;

		// handle the case where j becomes larger then number of objects in the
		// inventory
		if ( j >= numObjects )
		{
			j = j - (numObjects);
		}

		if ( pplocalTheInvList[j] != NULL )
        {
	        pplocalTheInvList[j]->draw();
        }
    }

	// draw description for the currently selected item from inventory

    if (pplocalTheInvList[currentIndex] != NULL)
    {
		// determine player coordinates so we can adjust descriptions accordingly
   		D3DXVECTOR3 playerLoc = g_cObjectManager.PlayerLocation();
		
		// see if the inv obj is delicious, it should be but the cast must be done anyway
		deliciousObj *invObj = dynamic_cast< deliciousObj * >( pplocalTheInvList[currentIndex] );

		if (invObj)
		{
			//char itemDescript[60];
			//sprintf( itemDescript,"%s", invObj->itemDescription() );

			if( Font )
			{
				Font[3]->DrawText( ( g_nScreenWidth * 0.15f), 
								   (float)g_nScreenHeight * 0.1f, 
								   0xffffffff, invObj->getCharDescription() );



				// what you have currently selected
			/*	char tempbuff[40];
				sprintf(tempbuff,"You have selected item: %d", currentIndex);

				Font[5]->DrawText( ( g_nScreenWidth * 0.15f), 
								   (float)g_nScreenHeight * 0.15f, 
								   0xffffffff, tempbuff );*/


			}
		}
		else
		{
			if( Font )
				Font[3]->DrawText( ( g_nScreenWidth / 2.0f),  
								   (float)g_nScreenHeight / 9.0f, 
								   0xffffffff, "You are screwed" );
		}
	}


/*****************************************************************************************/
//used for drawing description of all the items under them in the inventory
/*****************************************************************************************/

	//for (int i = g_cObjectManager.getCurrentInventoryIndex(); 
	//	     i < g_cObjectManager.getCurrentInventoryIndex() + 5; i++)
 //   {
	//	int j = i;

	//	if (j > numObjects)
	//	j = j - (numObjects + 1);

 //        if (pplocalTheInvList[j] != NULL)
 //        {
 //           // determine player coordinates so we can adjust descriptions accordingly
 //  	        D3DXVECTOR3 playerLoc = g_cObjectManager.PlayerLocation();
	//        
 //           // see if the inv obj is delicious, it should be but the cast must be done anyway
 //           deliciousObj *invObj = dynamic_cast< deliciousObj * >( pplocalTheInvList[j] );

 //           if (invObj)
 //           {
 //              	//char itemDescript[60];
 //               //sprintf( itemDescript,"%s", invObj->itemDescription() );

 //               if( Font )
	//                Font[3]->DrawText( ( g_nScreenWidth / 2.0f - 25.0f + ((float)i * 60.0f * 1.05f)), 
 //                                   ( (float)g_nScreenHeight - (invOffset * 0.2f) ), 
 //                                   0xffffffff, invObj->getCharDescription() );
 //           }
 //           else
 //           {
 //               if( Font )
 //                   Font[3]->DrawText( ( g_nScreenWidth / 2.0f - 25.0f + ((float)i * 60.0f * 1.05f) ), 
 //                                   ( (float)g_nScreenHeight - (invOffset * 0.2f) ), 
 //                                   0xffffffff, "blah blah" );
 //           }


 //        } // end if != NULL
 //   } // end for loop

}


// Returns the invOffset used to determine how far the inventory
// bar is drawn from the bottom of the screen
float LayerManager2d::getInvOffset()
{
	return invOffset;
}





//LayerManager2d::drawFrameRate(){}


	//// Draw "Power" text
	//if( Font )
	//	Font[8]->DrawText( ((float)g_nScreenWidth * 0.015f), ((float)g_nScreenHeight - invOffset * 0.45f), healthBarColor, "Power" );	


