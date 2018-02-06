/// \file helpers.cpp
/// Helper classes for game objects
/// Copyright Jeremy Smith, 2004.
/// Updated: 10/31/04

#include "helpers.h"
#include "debug.h"

sizeMatrix::sizeMatrix()
{
	theX = 0;
	theY = 0;
	theZ = 0;
}

sizeMatrix::sizeMatrix(int newX, int newY, int newZ)
{
	theX = newX;
	theY = newY;
	theZ = newZ;
}

int sizeMatrix::x()
{
	return theX;
}

int sizeMatrix::y()
{
	return theY;
}

int sizeMatrix::z()
{
	return theZ;
}

void sizeMatrix::setX(int newX)
{
	theX = newX;
}

void sizeMatrix::setY(int newY)
{
	theY = newY;
}

void sizeMatrix::setZ(int newZ)
{
	theZ = newZ;
}

