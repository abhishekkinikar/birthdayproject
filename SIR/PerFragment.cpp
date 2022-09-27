#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <malloc.h>

#include "vmath.h"
#include "Sphere.h"
#include "Tree.h"
using namespace vmath;

// OpenGL libraries
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Sphere.lib")

// Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Water
#define TESSEL_X 110
#define TESSEL_Z 110
#define QUADSIZE 100.0f
#define X_SHIFT_WATER (QUADSIZE * 2.0f) / TESSEL_X
#define Z_SHIFT_WATER (QUADSIZE * 2.0f) / TESSEL_Z

// Abhishek
#define CIRCLE_POINTS 181
#define PI 3.14159235

// global function prototype
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Global Veriable declaration
BOOL gbActiveWindow = FALSE;
FILE *gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
};

// water
GLuint SPObj_Water;
GLuint VAO_WATER;
GLuint VBO_POSITION_WATER, VBO_NORMAL;

GLuint modelMatrixUniform_water, viewMatrixUniform_water, projectionMatrixUniform_water;

GLuint laUniform_water, ldUniform_water, lsUniform_water;
GLuint lightPositionUniform_water;
GLuint kaUniform_water, kdUniform_water, ksUniform_water;
GLuint materialShininessUniform_water;
GLuint lightingEnabledUnifrom_water;
GLuint yTransUniform_water;

float freq = 20.0f;
float time = 0.0f;

float water_x[TESSEL_X + 1][TESSEL_Z + 1];
float water_y[TESSEL_X + 1][TESSEL_Z + 1];
float water_z[TESSEL_X + 1][TESSEL_Z + 1];

// Boat
GLuint SPObj_Boat;
GLuint VAO_BOAT;
GLuint VBO_boat_position;
GLuint VBO_boat_color;

GLuint modelMatrixUniform_boat, viewMatrixUniform_boat, projectionMatrixUniform_boat;

GLuint laUniform_boat, ldUniform_boat, lsUniform_boat;
GLuint lightPositionUniform_boat;
GLuint kaUniform_boat, kdUniform_boat, ksUniform_boat;
GLuint materialShininessUniform_boat;
GLuint lightingEnabledUnifrom_boat;
GLuint yTransUniform_boat;

GLfloat xTransBoat = -10.0f;

// Abhishek

GLuint SPObj_Iland;
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumVertices = 0;
unsigned int gNumElements = 0;

GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint vao_leaf;
GLuint vbo_leaf_position;
GLuint vbo_leaf_color;

GLuint vao_triangle;		  // vao : vertex array object
GLuint vbo_triangle_position; // vbo_position : vertex buffer object
GLuint vbo_triangle_color;

GLuint vao_cylender;
GLuint vbo_cylender;
GLuint vbo_normal_cylender;

mat4 translationMatrix = mat4::identity();
mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();
mat4 scaleMatrix = mat4::identity();
mat4 rotationMatrix = mat4::identity();

GLuint modelMatrixUniform_Iland;
GLuint viewMatrixUniform_Iland;
GLuint projectionMatrixUniform_Iland;
GLuint lightingEnabledUniform_Iland;

GLuint laUniform_Iland; // Light Ambient
GLuint ldUniform_Iland; // Light Diffuse
GLuint lsUniform_Iland; // Light Specular
GLuint lightPositionUniform_Iland;

GLuint kaUniform_Iland;
GLuint kdUniform_Iland;
GLuint ksUniform_Iland;
GLuint materialShinenessUniform_Iland;

GLfloat xPosLeaf = -1.2f;
GLfloat yPosLeaf = -1.4f;
GLfloat zPosLeaf = -10.0f;

mat4 matrixStack[16];
int top = 0;
float zTranslate = 5.0f;
float xRotate = 0.0f;
float cameraRadius = 30.0f;
int scene = 1;

GLfloat camPosZ = 0.0f;
GLfloat camPosX = 0.0f;
GLfloat camPosY = 0.0f;
GLfloat objPosY = 0.0f;
GLfloat objPosZ = 0.0f;
GLfloat objPosX = 0.0f;
// GLfloat *triangleVertices=NULL;

GLfloat circleData[CIRCLE_POINTS * 3];
GLfloat centerCoordinates[] = {0.0f, 0.0f, 0.0f};
GLfloat cylinderData[CIRCLE_POINTS * 4 * 3];
GLfloat cylinderNormals[CIRCLE_POINTS * 4 * 3];
float angleCylinder = 0.0f;

