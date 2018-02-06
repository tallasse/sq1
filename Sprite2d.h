/// \file sprite2d.h
/// Interface for the 2D sprite class C2DSprite.
/// Copyright Ian Parberry, 2004
/// Last updated October 15, 2004

#pragma once

#include <d3dx9.h>
#include "sprite.h"

/// The 2D sprite class.
/// This sprite is designed to be drawn directly on the screen
/// using the D3DX sprite class.

class C2DSprite: public CSprite{ //2D sprite
  public:
    C2DSprite(int framecount); //constructor
    void Draw(D3DXVECTOR3 p, int frame, LPD3DXSPRITE lpD3DSprite); //draw sprite at point p
};
