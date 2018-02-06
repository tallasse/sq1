/// \file sprite3d.h
/// Interface for the 3D sprite class C3DSprite.
/// Copyright Ian Parberry, 2004
/// Last updated October 15, 2004

#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "sprite.h" //base sprite class

class C3DSprite: public CSprite{ //3D sprite
  private:
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;  //vertex buffer
	//LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferReverse;  //vertex buffer
    D3DXMATRIXA16 m_matWorld; //translation matrix
  public:
    C3DSprite(int framecount); //constructor
    virtual BOOL Load(char* filename,int frame); //load surface
    virtual void Release(); //release sprite
    void Draw(D3DXVECTOR3 p, int frame, faceDirection theDirection); //draw sprite at point p
};