GLfloat lightAmbient_Iland[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightDiffuse_Iland[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightSpecular_Iland[] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightPosition_Iland[] = {0.0, 100.0, 0.0, 1.0};

GLfloat MaterialAmbient_Iland[4];
GLfloat MaterialDiffuse_Iland[4];
GLfloat MaterialSpecular_Iland[4];
GLfloat MaterialShininess_Iland;

//Sun 

GLfloat transitionPoint = -9.0f;

// general
mat4 perspectiveProjectionMatrix;
BOOL bLight = FALSE;

// GLfloat lightAmbient[] = { 0.0f, 0.0f ,0.0f ,1.0f };
// GLfloat lightDiffuse[] = { 1.0f ,1.0f ,1.0f ,1.0f };
// GLfloat lightSpecular[] = { 1.0f ,1.0f ,1.0f ,1.0f };
// GLfloat lightPosition[] = { 100.0f ,100.0f ,100.0f ,1.0f };
//
// GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
// GLfloat materialDiffuse[] = { 1.0f ,1.0f ,1.0f ,1.0f };
// GLfloat materialSpecular[] = { 1.0f ,1.0f ,1.0f ,1.0f };
// GLfloat materialShininess = 50.0f;

GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDiffuse[] = {0.5f, 0.2f, 0.7f, 1.0f};
GLfloat materialSpecular[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat materialShininess = 128.0f;

// Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreIntance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function prototype
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyFirstWindow");
	BOOL bDone = FALSE;
	int iRateVal = 0;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of log file failed. Exitting!!!"), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
		fprintf(gpFile, "Log file succefully created\n");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WindowProcedure;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("PerFragment"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, HWND_DESKTOP, NULL, hInstance, NULL);
	ghwnd = hwnd;

	// show window
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd); // use hwnd here since ghwnd is use for outer functions
	SetFocus(hwnd);

	// Initialize
	iRateVal = initialize();
	if (iRateVal != 0)
	{
		fprintf(gpFile, "Initialize() failed \n");
		uninitialize();
	}

	// Game loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				display();
				update();
			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}

// callback function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// funtion declarations
	void toggleFullScreen(void);
	void resize(int, int);

	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		return (0);
	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			toggleFullScreen();
			break;
		case 'L':
		case 'l':
			bLight = !bLight;
			break;

		case 'p':
			fprintf(gpFile, "boat X = %f Y= -3.7f Z = -12.5f \n  leaf X = %f Y = %f Z = %f", xTransBoat, xPosLeaf, yPosLeaf, zPosLeaf);
			break;

		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 27:
			DestroyWindow(hwnd);
			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void toggleFullScreen()
{
	// Variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// code
	wp.length = sizeof(WINDOWPLACEMENT);
	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void)
{
	// function prototype
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int width, int height);
	void initializeCircle(GLfloat radius, GLfloat * centre, int countOfPoints, GLfloat *circleVertices);			   // Abhishek
	void initCylinder(GLfloat radius, GLfloat * center, int countOfPoints, GLfloat height, GLfloat *cylinderVertices); // Abhishek

	// veriable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// code
	// initialization of pixel format discriptor
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
		return -1;
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -1;
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -1;
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -1;

	// GLEW intialization
	if (glewInit() != GLEW_OK)
		return -1;

	// print OpenGL info
	// printGLInfo();

	// Water Related changes
	for (int i = 0; i <= TESSEL_Z; i++)
	{
		for (int j = 0; j <= TESSEL_X; j++)
		{
			water_x[i][j] = -QUADSIZE + (j * X_SHIFT_WATER);
			water_y[i][j] = 0.0f;
			water_z[i][j] = -QUADSIZE + (i * Z_SHIFT_WATER);
		}
	}

	// Water
	// Vertex Shader
	const GLchar *vertexShaderSourceCode =
		"#version 440 core"
		"\n"
		"in vec4 a_position;"
		"in vec3 a_normal;"
		"uniform mat4 u_modelMatrix;"
		"uniform mat4 u_viewMatrix;"
		"uniform mat4 u_projectionMatrix;"
		"uniform vec4 u_lightPosition;"
		"uniform int u_lightEnabled;"
		"out vec3 transformedNormal;"
		"out vec3 lightDirection;"
		"out vec3 viewerVector;"
		"void main(void) {"
		"if(u_lightEnabled == 1) {"
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;"
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);"
		"transformedNormal = normalMatrix * a_normal;"
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;"
		"viewerVector = -eyeCoordinates.xyz;"
		"}"
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;"
		"lightDirection = vec3(0.0, 0.0, a_position.y);"
		"}";
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status, infoLoglength;
	char *log = NULL;
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Vertex Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	// Fragment Shader
	const GLchar *fragmentShaderSourceCode =
		"#version 440 core"
		"\n"
		"in vec3 transformedNormal;"
		"in vec3 lightDirection;"
		"in vec3 viewerVector;"
		"uniform vec3 u_la;"
		"uniform vec3 u_ld;"
		"uniform vec3 u_ls;"
		"uniform vec3 u_ka;"
		"uniform vec3 u_kd;"
		"uniform vec3 u_ks;"
		"uniform float u_materialShininess;"
		"uniform int u_lightEnabled;"
		"out vec4 FragColor;"
		"void main(void) {"
		"vec3 fong_ads_color;"
		"if(u_lightEnabled == 1) {"
		"vec3 ambient = u_la * u_ka;"
		"vec3 normalized_transformed_normals = normalize(transformedNormal);"
		"vec3 normalized_light_direction = normalize(lightDirection);"
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals),0.0);"
		"vec3 reflectionVector =  reflect(-normalized_light_direction, normalized_transformed_normals);"
		"vec3 normalized_viewer_vector = normalize(viewerVector);"
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewer_vector),0.0),u_materialShininess);"
		"fong_ads_color = ambient + diffuse + specular;"
		"}"
		"else {"
		"// fong_ads_color = vec3(1.0, 1.0, 1.0);\n"
		"fong_ads_color = lightDirection + vec3(0.0, 0.0, 0.5);"
		"}"
		"FragColor = vec4(fong_ads_color, 1.0);"
		"}";
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Fragment Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	// shader program object
	SPObj_Water = glCreateProgram();
	glAttachShader(SPObj_Water, vertexShaderObject);
	glAttachShader(SPObj_Water, fragmentShaderObject);
	glBindAttribLocation(SPObj_Water, AMC_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(SPObj_Water, AMC_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(SPObj_Water);
	glGetProgramiv(SPObj_Water, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(SPObj_Water, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(SPObj_Water, infoLoglength, &written, log);
				fprintf(gpFile, "Shader Program Water Link Log : %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}

	modelMatrixUniform_water = glGetUniformLocation(SPObj_Water, "u_modelMatrix");
	viewMatrixUniform_water = glGetUniformLocation(SPObj_Water, "u_viewMatrix");
	projectionMatrixUniform_water = glGetUniformLocation(SPObj_Water, "u_projectionMatrix");

	laUniform_water = glGetUniformLocation(SPObj_Water, "u_la");
	ldUniform_water = glGetUniformLocation(SPObj_Water, "u_ld");
	lsUniform_water = glGetUniformLocation(SPObj_Water, "u_ls");
	lightPositionUniform_water = glGetUniformLocation(SPObj_Water, "u_lightPosition");
	kaUniform_water = glGetUniformLocation(SPObj_Water, "u_ka");
	kdUniform_water = glGetUniformLocation(SPObj_Water, "u_kd");
	ksUniform_water = glGetUniformLocation(SPObj_Water, "u_ks");
	materialShininessUniform_water = glGetUniformLocation(SPObj_Water, "u_materialShininess");
	lightingEnabledUnifrom_water = glGetUniformLocation(SPObj_Water, "u_lightEnabled");

	glGenVertexArrays(1, &VAO_WATER);
	glBindVertexArray(VAO_WATER);
	{
		glGenBuffers(1, &VBO_POSITION_WATER);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_POSITION_WATER);
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 6 * TESSEL_X * TESSEL_Z, NULL, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Normal VBO
		//  glGenBuffers(1, &VBO_NORMAL);
		//  glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
		//  {
		//  	glBufferData(GL_ARRAY_BUFFER, sizeof(Normal), Normal, GL_STATIC_DRAW);
		//  	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		//  	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
		//  }
		//  glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	// Boat

	GLchar *vertexShaderSourceCode_Boat =
		"#version 440 core"
		"\n"
		"in vec4 a_position;"
		"in vec3 a_normal;"
		"uniform mat4 u_modelMatrix;"
		"uniform mat4 u_viewMatrix;"
		"uniform mat4 u_projectionMatrix;"
		"uniform vec4 u_lightPosition;"
		"uniform int u_lightEnabled;"
		"uniform float y_trans;"
		"out vec3 transformedNormal;"
		"out vec3 lightDirection;"
		"out vec3 viewerVector;"
		"void main(void) {"
		"if(u_lightEnabled == 1) {"
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;"
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);"
		"transformedNormal = normalMatrix * a_normal;"
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;"
		"viewerVector = -eyeCoordinates.xyz;"
		"}"
		"else"
		"{"
		"transformedNormal = a_normal;"
		"}"
		"mat4 modelMatrix = u_modelMatrix;"
		"modelMatrix[3][1] = modelMatrix[3][1] + y_trans;"
		"gl_Position = u_projectionMatrix * u_viewMatrix * modelMatrix * a_position;"
		"}";
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode_Boat, NULL);
	glCompileShader(vertexShaderObject);

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Vertex Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	GLchar *fragmentShaderSourceCode_Boat =
		"#version 440 core"
		"\n"
		"in vec3 transformedNormal;"
		"in vec3 lightDirection;"
		"in vec3 viewerVector;"
		"uniform vec3 u_la;"
		"uniform vec3 u_ld;"
		"uniform vec3 u_ls;"
		"uniform vec3 u_ka;"
		"uniform vec3 u_kd;"
		"uniform vec3 u_ks;"
		"uniform float u_materialShininess;"
		"uniform int u_lightEnabled;"
		"out vec4 FragColor;"
		"void main(void) {"
		"vec3 fong_ads_color;"
		"if(u_lightEnabled == 1) {"
		"vec3 ambient = u_la * u_ka;"
		"vec3 normalized_transformed_normals = normalize(transformedNormal);"
		"vec3 normalized_light_direction = normalize(lightDirection);"
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals),0.0);"
		"vec3 reflectionVector =  reflect(-normalized_light_direction, normalized_transformed_normals);"
		"vec3 normalized_viewer_vector = normalize(viewerVector);"
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewer_vector),0.0),u_materialShininess);"
		"fong_ads_color = ambient + diffuse + specular;"
		"}"
		"else {"
		" fong_ads_color = vec3(transformedNormal);\n"
		"}"
		"FragColor = vec4(fong_ads_color, 1.0);"
		"}";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode_Boat, NULL);
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Fragment Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	// shader program object
	SPObj_Boat = glCreateProgram();
	glAttachShader(SPObj_Boat, vertexShaderObject);
	glAttachShader(SPObj_Boat, fragmentShaderObject);
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(SPObj_Boat);
	glGetProgramiv(SPObj_Boat, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(SPObj_Boat, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(SPObj_Boat, infoLoglength, &written, log);
				fprintf(gpFile, "Shader Program Boat Link Log : %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}

	modelMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_modelMatrix");
	viewMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_viewMatrix");
	projectionMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_projectionMatrix");

	laUniform_boat = glGetUniformLocation(SPObj_Boat, "u_la");
	ldUniform_boat = glGetUniformLocation(SPObj_Boat, "u_ld");
	lsUniform_boat = glGetUniformLocation(SPObj_Boat, "u_ls");
	lightPositionUniform_boat = glGetUniformLocation(SPObj_Boat, "u_lightPosition");
	kaUniform_boat = glGetUniformLocation(SPObj_Boat, "u_ka");
	kdUniform_boat = glGetUniformLocation(SPObj_Boat, "u_kd");
	ksUniform_boat = glGetUniformLocation(SPObj_Boat, "u_ks");
	materialShininessUniform_boat = glGetUniformLocation(SPObj_Boat, "u_materialShininess");
	lightingEnabledUnifrom_boat = glGetUniformLocation(SPObj_Boat, "u_lightEnabled");
	yTransUniform_boat = glGetUniformLocation(SPObj_Boat, "y_trans");

	const GLfloat cubePosition[] =
		{
			// top
			/*1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,*/

			// bottom
			0.5f,
			-0.5f,
			-1.0f,
			-0.5f,
			-0.5f,
			-1.0f,
			-0.5f,
			-0.5f,
			1.0f,
			0.5f,
			-0.5f,
			1.0f,

			// front
			1.0f,
			1.0f,
			1.0f,
			-1.0f,
			1.0f,
			1.0f,
			-0.5f,
			-0.5f,
			1.0f,
			0.5f,
			-0.5f,
			1.0f,

			// back
			1.0f,
			1.0f,
			-1.0f,
			-1.0f,
			1.0f,
			-1.0f,
			-0.5f,
			-0.5f,
			-1.0f,
			0.5f,
			-0.5f,
			-1.0f,

			// right
			1.0f,
			1.0f,
			-1.0f,
			1.0f,
			1.0f,
			1.0f,
			0.5f,
			-0.5f,
			1.0f,
			0.5f,
			-0.5f,
			-1.0f,

			// left
			-1.0f,
			1.0f,
			1.0f,
			-1.0f,
			1.0f,
			-1.0f,
			-0.5f,
			-0.5f,
			-1.0f,
			-0.5f,
			-0.5f,
			1.0f,
		};

	const GLfloat cubeColor[] =
		{
			0.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			0.0f,
			0.0f,
			1.0f,
			0.0f,

			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,

			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,

			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,

			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,

			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,
			0.8f,
			0.4f,
			0.1f,

		};

	// cube
	// VAO_WATER and VBO related code
	// VAO_WATER
	glGenVertexArrays(1, &VAO_BOAT); // 5th step
	glBindVertexArray(VAO_BOAT);
	// VBO for position
	glGenBuffers(1, &VBO_boat_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// VBO for color
	glGenBuffers(1, &VBO_boat_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Abhishek
	//  Vertex Shader
	const GLchar *vertexShaderSourceCode_Iland =
		"#version 440 core"
		"\n"
		"in vec4 a_position;"
		"in vec4 a_color;"
		"in vec3 a_normal;"
		"uniform mat4 u_modelMatrix;"
		"uniform mat4 u_viewMatrix;"
		"uniform mat4 u_projectionMatrix;"
		"uniform vec4 u_lightPosition;"
		"uniform int u_lightingEnabled;"
		"out vec3 transformedNormal;"
		"out vec3 lightDirection;"
		"out vec3 viewerVector;"
		"out vec4 a_color_out;"
		"void main(void)"
		"{"
		"if(u_lightingEnabled == 1)"
		"{"
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;"
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);"
		"transformedNormal = normalMatrix * a_normal;"
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;"
		"viewerVector = -eyeCoordinates.xyz;"
		"}"
		"gl_Position = u_projectionMatrix * u_viewMatrix* u_modelMatrix * a_position;"
		"a_color_out = a_color;"
		"}";

	// Create shader object
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// Give source code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode_Iland, NULL);

	// Compile Shader
	glCompileShader(vertexShaderObject);

	// Getting length of log of compilation status
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);

			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLoglength, &written, log);

				fprintf(gpFile, "Vertex shader compilation log : %s\n", log);

				free(log);

				uninitialize();
			}
		}
	}

	// Fragment Shader
	const GLchar *fragmentShaderSourceCode_Iland =
		"#version 440 core"
		"\n"
		"in vec3 transformedNormal;\n"
		"in vec3 lightDirection;\n"
		"in vec3 viewerVector;\n"
		"in vec4 a_color_out;"
		"uniform vec3 u_la;"
		"uniform vec3 u_ld;"
		"uniform vec3 u_ls;"
		"uniform vec3 u_ka;"
		"uniform vec3 u_kd;"
		"uniform vec3 u_ks;"
		"uniform float u_materialShineness;"
		"uniform int u_lightingEnabled;"
		"vec3 fong_ads_light;"
		"out vec4 FragColor;"
		"void main(void)"
		"{"
		"if(u_lightingEnabled == 1)"
		"{"
		"vec3 ambient=u_la * u_ka;"
		"vec3 normalized_transformedNormals = normalize(transformedNormal);"
		"vec3 normalized_lightDirection = normalize(lightDirection);"
		"vec3 diffuse=u_ld * u_kd * max(dot(normalized_lightDirection,normalized_transformedNormals),0.0);"
		"vec3 reflectionVector=reflect(-normalized_lightDirection,normalized_transformedNormals);"
		"vec3 normalized_viewerVector = normalize(viewerVector);"
		"vec3 specular=u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewerVector),0.0),u_materialShineness);"
		"fong_ads_light = ambient + diffuse + specular;"
		"}"
		"else"
		"{"
		"	fong_ads_light=vec3(1.0,1.0,1.0);"
		"}"
		"FragColor = vec4(fong_ads_light,1.0);"
		"}";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode_Iland, NULL);

	glCompileShader(fragmentShaderObject);
	status = 0;
	infoLoglength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);

			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Fragment shader compilation log : %s \n", log);

				free(log);
				uninitialize();
			}
		}
	}

	// Shader program object.
	SPObj_Iland = glCreateProgram();

	// Attach desire shader
	glAttachShader(SPObj_Iland, vertexShaderObject);
	glAttachShader(SPObj_Iland, fragmentShaderObject);

	// Pre linked binding
	glBindAttribLocation(SPObj_Iland, AMC_ATTRIBUTE_POSITION, "a_position"); // ***Andhar***
	glBindAttribLocation(SPObj_Iland, AMC_ATTRIBUTE_NORMAL, "a_normal");
	glBindAttribLocation(SPObj_Iland, AMC_ATTRIBUTE_COLOR, "a_color"); // ***Andhar***

	// Link program
	glLinkProgram(SPObj_Iland);

	// Post link retriving
	modelMatrixUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_modelMatrix");
	viewMatrixUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_viewMatrix");			 // ***Andhar***
	projectionMatrixUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_projectionMatrix"); // ***Andhar***

	laUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_la");
	ldUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_ld");
	lsUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_ls");
	lightPositionUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_lightPosition");

	kaUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_ka");
	kdUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_kd");
	ksUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_ks");
	materialShinenessUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_materialShineness");

	lightingEnabledUniform_Iland = glGetUniformLocation(SPObj_Iland, "u_lightingEnabled");

	status = 0;
	infoLoglength = 0;
	log = NULL;

	glGetProgramiv(SPObj_Iland, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(SPObj_Iland, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0)
		{
			log = (char *)malloc(infoLoglength);

			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(SPObj_Iland, infoLoglength, &written, log);

				fprintf(gpFile, "Shader Program of Iland Link log : %s\n", log);

				free(log);
				uninitialize();
			}
		}
	}

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	const GLfloat trianglePosition[] =
		{
			0.0f,
			1.0f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			-0.5f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			-0.3f,
			0.5f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			0.3f,
			0.5f,
			0.0f,
			0.0f,
			0.0f,
			0.0f,
		};

	const GLfloat triangleColor[] =
		{
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f};

	float angle = 0.0f;
	GLfloat leafPosition[66]; // 22 points * 3
	GLfloat leafColor[66];
	leafColor[0] = 0.0f;
	leafColor[0] = 1.0f;
	leafColor[0] = 0.0f;

	leafPosition[0] = 0.0f;
	leafPosition[1] = 0.0f;
	leafPosition[2] = 0.0f;
	for (int i = 3; i < 66;)
	{
		float rad = angle * PI / 180.0f;
		float x = 0.5f * cos(rad);
		leafPosition[i] = x;
		leafColor[i] = 0.0f;
		i++;
		float y = 0.5f * sin(rad);
		leafPosition[i] = y;
		leafColor[i] = 1.0f;
		i++;
		leafPosition[i] = 0.0f;
		leafColor[i] = 0.0f;
		i++;
		angle = angle + 18.0f;
	}

	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);

	// vbo for position
	glGenBuffers(1, &vbo_triangle_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trianglePosition), trianglePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// vbo for color
	glGenBuffers(1, &vbo_triangle_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// vao unbinding
	glBindVertexArray(0);

	// vao for leaf
	glGenVertexArrays(1, &vao_leaf);
	glBindVertexArray(vao_leaf);

	// vbo for leaf_position
	glGenBuffers(1, &vbo_leaf_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_leaf_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leafPosition), leafPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo for color
	glGenBuffers(1, &vbo_leaf_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_leaf_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leafColor), leafColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// unbind vao
	glBindVertexArray(0);

	// vao
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	// initializeCircle(0.5f, centerCoordinates, CIRCLE_POINTS, circleData);
	initCylinder(0.04f, centerCoordinates, CIRCLE_POINTS, 1.0f, cylinderData);
	generateTree(6);

	// vao and vbo related code
	glGenVertexArrays(1, &vao_cylender);
	glBindVertexArray(vao_cylender);
	glGenBuffers(1, &vbo_cylender);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cylender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderData), cylinderData, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal_cylender);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cylender);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderNormals), cylinderNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Here starts OpenGL code
	// clear the screen using sky blue color
	glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void printGLInfo()
{
	// local variable declarations
	GLint numExtension;

	// code
	fprintf(gpFile, "OpenGL Vendor: %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
	fprintf(gpFile, "Number of supported Extension: %d \n", numExtension);

	for (int i = 0; i < numExtension; i++)
		fprintf(gpFile, " %s \n", glGetStringi(GL_EXTENSIONS, i));
}

void resize(int width, int height)
{
	// code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void initializeCircle(GLfloat radius, GLfloat *center, int countOfPoints, GLfloat *circleVertices)
{
	for (int iCnt = 0; iCnt < countOfPoints; iCnt++)
	{
		float radian = (iCnt * 2) * M_PI / 180.0f;
		circleVertices[(iCnt * 3) + 0] = (radius * cos(radian)) + center[0];
		circleVertices[(iCnt * 3) + 1] = (radius * sin(radian)) + center[1];
		circleVertices[(iCnt * 3) + 2] = ((radius * 0.0f)) + center[2];
	}
}

void initCylinder(GLfloat radius, GLfloat *center, int countOfPoints, GLfloat height, GLfloat *cylinderVertices)
{
	GLfloat *circle = NULL;
	circle = (GLfloat *)malloc(countOfPoints * 3 * sizeof(GLfloat));
	initializeCircle(radius, center, countOfPoints, circle);
	for (int iCnt = 0; iCnt < countOfPoints; iCnt++)
	{
		cylinderVertices[(iCnt * 12) + 0] = circle[(iCnt * 3) + 0];
		cylinderVertices[(iCnt * 12) + 1] = circle[(iCnt * 3) + 1];
		cylinderVertices[(iCnt * 12) + 2] = circle[(iCnt * 3) + 2];

		cylinderNormals[(iCnt * 12) + 0] = (circle[(iCnt * 3) + 0] - center[0]) / radius;
		cylinderNormals[(iCnt * 12) + 1] = (circle[(iCnt * 3) + 1] - center[1]) / radius;
		cylinderNormals[(iCnt * 12) + 2] = (circle[(iCnt * 3) + 2] - center[2]) / radius;

		cylinderVertices[(iCnt * 12) + 6] = circle[((iCnt + 1) * 3) + 0];
		cylinderVertices[(iCnt * 12) + 7] = circle[((iCnt + 1) * 3) + 1];
		cylinderVertices[(iCnt * 12) + 8] = circle[((iCnt + 1) * 3) + 2];

		cylinderNormals[(iCnt * 12) + 6] = (circle[((iCnt + 1) * 3) + 0] - center[0]) / radius;
		cylinderNormals[(iCnt * 12) + 7] = (circle[((iCnt + 1) * 3) + 1] - center[1]) / radius;
		cylinderNormals[(iCnt * 12) + 8] = (circle[((iCnt + 1) * 3) + 2] - center[2]) / radius;
	}
	GLfloat heightCenter[] = {center[0], center[1], center[2] + height};
	initializeCircle(radius, heightCenter, countOfPoints, circle);
	for (int iCnt = 0; iCnt < countOfPoints; iCnt++)
	{
		cylinderVertices[(iCnt * 12) + 9] = circle[((iCnt + 1) * 3) + 0];
		cylinderVertices[(iCnt * 12) + 10] = circle[((iCnt + 1) * 3) + 1];
		cylinderVertices[(iCnt * 12) + 11] = circle[((iCnt + 1) * 3) + 2];

		cylinderNormals[(iCnt * 12) + 9] = (circle[((iCnt + 1) * 3) + 0] - heightCenter[0]) / radius;
		cylinderNormals[(iCnt * 12) + 10] = (circle[((iCnt + 1) * 3) + 1] - heightCenter[1]) / radius;
		cylinderNormals[(iCnt * 12) + 11] = (circle[((iCnt + 1) * 3) + 2] - heightCenter[2]) / radius;

		cylinderVertices[(iCnt * 12) + 3] = circle[(iCnt * 3) + 0];
		cylinderVertices[(iCnt * 12) + 4] = circle[(iCnt * 3) + 1];
		cylinderVertices[(iCnt * 12) + 5] = circle[(iCnt * 3) + 2];

		cylinderNormals[(iCnt * 12) + 3] = (circle[(iCnt * 3) + 0] - heightCenter[0]) / radius;
		cylinderNormals[(iCnt * 12) + 4] = (circle[(iCnt * 3) + 1] - heightCenter[1]) / radius;
		cylinderNormals[(iCnt * 12) + 5] = (circle[(iCnt * 3) + 2] - heightCenter[2]) / radius;
	}
	free(circle);
	circle = NULL;
	vec3 quadVertices[4];
	// for(int iCnt = 0; iCnt < countOfPoints; iCnt++)
	// {
	// 	quadVertices[0] = vec3(cylinderVertices[(iCnt * 12) + 0],cylinderVertices[(iCnt * 12) + 1],cylinderVertices[(iCnt * 12) + 2]);
	// 	quadVertices[1] = vec3(cylinderVertices[(iCnt * 12) + 3],cylinderVertices[(iCnt * 12) + 4],cylinderVertices[(iCnt * 12) + 5]);
	// 	quadVertices[2] = vec3(cylinderVertices[(iCnt * 12) + 6],cylinderVertices[(iCnt * 12) + 7],cylinderVertices[(iCnt * 12) + 8]);
	// 	quadVertices[3] = vec3(cylinderVertices[(iCnt * 12) + 9],cylinderVertices[(iCnt * 12) + 10],cylinderVertices[(iCnt * 12) + 11]);

	// 	vec3 crossResult = cross((quadVertices[2]-quadVertices[0]),(quadVertices[1]-quadVertices[0]));
	// 	crossResult = crossResult / length(crossResult);
	// 	cylinderNormals[(iCnt * 12)+0] = crossResult[0];
	// 	cylinderNormals[(iCnt * 12)+1] = crossResult[1];
	// 	cylinderNormals[(iCnt * 12)+2] = crossResult[2];

	// 	cylinderNormals[(iCnt * 12)+3] = crossResult[0];
	// 	cylinderNormals[(iCnt * 12)+4] = crossResult[1];
	// 	cylinderNormals[(iCnt * 12)+5] = crossResult[2];

	// 	cylinderNormals[(iCnt * 12)+6] = crossResult[0];
	// 	cylinderNormals[(iCnt * 12)+7] = crossResult[1];
	// 	cylinderNormals[(iCnt * 12)+8] = crossResult[2];

	// 	cylinderNormals[(iCnt * 12)+9] = crossResult[0];
	// 	cylinderNormals[(iCnt * 12)+10] = crossResult[1];
	// 	cylinderNormals[(iCnt * 12)+11] = crossResult[2];
	// }
}

void display(void)
{
	// Function declaration
	void push(mat4);
	void drawTree(int);
	mat4 pop();
	void drawLeaf();

	// Variable Declaration
	GLfloat water_pos[3 * 6 * TESSEL_X * TESSEL_Z];
	static float boatHeight = 0.0f;

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the shader program object
	glUseProgram(SPObj_Water);

	viewMatrix = lookat(vec3(camPosX, 0.0f, camPosZ), vec3(objPosX, objPosY, -10.0f), vec3(0.0f, 1.0f, 0.0f));
	push(modelMatrix);

	translationMatrix = vmath::translate(0.0f, -3.8f, -5.0f);
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniform_water, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_water, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_water, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight)
	{
		glUniform1i(lightingEnabledUnifrom_water, 1);
		glUniform3fv(laUniform_water, 1, lightAmbient);
		glUniform3fv(ldUniform_water, 1, lightDiffuse);
		glUniform3fv(lsUniform_water, 1, lightSpecular);
		glUniform4fv(lightPositionUniform_water, 1, lightPosition);
		glUniform3fv(kaUniform_water, 1, materialAmbient);
		glUniform3fv(kdUniform_water, 1, materialDiffuse);
		glUniform3fv(ksUniform_water, 1, materialSpecular);
		glUniform1f(materialShininessUniform_water, materialShininess);
	}
	else
	{
		glUniform1i(lightingEnabledUnifrom_water, 0);
	}

	int k = 0;
	for (int i = 0; i < TESSEL_Z; i++)
	{
		for (int j = 0; j < TESSEL_X; j++)
		{
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 0] = water_x[i + 0][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 1] = water_y[i + 0][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 2] = water_z[i + 0][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 3] = water_x[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 4] = water_y[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 5] = water_z[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 6] = water_x[i + 1][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 7] = water_y[i + 1][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 8] = water_z[i + 1][j + 0];

			water_pos[(i * TESSEL_X * 18) + (j * 18) + 9] = water_x[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 10] = water_y[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 11] = water_z[i + 0][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 12] = water_x[i + 1][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 13] = water_y[i + 1][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 14] = water_z[i + 1][j + 0];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 15] = water_x[i + 1][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 16] = water_y[i + 1][j + 1];
			water_pos[(i * TESSEL_X * 18) + (j * 18) + 17] = water_z[i + 1][j + 1];
		}
	}

	glBindVertexArray(VAO_WATER);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_POSITION_WATER);
	glBufferData(GL_ARRAY_BUFFER, sizeof(water_pos), water_pos, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, TESSEL_X * TESSEL_Z * 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	modelMatrix = pop();

	glUseProgram(0);

	// Boat
	glUseProgram(SPObj_Boat);
	// Tranformations

	push(modelMatrix);

	translationMatrix = vmath::translate(xTransBoat, -3.7f, -12.5f);

	scaleMatrix = vmath::scale(1.0f, 0.25f, 0.25f);
	modelMatrix = translationMatrix * scaleMatrix;

	glUniformMatrix4fv(modelMatrixUniform_boat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_boat, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_boat, 1, GL_FALSE, perspectiveProjectionMatrix);
	boatHeight = boatHeight + 0.01f;
	glUniform1f(yTransUniform_boat, sinf(boatHeight) * 0.05f);

	glBindVertexArray(VAO_BOAT);

	// Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 0 pasun start kar 3 vertices draw kar
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

	modelMatrix = pop();
	glUseProgram(0);

	// Iland
	glUseProgram(SPObj_Iland);
	// viewMatrix = mat4::identity();

	push(modelMatrix);
	glUniform1i(lightingEnabledUniform_Iland, 1);
	glUniform3fv(laUniform_Iland, 1, lightAmbient_Iland);
	glUniform3fv(ldUniform_Iland, 1, lightDiffuse_Iland);
	glUniform3fv(lsUniform_Iland, 1, lightSpecular_Iland);
	glUniform4fv(lightPositionUniform_Iland, 1, lightPosition_Iland);

	// ambient material
	MaterialAmbient_Iland[0] = 0.19125; // r
	MaterialAmbient_Iland[1] = 0.0735;	// g
	MaterialAmbient_Iland[2] = 0.0225;	// b
	MaterialAmbient_Iland[3] = 1.0f;	// a
	glUniform3fv(kaUniform_Iland, 1, MaterialAmbient_Iland);

	// diffuse material
	MaterialDiffuse_Iland[0] = 0.7038;	// r
	MaterialDiffuse_Iland[1] = 0.27048; // g
	MaterialDiffuse_Iland[2] = 0.0828;	// b
	MaterialDiffuse_Iland[3] = 1.0f;	// a
	glUniform3fv(kdUniform_Iland, 1, MaterialDiffuse_Iland);

	// specular material
	MaterialSpecular_Iland[0] = 0.256777; // r
	MaterialSpecular_Iland[1] = 0.137622; // g
	MaterialSpecular_Iland[2] = 0.086014; // b
	MaterialSpecular_Iland[3] = 1.0f;	  // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular_Iland);

	// shininess
	MaterialShininess_Iland = 0.1 * 128;
	glUniform1f(materialShinenessUniform_Iland, MaterialShininess_Iland);

	translationMatrix = translate(0.0f, -4.0f, -10.0f); // glTradslatef replaced by this line
	modelMatrix = translationMatrix;
	//	modelMatrix *= scale(4.0f,4.0f,4.0f);
	rotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix * rotationMatrix;

	drawTree(5);

	modelMatrix = pop();

	push(modelMatrix);

	glUniform1i(lightingEnabledUniform_Iland, 1);

	glUniform3fv(laUniform_Iland, 1, lightAmbient_Iland);
	glUniform3fv(ldUniform_Iland, 1, lightDiffuse_Iland);
	glUniform3fv(lsUniform_Iland, 1, lightSpecular_Iland);
	glUniform4fv(lightPositionUniform_Iland, 1, lightPosition_Iland);

	// ambient material
	MaterialAmbient_Iland[0] = 0.0;	 // r
	MaterialAmbient_Iland[1] = 0.0;	 // g
	MaterialAmbient_Iland[2] = 0.0;	 // b
	MaterialAmbient_Iland[3] = 1.0f; // a
	glUniform3fv(kaUniform_Iland, 1, MaterialAmbient_Iland);

	// diffuse material
	MaterialDiffuse_Iland[0] = 0.5;	 // r
	MaterialDiffuse_Iland[1] = 0.5;	 // g
	MaterialDiffuse_Iland[2] = 0.0;	 // b
	MaterialDiffuse_Iland[3] = 1.0f; // a
	glUniform3fv(kdUniform_Iland, 1, MaterialDiffuse_Iland);

	// specular material
	MaterialSpecular_Iland[0] = 0.60; // r
	MaterialSpecular_Iland[1] = 0.60; // g
	MaterialSpecular_Iland[2] = 0.50; // b
	MaterialSpecular_Iland[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular_Iland);

	// shininess
	MaterialShininess_Iland = 0.25 * 128;
	glUniform1f(materialShinenessUniform_Iland, MaterialShininess_Iland);

	modelMatrix *= translate(0.0f, -4.0f, -10.0f);
	modelMatrix *= scale(7.0f, 1.0f, 4.0f);

	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	// *** bind vao ***
	glBindVertexArray(gVao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);
	glUniform1i(lightingEnabledUniform_Iland, 0);
	modelMatrix = pop();

	push(modelMatrix);
	modelMatrix *= translate(xPosLeaf, yPosLeaf, zPosLeaf);
	modelMatrix *= rotate(35.0f, 0.0f, 0.0f, 1.0f);
	drawLeaf();
	modelMatrix = pop();
	// unUse the shader program object

	// Sun

	push(modelMatrix);

	glUniform1i(lightingEnabledUniform_Iland, 1);

	glUniform3fv(laUniform_Iland, 1, lightAmbient_Iland);
	glUniform3fv(ldUniform_Iland, 1, lightDiffuse_Iland);
	glUniform3fv(lsUniform_Iland, 1, lightSpecular_Iland);
	glUniform4fv(lightPositionUniform_Iland, 1, lightPosition_Iland);

	// ambient material
	MaterialAmbient_Iland[0] = 0.329412f;	 // r
	MaterialAmbient_Iland[1] = 0.223529f;	 // g
	MaterialAmbient_Iland[2] = 0.027451f;	 // b
	MaterialAmbient_Iland[3] = 1.0f; // a
	glUniform3fv(kaUniform_Iland, 1, MaterialAmbient_Iland);

	// diffuse material
	MaterialDiffuse_Iland[0] = 0.780392f;	 // r
	MaterialDiffuse_Iland[1] = 0.568627f;	 // g
	MaterialDiffuse_Iland[2] = 0.113725f;	 // b
	MaterialDiffuse_Iland[3] = 1.0f; // a
	glUniform3fv(kdUniform_Iland, 1, MaterialDiffuse_Iland);

	// specular material
	MaterialSpecular_Iland[0] = 0.992157f; // r
	MaterialSpecular_Iland[1] = 0.941176f; // g
	MaterialSpecular_Iland[2] = 0.807843f; // b
	MaterialSpecular_Iland[3] = 1.0f; // a
	glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular_Iland);

	// shininess
	MaterialShininess_Iland = 0.21794872 * 128;
	glUniform1f(materialShinenessUniform_Iland, MaterialShininess_Iland);

	modelMatrix *= translate(0.0f, transitionPoint, -100.0f);
	modelMatrix *= scale(10.0f, 10.0f, 10.0f);

	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	// *** bind vao ***
	glBindVertexArray(gVao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// *** unbind vao ***
	glBindVertexArray(0);
	glUniform1i(lightingEnabledUniform_Iland, 0);
	modelMatrix = pop();

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void push(mat4 matrix)
{
	if (top == 15)
	{
		fprintf(gpFile, "Stack is Full\n\n");
	}
	// matrixStack[top]=matrix;  This is shallow copy

	// for deep copy you need to copy all the data of matrix to matrix stack of top
	memcpy(matrixStack[top], matrix, sizeof(matrix));
	top++;
}

void drawTree(int height)
{
	// function declarations
	mat4 pop(void);
	void push(mat4);
	void traverseTree(struct tree *, int);

	srand(0);
	push(modelMatrix);
	{
		// Transformations
		glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);
		glBindVertexArray(vao_cylender);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS - 1) * 4);
		glBindVertexArray(0);
		modelMatrix *= translate(0.0f, 0.0f, (float)height * 0.2f);
		traverseTree(root, height - 1);
	}
	modelMatrix = pop();
}

