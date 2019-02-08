#include "image.h"
#include "iostream"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}

//
void Image::drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Color color) {
	float dx, dy, D, n, r;
	dx = x1 - x0;
	dy = y1 - y0;
	if (dx != 0) {
		D = dy / dx;
		n = y0 - D * x0;
		for (unsigned int x = x0; x <= x1; x++) {
			for (unsigned int y = y0; y <= y1; y++) {
				if (y == roundf(D * x + n)) {
					setPixel(x, y, color);
				}
			}
		}
	}
	//for vertical lines
	else {
		for (unsigned int y = y0; y <= y1; y++) {
			setPixel(x0, y, color);
		}
	}
}

//for drawing vertical and horizontal bigger lines
void Image::drawLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned int g, Color color) {
	float dx, dy, D, n, r;
	dx = x1 - x0;
	dy = y1 - y0;
	if (dx == 0) {
		for (unsigned int y = y0; y <= y1; y++) {
			setPixel(x0, y, color);
			for (int z = 0; z < g / 2; z++) {
				setPixel(x0 + z, y, color);
				setPixel(x0 - z, y, color);
			}
		}
	}
	else if (dy == 0){
		std::cout << "entra en la linia horitzontal" << std::endl;
		for (unsigned int x = x0; x <= x1; x++) {
			setPixel(x, y0, color);
			for (int z = 0; z < g / 2; z++) {
				setPixel(x, y0 + z, color);
				setPixel(x, y0 - z, color);
			}
		}
	}
	//for vertical lines
	else {
		D = dy / dx;
		n = y0 - D * x0;
		for (unsigned int x = x0; x <= x1; x++) {
			for (unsigned int y = y0; y <= y1; y++) {
				if (y == roundf(D * x + n)) {
					setPixel(x, y, color);
				}
			}
		}
	}
}

void Image::DDA(int x1, int y1, int x2, int y2, Color c) {
	float dist, x, y;
	float dx, dy;
	if (fabs(x2 - x1) >= fabs(y2 - y1)) {
		dist = fabs(x2 - x1);
	}
	else {
		dist = fabs(y2 - y1);
	}
	dx = (x2 - x1) / dist;
	dy = (y2 - y1) / dist;
	x = x1 + sgn(x1) * 0.5;
	y = y1 + sgn(y1) * 0.5;
	for (int i = 0; i <= dist; i++) {
		setPixel(floor(x), floor(y), c);
		x += dx;
		y += dy;
	}
}

int Image::sgn(int x)
{
	if (x > 0) { return 1; }
	else if (x < 0) { return -1; }
	else { return 0; }
}

void Image::circle(int a, int b, int r, Color c)
{
	int v, x, y;
	x = 0;
	y = r;
	v = 1 - r;
	std::vector<sCell> table;
	table.resize(this->height);
	for (int i = 0; i < table.size(); i ++) {
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}

	if (isIn(x + a, y + b)) {setPixel(x + a, y + b, c); }
	while (y > x) {
		if (v < 0) {
			v = v + 2 * x + 3;
			x++;
		}
		else {
			v = v + 2 * (x - y) + 5;
			x++;
			y--;
		}
		if (isIn(a + x, b + y)) { setPixel(a + x, b + y, c); }
		if (isIn(a - x, b + y)) { setPixel(a - x, b + y, c); }
		if (isIn(a - x, b - y)) { setPixel(a - x, b - y, c); }
		if (isIn(a + x, b - y)) { setPixel(a + x, b - y, c); }
		if (isIn(a + y, b + x)) { setPixel(a + y, b + x, c); }
		if (isIn(a - y, b + x)) { setPixel(a - y, b + x, c); }
		if (isIn(a - y, b - x)) { setPixel(a - y, b - x, c); }
		if (isIn(a + y, b - x)) { setPixel(a + y, b - x, c); }
	}
}

bool Image::isIn(int x, int y) {
	if ((x > 0 && x < 800) && (y < 500 && y > 0) ) {
		return true;
	}
	return false;
}

void Image::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c, bool fill)
{
	DDA(x0, y0, x1, y1, c);
	DDA(x1, y1, x2, y2, c);
	DDA(x2, y2, x0, y0, c);
}

