/// \file setup.cpp
/// Windows and d3d setup.
/// Setup stuff that won't change much is hidden away in this file
/// so you won't have to keep looking at it.
/// Copyright Ian Parberry, 2004.
/// Last updated August 20, 2004.

//system includes
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>

#include "defines.h" ///< essential defines
#include "spriteman.h" ///< sprite manager
#include "random.h" ///< random numbers
#include "debug.h" ///< for debugging stupid terrain vertices thing.

extern IDirect3D9* g_d3d; ///< D3D object
extern IDirect3DDevice9* g_d3ddevice; ///< graphics device 
extern D3DPRESENT_PARAMETERS g_d3dpp; ///< device specs
extern LPDIRECT3DVERTEXBUFFER9 g_pBackgroundVB;  //vertex buffer for background
extern LPDIRECT3DVERTEXBUFFER9 g_pTerrainVB;  ///< vertex buffer for terrain
extern LPDIRECT3DINDEXBUFFER9 g_pTerrainIB; ///< index buffer for terrain
extern IDirect3DSurface9* g_d3dbackbuffer; ///< Back buffer for rendering.
extern int g_nScreenWidth; ///< screen width
extern int g_nScreenHeight; ///< screen height
extern int g_terrainVertices;
extern int g_terrainPrimitives;
extern BOOL g_bWindowed; ///< TRUE to run in a window, FALSE for fullscreen
extern BOOL g_bCameraDefaultMode; ///< FALSE for eagle-eye camera mode
extern CRandom g_cRandom; ///< random number generator
extern IDirect3DSurface9* g_pCursorBitmap; ///< surface for mouse cursor
extern CSpriteManager g_cSpriteManager; ///< sprite manager
extern D3DMATERIAL9 g_baseMaterial; ///< base material for sky and ground, etc
extern int g_gameLevel; ///< level
extern weatherType g_currentWeather; ///< what's the weather like?
extern timeOfDay g_currentTime; ///< what time of day is it?
extern groundType g_levelGround; ///< what is the ground texture?
extern locationType g_levelLocation; ///< where does the level take place?

const DWORD BILLBOARDVERTEX::FVF = D3DFVF_XYZ | D3DFVF_TEX1; //flexible vertex format

long CALLBACK WindowProc(HWND hwnd,UINT message,
  WPARAM wParam,LPARAM lParam); //from Main.cpp

/// Create a default window.
/// Registers and creates a full screen window.
///  \param name the name of this application
///  \param hInstance handle to the current instance of this application
///  \param nCmdShow specifies how the window is to be shown
///  \return handle to the application window

HWND CreateDefaultWindow(char* name,HINSTANCE hInstance,int nCmdShow){

  WNDCLASS wc; //window registration info
  //fill in registration information wc
  wc.style=CS_HREDRAW|CS_VREDRAW; //style
  wc.lpfnWndProc=WindowProc; //window message handler
  wc.cbClsExtra=wc.cbWndExtra=0;
  wc.hInstance=hInstance; //instance handle
  wc.hIcon=LoadIcon(hInstance,IDI_APPLICATION); //default icon
  wc.hCursor=NULL; //no cursor
  wc.hbrBackground=NULL; //we will draw background
  wc.lpszMenuName=NULL; //no menu
  wc.lpszClassName=name; //app name provided as parameter

  //register window
  RegisterClass(&wc);

  //create and set up fullscreen window
  HWND hwnd=CreateWindowEx(WS_EX_APPWINDOW|WS_EX_DLGMODALFRAME,name,name,
    WS_POPUP|WS_CLIPCHILDREN|WS_OVERLAPPEDWINDOW,0,0,
    GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
    NULL,NULL,hInstance,NULL);
  if(hwnd){ //if successfully created window
    ShowWindow(hwnd,nCmdShow); UpdateWindow(hwnd); //show and update
    SetFocus(hwnd); //get input from keyboard
  }

  return hwnd; //return window handle
} //CreateDefaultWindow

//graphics init functions

/// Create the D3D device.
///  \param hwnd window handle
///  \param mode display mode (width, height, color depth, refresh rate)
///  \return the error code returned by D3D, if any

