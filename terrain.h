// terrain header


#include "random.h" // random numbers
#include <d3d9.h>

const int terrainXSize = 64;
const int terrainZSize = 64;

#define D3DFVF_CUSTOMVERTEX	(D3DFVF_XYZ | D3DFVF_DIFFUSE)

class terrain
{

	protected:
		
		LPDIRECT3DVERTEXBUFFER9	m_pTerrainVB;
		LPDIRECT3DINDEXBUFFER9	m_pTerrainIB;
		DWORD m_dwTerrainVertices;
		DWORD m_dwTerrainPrimitives;


	public:
		
		terrain(void);
		void createTerrain();
        void drawTerrain();
};