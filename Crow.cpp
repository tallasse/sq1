/// \file crow.cpp
/// Code for the crow intelligent object class CCrowObject.
/// Copyright Ian Parberry, 2004.
/// Last updated August 18, 2004.

#include "crow.h"
#include "timer.h" //game timer
#include "random.h" //for random number generator

extern CTimer g_cTimer;  //game timer
extern CRandom g_cRandom; //random number generator

const int CLOSE_DISTANCE=300; ///< Distance for close to plane
const int FAR_DISTANCE=500; ///< Distance for "far from" plane.
const int FALLBACK_DISTANCE=150; ///< Fall back at this vertical distance from plane
const int BEHIND_DISTANCE=-5; ///< Horizontal distance considered to be behind plane

CCrowObject::CCrowObject(char* name, D3DXVECTOR3 location, float xspeed, float yspeed):
CIntelligentObject(CROW_OBJECT, name, location, xspeed, yspeed){ //constructor
  m_nDesiredHeight = 240; 
  m_nHeightTime = 0;  m_nHeightDelayTime = 0;
  m_nSpeedVariationTime = m_nSpeedVariationDuration = 0;
  m_fDistance = m_fHorizontalDistance = m_fVerticalDistance = 0.0f;
  m_eState = CRUISING_STATE;
  m_nLastAiTime = 0; m_nAiDelayTime = 0;
}

/// Intelligent move function.
/// Basically, just move like a dumb object then modify your private variables "intelligently".

void CCrowObject::move(){ //move object
  CGameObject::move(); //move like a dumb object
  ai(); //act intelligently
}

/// Main crow AI function.
/// The real work is done by a function for each state. Call the appropriate
/// function for the current state periodically, based on a timer.

void CCrowObject::ai(){ //main AI function
  //do the following periodically
  if(g_cTimer.elapsed(m_nLastAiTime,m_nAiDelayTime)) 
    switch(m_eState){ //behavior depends on state
      case CRUISING_STATE: CruisingAi(); break;
      case AVOIDING_STATE: AvoidingAi(); break;
      default: break;
    }
}

/// Set the current state.
/// Change the current state, taking whatever actions are necessary on entering the new state.
/// \param state new state

void CCrowObject::SetState(StateType state){

  m_eState=state; //change state

  switch(m_eState){ //change behavior settings

    case CRUISING_STATE:
      m_nAiDelayTime=400+g_cRandom.number(0,200);
      m_fXspeed=(float)-g_cRandom.number(1,4);
      m_nHeightDelayTime=8000+g_cRandom.number(0,5000);
      break;

    case AVOIDING_STATE:
      m_nAiDelayTime=250+g_cRandom.number(0,250); 
      m_fXspeed=-3;
      m_nDesiredHeight=g_cRandom.number(100,400);
      m_nHeightDelayTime=3000+g_cRandom.number(0,2000);
      m_nSpeedVariationDuration=5000+g_cRandom.number(0,2000);
      break;

    default: break;
  }
}

/// AI for crows in the cruising state.
/// These crows are just cruising along, periodically lokoing for the plane.

void CCrowObject::CruisingAi(){ //just cruising along

  //height variation
  if(m_nDesiredHeight > (int)m_structLocation.y+20)
    m_fYspeed = -(float)g_cRandom.number(1,4);
  else if(m_nDesiredHeight < m_structLocation.y-20)
    m_fYspeed=(float)g_cRandom.number(1,4);
  else m_fYspeed=0.0f;
  if(g_cTimer.elapsed(m_nHeightTime, m_nHeightDelayTime)){
    m_nDesiredHeight = g_cRandom.number(150,400);
    m_nHeightDelayTime = 15000+g_cRandom.number(0,5000);
  }

  //look for plane
  if(m_fDistance < CLOSE_DISTANCE&&
  m_fHorizontalDistance < BEHIND_DISTANCE)
    SetState(AVOIDING_STATE);
}

/// AI for crows in the avoinding state.
/// These crows are trying to get away from the plane.

void CCrowObject::AvoidingAi(){ //avoiding plane

  //height variation
  if(g_cTimer.elapsed(m_nHeightTime, m_nHeightDelayTime)){
    m_nDesiredHeight = g_cRandom.number(100,450);
    if(m_nDesiredHeight < m_structLocation.y)
      m_fYspeed= -(float)g_cRandom.number(3,7);
    if(m_nDesiredHeight > m_structLocation.y)
      m_fYspeed = (float)g_cRandom.number(3,7);
    m_nHeightDelayTime = 3000+g_cRandom.number(0,2000);
  }

  //speed variation
  if(g_cTimer.elapsed(m_nSpeedVariationTime, m_nSpeedVariationDuration)){
      m_fXspeed = (float)-g_cRandom.number(7,10); 
      m_nSpeedVariationDuration = 10000+g_cRandom.number(0,3000);
  }
  if(m_fHorizontalDistance > BEHIND_DISTANCE) //if behind
    m_fXspeed = -1; //slow down 

  //look for plane, maybe leave avoiding state
  if(m_fDistance>FAR_DISTANCE|| //if far away, or
    (m_fDistance<CLOSE_DISTANCE&& //close and
      m_fVerticalDistance>FALLBACK_DISTANCE)) //higher or lower
    SetState(CRUISING_STATE); //then back to cruising

}