HRESULT CreateD3DDevice(HWND hwnd,D3DDISPLAYMODE mode){ //create d3d device

  //set up device specs data structure
  ZeroMemory (&g_d3dpp,sizeof(D3DPRESENT_PARAMETERS)); //null it out
  g_d3dpp.BackBufferWidth=mode.Width; //width
  g_d3dpp.BackBufferHeight=mode.Height; //height
  g_d3dpp.BackBufferFormat=mode.Format; //color mode
  g_d3dpp.BackBufferCount=2; //two back buffers
  g_d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE; //unused
  g_d3dpp.SwapEffect=D3DSWAPEFFECT_FLIP; //flip pages
  g_d3dpp.hDeviceWindow=NULL; //full screen
  g_d3dpp.Windowed=0; //full screen
  g_d3dpp.EnableAutoDepthStencil=FALSE; //unused
  g_d3dpp.AutoDepthStencilFormat=(D3DFORMAT)0; //unused
  g_d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER; //can lock buffer
  g_d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;

  //refresh rate - use from mode descriptor if available
  if(mode.RefreshRate)
    g_d3dpp.FullScreen_RefreshRateInHz=mode.RefreshRate;
  else
    g_d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

  //check for windowed mode
  if(g_bWindowed){ //running in windowed mode, change some settings
		g_d3dpp.Windowed = 1; //windowed
		g_d3dpp.BackBufferCount = 1; //1 back buffer
		g_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY; //blit instead of swapping
		g_d3dpp.FullScreen_RefreshRateInHz = 0; //unused
    //center window on screen
    int x = (GetSystemMetrics(SM_CXSCREEN)-g_nScreenWidth)/2;
    int y = (GetSystemMetrics(SM_CYSCREEN)-g_nScreenHeight)/2;
		::SetWindowPos(hwnd,NULL,x,y,g_nScreenWidth,g_nScreenHeight,
			SWP_NOZORDER|SWP_SHOWWINDOW);
  }

  //now we can create the device
  return g_d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,
    D3DCREATE_SOFTWARE_VERTEXPROCESSING,&g_d3dpp,&g_d3ddevice);
} //CreateD3DDevice

/// Choose which graphics mode to use.
/// Queries D3D to see which graphics modes are compatible with a screen of
/// size g_nScreenWidth by g_nScreenHeight, and chooses the one with the 
/// fastest refresh rate.
///  \param chosenmode returns which D3D graphics mode has been chosen
///  \return TRUE if a compatible mode was found

BOOL ChooseGraphicsMode(D3DDISPLAYMODE& chosenmode){

  D3DFORMAT format=D3DFMT_X8R8G8B8; //graphics format
  int nNumGraphicsModes = g_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT,format);
  if(nNumGraphicsModes<=0)return FALSE; //no available graphics modes
  BOOL bChosenMode=FALSE; //TRUE when mode chosen
  D3DDISPLAYMODE curmode; //current mode under consideration
  unsigned int nRefreshRate=0; //chosen mode's refresh rate

  for(int i=0; i<nNumGraphicsModes; i++){ //for each mode
    g_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT,format,i,&curmode); //get specs in curmode
    if( curmode.Width==g_nScreenWidth && curmode.Height==g_nScreenHeight ) //right size
      if( !bChosenMode || curmode.RefreshRate==0 || curmode.RefreshRate>nRefreshRate ){
        //and faster refresh rate, choose this mode instead
        chosenmode = curmode; 
        nRefreshRate = curmode.RefreshRate; 
        bChosenMode = TRUE;
      }
  }

  return bChosenMode;
} //ChooseGraphicsMode

///Initialize D3D.
/// Creates a D3D object, sets the graphics mode, creates a vertex buffer 
/// for the background.
///  \param hInstance instance handle
///  \param hwnd window handle
///  \return TRUE if it succeeded