std::vector<sCell> Image::DDAWithTable(int x1, int y1, int x2, int y2, std::vector<sCell> table)
{
	float dist, x, y;
	float dx, dy;
	if (fabs(x2 - x1) >= fabs(y2 - y1))
	{
		dist = fabs(x2 - x1);
	}
	else
	{
		dist = fabs(y2 - y1);
	}
	dx = (x2 - x1) / dist;
	dy = (y2 - y1) / dist;
	x = x1 + sgn(x1)*0.5;
	y = y1 + sgn(x1)*0.5;
	for (int i = 0; i <= dist; i++)
	{
		if (x > table[y].maxx)
		{
			table[y].maxx = x;
		}
		if (x < table[y].minx)
		{
			table[y].minx = x;
		}
		//printf("%d, %d \n", table[y].minx, table[y].maxx);
		x = x + dx;
		y = y + dy;
	}
	return table;
}

void Image::drawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c)
{
	std::vector<sCell> table;
	table.resize(this->height);
	//inicialize table
	for (int i = 0; i < table.size(); i++)
	{
		table[i].minx = 100000;
		table[i].maxx = -100000;
	}
	table = rasterLines(x0, y0, x1, y1, x2, y2, table);
	fillTriangle(table, c);
	//printTable(table);
	//printf("Table size: %d", table.size());
}
std::vector<sCell> Image::rasterLines(int x0, int y0, int x1, int y1, int x2, int y2, std::vector<sCell> table)
{
	std::vector<sCell> aux;
	aux = DDAWithTable(x0, y0, x1, y1, table);
	aux = DDAWithTable(x1, y1, x2, y2, aux);
	aux = DDAWithTable(x2, y2, x0, y0, aux);
	return aux;
}

void Image::fillTriangle(std::vector<sCell> table, Color c)
{
	for (int i = 0; i < table.size(); i++)
	{
		if (table[i].maxx > table[i].minx)
		{
			for (int j = table[i].minx; j < table[i].maxx; j++)
			{
				setPixel(j, i, c);
			}
		}
	}
}


void Image::Bresenham(int x0, int y0, int x1, int y1, Color color) {
	int dx, dy, inc_E, inc_NE, d, x, y;
	int i;
	dx = x1 - x0;
	dy = y1 - y0;
	inc_E = 2 * dy;
	inc_NE = 2 * (dy - dx);
	d = 2 * dy - dx;
	x = x0;
	y = y0;
	setPixel(x, y, color);
	while (x < x1) {
		if (d <= 0) {
			d = d + inc_E;
			x += 1;
		}
		else {
			d = d + inc_NE;
			x += 1;
			y += 1;
		}
		setPixel(x, y, color);
	}
}

//draw a sqare of size z at (x,y)
void Image::drawSquare(unsigned int x, unsigned int y, unsigned int z, Color color, bool contorn) {
	for (int posx = x; posx < x + z; posx++) {
		for (int posy = y; posy < y + z; posy++) {
			if (posx == x || posx == x + z - 1|| posy == y || posy == y + z - 1) {
				if (contorn) { setPixel(posx, posy, Color(0, 0, 0)); }
			}
			else {
				setPixel(posx, posy, color);
			}
		}
	}
}

void Image::drawEmptySquare(unsigned int x, unsigned int y, unsigned int z, Color color) {
	for (int posx = x; posx < x + z; posx++) {
		for (int posy = y; posy < y + z; posy++) {
			if (posx == x || posx == x + z - 1 || posy == y || posy == y + z - 1) {
				setPixel(posx, posy, color);
			}
		}
	}
}

void Image::printCircle(unsigned int x, unsigned int y, unsigned int r, Color color)
{
	for (double a = 0; a < 2 * PI; a += 0.0001)
	{
		double x0, y0;
		x0 = x + r * cos(a);
		y0 = y + r * sin(a);
		setPixel(x0, y0, color);
	}
}


void Image::printAllCircle(unsigned int x, unsigned int y, unsigned int r, Color color) {
	for (int a = 0; a < r; a++) {
		this->circle(x, y, a, color);
	}
}


void Image::printEllipse(unsigned int x, unsigned int y, unsigned int a, unsigned int b, Color color)
{
	for (double o = 0; o < 2 * PI; o += 0.0000001)
	{
		double x0, y0;
		x0 = x + a * cos(o);
		y0 = y + b * sin(o);
		setPixel(x0, y0, color);
	}
}

//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}

#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif