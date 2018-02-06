/// \file input.cpp
/// Code for the input manager class CInputManager.
/// Copyright Ian Parberry, 2004.
/// Last updated October 22, 2004.

#include <objbase.h>
#include <stdio.h>

#include "input.h"
#include "objman.h" //for object manager
#include "sndlist.h" //for list of sounds
#include "debug.h"
#include "random.h"

extern CRandom g_cRandom;
extern GameStateType g_nCurrentGameState; //current game state
extern GameStateType g_nNextGameState; //next game state
extern BOOL g_bEndGameState; //should we abort current game state?
extern ObjectManager g_cObjectManager; //object manager
extern IDirect3DDevice9* g_d3ddevice; //graphics device 
extern IDirect3DSurface9* g_pCursorBitmap; //surface for mouse cursor
extern BOOL g_bCameraDefaultMode; //FALSE for eagle-eye camera mode
extern int g_nScreenWidth; //screen width
extern int g_nScreenHeight; //screen height
extern BOOL g_bWindowed; //windowed or fullscreen
extern bool g_bWaitingForKeystroke; ///< waiting for keystroke

/// Constructor.
/// \param hInstance Instance handle
/// \param hwnd Window handle

CInputManager::CInputManager(HINSTANCE hInstance, HWND hwnd) //constructor
{
  
	g_bCursorShowing = FALSE; //no mouse cursor yet
	m_nInputDevice = KEYBOARD_INPUT; //default to keyboard input
	m_bJoystickHasSlider = FALSE; //assume no slider unless we find one
	m_nLastKeyStroke = 0;
	m_nLastKeyPressed = 0;
	//button manager doesn't exist yet
	m_pButtonManager = NULL; //for managing menu buttons

	//create DirectInput object
	HRESULT hr=DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
									IID_IDirectInput8A, (void**)&m_lpDirectInput, NULL); 
	if(FAILED(hr))
	{
		m_lpDirectInput = NULL; //in case it fails
		return; //bail out
	}

	//device initializations
	InitKeyboardInput(hwnd); //set up keyboard
	InitMouseInput(hwnd); //set up mouse
	InitJoystickInput(hwnd); //set up joystick
}

CInputManager::~CInputManager(){ //destructor

  if(m_pKeyboard){ //release keyboard
    m_pKeyboard->Unacquire(); //unacquire device
    m_pKeyboard->Release(); //release it
  }

  if(m_pMouse){ //release mouse
    m_pMouse->Unacquire(); //unacquire device
    m_pMouse->Release(); //release it
  }

  if(m_pJoystick){ //release joystick
    m_pJoystick->Unacquire(); //unacquire device
    m_pJoystick->Release(); //release it
  }

  //now release DirectInput
  if(m_lpDirectInput)m_lpDirectInput->Release();

  //get rid of the button manager
  delete m_pButtonManager;
}

//DirectInput setup routines

/// Initialize keyboard input.
/// Create the DirectInput keyboard device, set the cooperative level,
///  set for buffered input, and acquire the keyboard.
/// \param hwnd Window handle

BOOL CInputManager::InitKeyboardInput(HWND hwnd){ //set up keyboard input

  //create keyboard device
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,NULL); 
  if(FAILED(hr)){
    m_pKeyboard=NULL; //in case it fails
    return FALSE; //bail out
  }

  //set keyboard data format, c_dfDIKeyboard is a "predefined global"
  //yucko ugly part of DirectInput, thanks a lot Microsoft
  hr=m_pKeyboard->SetDataFormat(&c_dfDIKeyboard); 
  if(FAILED(hr))return FALSE; //fail and bail

  //set the cooperative level 
  hr=m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);  
  if(FAILED(hr))return FALSE; //fail and bail

  //set buffer size for buffered input
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the keyboard
  hr=m_pKeyboard->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

/// Initialize mouse input.
/// Create the DirectInput mouse device, set the cooperative level,
/// set for buffered input, and acquire the mouse. Also creates the
/// mouse event to handle mouse interrupts, and sets the mouse cursor.
/// \param hwnd Window handle

