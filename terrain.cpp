// terrain implementation

#include "terrain.h"
#include "random.h"
#include "defines.h"
#include <fstream>
#include <d3d9.h>

using namespace std;


extern CRandom g_cRandom; ///< random number generator
extern IDirect3DDevice9* g_d3ddevice; ///< graphics device 
extern LPDIRECT3DTEXTURE9 g_pFloorTexture; ///< Texture for floor.

terrain::terrain()
{
	m_pTerrainVB = NULL;
	m_pTerrainIB = NULL;

	m_dwTerrainVertices = (terrainXSize + 1) * (terrainZSize + 1);
	m_dwTerrainPrimitives = terrainXSize * terrainZSize * 2;

	createTerrain();
}//CTerrain

/************************************************************************/
/* Name:		CreateTerrain											*/
/* Description:	fill vertex and index buffer							*/
/************************************************************************/

void terrain::createTerrain()
{
 //   char terrainHeightMap[terrainXSize + 1][terrainZSize + 1];
	D3DVERTEX* vertexData;
	short* pIndexData;
	ifstream inFile;

	//load height information from file
	/*inFile.open("terrain.raw", ios::binary);

	for(int z = 0; z < (terrainZSize + 1); z++)
	{
		for(int x = 0; x < (terrainXSize + 1); x++)
		{
			inFile.get(terrainHeightMap[x][z]);
		} // end for x
	} // end for z

	inFile.close();
*/
	//create vertex buffer and fill in vertices
	g_d3ddevice->CreateVertexBuffer(sizeof(D3DVERTEX) * m_dwTerrainVertices,D3DUSAGE_WRITEONLY,D3DFVF_CUSTOMVERTEX,D3DPOOL_MANAGED,&m_pTerrainVB,NULL);
	m_pTerrainVB->Lock(0,0,(void**)&vertexData,0);

	for(int z = 0; z < (terrainZSize + 1); z++)
	{
		for(int x = 0; x < (terrainXSize + 1); x++)
		{
			vertexData[x + z * (terrainXSize + 1)].fX = (float)x;
			vertexData[x + z * (terrainXSize + 1)].fY = (float)g_cRandom.number(-2, 1);
			vertexData[x + z * (terrainXSize + 1)].fZ = (float)z;
			vertexData[x + z * (terrainXSize + 1)].dwColor = 0xffffffff;
		}
	}

	m_pTerrainVB->Unlock();

	//create index buffer and fill in indices
	g_d3ddevice->CreateIndexBuffer(sizeof(short) * m_dwTerrainPrimitives * 3,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pTerrainIB,NULL);
	m_pTerrainIB->Lock(0,0,(void**)&pIndexData,0);

	for(z = 0; z < terrainZSize; z++)
	{
		for(int x = 0; x < terrainXSize; x++)
		{
			*pIndexData++ = x + z * (terrainXSize + 1);
			*pIndexData++ = x + 1 + z * (terrainXSize + 1);
			*pIndexData++ = x + 1 + (z + 1) * (terrainXSize + 1);

			*pIndexData++ = x + z * (terrainXSize + 1);
			*pIndexData++ = x + 1 + (z + 1) * (terrainXSize + 1);
			*pIndexData++ = x + (z + 1) * (terrainXSize + 1);
		}
	}

	m_pTerrainIB->Unlock();
} // end createTerrain

/************************************************************************/
/* Name:		Draw													*/
/* Description:	draw terrain											*/
/************************************************************************/

void terrain::drawTerrain()
{
	//g_d3ddevice->SetTexture(0,g_pFloorTexture); //set floor texture
    g_d3ddevice->SetStreamSource(0,m_pTerrainVB,0,sizeof(D3DVERTEX));
	g_d3ddevice->SetIndices(m_pTerrainIB);
	g_d3ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_dwTerrainVertices,0,m_dwTerrainPrimitives);
} // end drawTerrain