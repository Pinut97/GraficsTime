#pragma once
#ifndef FLAKE_H
#define FLAKE_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"

class Flake {
	public:

		unsigned int width;
		unsigned int height;
		unsigned int velx;
		unsigned int vely;
		unsigned int posx;
		unsigned int posy;

		Color* pixels;

		Flake();
		Flake(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		~Flake();

		void fill(const Color& c) { for (unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = c; }
		void move();
};

#endif // !FLAKE_H