BOOL CInputManager::InitMouseInput(HWND hwnd){ //set up mouse input

  //create mouse device 
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse,NULL);
    if(FAILED(hr)){
    m_pMouse = NULL; //in case it fails
    return FALSE; //bail out
  }

  //set mouse data format
  hr=m_pMouse->SetDataFormat(&c_dfDIMouse);
  if(FAILED(hr))return FALSE;  //fail and bail

  //set the cooperative level
  hr=m_pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail

  //set up mouse event
  m_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if(!m_hMouseEvent)return FALSE; //fail and bail
  hr = m_pMouse->SetEventNotification(m_hMouseEvent);
  if(FAILED(hr))return FALSE; //fail and bail

   //set buffer size
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the mouse
  hr=m_pMouse->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  //mouse cursor
  g_d3ddevice->SetCursorProperties(7, 0, g_pCursorBitmap); //assign bitmap and hotspot

  return TRUE;
}
//next 2 globals are a kluge for callback functions
LPDIRECTINPUT8 g_lpDirectInput; ///< DirectInput object, for callback function.
LPDIRECTINPUTDEVICE8 g_pJoystick; ///< Joystick device, for callback function.
BOOL g_bJoystickHasSlider=FALSE; ///< Whether joystick has slider, for callback function.

///< Callback function for joystick enumeration.
///< \param pdidInstance Pointer to a DirectInput device instance.
///< \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a joystick, DIENUM_CONTINUE otherwise

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext){

  //attempt to create joystick device.
  HRESULT hr = g_lpDirectInput->CreateDevice(pdidInstance->guidInstance, &g_pJoystick, NULL);
  
  //if it failed, then we can't use this joystick. Maybe the user unplugged it.
  if(FAILED(hr))return DIENUM_CONTINUE;

  //otherwise take the first joystick we get
  return DIENUM_STOP;
}

/// Callback function to enumerate joystick objects (eg axes, slider).
/// \param pdidoi Pointer to a DirectInput device object
/// \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a slider, DIENUM_CONTINUE otherwise

BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext){
  if(pdidoi->dwType&DIDFT_AXIS){ //if object is an axis

    //specify range
    DIPROPRANGE diprg; //range property structure
    diprg.diph.dwSize = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow = DIPH_BYID; //by identifier
    diprg.diph.dwObj = pdidoi->dwType; //specify the enumerated axis
    diprg.lMin = 0; //minimum
    diprg.lMax = 100; //maximum

    //now set range
    if(FAILED(g_pJoystick->SetProperty(DIPROP_RANGE, &diprg.diph))) 
      return DIENUM_STOP; //player may have unplugged joystick
  }

  if(pdidoi->guidType == GUID_Slider)g_bJoystickHasSlider = TRUE;
  return DIENUM_CONTINUE;
}  
                                   
/// Initialize joysticl input.
/// Create the DirectInput joystick device, set the cooperative level,
/// set the data format, get the slider, and acquire the joystick. 
/// \param hwnd Window handle
/// \return TRUE if it succeeds

BOOL CInputManager::InitJoystickInput(HWND hwnd){ //set up joystick input

  HRESULT hr; //return result from DirectInput calls

  //here we go through contortions because a callback function cannot be a member function
  g_lpDirectInput = m_lpDirectInput; //save member variable to global

  //enumerate joysticks using EnumJoysticksCallback function
  hr = m_lpDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
    EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
  if(FAILED(hr))return FALSE; //fail and bail
  m_pJoystick = g_pJoystick; //save resulting device as member variable
  if(m_pJoystick==NULL)return FALSE; //make sure we got one

  //set the data format to "simple joystick" - a predefined data format 
  hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
  if(FAILED(hr))return FALSE; //fail and bail   

  //set the cooperative level
  hr = m_pJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail 

  //enumerate joystick components (eg axes, slider)
  hr = m_pJoystick->EnumObjects(EnumObjectsCallback, (VOID*)hwnd, DIDFT_ALL);
  if(FAILED(hr))return FALSE; //fail and bail 
  m_bJoystickHasSlider = g_bJoystickHasSlider;

  //acquire the joystick
  hr = m_pJoystick->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

//keyboard processing

/// Process keyboard input.
/// Processes keyboard events that have been buffered since the last
/// time this function was called.
/// \return TRUE if user has hit the ESC key

BOOL CInputManager::ProcessKeyboardInput() //process buffered keyboard events
{
	BOOL result = FALSE; //return result
	if(!m_pKeyboard)
		return FALSE; //no device so fail and bail
	
	DWORD dwElements=DI_BUFSIZE; //number of events to process

	//get number of events to process into member buffer m_DIod
	HRESULT hr = m_pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), m_DIod,&dwElements, 0);

	//if it failed, we may be able to acquire the device
	if(FAILED(hr)) //lost the device somehow
	{ 
        hr = m_pKeyboard->Acquire(); //try to reacquire
        while(hr == DIERR_INPUTLOST) 
			hr = m_pKeyboard->Acquire(); //try real hard
		return FALSE; //live to fight another day
	}
	
	if(FAILED(hr))
		return FALSE; //fail and bail paranoia
  
	//process inputs from m_DIod now
	for(int i = 0; (i < (int)dwElements) /*&& !result*/; i++) //for each input
	{
		//if key changes, send key to keyboard handler
		if( ( m_DIod[i].dwData != m_nLastKeyStroke ) || ( m_DIod[i].dwOfs != m_nLastKeyPressed) )
		{
			m_nLastKeyStroke = m_DIod[i].dwData;
			m_nLastKeyPressed = m_DIod[i].dwOfs;
			//DEBUGPRINTF("Input::KeyboardHandler -  keyboard value changed\n");				
			//DEBUGPRINTF("0x%02x%s ", m_DIod[i].dwOfs, (m_DIod[i].dwData & 0x80) ? "D\n" : "U\n");
			result = result || KeyboardHandler(m_DIod[i].dwOfs, m_DIod[i].dwData); //handle it
		}
		else
		{
			//DEBUGPRINTF("Input::KeyboardHandler -  no key change\n");				
		}
		/*
		if( (m_DIod[i].dwData & 0x80) || (m_DIod[i].dwData & 0x00) ) //if key went down
		{
			result = result || KeyboardHandler(m_DIod[i].dwOfs, m_DIod[i].dwData); //handle it
		}
		*/
	}
	return result;
}

