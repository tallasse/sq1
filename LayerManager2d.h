// Updated by Russell Whitmire 11/30/2004


// Manages the 2d layers for:
// health bar
// power bar
// inventory

#include <d3dx9.h>



struct BarVertex 
{
public:

	float _x, _y, _z, _rhw;
	DWORD _color;
	
	BarVertex(){}
	BarVertex(float x, float y, float z, float rhw, DWORD color)
	{
		_x = x;
		_y = y;
		_z = z;
		_rhw = rhw;
		_color = color;
	}
	static const DWORD FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
};




class LayerManager2d
{
public:
	LayerManager2d();
	void draw2dLayer();
	float getInvOffset();

private:
	
// data members
	//IDirect3DVertexBuffer9* pHealthbar;
	float invOffset; // determines how far the inventory bar is from bottom of screen

// data functions
	void drawBar(BarVertex *pVertices);
    
	void drawHealthNumber(const DWORD &healthBarColor);
	void drawPowerNumber(const DWORD &powerBarColor);

	void drawHealthBar(const DWORD &healthBarColor, float barRatio);
	void drawPowerBar(const DWORD &powerBarColor, float barRatio);

	void drawInventory();
	void drawStats();

	void drawItemInventory();

//	void drawFrameRate();



};