BOOL InitD3D(HINSTANCE hInstance,HWND hwnd)
{ 

	HRESULT hr; //result from D3D function calls

	//create d3d object
	g_d3d=Direct3DCreate9(D3D_SDK_VERSION); //create D3D object
	if(g_d3d == NULL) // if failed
	{
		return FALSE; //bail
	} // end if failed

	//choose graphics format
	D3DDISPLAYMODE displaymode; //display mode
	if(!ChooseGraphicsMode(displaymode))return FALSE; //bail and fail if no mode acceptable

	//create d3d device for mode
	hr=CreateD3DDevice(hwnd,displaymode);
	if(FAILED(hr)) //bail if failed
	{ 
		g_d3ddevice=NULL; return FALSE;
	}

	//create vertex buffer for background
	hr = g_d3ddevice->CreateVertexBuffer(4 * sizeof(BILLBOARDVERTEX),D3DUSAGE_WRITEONLY,BILLBOARDVERTEX::FVF,D3DPOOL_MANAGED,&g_pBackgroundVB,NULL);
	hr = g_d3ddevice->CreateVertexBuffer(g_terrainVertices * sizeof(BILLBOARDVERTEX),D3DUSAGE_WRITEONLY,BILLBOARDVERTEX::FVF,D3DPOOL_MANAGED,&g_pTerrainVB,NULL);
	hr = g_d3ddevice->CreateIndexBuffer(g_terrainPrimitives * 3 * sizeof(short),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&g_pTerrainIB,NULL);
	if(FAILED(hr))
	{
		return FALSE; //bail if failed
	}
  
	//get interface to back buffer for rendering
	hr = g_d3ddevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_d3dbackbuffer);
	if(FAILED(hr)) //bail if failed
	{
		g_d3dbackbuffer = NULL; return FALSE;
	}

	return TRUE; //success exit
} //InitD3D

/// Set the world matrix.
/// Sets the world matrix to a translation matrix a given distance along the X-axis.
///  \param dx distance along the X-axis

void SetWorldMatrix(float dx=0.0f)
{
	D3DXMATRIXA16 matWorld;   
	D3DXMatrixTranslation(&matWorld,dx,0.0,0.0);
	g_d3ddevice->SetTransform(D3DTS_WORLD,&matWorld );
} //SetWorldMatrix

/// Set the view matrix.
/// Sets the view matrix to a hard-coded position, orientation, and direction. If bDefaultView
/// is TRUE, it sets it to the same matrix as in Demo 2. Otherwise it pulls the camera up and back
/// so that the programmer can get an eagle-eye view of the  virtual environment.
/// \param x x coordinate of camera
/// \param y y coordinate of camera
/// \param bDefaultView TRUE for camera in game position, otherwise eagle-eye view


void SetViewMatrix(float x, float y, BOOL bDefaultView=TRUE) //set view matrix
{

	D3DXVECTOR3 vUpVec(0.0f,1.0f,0.0f); //up vector
	D3DXVECTOR3 vEyePt; //camera position
	D3DXVECTOR3 vLookatPt; //camera view vector
	D3DXMATRIXA16 matView; //view matrix

	if(bDefaultView) //default camera view
	{
	//	vEyePt = D3DXVECTOR3(x-200,400.0f,-500.0f);
		vEyePt = D3DXVECTOR3(x,400.0f-75.0f,-500.0f);
	//	vLookatPt = D3DXVECTOR3(x,400.0,500.0f);
		vLookatPt = D3DXVECTOR3(x,350.0-75.0f,500.0f);
	}
	else //eagle eye view
	{
		vEyePt = D3DXVECTOR3(x+3000.0f,1000.0f,-5000.0f);
		vLookatPt = D3DXVECTOR3(x,1000.0f,1.0f);
	}

	D3DXMatrixLookAtLH(&matView,&vEyePt,&vLookatPt,&vUpVec); //compute view matrix
	
	g_d3ddevice->SetTransform(D3DTS_VIEW,&matView); //set it

} //SetViewMatrix

/// Set the projection matrix.
/// Sets the projection matrix to the correct aspect ratio, and a 90 degree field of view.

void SetProjectionMatrix()
{ 
	D3DXMATRIXA16 matProj;
	FLOAT fAspect=(float)g_nScreenWidth/(float)g_nScreenHeight;
	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI/4.0f,fAspect,1.0f,15000.0f);
	g_d3ddevice->SetTransform(D3DTS_PROJECTION,&matProj);
} //SetProjectionMatrix


