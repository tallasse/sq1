/// \file defines.h
/// Essential defines.
/// Copyright Ian Parberry, 2004.
/// Last updated August 25, 2004.

/// Modified by Jeremy Smith
/// Last update: 10/31/04

#pragma once

#include "d3dx9.h"

/// Billboard vertex structure. 
/// Custom vertex format for representing a vanilla billboard object. Consists 
/// of position, texture coordinates, and flexible vertex format.

const int UpperZBoundary = 990;
const int LowerZBoundary = 400;
const int playerSnapBounds = 1500;
const int terrainXSize = 32;
const int terrainZSize = 32;

struct BILLBOARDVERTEX
{ 
	D3DXVECTOR3 p; ///< Position.
	float tu,tv; ///< Texture coordinates.
	static const DWORD FVF; ///< Flexible vertex format.
};

// base object types, for instancing new ones

enum baseObjectType
{
	PLAYER_TYPE, ENEMY_TYPE, OBSTACLE_TYPE, BLOOD_TYPE, BACKGROUND_TYPE, FOREGROUND_TYPE
};

enum faceDirection
{
	FACE_LEFT, FACE_RIGHT
};

/// Game object types.
/// Types of game object that can appear in the game. Note: NUM_SPRITES 
/// must be last.

enum ObjectType{

	main_stand, main_walk, main_run, main_slash, main_stab, main_cut, main_defend, main_jump, main_die, main_duck, // player
	sam_stand, sam_walk, sam_slash, spear_stand, spear_walk, spear_stab, bow_stand, bow_walk, bow_shoot, sam_die, // evil samurai
//	tiger_stand, tiger_run, tiger_slash, tiger_jump, tiger_die, // tiger
	rock, BUTTON_OBJECT, blood, arrow, mountains, fire1, fire2, fire3, grass, tree1, tree2, smalltree1, 
	smoke, particle_p, shard_p, leaf_p, rain_p,
	majou_stand, majou_attack, majou_die,
	bigrice, smallrice, cottage, burned_cottage, smallbush, castle, cave, shuriken,
	ninja_stand, ninja_walk, ninja_slash, ninja_throw, ninja_die, ninja_flip, main_hi_blow, main_lo_blow, 
	nothing_icon, armor, insidewall, lantern,
	daimyo_stand, daimyo_walk, daimyo_stab, daimyo_slash, daimyo_die, daimyo_jump,
	onmyou_stand, onmyou_cast, onmyou_attack, onmyou_die, 
	dragon_stand, dragon_attack, dragon_shoot, dragon_die,
	barrier, katana, empress, monk,
	NUM_SPRITES
};

enum aiState
{
	WAIT, CLOSE_TO_TARGET, ATTACK_MELEE, ATTACK_RANGED, ESCAPE, DIE, GET_OUT_OF_RANGE, JUMP_ABOUT
};

enum weaponType // types of weapon
{
	SWORD, ARROW, SHURIKEN, FIRE, CLAW, STAFF, SPELL, SPEAR
};

enum directionType // direction types
{
	UP, DOWN, LEFT, RIGHT, BELOW, ABOVE
};

enum factionType // which faction is the object in?
{
	PLAYER_FACTION, ENEMY_FACTION, ANIMAL_FACTION, INANIMATE_OBJECT
};

enum particleType
{
	SPARK_TYPE, SMOKE_TYPE, FIRE_TYPE, ROTATE_HORIZONTAL_TYPE, ROTATE_VERTICAL_TYPE, GLOW_TYPE, RAIN_TYPE
};

enum particleColor
{
	COLOR_PARTICLE,	SHARD_PARTICLE, LEAF_PARTICLE, SMOKE_PARTICLE, RAIN_PARTICLE
};

enum actionType // which action is the player taking?
{
	PLAYER_WALK, //PLAYER_RUN,
	PLAYER_JUMP, PLAYER_ATTACK, PLAYER_DEFEND,
	PLAYER_DUCK, PLAYER_STAY_DUCKED,
	SWITCH_WEAPON, PLAYER_WAIT, PLAYER_STALL,

	ENEMY_WALK,
	ENEMY_JUMP, ENEMY_ATTACK_SWORD, ENEMY_ATTACK_BOW, ENEMY_DEFEND, ENEMY_DUCK,
	ENEMY_SWITCH_WEAPON, ENEMY_WAIT, ENEMY_STALL,

	nothing
};


// used for lighting and texture loading
enum timeOfDay
{
	DAWN_TIME, DAY_TIME, DUSK_TIME, NIGHT_TIME
};


//used for weather settings
enum weatherType
{
	CLEAR_WEATHER, RAINING_WEATHER
};

//used for texture settings
enum groundType
{
	GRASSY_GROUND, WOOD_GROUND, STONE_GROUND, SAND_GROUND
};

//used for texture settings
enum locationType
{
	MOUNTAIN_LOCATION, SEASHORE_LOCATION, CITY_LOCATION
};


/// File name types.
/// Indexes for various image file names.
enum FileNameNameType
{
  CLEARDAY_BACKGROUND_IMAGEFILE, 
  CLOUDYDAY_BACKGROUND_IMAGEFILE, 
  CLEARSUNSET_BACKGROUND_IMAGEFILE,
  CLEARNIGHT_BACKGROUND_IMAGEFILE, 
  CLOUDYNIGHT_BACKGROUND_IMAGEFILE, 
  CLEARSEASHORE_BACKGROUND_IMAGEFILE,
  SUNSETSEASHORE_BACKGROUND_IMAGEFILE,
  CLEARNIGHTCITY_BACKGROUND_IMAGEFILE,

  GRASSY_GROUND_IMAGEFILE, 
  WOOD_GROUND_IMAGEFILE,
  STONE_GROUND_IMAGEFILE,
  SAND_GROUND_IMAGEFILE,

  LOGO_IMAGEFILE, TITLE_IMAGEFILE,
  CREDITS_IMAGEFILE, CREDITS1_IMAGEFILE, CREDITS2_IMAGEFILE, MENU_IMAGEFILE, CURSOR_IMAGEFILE, DEVICEMENU_IMAGEFILE,

  GAMESTART_IMAGEFILE,
  NEXTLEVEL1_IMAGEFILE, NEXTLEVEL2_IMAGEFILE, NEXTLEVEL3_IMAGEFILE,
  NEXTLEVEL4_IMAGEFILE, NEXTLEVEL5_IMAGEFILE, NEXTLEVEL6_IMAGEFILE,
  NEXTLEVEL7_IMAGEFILE, NEXTLEVEL8_IMAGEFILE, NEXTLEVEL9_IMAGEFILE, 
};

/// Game state types.
/// States of the game shell.

enum GameStateType{
  LOGO_GAMESTATE, TITLE_GAMESTATE, CREDITS_GAMESTATE, CREDITS1_GAMESTATE, CREDITS2_GAMESTATE, MENU_GAMESTATE, 
  PLAYING_GAMESTATE, DEVICEMENU_GAMESTATE, PAUSED_GAMESTATE, NEXTLEVEL_GAMESTATE // added paused
};

/// Input device types.
/// Which device the player is currently using to play the game.

enum InputDeviceType{
  KEYBOARD_INPUT, MOUSE_INPUT, JOYSTICK_INPUT
};