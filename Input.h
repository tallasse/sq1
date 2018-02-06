/// \file input.h
/// Interface for the input manager class CInputManager.
/// Copyright Ian Parberry, 2004
/// Last updated October 22, 2004

#pragma once

#define DIRECTINPUT_VERSION 0x0800 //put before #include dinput.h

#include <windows.h>
#include <dinput.h> //DirectInput

#include "defines.h" //essential defines
#include "buttons.h" //for button manager

#define DI_BUFSIZE 4 ///< Buffer size for DirectInput events from devices

/// Input manager class.
/// Manages keyboard and mouse input using DirectInput.

class CInputManager{

  private:
    LPDIRECTINPUT8 m_lpDirectInput; ///< DirectInput object.
    LPDIRECTINPUTDEVICE8 m_pKeyboard; ///< Keyboard device.
    DIDEVICEOBJECTDATA m_DIod[DI_BUFSIZE]; ///< Input device buffer.
	//DIDEVICEOBJECTDATA m_JoyDIod[DI_BUFSIZE]; ///< Input device buffer.
    LPDIRECTINPUTDEVICE8 m_pMouse; ///< Mouse device.
    HANDLE m_hMouseEvent; ///< Mouse interrupt event.
    POINT m_pointMouse; ///< Mouse point.
    BOOL g_bCursorShowing; ///< TRUE if mouse cursor is showing.
    CButtonManager *m_pButtonManager; ///< Button manager for  menu buttons.
    LPDIRECTINPUTDEVICE8 m_pJoystick; ///< Joystick device.
    InputDeviceType m_nInputDevice; ///< Current input device.
    BOOL m_bJoystickHasSlider; ///< TRUE if the joystick has a slider.
	DWORD m_nLastKeyStroke; ///< last keystroke data, used for comparing keyup/down
	DWORD m_nLastKeyPressed; ///< last key pressed, used for comparing new key

    //private helper functions
    BOOL KeyboardHandler(DWORD keystroke, DWORD upDown); ///< Keyboard handler.
    void IntroKeyboardHandler(DWORD keystroke); ///< Keyboard handler for intro.
	void displayScreenKeyboardHandler(DWORD keystroke); ///< Keyboard handler for cutscreens
    BOOL MenuKeyboardHandler(DWORD keystroke); ///< Keyboard handler for menu.
    void DeviceMenuKeyboardHandler(DWORD keystroke); ///< Keyboard handler for device menu.
    void GameKeyboardHandler(DWORD keystroke, DWORD upDown, BOOL override=FALSE); ///< Keyboard handler for game. 
    void SetPlaneSpeed(POINT point, SIZE extent); ///< Set plane speed from mouse.

    //setup functions
    BOOL InitKeyboardInput(HWND hwnd); ///< Set up keyboard input.
    BOOL InitMouseInput(HWND hwnd); ///< Set up mouse input.
    BOOL InitJoystickInput(HWND hwnd); ///< Set up joystick input
    void SetupMenuButtons(int count,int x,int y,int ydelta); ///< Set up button manager.
    void SetupMainMenuButtons(); ///< Set up main menu button manager.
    void SetupDeviceMenuButtons(); ///< Set up device menu button manager.

    //mouse handlers
    void MouseMove(int x,int y); ///< Handle mouse motion.

    //left mouse button down handlers
    void MainMenuLMouseDown(); ///< Mouse down handler for main menu.
    void DeviceMenuLMouseDown(); ///< Mouse down handler for device menu.
    void GameLMouseDown(); ///< Mouse down handler for game. 

    //left mouse button up handlers
    void IntroLMouseUp(); ///< Mouse up handler for intro.
    BOOL MainMenuLMouseUp(); ///< Mouse up handler for main menu.
    void DeviceMenuLMouseUp(); ///< Mouse up handler for device menu.
    void LMouseDown(); ///< Main mouse down handler.
    BOOL LMouseUp(); ///< Main mouse up handler.

  public:
    CInputManager(HINSTANCE hInstance, HWND hwnd); ///< Constructor.
    ~CInputManager(); ///< Destructor.
    BOOL ProcessKeyboardInput(); ///< Process buffered keyboard events.
    BOOL ProcessMouseInput(); ///< Process buffered mouse events.
    BOOL ProcessJoystickInput(); ///< Process polled joystick events.
    void SetupButtons(GameStateType phase); ///< Set up menu buttons.
    void DrawButtons(); ///< Draw main menu buttons.
    void ChangeState(GameStateType state); ///< Change game state.
};