// set base material for backgrounds, etc
void loadBaseMaterial()
{
	ZeroMemory(&g_baseMaterial,sizeof(g_baseMaterial));
	
	g_baseMaterial.Ambient.r = 1.0f; // original color
	g_baseMaterial.Ambient.g = 1.0f; // original color
	g_baseMaterial.Ambient.b = 1.0f; // original color
	g_baseMaterial.Ambient.a = 1.0f; // original color
	
	g_baseMaterial.Diffuse.r = 0.0f; // original color
	g_baseMaterial.Diffuse.g = 0.0f; // original color
	g_baseMaterial.Diffuse.b = 0.0f; // original color
	g_baseMaterial.Diffuse.a = 0.0f; // original color

	g_baseMaterial.Specular.r = 0.0f; // original color
	g_baseMaterial.Specular.g = 0.0f; // original color
	g_baseMaterial.Specular.b = 0.0f; // original color
	g_baseMaterial.Specular.a = 0.0f; // original color
	
	
} // end loadBaseMaterial


/// Set the ambient lighting.
/// Actually, it does a whole bunch of stuff
/// 
void SetAmbientLighting(int level)
{ 
	g_d3ddevice->SetRenderState(D3DRS_AMBIENT,0x00111111);

} //SetLighting

// set time and weather based on level






/// Set the texture states.
/// Sets the D3D texture states to useful defaults.

void SetTextureStates(){ //set texture states to default
  g_d3ddevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
  g_d3ddevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
  g_d3ddevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
  g_d3ddevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
  g_d3ddevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
  g_d3ddevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
  g_d3ddevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
  g_d3ddevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
  g_d3ddevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
  g_d3ddevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
} //SetTextureStates

/// Load background vertex buffer.
/// Initializes the positions and texture coordinates of the vertices in the vertex buffer
/// used to store the background. Uses hard-coded values.

void LoadBgVertexBuffer() //load background vertex buffer
{

	BILLBOARDVERTEX* v; // vertex buffer data
	BILLBOARDVERTEX* t; // terrain vertex buffer data

	float w = 2.0f * (float) g_nScreenWidth; // width
	float h = 2.0f * (float) g_nScreenHeight; // height
	int terrainXScale = (int)w / terrainXSize;
	int terrainZScale = (int)h / terrainZSize;

	if(SUCCEEDED(g_pBackgroundVB->Lock(0,0,(void**)&v,0))) //lock buffer
	{
		//vertex information, first triangle in clockwise order

		v[0].p=D3DXVECTOR3(w*2,-3*h+500,5000);
		v[0].tu=1.0f; v[0].tv=1.0f;

	    v[1].p=D3DXVECTOR3(-w*2,-3*h+500,5000);
		v[1].tu=0.0f; v[1].tv=1.0f;

		v[2].p=D3DXVECTOR3(w*2,2*h-500,5000);
		v[2].tu=1.0f; v[2].tv=0.0f;

		v[3].p=D3DXVECTOR3(-w*2,2*h-500,5000);
		v[3].tu=0.0f; v[3].tv=0.0f;

		g_pBackgroundVB->Unlock();
	} // end if succeeded

	if(SUCCEEDED(g_pTerrainVB->Lock(0,0,(void**)&t,0))) //lock buffer
	{
		short* pIndexData;
		int firstIndex, secondIndex, offset;

		//create vertex buffer and fill in vertices
		
		for(int z = 0; z < (terrainZSize + 1); z++)
		{
			for(int x = 0; x < (terrainXSize + 1); x++)
			{
				firstIndex = x + z * (terrainXSize + 1);
				t[firstIndex].p.x = (float)((terrainXSize * terrainXScale) - (x * terrainXScale));
				switch (g_levelGround)
				{
					case GRASSY_GROUND:
						t[firstIndex].p.y = (float)g_cRandom.number(-5, 10);
						break;
					case WOOD_GROUND:
						t[firstIndex].p.y = 0.0f;
						break;
					case STONE_GROUND:
						t[firstIndex].p.y = (float)g_cRandom.number(-10, 20);
						break;
					case SAND_GROUND:
						t[firstIndex].p.y = (float)g_cRandom.number(-3, 3);
						break;
					default:
						t[firstIndex].p.y = (float)g_cRandom.number(-50, 50);
				} // end switch terrain
				
				t[firstIndex].p.z = (float)(z * terrainZScale);
				t[firstIndex].tu = ((float)(terrainXSize - x) / terrainXSize);
				t[firstIndex].tv = ((float)(terrainZSize - z) / terrainZSize);
				//DEBUGPRINTF("LoadBgVertexBuffer - terrain vertex t[%d] is at x: %f, y: %f, z: %f, u: %f, v: %f\n", x + z * (terrainXSize + 1), t[firstIndex].p.x, t[firstIndex].p.y, t[firstIndex].p.z, t[firstIndex].tu, t[firstIndex].tv);
			}
		}
		
		for (int x = 1; x < (g_terrainVertices / 32); x++)
		{
			offset = x - 1;
			firstIndex = (x * 32) + offset;
			secondIndex = ((x - 1) * 32) + offset;
			t[firstIndex].p.y = t[secondIndex].p.y;
			//DEBUGPRINTF("LoadBgVertexBuffer - vertex t[%d].y: %f, is equal to vertex[%d].y: %f\n", firstIndex, t[firstIndex].p.y, secondIndex, t[secondIndex].p.y);
		}

		g_pTerrainVB->Unlock();

		//create index buffer and fill in indices
		
		
		g_pTerrainIB->Lock(0,0,(void**)&pIndexData,0);

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

		g_pTerrainIB->Unlock();

	} // end if succeeded


} //LoadBgVertexBuffer