void traverseTree(struct tree *node, int height)
{
	// function declarations
	mat4 pop(void);
	void push(mat4);

	push(modelMatrix);
	{
		if (node->branch1 != NULL && height != 0)
		{
			float rotateAngle = (float)(rand() % 40);
			srand(0);
			modelMatrix *= rotate(rotateAngle, 0.0f, 1.0f, 0.0f);
			// Transformations
			glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);
			glBindVertexArray(vao_cylender);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS - 1) * 4);
			glBindVertexArray(0);

			modelMatrix *= translate(0.0f, 0.0f, (float)height * 0.2f);

			traverseTree(node->branch1, height - 1);
		}
	}
	modelMatrix = pop();

	push(modelMatrix);
	{
		if (node->branch2 != NULL && height != 0)
		{
			float rotateAngle = (float)(rand() % 40);
			srand(10);
			modelMatrix *= rotate(rotateAngle, 0.0f, -1.0f, 0.0f);

			// Transformations
			glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);
			glBindVertexArray(vao_cylender);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS - 1) * 4);
			glBindVertexArray(0);
			modelMatrix *= translate(0.0f, 0.0f, (float)height * 0.2f);
			traverseTree(node->branch2, height - 1);
		}
	}
	modelMatrix = pop();
}
void drawLeaf()
{
	// function declarations
	mat4 pop(void);
	void push(mat4);

	push(modelMatrix);
	// modelMatrix *= translate(0.0f, 0.0f, -7.0f);
	modelMatrix *= scale(0.1f, 0.1f, 0.1f);
	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao_triangle);

	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_LINES, 2, 2);
	glDrawArrays(GL_LINES, 4, 2);
	glDrawArrays(GL_LINES, 6, 2);

	glBindVertexArray(0);

	modelMatrix = pop();

	push(modelMatrix);
	modelMatrix *= translate(0.0f, 0.1f, 0.0f);
	modelMatrix *= scale(0.05f, 0.1f, 0.05f);

	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao_leaf);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

	glBindVertexArray(0);
	modelMatrix = pop();

	push(modelMatrix);
	modelMatrix *= translate(-0.05f, 0.07f, 0.0f);
	modelMatrix *= rotate(35.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix *= scale(0.03f, 0.06f, 0.03f);
	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao_leaf);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

	glBindVertexArray(0);
	modelMatrix = pop();

	push(modelMatrix);
	modelMatrix *= translate(0.05f, 0.07f, 0.0f);
	modelMatrix *= rotate(325.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix *= scale(0.03f, 0.06f, 0.03f);
	glUniformMatrix4fv(modelMatrixUniform_Iland, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_Iland, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_Iland, 1, GL_FALSE, perspectiveProjectionMatrix);

	glBindVertexArray(vao_leaf);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

	glBindVertexArray(0);
	modelMatrix = pop();
}

mat4 pop()
{
	if (top == 0)
	{
		fprintf(gpFile, "Stack is Empty\n\n");
	}
	top = top - 1;
	return (matrixStack[top]);
}

void update(void)
{
	// code
	// Water
	time += 0.01f;

	for (int i = 0; i <= TESSEL_Z; i++)
	{
		for (int j = 0; j <= TESSEL_X; j++)
		{
			float temp_x = (float)(j + 1) / (float)TESSEL_X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)(i + 1) / (float)TESSEL_Z;
			temp_z = (temp_z * 2.0f) - 1.0f;
			water_y[i][j] = sin(temp_x * freq + time) * sin(temp_z * freq + time) * 0.07f;
		}
	}

	xTransBoat = xTransBoat + 0.001f;
	if (scene == 1)
	{
		if (xTransBoat > -9.0f)
		{
			camPosZ = camPosZ - 0.0014f;
			objPosY = objPosY - 0.0002f;

			if (objPosY < -1.0f || camPosZ < -7.0f)
			{
				scene = 2;
			}
		}
	}

	if (scene == 2)
	{

		xPosLeaf = xPosLeaf + 0.0001f;
		yPosLeaf = yPosLeaf - 0.001f;
		zPosLeaf = zPosLeaf - 0.0012f;
		if (xTransBoat >= -1.840619)
		{
			scene = 3;
			// yPosLeaf = -3.559915f;
			// zPosLeaf = -12.591400f;
		}
	}

	if (scene == 3)
	{
		xPosLeaf = xPosLeaf + 0.001f;
		camPosX = camPosX + 0.001f;
		objPosX = objPosX + 0.001f;

		if (xTransBoat >= 4.531265)
		{
			camPosZ = camPosZ + 0.0014f;
			objPosY = objPosY + 0.0002f;
			if (objPosY > 0.0f || camPosZ > 0.0f)
			{
				camPosZ = 0.0f;
				objPosY = 0.0f;
				transitionPoint = transitionPoint + 0.01f;
			}
		}
	}

	// Iland
	//  const float radian = M_PI / 180.0f;
	//  static float angle=0.0f;
	//  // Code
	//  xRotate = cameraRadius * cos(M_PI * (angle / 180.0f));

	// zTranslate = cameraRadius * sin(M_PI * (angle / 180.0f));

	// angle += 0.1f;
	// if(angle >= 360.0f)
	// 	angle -= 360.0f;
}

