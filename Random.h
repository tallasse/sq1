/// \file random.h
/// Interface for the random number generator class CRandom.
/// Copyright Ian Parberry, 2004
/// Last updated August 18, 2004

#include <windows.h>
#include <wincrypt.h>

#pragma once

/// Random number generator.
/// Cryptographically secure random number generator
/// modified from the book "Writing Secure Code" by
/// Michael Howard and David LeBlanc, Microsoft Press, 2003.
/// This is a FIPS 140-1 compliant algorithm.
/// This way cool random number generator automatically reseeds itself
/// from a variety of sources, including time of day.

class CRandom {

  private:
    BOOL get(void *lpGoop,DWORD cbGoop); ///< Get random bytes.
    HCRYPTPROV m_hProv; ///< Handle to the crypto provider

  public:
    CRandom(); ///< Constructor.
    virtual ~CRandom(); ///< Destructor
    int number(int i,int j); ///< return random number in i..j

};
