#include "flake.h"

Flake::flake() {
	pos.x = 0;
	pos.y = 0;
	vel.x = 0;
	vel.y = 0;
	width = 0;
	height = 0;
	pixels = NULL;
}

Flake::flake(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	this->posx = x;
	this->posy = y;
	this->velx = 1;
	this->vely = 0;
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

Flake::~Flake() {
	if (pixels)
		delete pixels;
}

void Flake::move() {
	this->posx += this->velx;
}