#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;
Light* light = NULL;
Light* light2 = NULL;
Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;
Shader* texture_shader = NULL;
Texture* texture_specular = NULL;
Texture* texture_normal = NULL;

float angle = 0;

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
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ( "lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	shader = new Shader();

	//load your Gouraud and Phong shaders here
	gouraud_shader = new Shader();
	gouraud_shader->load("gouraud.vs", "gouraud.ps");

	phong_shader = new Shader();
	phong_shader->load("phong.vs", "phong.ps");

	texture_shader = new Shader();
	texture_shader->load("texture_shader.vs", "texture_shader.ps");

	//default
	shader = texture_shader;

	//CODE HERE:
	//create a light (or several) and a materials
	light = new Light();
	light2 = new Light();
	light2->position.set(0,0,0);
	material = new Material();
	texture_normal = new Texture();
	texture_specular = new Texture();
	if (!texture_specular->load("lee_color_specular.tga") || !texture_normal->load("lee_normal.tga"))
	{
		std::cout << "Texture NOT FOUND!" << std::endl;
		exit(1);
	}

}

//render one frame
void Application::render(void)
{
	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);

	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	Vector3 ka = material->ambient;
	Vector3 ia = light->ambient;
	Vector3 kd = material->diffuse;
	Vector3 id = light->diffuse_color;
	Vector3 is = light->specular_color;
	Vector3 ks = material->specular;
	Vector3 V = camera->eye;
	Vector3 L = light->position;
	float alpha = material->shininess;

	//second light
	Vector3 id2 = light2->diffuse_color;
	Vector3 is2 = light2->specular_color;
	Vector3 L2 = light2->position;

	//choose a shader and enable it
	shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
	shader->setTexture("texture_color", texture_specular);
	shader->setTexture("texture_normal", texture_normal);

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader
	//...
	
	shader->setVector3("ka", ka);
	shader->setVector3("ia", ia);
	shader->setVector3("kd", kd);
	shader->setVector3("id", id);
	shader->setVector3("is", is);
	shader->setVector3("ks", ks);
	shader->setVector3("eye", V);
	shader->setVector3("light", L);
	shader->setFloat("alpha", alpha);

	shader->setVector3("is2", is2);
	shader->setVector3("id2", id2);
	shader->setVector3("light2", L2);

	//do the draw call into the GPU
	mesh->render(GL_TRIANGLES);

	//disable shader when we do not need it any more
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;
	if (keystate[SDL_SCANCODE_DELETE])
		angle -= seconds_elapsed;

	//chose shader
	if (keystate[SDL_SCANCODE_1])
		shader = gouraud_shader;
	if (keystate[SDL_SCANCODE_2])
		shader = phong_shader;
	if (keystate[SDL_SCANCODE_3])
		shader = texture_shader;

	//change light position
	if (keystate[SDL_SCANCODE_A])
		light->position.x += 40 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_D])
		light->position.x -= 40 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_W])
		light->position.y -= 40 * seconds_elapsed;
	if (keystate[SDL_SCANCODE_S])
		light->position.y += 40 * seconds_elapsed;

	//change light color
	if (keystate[SDL_SCANCODE_G]) //green
		light->diffuse_color.set(0.0, 1.0, 0.0);
	if (keystate[SDL_SCANCODE_B]) //blue
		light->diffuse_color.set(0.0, 0.0, 1.0);
	if (keystate[SDL_SCANCODE_R]) //red
		light->diffuse_color.set(1.0, 0.0, 0.0);
	if (keystate[SDL_SCANCODE_T])
		light->diffuse_color.set(0.6f, 0.6f, 0.6f);

	//change material color
	if (keystate[SDL_SCANCODE_Z])
		material->ambient.set(1.0, 0.0, 0.0);
	if (keystate[SDL_SCANCODE_X])
		material->ambient.set(0.0, 1.0, 0.0);
	if (keystate[SDL_SCANCODE_C])
		material->ambient.set(0.0, 0.0, 1.0);

	//initial state
	if (keystate[SDL_SCANCODE_I]) {
		light->diffuse_color.set(0.6, 0.6, 0.6);
		light->position.set(50, 50, 0);
		material->ambient.set(1, 1, 1);
		camera->lookAt(Vector3(0, 20, 20), Vector3(0, 10, 0), Vector3(0, 1, 0));
	}
		
	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
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
