/// \file sprite3d.cpp
/// Code for the 3D sprite class C3DSprite.
/// Copyright Ian Parberry, 2004.
/// Last updated October 15, 2004.

#include "sprite3d.h"
#include "defines.h"

extern int g_nScreenWidth;

extern IDirect3DDevice9* g_d3ddevice; //graphics device 

C3DSprite::C3DSprite(int framecount):CSprite(framecount){ //constructor
  m_bIs3D = TRUE; //yes, I'm 3D
  m_pVertexBuffer = NULL; //vertex buffer
  D3DXMatrixIdentity(&m_matWorld); //world matrix
}

/// The Load function loads the sprite image into a texture from a given file name.
/// It also creates a vertex buffer for the billboard image containing 4 corner vertices
/// spaced apart the appropriate width and height.
/// \param filename the name of the image file
/// \param frame frame number

BOOL C3DSprite::Load(char* filename,int frame){

  CSprite::Load(filename,frame); //load the image

  //create vertex buffer
  if(m_pVertexBuffer == NULL){ //create only when first frame is loaded
    HRESULT hr = g_d3ddevice->CreateVertexBuffer( 
      4*sizeof(BILLBOARDVERTEX), D3DUSAGE_WRITEONLY, BILLBOARDVERTEX::FVF,
      D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
    if(FAILED(hr))return FALSE; //bail if failed

    //load vertex buffer
    BILLBOARDVERTEX* v; //vertex buffer data
    float w = (float)m_structImageInfo.Width/2.0f; //width
    float h = (float)m_structImageInfo.Height/2.0f; //height
    if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&v, 0))){ //lock buffer
      //vertex information, first triangle in clockwise order
      v[0].p = D3DXVECTOR3(w, h, 0.0f);
      v[0].tu = 1.0f; v[0].tv = 0.0f;
      v[1].p = D3DXVECTOR3(w, -h, 0.0f);
      v[1].tu = 1.0f; v[1].tv = 1.0f;
      v[2].p = D3DXVECTOR3(-w, h, 0.0f);
      v[2].tu = 0.0f; v[2].tv = 0.0f;
      v[3].p = D3DXVECTOR3(-w, -h, 0.0f);
      v[3].tu = 0.0f; v[3].tv = 1.0f;
      m_pVertexBuffer->Unlock(); //unlock buffer
    }
    else return FALSE;
  }
  return TRUE; //successful
}

/// Draws the sprite image at a given point in 3D space relative to the center of the screen.
/// \param p point in 3D space at which to draw the sprite
/// \param frame frame number

void C3DSprite::Draw(D3DXVECTOR3 p, int frame, faceDirection theDirection)
{
	if (theDirection == FACE_RIGHT)
	{
		BILLBOARDVERTEX* v; //vertex buffer data
		float w = (float)m_structImageInfo.Width/2.0f; //width
		float h = (float)m_structImageInfo.Height/2.0f; //height
		if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&v, 0))) //lock buffer
		{
			//vertex information, first triangle in clockwise order
			v[0].p = D3DXVECTOR3(w, h, 0.0f);
			v[0].tu = 1.0f; v[0].tv = 0.0f;
			v[1].p = D3DXVECTOR3(w, -h, 0.0f);
		    v[1].tu = 1.0f; v[1].tv = 1.0f;
			v[2].p = D3DXVECTOR3(-w, h, 0.0f);
		    v[2].tu = 0.0f; v[2].tv = 0.0f;
			v[3].p = D3DXVECTOR3(-w, -h, 0.0f);
			v[3].tu = 0.0f; v[3].tv = 1.0f;
			m_pVertexBuffer->Unlock(); //unlock buffer
	    }	

	}
	else
	{
		BILLBOARDVERTEX* v; //vertex buffer data
		float w = (float)m_structImageInfo.Width/2.0f; //width
		float h = (float)m_structImageInfo.Height/2.0f; //height
		if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, (void**)&v, 0))) //lock buffer
		{
			//vertex information, first triangle in clockwise order
			v[0].p = D3DXVECTOR3(w, h, 0.0f);
			v[0].tu = 0.0f; v[0].tv = 0.0f;
			v[1].p = D3DXVECTOR3(w, -h, 0.0f);
		    v[1].tu = 0.0f; v[1].tv = 1.0f;
			v[2].p = D3DXVECTOR3(-w, h, 0.0f);
			v[2].tu = 1.0f; v[2].tv = 0.0f;
			v[3].p = D3DXVECTOR3(-w, -h, 0.0f);
		    v[3].tu = 1.0f; v[3].tv = 1.0f;
			m_pVertexBuffer->Unlock(); //unlock buffer
		}
	}

  //set diffuse blending for alpha set in vertices
  //g_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	

  g_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  g_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  //set translation matrix
  D3DXMatrixTranslation(&m_matWorld, p.x + g_nScreenWidth/2, p.y, p.z);
  g_d3ddevice->SetTransform(D3DTS_WORLD,&m_matWorld);

  //set vertex buffer
  g_d3ddevice->SetStreamSource(0,m_pVertexBuffer, 0, sizeof(BILLBOARDVERTEX));
  g_d3ddevice->SetFVF(BILLBOARDVERTEX::FVF); //flexible vertex format

  //set texture
  if(frame < m_nFrameCount && m_lpTexture[frame]) //if ok
    g_d3ddevice->SetTexture(0,m_lpTexture[frame]); //set sprite texture

	g_d3ddevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_d3ddevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	g_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


  //draw sprite
  g_d3ddevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  //restore alpha blend state
  g_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

/// Release the sprite.
/// Releases all of the texture frames.

void C3DSprite::Release(){ //release sprite
  CSprite::Release(); //release the textures
  if(m_pVertexBuffer)m_pVertexBuffer->Release(); //release vertex buffer
}

