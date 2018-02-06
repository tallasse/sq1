/// \file sprite.cpp
/// Code for the base sprite class CSprite.
/// Copyright Ian Parberry, 2004.
/// Last updated October 15, 2004.

#include "sprite.h"
#include "defines.h"

extern IDirect3DDevice9* g_d3ddevice; //graphics device 
extern int g_nScreenWidth;

CSprite::CSprite(int framecount){ //constructor
  m_bIs3D = FALSE; //assume not 3D to start
  m_nFrameCount = framecount; //number of frames 
  m_lpTexture = new LPDIRECT3DTEXTURE9[framecount]; ///texture array
  for(int i=0; i<framecount; i++) m_lpTexture[i]=NULL; //null it out
  ZeroMemory(&m_structImageInfo,sizeof(D3DXIMAGE_INFO)); //dimensions
}

CSprite::~CSprite(){ //destructor
  delete [] m_lpTexture;
}

/// The Load function loads the sprite image into a texture from a given file name.
/// \param filename the name of the image file
/// \param frame frame number

BOOL CSprite::Load(char* filename, int frame){

  HRESULT hr; //D3D result

  //create texture to store image
  hr=D3DXCreateTextureFromFileEx(g_d3ddevice,filename,
    0,0,0,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,
    D3DX_FILTER_NONE,D3DX_DEFAULT,0,&m_structImageInfo,
    NULL,&m_lpTexture[frame]);
  if(FAILED(hr)){ //fail and bail
    m_lpTexture[frame]=NULL; return FALSE;
  }

  return TRUE; //successful
}

/// Release all textures.

void CSprite::Release(){ //release sprite
  for(int i=0; i<m_nFrameCount; i++) //for each frame
    if(m_lpTexture[i]) m_lpTexture[i]->Release(); //release texture
}

