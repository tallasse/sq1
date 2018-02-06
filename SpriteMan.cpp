/// \file SpriteMan.cpp
/// Code for the sprite manager class CSpriteManager.
/// Copyright Ian Parberry, 2004.
/// Last updated October 15, 2004.

#include <stdio.h>
#include <string>

#include "SpriteMan.h"
#include "abort.h"
#include "sprite2d.h"
#include "sprite3d.h"

using namespace std;

extern TiXmlElement* g_xmlSettings; //global XML settings
extern IDirect3DDevice9* g_d3ddevice; //graphics device

CSpriteManager::CSpriteManager(){ //constructor
  for(int i=0; i<NUM_SPRITES; i++) m_pSprite[i] = NULL; 
  m_lpD3DSprite = NULL;
}

CSpriteManager::~CSpriteManager(){ //destructor
  for(int i=0; i<NUM_SPRITES; i++)delete m_pSprite[i]; 
}

/// Load sprite from files.
/// Given a file name and extension such as "foo" and "bmp", reads
/// in sprite frames from "foo1.bmp", "foo2.bmp," etc.
/// \param object object type
/// \param file file name prefix
/// \param ext file extension
/// \param frames number of frames
/// \param b3D TRUE if this is a 3D sprite
/// \return TRUE if load succeeded

BOOL CSpriteManager::Load(ObjectType object, const string file, const string ext, int frames, BOOL b3D)
{

  BOOL success = TRUE; //are reading sucessfully

  if(b3D)m_pSprite[object]=new C3DSprite(frames); //3D
  else m_pSprite[object]=new C2DSprite(frames); //2D

  for(int i=0; i<frames && success; i++) //for each frame
  {
		m_pBuffer = file;
		/*if (m_pBuffer.empty())
			//DEBUGPRINTF("CSpriteManager::Load2 - file name is NULL!\n");
		else
			//DEBUGPRINTF("CSpriteManager::Load2 - file name is NOT null!\n");*/
		//DEBUGPRINTF("CSpriteManager::Load2 - assembling file name, file name: %s \n\n", m_pBuffer.c_str());
		m_pBuffer = file + "0";
		//DEBUGPRINTF("CSpriteManager::Load2 - assembling file name, file name: %s \n\n", m_pBuffer.c_str());
			
		string theNumber;
		ostringstream o; 
		o << i ;
		theNumber = o.str();
		m_pBuffer = m_pBuffer + theNumber;
		
		//DEBUGPRINTF("CSpriteManager::Load2 - assembling file name, file name: %s \n\n", m_pBuffer.c_str());
		m_pBuffer = m_pBuffer + '.';
		//DEBUGPRINTF("CSpriteManager::Load2 - assembling file name, file name: %s \n\n", m_pBuffer.c_str());
		m_pBuffer = m_pBuffer + ext;
		/*if (m_pBuffer.empty())
			//DEBUGPRINTF("CSpriteManager::Load2 - file name is NULL!\n");
		else
			//DEBUGPRINTF("CSpriteManager::Load2 - file name is NOT null!\n");*/
		char theFileName[MAX_PATH];
		//DEBUGPRINTF("CSpriteManager::Load2 - char array created \n");
		strcpy(theFileName, m_pBuffer.c_str());// theFileName = m_pBuffer;
		//DEBUGPRINTF("CSpriteManager::Load2 - new file name converted, name is: %s \n\n", theFileName);
		//DEBUGPRINTF("CSpriteManager::Load2 - new file name assembled and converted, loading from file\n");
		success = m_pSprite[object]->Load(theFileName,i); //load from that file name
		//DEBUGPRINTF("CSpriteManager::Load2 - loaded\n");
  }

  if(!m_lpD3DSprite)D3DXCreateSprite(g_d3ddevice,&m_lpD3DSprite);

  return success; // return success, obviously some work needs to be done here
}

/// Load sprite from info in XML settings.
/// Loads information about the sprite from global variable g_xmlSettings, then
/// loads the sprite images as per that information. Aborts if something goes wrong.
/// \param object object type
/// \param name object name in XML file
/// \param b3D TRUE if this is a 3D sprite