//keyboard handlers for different game states

/// Main keyboard handler.
/// This function checks the current game state, then calls the
/// keyboard handler for the appropriate state.
/// \param keystroke Keyboard event to process
/// \return TRUE if the player hit ESC

BOOL CInputManager::KeyboardHandler(DWORD keystroke, DWORD upDown){ //main keyboard handler

  BOOL result = FALSE; //TRUE if we are to exit game

  switch(g_nCurrentGameState){ //select handler for game statez
    case LOGO_GAMESTATE:
    case TITLE_GAMESTATE: 
    case CREDITS_GAMESTATE: 
	case CREDITS1_GAMESTATE: 
	case CREDITS2_GAMESTATE: 
			if (upDown & 0x80)
			{
				IntroKeyboardHandler(keystroke);
			}
			else
			{
				//g_cObjectManager.stopPlayerX();
				//DEBUGPRINTF("Input:: DIK_A released\n");				
			}
			break;
    case MENU_GAMESTATE: 
			if (upDown & 0x80)
			{
				result=MenuKeyboardHandler(keystroke);
			}
			else
			{
				//g_cObjectManager.stopPlayerX();
				//DEBUGPRINTF("Input:: DIK_A released\n");				
			}
			break;
    case DEVICEMENU_GAMESTATE:
			if (upDown & 0x80)
			{
				DEBUGPRINTF("Input:: sending keystroke to DeviceMenuKeyboardHandler\n");
				DeviceMenuKeyboardHandler(keystroke);
			}
			else
			{
				//g_cObjectManager.stopPlayerX();
				DEBUGPRINTF("Input:: DeviceMenuKeyboardHandler is retarded, didn't look at keystrokes\n");			
			}
			break;
	case PLAYING_GAMESTATE: 
		if( (g_bWaitingForKeystroke) && (upDown & 0x80))
		{
			g_bWaitingForKeystroke = false;
		}
		GameKeyboardHandler(keystroke, upDown);
		break;
  }

  return result;
} //KeyboardHandler

/// Intro keyboard handler.
/// Sets g_bEndGameState in response to any keystroke.

void CInputManager::IntroKeyboardHandler(DWORD keystroke){ //intro
  g_bEndGameState = TRUE; //any key ends the state
} //IntroKeyboardHandler

/// Main menu keyboard handler.
/// Selects menu items in response to the appropriate keystroke.
/// \param keystroke Keyboard event to process
/// \return TRUE if the player hit ESC

BOOL CInputManager::MenuKeyboardHandler(DWORD keystroke){ //menu
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_ESCAPE:
    case DIK_Q: result = TRUE; break; //exit game
    case DIK_N: //play new game
      g_bEndGameState = TRUE; g_nNextGameState = PLAYING_GAMESTATE;
      break;
    case DIK_D: //device menu
      g_bEndGameState = TRUE; g_nNextGameState = DEVICEMENU_GAMESTATE;
      break;
    default: break; //do nothing
  }
  return result;
} //MenuKeyboardHandler

/// Keyboard handler for device menu.
/// Sets g_bEndGameState to TRUE if we are to exit the device menu.
/// \param keystroke Keyboard event to process

void CInputManager::DeviceMenuKeyboardHandler(DWORD keystroke)
{

  switch(keystroke){
    case DIK_ESCAPE:
    case DIK_D: //exit menu
      g_bEndGameState = TRUE;
      break;

    case DIK_K: //play using keyboard
      m_nInputDevice = KEYBOARD_INPUT; //set device
      m_pButtonManager->buttondown(KEYBOARD_INPUT);
      break;

    case DIK_J: //play using joystick
      if(m_pJoystick)
	  { //if there is a joystick interface
        m_nInputDevice = JOYSTICK_INPUT; //set device
        m_pButtonManager->buttondown(JOYSTICK_INPUT);
      }
      break;

    default: break;
  }
} //DeviceMenuKeyboardHandler