void uninitialize(void)
{
	// function declarations
	void toggleFullScreen(void);

	// code
	if (gbFullScreen)
		toggleFullScreen();
	if (VBO_POSITION_WATER)
	{
		glDeleteBuffers(1, &VBO_POSITION_WATER);
		VBO_POSITION_WATER = 0;
	}

	if (VBO_boat_position)
	{
		glDeleteBuffers(1, &VBO_boat_position);
		VBO_boat_position = 0;
	}

	if (VBO_boat_color)
	{
		glDeleteBuffers(1, &VBO_boat_color);
		VBO_boat_color = 0;
	}
	// Deletetion and Uninitialization of VAO_WATER
	if (VAO_BOAT)
	{
		glDeleteVertexArrays(1, &VAO_BOAT);
		VAO_BOAT = 0;
	}

	if (vbo_cylender)
	{
		glDeleteBuffers(1, &vbo_cylender);
		vbo_cylender = 0;
	}

	// Deletion and uninitialization of vao
	if (vao_cylender)
	{
		glDeleteVertexArrays(1, &vao_cylender);
		vao_cylender = 0;
	}

	// if (VBO_NORMAL) {
	// 	glDeleteBuffers(1, &VBO_NORMAL);
	// 	VBO_NORMAL = 0;
	// }
	if (VAO_WATER)
	{
		glDeleteVertexArrays(1, &VAO_WATER);
		VAO_WATER = 0;
	}
	if (SPObj_Water)
	{
		GLsizei numAttachedShaders;
		GLuint *shaderObject = NULL;
		glUseProgram(SPObj_Water);
		glGetProgramiv(SPObj_Water, GL_ATTACHED_SHADERS, &numAttachedShaders);
		shaderObject = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));
		glGetAttachedShaders(SPObj_Water, numAttachedShaders, &numAttachedShaders, shaderObject);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(SPObj_Water, shaderObject[i]);
			glDeleteShader(shaderObject[i]);
			shaderObject[i] = 0;
		}
		free(shaderObject);
		shaderObject = NULL;
		glUseProgram(0);
		glDeleteProgram(SPObj_Water);
		SPObj_Water = 0;
	}
	if (wglGetCurrentContext() == ghrc)
		wglMakeCurrent(NULL, NULL);
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
	if (gpFile)
	{
		fprintf(gpFile, "\nLog file succefully closed");
		fclose(gpFile);
		gpFile = NULL;
	}
}
