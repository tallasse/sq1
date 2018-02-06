/// \file helpers.h
/// Helper classes for game objects
/// Copyright Jeremy Smith, 2004.
/// Updated: 10/31/04

#pragma once

class sizeMatrix
{
	private:

		int theX;
		int theY;
		int theZ;

	public:
		
		sizeMatrix(); // default constructor
		sizeMatrix(int newX, int newY, int newZ); // constructor
		int x(); // returns the x length
		int y(); // returns the y length
		int z(); // returns the z length
		void setX(int newX); //sets new X
		void setY(int newY); //sets new X
		void setZ(int newZ); //sets new X

}; // end sizeMatrix class

