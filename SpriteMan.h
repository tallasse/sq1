/// \file SpriteMan.h
/// Interface for the sprite manager class CSpriteManager.
/// Copyright Ian Parberry, 2004
/// Last updated October 15, 2004

#pragma once

#include "defines.h"
#include "sprite.h"
#include "tinyxml.h"
#include <string>
#include <sstream>

using namespace std;

/// The sprite manager. The sprite manager is responsible for managing
/// sprites.

class CSpriteManager{

private:
  CSprite* m_pSprite[NUM_SPRITES]; ///< Sprite pointers.
  string m_pBuffer; ///< File name buffer.
  LPD3DXSPRITE m_lpD3DSprite; ///< D3DX sprite object.
  BOOL Load(ObjectType object,
    const string, const string ext, int frames, BOOL b3D); ///< Load sprite.

public:
	CSpriteManager(); ///< Constructor
	~CSpriteManager(); ///< Destructor
  void Load(ObjectType object, string name, BOOL b3D=TRUE); ///< Load sprite.
  void Draw(ObjectType object, D3DXVECTOR3 p, int frame, faceDirection theDirection); ///< Draw sprite.
  int FrameCount(ObjectType object); ///< Number of frames.
  int Height(ObjectType object); ///< Frame height.
  int Width(ObjectType object); ///< Frame width.
  void Release();  ///< Release textures.
  void OnLostDevice(); ///< Preprocessing for lost device.
  void OnResetDevice(); ///< Postprocessing for lost device.
  void Begin(); ///< Begin 2D sprite processing.
  void End(); ///< End 2D sprite processing.

};
