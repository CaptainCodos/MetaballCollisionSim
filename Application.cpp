#include <memory>
#include <functional>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

#include "Application.h"


int Application::SCREEN_WIDTH = 0;
int Application::SCREEN_HEIGHT = 0;

// Camera
Camera Application::camera = Camera::Camera(glm::vec3(0.0f, 0.0f, 0.0f));;
double Application::lastX = WIDTH / 2.0;
double Application::lastY = HEIGHT / 2.0;

bool Application::firstMouse = true;
bool Application::keys[1024];


Application::Application()
{
}


Application::~Application()
{
}


// Moves/alters the camera positions based on user input
void Application::doMovement(GLfloat deltaTime)
{
	glfwPollEvents();

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

// Is called whenever a key is pressed/released via GLFW
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			Application::keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			Application::keys[key] = false;
		}
	}
}

static void mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (Application::firstMouse)
	{
		Application::lastX = xPos;
		Application::lastY = yPos;
		Application::firstMouse = false;
	}

	double xOffset = xPos - Application::lastX;
	double yOffset = Application::lastY - yPos;

	Application::lastX = xPos;
	Application::lastY = yPos;

	Application::camera.ProcessMouseMovement((GLfloat)xOffset, (GLfloat)yOffset);
}


static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
	Application::camera.ProcessMouseScroll((GLfloat)yOffset);
}

// Renderer initialisation
int Application::initRender() {
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "Physics-Based Animation", nullptr, nullptr);

	if (nullptr == m_window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(m_window);
	glfwGetFramebufferSize(m_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(m_window, keyCallback);
	glfwSetCursorPosCallback(m_window, mouseCallback);
	glfwSetScrollCallback(m_window, scrollCallback);

	// remove the mouse cursor
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// moder GLEW approach
	glewExperimental = GL_FALSE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	return 1;
}

// clear buffer
void Application::clear() {
	// Clear the colorbuffer
	glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// use mesh shader early
void Application::useMeshShader(Mesh &mesh)
{
	mesh.getShader().Use();
}

// draw mesh
void Application::draw(Mesh &mesh)
{
	//mesh.getShader().Use();
	// view and projection matrices
	m_projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_view = camera.GetViewMatrix();

	// Get the uniform locations
	GLint colorLoc = glGetUniformLocation(mesh.getShader().Program, "inColor");
	GLint modelLoc = glGetUniformLocation(mesh.getShader().Program, "model");
	GLint viewLoc = glGetUniformLocation(mesh.getShader().Program, "view");
	GLint projLoc = glGetUniformLocation(mesh.getShader().Program, "projection");

	// Pass the matrices to the shader
	glUniform4fv(colorLoc, 1, glm::value_ptr(glm::vec4(0.7f, 0.7f, 0.7f, 0.4f)));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mesh.getModel()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

	/*glBindVertexArray(mesh.getVertexArrayObject());
	glDrawArrays(mesh.getMode(), 0, mesh.getNumIndices());*/
	mesh.SetUniforms();
	glBindVertexArray(0);
}
void Application::draw(Mesh &mesh, glm::vec4 color)
{
	//mesh.getShader().Use();
	// view and projection matrices
	m_projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);
	m_view = camera.GetViewMatrix();

	// Get the uniform locations
	GLint colorLoc = glGetUniformLocation(mesh.getShader().Program, "inColor");
	GLint modelLoc = glGetUniformLocation(mesh.getShader().Program, "model");
	GLint viewLoc = glGetUniformLocation(mesh.getShader().Program, "view");
	GLint projLoc = glGetUniformLocation(mesh.getShader().Program, "projection");

	// Pass the matrices to the shader
	glUniform4fv(colorLoc, 1, glm::value_ptr(color));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mesh.getModel()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

	/*glBindVertexArray(mesh.getVertexArrayObject());
	glDrawArrays(mesh.getMode(), 0, mesh.getNumIndices());*/
	mesh.SetUniforms();
	glBindVertexArray(0);
}

void Application::display() {
	glBindVertexArray(0);
	// Swap the buffers
	glFlush();
}