/// Set the D3D render states.
/// Sets the D3D render states (texture states, world matrix, view matrix,
/// projection matrix, background vertex buffer) to useful values for the game.

void SetD3dStates()
{
	//g_d3ddevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	//g_d3ddevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//g_d3ddevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//g_d3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//g_d3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//g_d3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	SetTextureStates(); //set default texture states
	SetWorldMatrix(); //set world matrix
	SetProjectionMatrix(); //set projection matrix
	SetAmbientLighting(g_gameLevel); //set ambient lighting
//	setEnvironment();
	LoadBgVertexBuffer(); //load background vertex buffer
	loadBaseMaterial(); // load base material
} //SetD3dStates

//graphics release functions

/// Release D3D.
/// Releases the background vertex buffer, D3D device, and D3D object.

void ReleaseD3D()
{ 
	if(g_d3dbackbuffer)g_d3dbackbuffer->Release(); //release back buffer
	if(g_pBackgroundVB)g_pBackgroundVB->Release(); //release background vertex buffer
	if(g_d3ddevice)g_d3ddevice->Release(); //release device
	if(g_d3d)g_d3d->Release(); //release d3d object
} //ReleaseD3D

//graphics restore functions

/// Restore the D3D device. The D3D device can be lost, for example, by the use alt+tabbing out
/// of the app. This function restores the device, resets the default states that we initially set,
/// and tests the result.
/// \return TRUE if it succeeds

BOOL RestoreDevice(){ //restore lost device and all surfaces
  //release back buffer
  g_d3dbackbuffer->Release(); g_d3dbackbuffer = NULL;
  //sprites
  g_cSpriteManager.OnLostDevice();
  //reset device
  if(FAILED(g_d3ddevice->Reset(&g_d3dpp)))
    return FALSE;
  //get new back buffer
  if( FAILED(g_d3ddevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &g_d3dbackbuffer)) )
    return FALSE;
  //sprites again
  g_cSpriteManager.OnResetDevice();
  //etc.
  SetD3dStates(); //set D3D default states
  //restore mouse cursor
  g_d3ddevice->SetCursorProperties(7, 0, g_pCursorBitmap);
  //and exit
  return SUCCEEDED(g_d3ddevice->TestCooperativeLevel()); //just to be sure
} //RestoreDevice

/// Check for lost D3D device.
/// \return TRUE if the D3D device is lost

BOOL DeviceLost(){ //check for lost device
  return FAILED(g_d3ddevice->TestCooperativeLevel());
} //DeviceLost


