#include "application.h"
#include "utils.h"
#include "image.h"
#include "flake.h"

//Image framebuffer(800, 600);
bool clic = false;
bool pressed = false;
bool trianglePressed = false;
bool background = false;
Image framebuffer(800, 600);
Color c;
int penSize = 10; //standars size at start

//bools for diferent forms and objects (state of the stroke)
bool square = false;
bool circle = false;
bool fullCircle = true; //we want to start with the stroke, that is why it's the only one on true
bool fullSquare = false;
bool sizeCircle = false;
bool userLine = false;
bool triangle = false;

int x, y;
int xf = NULL;
int yf = NULL;
int xt, yt;//per al triangle

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//here add your init stuff
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	std::cout << "width = " << w << " height = " << h << std::endl;
	framebuffer.fill(Color::WHITE);	//set a white background canvas

	//the header of the image
	framebuffer.drawLine(0, 500, 800, 500, Color::BLACK);

	int z = 229;
	//background of the header with a changing grey color
	for (unsigned int y = 501; y < framebuffer.height; y++) {
		z -= 1;
		for (unsigned int x = 0; x < framebuffer.width; x++) {
			framebuffer.setPixel(x, y, Color(z, z, z));
		}
	}

	//color palette
	framebuffer.drawSquare(400, 520, 30, Color::BLACK, true);
	framebuffer.drawSquare(400, 560, 30, Color::GRAY, true);
	framebuffer.drawSquare(440, 520, 30, Color::RED, true);
	framebuffer.drawSquare(440, 560, 30, Color::GREEN, true);
	framebuffer.drawSquare(480, 520, 30, Color::BLUE, true);
	framebuffer.drawSquare(480, 560, 30, Color::YELLOW, true);

	//button for sizes
	//add size square
	framebuffer.drawSquare(60, 530, 30, Color::WHITE, true);	
	framebuffer.drawLine(75, 535, 75, 555, 5, Color::BLACK);
	framebuffer.drawLine(65, 545, 85, 545, 5, Color::BLACK);

	framebuffer.Bresenham(600, 470, 450, 150, Color::BLACK);
	framebuffer.circle(300, 300, 50, Color::BLACK);

	//substract size square
	framebuffer.drawSquare(100, 530, 30, Color::WHITE, true);	
	framebuffer.drawLine(105, 545, 125, 545, 5, Color::BLACK);

	//forms buttons
	//circle button
	framebuffer.drawSquare(200, 530, 30, Color::WHITE, true);
	framebuffer.printCircle(215, 545, 10, Color::BLACK);
	//square button
	framebuffer.drawSquare(240, 530, 30, Color::WHITE, true);
	framebuffer.drawSquare(245, 535, 20, Color::WHITE, true);
	//full circle button
	framebuffer.drawSquare(280, 530, 30, Color::WHITE, true);
	framebuffer.printAllCircle(295, 545, 10, Color::BLACK);
	//full sqare button
	framebuffer.drawSquare(320, 530, 30, Color::WHITE, true);
	framebuffer.drawSquare(325, 535, 20, Color::BLACK, false);
	
}

