#include "application.h"
#include "utils.h"
#include "image.h"
#include "mesh.h"

Mesh* mesh = NULL;
Camera* camera = NULL;

int window_width = 800;
int window_height = 600;
int buffer = 1;
Image framebuffer(window_width, window_height);
Image framebuffer2(window_width, window_height);
FloatImage depthBuffer(window_width, window_height);
Color colors[9] = { Color::WHITE, Color::BLACK, Color::GRAY, Color::RED, Color::GREEN, Color::PURPLE, Color::CYAN, Color::YELLOW, Color::CYAN };
int color = 0;

bool change = true;

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
//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,10,20),Vector3(0,10,0),Vector3(0,1,0)); //define eye,center,up
	camera->perspective(20, window_width / (float)window_height, 0.1, 10000); //define fov,aspect,near,far

	//load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ("lee.obj") )
		std::cout << "FILE Lee.obj NOT FOUND" << std::endl;
	/*
	for (int i = 0; i < window_width; i++) 
	{
		for (int j = 0; j < window_height; j++) 
		{
			depthBuffer.setPixel(i, j, 1000000.0f);
		}
	}*/
	
	//maybe it would be useful here to create some random points and store them in a global container like std::vector<Vector3> points;
}

//render one frame
void Application::render(void)
{
	//render framebuffer
	if (change)
	{
		if (buffer == 1)
		{
			framebuffer.fill(Color(40, 45, 60)); //pale blue
		}
		else
		{
			framebuffer2.fill(Color(40, 45, 60)); //pale blue
		}
		//framebuffer.drawSquare(200, 200, 40, Color::WHITE, true);
		//framebuffer.drawSquare(400, 200, 40, Color::WHITE, false);
		//framebuffer.drawTriangle(120, 120, 200, 120, 300, 40, Color::WHITE, true);
		//framebuffer.drawCircle(300, 300, 40, Color::WHITE, true);
		Vector3 firstPoint;
		Vector3 secondPoint;
		Vector3 thirdPoint;
		int numTriangle = 1;
		//for every point of the mesh (to draw triangles take three points each time and connect the points between them (1,2,3,   4,5,6,   ... )

		for (int i = 0; i < mesh->vertices.size(); ++i)
		{
			Vector3 vertex = mesh->vertices[i]; //extract vertex from mesh
			Vector4 fVertex = Vector4(vertex.x, vertex.y, vertex.z, 1.0f);

			fVertex = camera->viewprojection_matrix * fVertex;
			vertex = Vector3(fVertex.x / fVertex.w, fVertex.y / fVertex.w, fVertex.z / fVertex.w);
			
			if (vertex.x <= 1 && vertex.x >= -1 && vertex.y <= 1 && vertex.y >= -1)
			{
				vertex.x = window_width / 2 + vertex.x * window_width / 2;
				vertex.y = window_height / 2 + vertex.y * window_height / 2;
			}
			else
			{
				if (vertex.x > 1) 
				{
					vertex.x = 1;
				}
				if (vertex.x < -1 ) 
				{
					vertex.x = -1;
				}
				if (vertex.y > 1) 
				{
					vertex.y = 1;
				}
				if (vertex.y < -1) 
				{
					vertex.y = -1;
				}
				vertex.x = window_width / 2 + vertex.x * window_width / 2;
				vertex.y = window_height / 2 + vertex.y * window_height / 2;
			}
			if (numTriangle == 3)
			{
				thirdPoint = vertex;
				if (true)
				{
					if (buffer == 1)
					{
						framebuffer.drawTriangle(firstPoint.x, firstPoint.y, firstPoint.z, secondPoint.x, secondPoint.y, secondPoint.z, thirdPoint.x, thirdPoint.y, thirdPoint.z, colors[color], true);
					}
					else
					{
						framebuffer2.drawTriangle(firstPoint.x, firstPoint.y, firstPoint.z, secondPoint.x, secondPoint.y, secondPoint.z, thirdPoint.x, thirdPoint.y, thirdPoint.z, colors[color], true);
					}
					color++;
					if (color == 9) { color = 0; }
				}
			}
			else if (numTriangle == 2)
			{
				secondPoint = vertex;
			}
			else
			{
				firstPoint = vertex;
			}

			numTriangle++;
			if (numTriangle == 4)
			{
				numTriangle = 1;
			}

			//project every point in the mesh to normalized coordinates using the viewprojection_matrix inside camera
			//Vector3 normalized_point = camera->projectVector( vertex );

			//convert from normalized (-1 to +1) to framebuffer coordinates (0,W)
			//...

			//paint point in framebuffer
			//...
		}
		change = false;
	}
	

	//render to screen
	if (buffer == 1) 
	{
		showImage(&framebuffer);
	}
	else 
	{
		showImage(&framebuffer2);
	}
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
	{
		//...
	}

	//if we modify the camera fields, then update matrices
	//camera->updateViewMatrix();
	//camera->updateProjectionMatrix();
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		//zoom
		case SDLK_f: camera->fov -= 5; change = true; break;
		case SDLK_g: camera->fov += 5; change = true; break;
		//center position (where the camera is pointing to)
		case SDLK_w: camera->center.z -= 5; change = true; break;
		case SDLK_s: camera->center.z += 5; change = true; break;
		case SDLK_a: camera->center.x += 5; change = true; break;
		case SDLK_d: camera->center.x -= 5; change = true; break;
		//camera position
		case SDLK_LEFT: camera->eye.x -= 2; change = true; break;
		case SDLK_RIGHT: camera->eye.x += 2; change = true; break;
		case SDLK_UP: camera->center.z += 2; change = true; break;
		case SDLK_DOWN: camera->center.z -= 2; change = true; break;
	}
	if (buffer == 1) 
	{
		buffer = 2;
	}
	else 
	{
		buffer = 1;
	}
	camera->updateViewMatrix();
	camera->updateProjectionMatrix();
}

//keyboard released event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{

	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