void CSpriteManager::Load(ObjectType object, string name, BOOL b3D/*=TRUE*/)
{
	//DEBUGPRINTF("CSpriteManager::Load function begin.\n");

	int frames = 0; //number of frames in sprite
	BOOL success = FALSE; //TRUE if loaded successfully

	if(g_xmlSettings) //got "settings" tag
	{
		//get "sprites" tag
		TiXmlElement* spriteSettings = g_xmlSettings->FirstChildElement("sprites"); //sprites tag
		
		//DEBUGPRINTF("CSpriteManager::Load - settings tag open, look for sprites tag\n");

		if(spriteSettings) //got "sprites" tag
		{
			//DEBUGPRINTF("CSpriteManager::Load - sprites tag found\n");

			//load "sprite" tag with correct name attribute
			TiXmlElement* spr; //sprite element

			//DEBUGPRINTF("CSpriteManager::Load - create new element\n");

			//set spr to the first "sprite" tag with the correct name
			spr = spriteSettings->FirstChildElement("sprite");

			//DEBUGPRINTF("CSpriteManager::Load - new element is first <sprite> tag\n");

			while(spr && !(name == spr->Attribute("name") ) )
			{
				spr = spr->NextSiblingElement("sprite");
				//DEBUGPRINTF("CSpriteManager::Load - wrong name, traverse to next <sprite> tag\n");
			} // end while

			if(spr) //got "sprite" tag with right name
			{
				//DEBUGPRINTF("CSpriteManager::Load - correct <sprite> tag found\n");
				//get sprite information from tag
				spr->Attribute("frames",&frames);
				//DEBUGPRINTF("CSpriteManager::Load - frames loaded from <sprite> tag, frames = %d \n", frames);
			    //now load the sprite from the information loaded
				//DEBUGPRINTF("CSpriteManager::Load - load sprite with other Load function\n");
				
				success = Load(object, spr->Attribute("file"), spr->Attribute("ext"), frames, b3D);
				
				//DEBUGPRINTF("CSpriteManager::Load - Sprite loaded from Load2\n");

			} // end if (spr)

		} // end if (spriteSettings)
		else
		{
			ABORT("Cannot find sprite settings.");
		}

	} // end if (g_xmlSettings)

	if(!success)
	{
		//DEBUGPRINTF("CSpriteManager::Load - WTF not possible\n");
		ABORT("Cannot load sprite \"%s\".\n",name);
	} // end if !success

} // end Load

/// Draw object.
/// Draw a frame of an object of a given type, at a given place. Version for 2D sprites.
/// \param object object type
/// \param p position
/// \param frame frame number

void CSpriteManager::Draw(ObjectType object, D3DXVECTOR3 p, int frame, faceDirection theDirection){ //draw 3D sprite
  if(m_pSprite[object]) //check safety of object
    if(m_pSprite[object]->Is3D()) //if it is 3D
      (m_pSprite[object])->Draw(p, frame, theDirection); //do a 3D draw
    else (m_pSprite[object])->Draw(p, frame, m_lpD3DSprite); //else do a 2D draw
}

/// Reader function for frame count.
/// \param object object type
/// \return number of frames of animation for that object

int CSpriteManager::FrameCount(ObjectType object){ //number of frames
  if(m_pSprite[object])return m_pSprite[object]->m_nFrameCount;
  else return 0;
}

/// Reader function for height.
/// \param object object type
/// \return height of that object

int CSpriteManager::Height(ObjectType object){ //frame height
  if(m_pSprite[object]) return m_pSprite[object]->m_structImageInfo.Height;
  else return 0;
}

/// Reader function for width.
/// \param object object type
/// \return width of that object

int CSpriteManager::Width(ObjectType object){ //frame width
  if(m_pSprite[object]) return m_pSprite[object]->m_structImageInfo.Width;
  else return 0;
}

/// Release function.
/// Releases all textures from all sprites.

void CSpriteManager::Release(){ //release textures from sprites
  for(int i=0; i<NUM_SPRITES; i++)
    if(m_pSprite[i])m_pSprite[i]->Release();
}

/// Preprocessing for lost device. To be called before a reset.

void CSpriteManager::OnLostDevice(){
  if(m_lpD3DSprite)m_lpD3DSprite->OnLostDevice();
}

/// Postprocessing for lost device. To be called after a reset.

void CSpriteManager::OnResetDevice(){  //postprocessing for lost device
  if(m_lpD3DSprite)m_lpD3DSprite->OnResetDevice();
}

/// Begin processing 2D sprites. 
/// The D3DX sprite object changed radically
/// in DirectX 9.0c. Now it's a pipelined process, and this function starts
/// the pipeline.

void CSpriteManager::Begin(){
  m_lpD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
  m_lpD3DSprite->SetWorldViewLH(NULL, NULL);
}

/// End processing 2D sprites. 
/// Matches the Begin() function.

void CSpriteManager::End(){
  m_lpD3DSprite->End();
}