//render one frame
void Application::render(void)
{
	//Create a new Image (or we could create a global one if we want to keep the previous frame)

	//here you can add your code to fill the framebuffer
	
	//charge rubber image
	Image eraser(1,1);
	eraser.loadTGA("rubber.tga");
	if (eraser.loadTGA("rubber.tga") == false) { std::cout << "image not found" << std::endl; }
	//else { std::cout << "image found" << std::endl; }
	eraser.flipY(); //it needs to be flip

	if (mouse_position.y < 470 && clic == true) {
		if (sizeCircle && xf != 0 && yf != 0) {

			int dx = xf - x;
			int dy = yf - y;
			int radi = sqrt(pow(dx, 2) + pow(dy, 2));
			framebuffer.circle(x, y, radi, c);
			x = NULL;
			y = NULL;
			xf = NULL;
			yf = NULL;
		}

		else if (userLine && xf != 0 && yf != 0) {
			framebuffer.DDA(x, y, xf, yf, c);
			x = NULL;
			y = NULL;
			xf = NULL;
			yf = NULL;
		}

		else if (triangle && xf != 0 && yf != 0) {
			framebuffer.drawFilledTriangle(x, y, xf, yf, xt, yt, c);
			x = NULL;
			y = NULL;
			xf = NULL;
			yf = NULL;
			xt = NULL;
			yt = NULL;
		}
		
	}

	framebuffer.drawTriangle(50, 50, 100, 50, 75, 100, c, false);
	framebuffer.drawFilledTriangle(50, 50, 100, 50, 75, 100, c);

	//print points while on click
	
	if (mouse_position.y < 470 && clic == true){
		if(square){ 
			framebuffer.drawEmptySquare(mouse_position.x, mouse_position.y, penSize, c);
			clic = false; //for empty squares we only want to draw one per click
		}
		else if (circle) {
			framebuffer.circle(mouse_position.x, mouse_position.y, penSize, c);
			clic = false; //for empty circles we only want to draw one per click
		}
		//for fullCircle and fullSqares we want them to be used as strokes
		else if (fullCircle) { framebuffer.printAllCircle(mouse_position.x, mouse_position.y, penSize, c); }
		else if (fullSquare) { framebuffer.drawSquare(mouse_position.x, mouse_position.y, penSize, c, false); }
	}

	//if Space bar is pressed, canvas is cleared
	if (background){
		for (int x = 0; x < 800; x++) {
			for (int y = 0; y < 500; y ++) {
				framebuffer.setPixel(x, y, Color::WHITE);
			}
		}
		background = false;
	}

	//send image to screen
	showImage( &framebuffer );
	showImage(&eraser);
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE]) //if key space is pressed
	{
		//...
	}

	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			exit(0); 
			break; //ESC key, kill the app

		case SDL_SCANCODE_SPACE:
			background = true; //SPACE key, clean the canvas
			break;

		case SDL_SCANCODE_1:
			trianglePressed = false;
			userLine = true;
			sizeCircle = false;
			circle = false;
			square = false;
			fullSquare = false;
			fullCircle = false;
			break;

		case SDL_SCANCODE_2:
			sizeCircle = true;
			userLine = false;
			circle = false;
			square = false;
			fullSquare = false;
			fullCircle = false;
			break;

		case SDL_SCANCODE_3:
			trianglePressed = true;
			triangle = true;
			userLine = false;
			sizeCircle = false;
			circle = false;
			square = false;
			fullSquare = false;
			fullCircle = false;
			break;

		case SDL_SCANCODE_4:
			break;
	}
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...

}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		//if you read mouse position from the event, careful, Y is reversed, use mouse_position instead
		std::cout << "Mouse button pressed" << std::endl;
		std::cout << "Pos x: " << mouse_position.x << " Pos y: "<< mouse_position.y << std::endl;
		if (mouse_position.y < 500 && pressed == false && !trianglePressed) {
			x = mouse_position.x;
			y = mouse_position.y;
			pressed = true;
		}
		else if (mouse_position.y < 500 && pressed && !trianglePressed) {
			xf = mouse_position.x;
			yf = mouse_position.y;
			pressed = false;
			if (triangle) trianglePressed = true;
		}
		else if (mouse_position.y < 500 && trianglePressed) {
			xt = mouse_position.x;
			yt = mouse_position.y;
			trianglePressed = false;
		}
		
		clic = true;

		//change color
		if (mouse_position.x > 400 && mouse_position.y > 560 && mouse_position.x < 430 && mouse_position.y < 590) {c = Color::GRAY;}
		if (mouse_position.x > 440 && mouse_position.y > 560 && mouse_position.x < 470 && mouse_position.y < 590) { c = Color::GREEN;}
		if (mouse_position.x > 480 && mouse_position.y > 560 && mouse_position.x < 510 && mouse_position.y < 590) { c = Color::YELLOW;}
		if (mouse_position.x > 400 && mouse_position.y > 520 && mouse_position.x < 430 && mouse_position.y < 550) { c = Color::BLACK;}
		if (mouse_position.x > 440 && mouse_position.y > 520 && mouse_position.x < 470 && mouse_position.y < 550) { c = Color::RED;}
		if (mouse_position.x > 480 && mouse_position.y > 520 && mouse_position.x < 510 && mouse_position.y < 550) { c = Color::BLUE;}

		//Change pen size
		if (mouse_position.x > 60 && mouse_position.y > 530 && mouse_position.x < 90 && mouse_position.y < 560) { penSize += 2; }
		if (mouse_position.x > 100 && mouse_position.y > 530 && mouse_position.x < 130 && mouse_position.y < 560 && penSize != 1) { penSize -= 2; }

		//Change to forms
		if (mouse_position.x > 200 && mouse_position.y > 530 && mouse_position.x < 230 && mouse_position.y < 560) { 
			circle = true; 
			square = false;
			fullCircle = false;
			fullSquare = false;
			trianglePressed = false;
			triangle = false;
			userLine = false;
			sizeCircle = false;
		}
		else if (mouse_position.x > 240 && mouse_position.y > 530 && mouse_position.x < 270 && mouse_position.y < 560) { 
			square = true; 
			circle = false;
			fullCircle = false;
			fullSquare = false;
			trianglePressed = false;
			triangle = false;
			userLine = false;
			sizeCircle = false;
		}
		else if (mouse_position.x > 280 && mouse_position.y > 530 && mouse_position.x < 310 && mouse_position.y < 560) {
			square = false;
			circle = false;
			fullCircle = true;
			fullSquare = false;
			trianglePressed = false;
			triangle = false;
			userLine = false;
			sizeCircle = false;
		}
		else if (mouse_position.x > 320 && mouse_position.y > 530 && mouse_position.x < 350 && mouse_position.y < 560) {
			square = false;
			circle = false;
			fullCircle = false;
			fullSquare = true;
			trianglePressed = false;
			triangle = false;
			userLine = false;
			sizeCircle = false;
		}
	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

		clic = false; //when button is released, change the clic to false
		
	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