/// Game menu keyboard handler.
/// Handles keystroke during game play.
/// Sets g_bEndGameState if the player hit ESC
/// \param keystroke Keyboard event to process
/// \param override TRUE if we to accept keystroke even when not in keyboard mode

void CInputManager::GameKeyboardHandler(DWORD keystroke, DWORD upDown, BOOL override)  // game input handler
{ 
  
  //function keys work all the time
	switch(keystroke)
	{
		case DIK_ESCAPE: g_bEndGameState = TRUE; break;
		case DIK_F1:
			if (upDown & 0x80)
			{
				g_bCameraDefaultMode = !g_bCameraDefaultMode;		
			}
			break;	
	}
  
	if (m_nInputDevice == KEYBOARD_INPUT) //read these keys when input is keyboard
	{
	
		switch(keystroke)
		{
			case DIK_UP: 
				//DEBUGPRINTF("keystate = %02x\n", upDown);
				if (upDown & 0x80)
				{
					g_cObjectManager.Action(PLAYER_WALK, 0, 1); 
					//DEBUGPRINTF("Input:: DIK_UP pressed\n");				
				}
				else
				{
					g_cObjectManager.stopPlayerZ();
					//DEBUGPRINTF("Input:: DIK_UP released\n");				
				}
				break;
			case DIK_DOWN: 
				if (upDown & 0x80)
				{
					g_cObjectManager.Action(PLAYER_WALK, 0, -1); 
					//DEBUGPRINTF("Input:: DIK_DOWN pressed\n");				
				}
				else
				{
					g_cObjectManager.stopPlayerZ();
					//DEBUGPRINTF("Input:: DIK_DOWN released\n");				
				}
				//DEBUGPRINTF("Input:: MOVING_DOWN\n");
				break;

			case DIK_LEFT: 
				if (upDown & 0x80)
				{
					g_cObjectManager.Action(PLAYER_WALK, -3, 0); 
					//DEBUGPRINTF("Input:: DIK_LEFT pressed\n");				
				}
				else
				{
					g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_LEFT released\n");				
				}

			break;
				case DIK_RIGHT: 
				if (upDown & 0x80)
				{
					g_cObjectManager.Action(PLAYER_WALK, 3, 0); 
					//DEBUGPRINTF("Input:: DIK_RIGHT pressed\n");				
				}
				else
				{
					g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_RIGHT released\n");				
				}

			break;
			case DIK_A: 
				if (upDown & 0x80)
				{
					g_cObjectManager.stopPlayerX();
					g_cObjectManager.stopPlayerZ();
					g_cObjectManager.Action(PLAYER_ATTACK, 0, 0); 
					//DEBUGPRINTF("Input:: DIK_A pressed\n");				
				}
				else
				{
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_A released\n");				
				}
				//DEBUGPRINTF("Input:: ATTACKING\n");
				break;

			case DIK_Z: 
				if (upDown & 0x80)
				{
					g_cObjectManager.stopPlayerX();
					g_cObjectManager.stopPlayerZ();
					g_cObjectManager.playerDucking(true);
					g_cObjectManager.Action(PLAYER_DUCK, 0, 0); 
					//DEBUGPRINTF("Input:: DIK_Z pressed\n");				
				}
				else
				{
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_Z released\n");				
				}
				//DEBUGPRINTF("Input:: DUCKING\n");
				break;

			case DIK_Q: 
				if (upDown & 0x80)
				{
					g_cObjectManager.Action(PLAYER_JUMP, 0, 0); 
					//DEBUGPRINTF("Input:: DIK_Q pressed\n");				
				}
				else	
				{
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_Q released\n");				
				}
				//DEBUGPRINTF("Input:: JUMPING\n");
				break;

			case DIK_S: 
				if (upDown & 0x80)
				{
					g_cObjectManager.stopPlayerX();
					g_cObjectManager.stopPlayerZ();
					g_cObjectManager.playerDefending(true);
					g_cObjectManager.Action(PLAYER_DEFEND, 0, 0); 

					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_X: 
				if (upDown & 0x80)
				{
					g_cObjectManager.playerRunning();
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;
			case DIK_5: 
				if (upDown & 0x80)
				{
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(30, GLOW_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_6: 
				if (upDown & 0x80)
				{
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(30, ROTATE_VERTICAL_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_7: 
				if (upDown & 0x80)
				{
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(30, ROTATE_HORIZONTAL_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_8: 
				if (upDown & 0x80)
				{
					int pickColor = g_cRandom.number(0, 8);
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(30, SPARK_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_9: 
				if (upDown & 0x80)
				{
					int pickColor = g_cRandom.number(0, 8);
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(30, FIRE_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;

			case DIK_0: 
				if (upDown & 0x80)
				{
					int pickColor = g_cRandom.number(0, 8);
					float redColor, greenColor, blueColor;
					redColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					greenColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
					blueColor = (float)g_cRandom.number(0, 1000) / 1000.0f;
                    g_cObjectManager.generateParticles(1, SMOKE_TYPE, COLOR_PARTICLE, 100, 0, redColor, greenColor, blueColor, 1.0f);
					break;
					
					//DEBUGPRINTF("Input:: DIK_S pressed\n");				
				}
				else
				{
					//g_cObjectManager.playerWalking();
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: DIK_S released\n");				
				}
				//DEBUGPRINTF("Input:: DEFENDING\n");
				break;
			case DIK_1: 
				if (upDown & 0x80)
				{
					g_cObjectManager.inventoryBackward();
					//DEBUGPRINTF("Input:: DIK_O pressed\n");				
					break;
				}
				else
				{
					//DEBUGPRINTF("Input:: DIK_O released\n");				
				}
				break;
			case DIK_2: 
				if (upDown & 0x80)
				{
					g_cObjectManager.inventoryForward();
					//DEBUGPRINTF("Input:: DIK_P pressed\n");				
					break;
				}
				else
				{
					//DEBUGPRINTF("Input:: DIK_P released\n");				
				}
				break;

			case DIK_SPACE: 
				if (upDown & 0x80)
				{
					g_cObjectManager.useInventoryObject();
					//DEBUGPRINTF("Input:: DIK_P pressed\n");				
					break;
				}
				else
				{
					//DEBUGPRINTF("Input:: DIK_P released\n");				
				}
				break;

			case DIK_W: 
				if (upDown & 0x80)
				{
					g_cObjectManager.CriticalKeyHit();
					//DEBUGPRINTF("Input:: DIK_W pressed\n");				
				}
				break;

			case DIK_K: 
				if (upDown & 0x80)
				{
					g_cObjectManager.killBoss();
					//DEBUGPRINTF("Input:: DIK_K pressed\n");				
				}
				break;

			default: 
				if (upDown & 0x80)
				{
					//g_cObjectManager.stopPlayerX();
					//g_cObjectManager.stopPlayerZ();
					//g_cObjectManager.Action(PLAYER_WAIT, 0, 0); 
					//DEBUGPRINTF("Input:: random key pressed\n");				
				}
				else
				{
					//g_cObjectManager.stopPlayerX();
					//DEBUGPRINTF("Input:: random key released\n");				
				}
		//default:
		//	g_cObjectManager.Action(PLAYER_WAIT, 0, 0); 
		} // end switch keystroke
	} // end if controller is keyboard
} //GameKeyboardHandler

//mouse processing

/// Process mouse input.
/// Processes mouse events that have been buffered since the last
/// time this function was called.
/// \return TRUE if we still have control of the mouse

BOOL CInputManager::ProcessMouseInput(){ //process buffered mouse events

  BOOL result=FALSE; //TRUE if game is over
  if(!m_pMouse)return FALSE; //no device so fail and bail
  DWORD dwElements=DI_BUFSIZE; //number of events to process

  //get number of events to process into member buffer m_DIod
  HRESULT hr=m_pMouse->GetDeviceData(
    sizeof(DIDEVICEOBJECTDATA), m_DIod,&dwElements, 0);

  //if it failed, we may be able to acquire the device
  if(FAILED(hr)){ //lost the device somehow
    hr=m_pMouse->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pMouse->Acquire(); //try real hard
    return FALSE; //live to fight another day
  }
  if(FAILED(hr))return FALSE; //fail and bail (paranoia)

  //process inputs from m_DIod now
  for(int i=0; i<(int)dwElements; i++){ //for each input
    switch(m_DIod[i].dwOfs){
      case DIMOFS_BUTTON0: //left mouse button
        if(m_DIod[i].dwData & 0x80)
			LMouseDown(); //down
        else result=LMouseUp(); //up
        break;
      case DIMOFS_X: MouseMove(m_DIod[i].dwData, 0); break;
      case DIMOFS_Y: MouseMove(0, m_DIod[i].dwData); break;
    }
  }
  return result;
}

//mouse handlers

/// Set the plane speed from the mouse position.
/// The mouse acts like a joystick, but upside down.
/// \param point Mouse position
/// \param extent Extent of mouse position, meaning limits on how far the mouse can move

void CInputManager::SetPlaneSpeed(POINT point, SIZE extent){

  int xmin, xmax, ymin, ymax; //plane speed limits
  //g_cObjectManager.GetSpeedLimits(xmin, xmax, ymin, ymax); //get them

  //bands for speed assignment
  const int XBANDWIDTH=extent.cx/(xmax-xmin+1);
  const int YBANDWIDTH=extent.cy/(ymax-ymin+1);

  int xspeed = point.x/XBANDWIDTH+xmin; //horizontal speed
  int yspeed = point.y/YBANDWIDTH+ymin; //vertical speed
  //g_cObjectManager.SetSpeed(xspeed, yspeed); //pass to plane
}

/// Handle mouse motion.
/// \param x Mouse x position
/// \param y Mouse y position

void CInputManager::MouseMove(int x, int y){

  //process mouse move
  m_pointMouse.x += x; m_pointMouse.y += y;
  g_d3ddevice->SetCursorPosition(m_pointMouse.x, m_pointMouse.y, 0);

  //mouse as joystick
  if(g_nCurrentGameState!=PLAYING_GAMESTATE)return; //bail if not playing
  if(m_nInputDevice!=MOUSE_INPUT)return; //bail if mouse not selected
  //set extent
  SIZE extent; //extent that mouse moves in
  extent.cx = g_nScreenWidth;
  extent.cy = g_nScreenHeight; 
  //set plane speed based on point and extent
  SetPlaneSpeed(m_pointMouse,extent); 
}

/// Main menu left mouse button down handler.

void CInputManager::MainMenuLMouseDown(){
  if(m_pButtonManager->hit(m_pointMouse) >= 0) //if a valid hit
    m_pButtonManager->buttondown(m_pointMouse); //animate a button down
}

/// Mouse left button down handler for device menu.

void CInputManager::DeviceMenuLMouseDown(){

  int hit = m_pButtonManager->hit(m_pointMouse); //which button is hit by mouse

  //fake mouse action by making keyboard handler so all the work
  switch(hit){
    case 0: DeviceMenuKeyboardHandler(DIK_K); break; //keyboard
    case 1: DeviceMenuKeyboardHandler(DIK_J); break; //mouse
    case 2:
      m_pButtonManager->buttondown(hit); //quit button down
      break;
    default: break;
  }
}

/// Game left mouse button down handler.

void CInputManager::GameLMouseDown(){
  if(m_nInputDevice != MOUSE_INPUT)return; //bail if mouse not selected
  GameKeyboardHandler(DIK_A, TRUE); //let keyboard manager handle it
}

/// Main mouse left button down handler.

void CInputManager::LMouseDown(){
  switch(g_nCurrentGameState){
    case MENU_GAMESTATE: MainMenuLMouseDown(); break;
    case PLAYING_GAMESTATE: GameLMouseDown(); break;
    case DEVICEMENU_GAMESTATE: DeviceMenuLMouseDown(); break;
  }
}

//mouse left button up handlers

/// Intro left mouse button up handler.

void CInputManager::IntroLMouseUp(){
  g_bEndGameState=TRUE;
}

/// Main menu left mouse button up handler.
/// \return TRUE if player clicked on the Quit button

BOOL CInputManager::MainMenuLMouseUp(){ 

  int hit = m_pButtonManager->release(m_pointMouse); //get button hit
  BOOL result=FALSE; //return result

  //take action depending on which button was hit
  switch(hit){ //depending on which button was hit
    case 0: MenuKeyboardHandler(DIK_N); break; //new game
    //case 1: MenuKeyboardHandler(DIK_S); break; //saved game
    case 1: MenuKeyboardHandler(DIK_D); break; //devices
    //case 3: MenuKeyboardHandler(DIK_L); break; //high score list
    case 2: MenuKeyboardHandler(DIK_H); break; //help
    case 3: result = MenuKeyboardHandler(DIK_Q); break; //quit
    default: break;
  }

  //animate button images
  if(hit>=0)m_pButtonManager->buttonup(hit); //hit
  else m_pButtonManager->allbuttonsup(); //nonhit

  return result;
}

/// Mouse left button up handler for device menu.

void CInputManager::DeviceMenuLMouseUp(){
  int hit = m_pButtonManager->release(m_pointMouse);
  switch(hit){
    case 2: //quit button up
      m_pButtonManager->buttonup(hit); //show quit button up
      DeviceMenuKeyboardHandler(DIK_ESCAPE);
      break;
  }
  if(hit<0)m_pButtonManager->allbuttonsup(); //no hit
}

/// Main mouse left button up handler
/// return TRUE if player wants to exit the main menu

BOOL CInputManager::LMouseUp(){ 

  BOOL result = FALSE; //return result

  switch(g_nCurrentGameState){
    case LOGO_GAMESTATE:
    case TITLE_GAMESTATE: 
    case CREDITS_GAMESTATE: IntroLMouseUp(); break;
    case MENU_GAMESTATE: result = MainMenuLMouseUp(); break;
    case DEVICEMENU_GAMESTATE: DeviceMenuLMouseUp(); break;
  }

  return result;
}

/// Set up button manager.
/// \param count Number of buttons.
/// \param x Horizontal coordinate of first button
/// \param y Vertical coordinate of first button
/// \param ydelta Vertical spacing between buttons

void CInputManager::SetupMenuButtons(int count, int x, int y, int ydelta){ 

  SIZE size; //size of buttons
  size.cx = 40; size.cy = 40; //size of buttons

  POINT point; //top left corner of first button
  point.x=x; point.y=y; //first button location

  delete m_pButtonManager; //delete any old one
  m_pButtonManager=new //create new button manager
    CButtonManager(count, size, point, ydelta);

  //set button sounds
  m_pButtonManager->setsounds(BIGCLICK_SOUND,SMALLCLICK_SOUND);
}

/// Set up main menu buttons.
/// Button positions are hard-coded.

void CInputManager::SetupMainMenuButtons(){ 
//set up main menu button manager
  SetupMenuButtons(4, 300, 260, 78);
}

/// Set up device menu buttons.
/// Button positions are hard-coded.

void CInputManager::SetupDeviceMenuButtons(){ 
  SetupMenuButtons(3, 300, 295, 85);
  //initialize image of radio button
  m_pButtonManager->SetRadio(); //radio buttons
  m_pButtonManager->buttondown(m_nInputDevice, FALSE); //no sound
}

/// Set up menu buttons for the appropriate game state.
/// \param state Current game state.

void CInputManager::SetupButtons(GameStateType state){
//set up menu buttons
  switch(state){
    case MENU_GAMESTATE: SetupMainMenuButtons(); break;
    case DEVICEMENU_GAMESTATE: SetupDeviceMenuButtons(); break;
  }
}

/// Draw mouse buttons.

void CInputManager::DrawButtons(){ //draw buttons
  m_pButtonManager->DrawButtons();
}

/// Change game state.
/// The mouse behaves differently in different game states.

void CInputManager::ChangeState(GameStateType state){ //change game state

  switch(g_nCurrentGameState){

    case MENU_GAMESTATE:
      m_pointMouse.x = g_nScreenWidth/2; 
      m_pointMouse.y = g_nScreenHeight/2;
      g_d3ddevice->SetCursorPosition(m_pointMouse.x, m_pointMouse.y, 0);

      //show mouse cursor if not already showing
      if(!g_bCursorShowing){
        g_bCursorShowing = TRUE;
        g_d3ddevice->ShowCursor(TRUE); //show d3d cursor
      }
      break;

    case PLAYING_GAMESTATE:
      m_pointMouse.x = g_nScreenWidth; 
      m_pointMouse.y = g_nScreenHeight/2; //for initial plane speed
          
      //hide mouse cursor if not already hidden
      if(g_bCursorShowing){
        g_bCursorShowing = FALSE;
        g_d3ddevice->ShowCursor(FALSE); //hide d3d cursor
      }
      break;
  }
}

/// Process joystick input.
/// Poll the joystick. Try to reacquire it if its been lost.
/// Process button 0 as if it were the space bar. Process
/// plane speed from the slider (horizontal) and the y axis
/// (vertical). If there's no slider, use the x axis, which
/// works best for the gamepad and is weird to play but works for
/// a regular joystick.

BOOL CInputManager::ProcessJoystickInput() //process polled joystick events
{
	HRESULT hr; //result
	DIJOYSTATE2 js; //joystick state 

	if(g_nCurrentGameState != PLAYING_GAMESTATE) //bail if not in game play
		return FALSE; 
	if(m_nInputDevice != JOYSTICK_INPUT) //bail if joystick not selected
		return FALSE;
	if(g_pJoystick == NULL) //bail and fail
		return FALSE;

	//poll the device to read the current state
	hr = m_pJoystick->Poll(); 
	if(FAILED(hr)) //something bad has happened
	{
		hr = g_pJoystick->Acquire(); //try to reacquire
		while(hr == DIERR_INPUTLOST)
		{
			hr = g_pJoystick->Acquire(); //try real hard
		} // end while
    //if something *very* bad has happened, pretend it hasn't 
    return FALSE; 
	} // end if failed

	//get the input's device state
	//hr = g_pJoystick->GetDeviceData(
	hr = g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js);
	if(FAILED(hr)) //should have been reacquired during the Poll()
		return FALSE;


	//DEBUGPRINTF("Input:: lX = %f, lY = %f\n", (double)js.lX, (double)js.lY);

	// fake keyboard keys

	bool buttonPush = false;

	if(js.rgbButtons[5] & 0x80) // button 5 is down
	{
		g_cObjectManager.playerRunning();
		//DEBUGPRINTF("Input:: Joystick Button 5 pressed\n");	
	}
	else // button 5 is up
	{
		g_cObjectManager.playerWalking();
		//DEBUGPRINTF("Input:: Joystick Button 5 released\n");	
	}

	if(js.rgbButtons[0] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.stopPlayerX();
		g_cObjectManager.stopPlayerZ();
		g_cObjectManager.Action(PLAYER_ATTACK, 0, 0, true); 
		//DEBUGPRINTF("Input:: Joystick Button 0 pressed\n");	
	}

	if(js.rgbButtons[3] & 0x80)
	{
		//buttonPush = true;
		g_cObjectManager.Action(PLAYER_JUMP, 0, 0, true); 
		//DEBUGPRINTF("Input:: Joystick Button 1 pressed\n");	
	}

	if(js.rgbButtons[1] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.stopPlayerX();
		g_cObjectManager.stopPlayerZ();
		g_cObjectManager.playerDucking(true);
		g_cObjectManager.Action(PLAYER_DUCK, 0, 0, true); 
		//DEBUGPRINTF("Input:: Joystick Button 2 pressed\n");	
	}
	
	if(js.rgbButtons[2] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.stopPlayerX();
		g_cObjectManager.stopPlayerZ();
		g_cObjectManager.playerDefending(true);
		g_cObjectManager.Action(PLAYER_DEFEND, 0, 0, true); 
		//DEBUGPRINTF("Input:: Joystick Button 3 pressed\n");	
	}

	if(js.rgbButtons[4] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.CriticalKeyHit();
		//DEBUGPRINTF("Input:: Joystick Button 4 pressed\n");	

	}

	if(js.rgbButtons[6] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.inventoryForward();
		//DEBUGPRINTF("Input:: Joystick Button 5 pressed\n");	

	}

	if(js.rgbButtons[7] & 0x80)
	{
		buttonPush = true;
		g_cObjectManager.useInventoryObject();
		//DEBUGPRINTF("Input:: Joystick Button 6 pressed\n");	

	}
	
	if (!g_cObjectManager.playerBusy() )
	{
		if ( ( (js.lY > 45.0) && (js.lY < 55.0) ) && ( (js.lX > 45.0) && (js.lX < 55.0) ) )
		{
			g_cObjectManager.stopPlayerX();
			g_cObjectManager.stopPlayerZ();
			//if (!g_cObjectManager.playerBusy())
			//{
			//	g_cObjectManager.Action(PLAYER_WAIT, 0, 0, true); 
			//}
		}
		else
		{
			if ( (js.lY < 55.0) && (js.lY > 45.0) )
			{
				g_cObjectManager.stopPlayerZ();
				//DEBUGPRINTF("Input:: Joystick Y axis released\n");	
			}
			else
			{

				if (js.lY > 55.0)
				{
					g_cObjectManager.Action(PLAYER_WALK, 0, -1, true);
					//DEBUGPRINTF("Input:: Joystick Down Pressed\n");		
				}
				if (js.lY < 45.0)
				{
					g_cObjectManager.Action(PLAYER_WALK, 0, 1, true); 
					//DEBUGPRINTF("Input:: Joystick Up pressed\n");			
				}
			} // end if y-axis used
	
			if ( (js.lX < 55.0) && (js.lX > 45.0) )
			{
				g_cObjectManager.stopPlayerX();
				//DEBUGPRINTF("Input:: Joystick X axis released\n");	
			}
			else
			{	
	
				if (js.lX < 45.0)
				{
					g_cObjectManager.Action(PLAYER_WALK, -3, 0, true); 
					//DEBUGPRINTF("Input:: Joystick Left pressed\n");	
				}
				if (js.lX > 55.0)
				{
					g_cObjectManager.Action(PLAYER_WALK, 3, 0, true); 
					//DEBUGPRINTF("Input:: Joystick Right pressed\n");	
				}
			} // end if x-axis used
		} // end else
	} // end if not button pushed

	//player movement from slider 0 and y axis
	if(g_nCurrentGameState != PLAYING_GAMESTATE)
		return TRUE; //bail if not playing

	return TRUE;
} // end CInputManager::ProcessJoystickInput



