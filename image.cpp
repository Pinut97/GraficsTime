#include "image.h"


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

int Image::sgn(int x)
{
	if (x > 0) { return 1; }
	else if (x < 0) { return -1; }
	else { return 0; }
}

bool Image::isIn(int x, int y) {
	if ((x > 0 && x < 800) && (y < 500 && y > 0)) {
		return true;
	}
	return false;
}

//draw functions
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

void Image::circle(int a, int b, int r, Color c, bool filled)
{
	int v, x, y;
	x = 0;
	y = r;
	v = 1 - r;
	if (isIn(x + a, y + b)) { setPixel(x + a, y + b, c); }
	if (filled) { DDA(a - r, b, a + r, b, c); }
	while (y >= x) {
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
		if (filled)
		{
			DDA(a - y, b + x, a + y, b + x, c);
			DDA(a - x, b + y, a + x, b + y, c);
			DDA(a - x, b - y, a + x, b - y, c);
			DDA(a - y, b - x, a + y, b - x, c);
		}
	}
}

void Image::drawUnfilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c)
{
	DDA(x0, y0, x1, y1, c);
	DDA(x1, y1, x2, y2, c);
	DDA(x2, y2, x0, y0, c);
}

std::vector<sCell> Image::DDAWithTable(int x1, int y1, int x2, int y2, std::vector<sCell> &table)
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

void Image::drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c, bool filled)
{
	if (filled) 
	{
		std::vector<sCell> table;
		table.resize(this->height);
		//inicialize table
		for (int i = 0; i < table.size(); i++)
		{
			table[i].minx = 100000;
			table[i].maxx = -100000;
		}
		rasterLines(x0, y0, x1, y1, x2, y2, table);
		fillTriangle(table, c);
	}
	else 
	{
		drawUnfilledTriangle(x0, y0, x1, y1, x2, y2, c);
	}
}

void Image::rasterLines(int x0, int y0, int x1, int y1, int x2, int y2, std::vector<sCell> &table)
{
	DDAWithTable(x0, y0, x1, y1, table);
	DDAWithTable(x1, y1, x2, y2, table);
	DDAWithTable(x2, y2, x0, y0, table);
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

Color Image::getColor(int x0, int y0, int x1, int y1, int x2, int y2, int a, int b)
{
	Vector2 v0(x1 - x0, y1 - y0);
	Vector2 v1(x2 - x0, y2 - y0);
	Vector2 v2(a - x0, b - y0);

	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);
	float d20 = v2.dot(v0);
	float d21 = v2.dot(v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0 - v - w;

	//use weights to compute final color
	printf("Colorete: %f, %f, %f \n", u, v, w);
	Color c = Color((int)u, (int)v, (int)w);
	printf("Colorete juju: %f, %f, %f \n", u, v, w);
	return c;
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


FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width*height];
	memset(pixels, 0, width * height * sizeof(float));
}

//copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
}

//assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height * sizeof(float)];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}


//change image size (the old one will remain in the top-left corner)
void FloatImage::resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = getPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
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