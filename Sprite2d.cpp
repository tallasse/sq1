/// \file sprite2d.cpp
/// Code for the 2D sprite class C2DSprite.
/// Copyright Ian Parberry, 2004.
/// Last updated October 22, 2004.

#include "sprite2d.h"

extern IDirect3DDevice9* g_d3ddevice; //graphics device 

C2DSprite::C2DSprite(int framecount):CSprite(framecount){ //constructor
  m_bIs3D=FALSE; //not 3D
}

/// Draw sprite.
/// \param p Point at which to draw center of sprite
/// \param frame Frame number to draw from sprite
/// \param lpD3DSprite Pointer to D3DX sprite object to manage sprite drawing

void C2DSprite::Draw(D3DXVECTOR3 p, int frame, LPD3DXSPRITE lpD3DSprite){
  //find top left corner of sprite on screen
  p.x -= (float)m_structImageInfo.Width/2.0f;
  p.y -= (float)m_structImageInfo.Height;
  //and draw it
  if(frame<m_nFrameCount && m_lpTexture[frame] && lpD3DSprite) //if ok
    lpD3DSprite->Draw(m_lpTexture[frame], NULL, NULL, &p, 0xFFFFFFFF);
}

