/// \file sndlist.h
/// Enumerated types for sounds.
/// Copyright Ian Parberry, 2004
/// Last updated October 1, 2004

#pragma once

enum GameSoundType //sounds used in game engine
{

	ARROW_SOUND, //sound an arrow makes
	BIGCLICK_SOUND, //loud click
	BOMB_SOUND, //sound of explosion
	DEATH1_SOUND, // death sound 1
	DEATH2_SOUND, // death sound 2
	DEATH3_SOUND, // death sound 3
	DEATH4_SOUND, // death sound 3
	DEFEND_SOUND, // defend sound
	HURT1_SOUND, // injury sound 1
	HURT2_SOUND, // injury sound 2
	HURT3_SOUND, // injury sound 3
	SMALLCLICK_SOUND, //soft click
	SWING1_SOUND, // sword swing sound 1
	SWING2_SOUND, // sword swing sound 1
	SWING3_SOUND, // sword swing sound 1
	SWORDHIT1_SOUND, // sword hit sound 1
	SWORDHIT2_SOUND, // sword hit sound 1
	CRUNCH_SOUND, //sound of object breaking
	FIREBALL_SOUND, //FIRE!
	SPLAT_SOUND, // splat
	MAJOUSCREAM_SOUND, // majou screams, dies
	MAJOUHIT_SOUND, // majou damaged
	SHATTER_SOUND, // glass shatters
	HEAL_SOUND, //healy chime thing
	WIND_SOUND, // wind sound
	WIND2_SOUND, // more wind blowing
	INSECT_SOUND, // bug sound
	FROG_SOUND, // frog sound
	BIRD1_SOUND, // bird sound
	BIRD2_SOUND, // bird sound
	BIRD3_SOUND, // bird sound
	BIRD4_SOUND, // bird sound
	BIRD5_SOUND, // bird sound
	OWL_SOUND, // owl sound
	CRICKETLOOP1_SOUND, //cricket sound
	CRICKETLOOP2_SOUND, //cricket sound
	CRICKETLOOP3_SOUND, //cricket sound
	WINDLOOP_SOUND, // daytime wind sound
	RAINLOOP_SOUND, // raining sound
	LIGHTNING1_SOUND, // lightning
	LIGHTNING2_SOUND, // lightning
	LIGHTNING3_SOUND, // lightning
	WAVELOOP_SOUND, // sea waves
	SEAGULL1_SOUND, // seagull maybe?
	SEAGULL2_SOUND, // seagull again
	CICADA1_SOUND, // japanese night time constant
	CICADA2_SOUND, // more of those bugs
	CITY1_SOUND, // hammery noises
	CITY2_SOUND, // hammery noises
	STEPS_SOUND, // more city stuff
	HORSE1_SOUND, // horse sound
	HORSE2_SOUND, // another one
	DOG_SOUND, // dog barking
	HORN1_SOUND, // japanese horn
	HORN2_SOUND, // japanese horn

	DRAGONSCREAM_SOUND, // dragon screaming
	DRAGONDIE_SOUND, // dragon dying
	ONMYOUHIT_SOUND, // onmyou getting hit
	ONMYOUDIE_SOUND, // onmyou dying
	DAIMYODIE_SOUND, // daimyo dying
	SPARKS_SOUND, // electric sparks
	CLANG_SOUND, // clanging noise
	RUSTLE_SOUND, // leaves
	// ambient music 

	SHAKU1_SOUND, // shakuhachi
	SHAKU2_SOUND, // shakuhachi
	SHAKU3_SOUND, // shakuhachi
	SHAKU4_SOUND, // shakuhachi
	SHAKU5_SOUND, // shakuhachi
	KOTO1_SOUND, // koto
	KOTO2_SOUND, // koto
	KOTO3_SOUND, // koto
	KOTO4_SOUND, // koto
	KOTO5_SOUND, // koto
	SHAMI1_SOUND, // shamisen
	SHAMI2_SOUND, // shamisen
	SHAMI3_SOUND, // shamisen
	SHAMI4_SOUND, // shamisen
	SHAMI5_SOUND // shamisen
};

enum IntroSoundType{ //sounds used during the intro
  TITLE_SOUND, //sound used during title screen
  LOGO_SOUND, //signature chord
  CREDITS_SOUND, //credits sound
};