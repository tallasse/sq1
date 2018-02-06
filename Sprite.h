/// \file sprite.h
/// Interface for the base sprite class C3DSprite.
/// Copyright Ian Parberry, 2004
/// Last updated October 1, 2004

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "tinyxml.h"
#include "defines.h"

/// The base sprite class. The base sprite contains basic information for drawing a
/// billboard sprite in our 3D world.

class CSprite{ //base sprite

  friend class CSpriteManager;

  protected:
    LPDIRECT3DTEXTURE9* m_lpTexture; ///< Pointer to the texture containing the sprite image.
    D3DXIMAGE_INFO m_structImageInfo; ///< Image information.
    int m_nFrameCount; ///< Number of sprite frames.
    BOOL m_bIs3D; ///< TRUE if sprite is 3D, FALSE for 2D

  public:
    CSprite(int framecount); ///< Constructor.
    CSprite::~CSprite(); ///< Destructor.
    virtual BOOL Load(char* filename,int frame); ///< Load texture image from file.
    virtual void Release(); ///< Release sprite.
    BOOL Is3D(){return m_bIs3D;}; ///< Is this the base of a 3D sprite?
    virtual void Draw(D3DXVECTOR3 p,int frame, faceDirection theDirection){}; ///< Null virtual function for 3D draw.
    virtual void Draw(D3DXVECTOR3 p, int frame, LPD3DXSPRITE lpD3DSprite){}; ///< Null virtual function for 2D draw.

};