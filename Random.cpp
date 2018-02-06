/// \file random.cpp
/// Code for the random number generator class CRandom.
/// Copyright Ian Parberry, 2004.
/// Last updated August 18, 2004.

//Cryptographically secure random number generator
//modified from the book "Writing Secure Code" by
//Michael Howard and David LeBlanc, Microsoft Press, 2003.

//This is a FIPS 140-1 compliant algorithm.

//This way cool random number generator automatically reseeds itself
//from a variety of sources, including time of day.

//Copyright Ian Parberry, 2004
//Last updated February 18, 2004

#include "Random.h"

CRandom::CRandom(){ //constructor
  m_hProv=NULL; //default
  CryptAcquireContext(&m_hProv, NULL, NULL,
    PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
}

CRandom::~CRandom(){ //destructor
  if(m_hProv)CryptReleaseContext(m_hProv, 0); //safe release
}

/// Get a bunch of random bytes.
/// \param lpGoop pointer to place to put random bytes
/// \param cbGoop number of random bytes to generate

BOOL CRandom::get(void* lpGoop,DWORD cbGoop){
  if(!m_hProv)return FALSE;
  return CryptGenRandom(m_hProv,cbGoop,
    reinterpret_cast<LPBYTE>(lpGoop));
}

/// Generate a random number in a given range.
/// \param i lower limit
/// \param j upper limit
/// \return random int >= i, and <=j

int CRandom::number(int i,int j){  
  //return random number in  range i..j
  DWORD r; //random bytes
  get(&r,sizeof r); //get some random bytes into r
  return j>i ? r%(j-i+1)+i: r%(i-j+1)+j; //compute result
}

