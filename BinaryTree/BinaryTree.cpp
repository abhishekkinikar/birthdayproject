// Header Files
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include <malloc.h>

// OpenGL Header files
#include<GL/glew.h>  // This must be above #include<GL/gl.h>
#include<GL/gl.h>
#include"vmath.h"
#include"BinaryTree.h"
#include"tree.h"
using namespace vmath;

#define WIN_WIDTH 600
#define WIN_HEIGHT 500
#define CIRCLE_POINTS 181
#define PI 3.14159235

// OpenGL Libraries
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"Sphere.lib")

// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;

// Programmable Pipeline related global variables
GLuint shaderProgramObject;
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

GLuint vao_triangle; // vao : vertex array object
GLuint vbo_triangle_position; // vbo_position : vertex buffer object
GLuint vbo_triangle_color;

enum
{
	ARK_ATTRIBUTE_POSITION=0,
	ARK_ATTRIBUTE_COLOR,
	ARK_ATTRIBUTE_NORMAL,
	ARK_ATTRIBUTE_TEXTURE0
};
GLuint vao;
GLuint vbo;
GLuint vbo_normal;
GLuint mvpMatrixUniform;
mat4 perspectiveProjectionMatrix;

mat4 translationMatrix = mat4::identity();
mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();
mat4 scaleMatrix = mat4::identity();
mat4 rotationMatrix = mat4::identity();

GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;
GLuint lightingEnabledUniform;

GLuint laUniform;   // Light Ambient
GLuint ldUniform;	// Light Diffuse
GLuint lsUniform;	// Light Specular
GLuint lightPositionUniform;

GLuint kaUniform;   
GLuint kdUniform;	
GLuint ksUniform;
GLuint materialShinenessUniform;

mat4 matrixStack[16];
int top=0;
float zTranslate=5.0f;
float xRotate=0.0f;
float cameraRadius=30.0f;
//GLfloat *triangleVertices=NULL;

GLfloat circleData[CIRCLE_POINTS * 3];
GLfloat centerCoordinates[] = { 0.0f, 0.0f, 0.0f };
GLfloat cylinderData[CIRCLE_POINTS * 4 * 3];
GLfloat cylinderNormals[CIRCLE_POINTS * 4 * 3];
float angleCylinder = 0.0f;

GLfloat lightAmbient[] = { 1.0,1.0,1.0,1.0 };
GLfloat lightDiffuse[] = { 1.0,1.0,1.0,1.0 };
GLfloat lightSpecular[] = { 1.0,1.0,1.0,1.0 };
GLfloat lightPosition[] = { 0.0,100.0,0.0,1.0 };

GLfloat MaterialAmbient[4];
GLfloat MaterialDiffuse[4];
GLfloat MaterialSpecular[4];
GLfloat MaterialShininess;

// Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);
	
	// Variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Perspective_Triangle");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// Code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of log file failed. Exitting..."), TEXT("File I/O Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is successfully created.\n");
	}

	// Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Registering above WNDCLASSEX
	RegisterClassEx(&wndclass);

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("ARK:Tree"),
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE,
		400,
		200,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// Initialize
	iRetVal = initialize();

	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel Format failed\n\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format failed\n\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL Context failed\n\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL Context as current context failed\n\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "GLEW Initialization failed\n\n");
		uninitialize();
	}
	else
	{
		fprintf(gpFile, "Making OpenGL Context Successfull\n\n");
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	// Foregrounding and focusing thw window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
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
				// Render The scene 
				display();

				// Update The Scene
				update();
			}
		}
	}
	uninitialize();
	return((int)msg.wParam);
}

// CallBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullScreen();
	void resize(int, int);

	// Code
	switch (iMsg)
	{
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
	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullScreen();
			break;
		default:
			break;
		}
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:
		return(0);
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
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	// Variable declarations
	static DWORD dwstyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	// Code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}
int initialize(void)
{
	// Function declarations
	void uninitialize(void);
	void printGLInfo(void);
	void resize(int, int);
	void initializeCircle(GLfloat radius, GLfloat *centre, int countOfPoints, GLfloat * circleVertices);
	void initCylinder(GLfloat radius, GLfloat * center, int countOfPoints, GLfloat height, GLfloat *cylinderVertices);

	// Variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	// Code
	// Initialization of PIXELFORMATDESCRIPTOR structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));  // memset((void *) &pfd, NULL, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// Get DC
	ghdc = GetDC(ghwnd);

	// Choose Pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
		return(-1);

	// Set choosen Pixel format 
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return(-2);
			
	// Create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return(-3);

	// Make The rendering context as current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return(-4);

	
	// glew initialization
	if (glewInit() != GLEW_OK)
		return(-5);

	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 440 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec4 a_color;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"out vec4 a_color_out;" \
		"void main(void)" \
		"{" \
			"if(u_lightingEnabled == 1)" \
				"{" \
					"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
					"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
					"transformedNormals = normalMatrix * a_normal;" \
					"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
					"viewerVector = -eyeCoordinates.xyz;" \
				"}" \
			"gl_Position = u_projectionMatrix * u_viewMatrix* u_modelMatrix * a_position;" \
			"a_color_out = a_color;" \
		"}";

	// Create shader object
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// Give source code to shader object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	//Compile Shader
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLenth;
	char* log = NULL;

	// Getting length of log of compilation status
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLenth);
		if (infoLogLenth > 0)
		{
			log = (char*)malloc(infoLogLenth);

			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLenth, &written, log);

				fprintf(gpFile, "Vertex shader compilation log : %s\n", log);

				free(log);

				uninitialize();
			}
		}
	}

	// Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 440 core" \
		"\n" \
		"in vec3 transformedNormals;\n" \
		"in vec3 lightDirection;\n" \
		"in vec3 viewerVector;\n" \
		"in vec4 a_color_out;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShineness;" \
		"uniform int u_lightingEnabled;" \
		"vec3 fong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
			"if(u_lightingEnabled == 1)" \
			"{" \
				"vec3 ambient=u_la * u_ka;" \
				"vec3 normalized_transformedNormals = normalize(transformedNormals);" \
				"vec3 normalized_lightDirection = normalize(lightDirection);" \
				"vec3 diffuse=u_ld * u_kd * max(dot(normalized_lightDirection,normalized_transformedNormals),0.0);" \
				"vec3 reflectionVector=reflect(-normalized_lightDirection,normalized_transformedNormals);" \
				"vec3 normalized_viewerVector = normalize(viewerVector);" \
				"vec3 specular=u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewerVector),0.0),u_materialShineness);" \
				"fong_ads_light = ambient + diffuse + specular;" \
			"}" \
			"else" \
			"{" \
			"	fong_ads_light=vec3(1.0,1.0,1.0);" \
			"}" \
			"FragColor = vec4(fong_ads_light,1.0);" \
		"}";
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);
	status = 0;
	infoLogLenth = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLenth);
		if (infoLogLenth > 0)
		{
			log = (char*)malloc(infoLogLenth);

			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLenth, &written, log);
				fprintf(gpFile, "Fragment shader compilation log : %s \n", log);

				free(log);
				uninitialize();
			}
		}
	}

	// Shader program object.
	shaderProgramObject = glCreateProgram();

	// Attach desire shader
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	// Pre linked binding
	glBindAttribLocation(shaderProgramObject, ARK_ATTRIBUTE_POSITION, "a_position");  // ***Andhar***		
	glBindAttribLocation(shaderProgramObject, ARK_ATTRIBUTE_NORMAL, "a_normal");
	glBindAttribLocation(shaderProgramObject, ARK_ATTRIBUTE_COLOR, "a_color");  // ***Andhar***		

	// Link program
	glLinkProgram(shaderProgramObject);

	// Post link retriving
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix"); // ***Andhar***
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix"); // ***Andhar***
	
	laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

	kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
	materialShinenessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShineness");

	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

	status = 0;
	infoLogLenth = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLenth);
		if (infoLogLenth > 0)
		{
			log = (char*)malloc(infoLogLenth);

			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLenth, &written, log);

				fprintf(gpFile, "Shader Program Link log : %s\n", log);

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
		0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f,
		0.0f, 0.0f, 0.0f,
		-0.3f, 0.5f, 0.0f, 
		0.0f, 0.0f, 0.0f,
		0.3f, 0.5f, 0.0f, 
		0.0f, 0.0f, 0.0f,
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
		0.0f, 1.0f, 0.0f
	};

	float angle = 0.0f;
	GLfloat leafPosition[66]; // 22 points * 3
	GLfloat leafColor[66];
	leafColor[0] = 0.0f;
	leafColor[0] = 1.0f;
	leafColor[0] = 0.0f;

	leafPosition[0] = 0.0f;
	leafPosition[1] = 0.0f;
	leafPosition[2] = 0.0f;
	for(int i = 3; i < 66; )
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
	glVertexAttribPointer(ARK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ARK_ATTRIBUTE_POSITION);

	// vbo for color 
	glGenBuffers(1, &vbo_triangle_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
	glVertexAttribPointer(ARK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ARK_ATTRIBUTE_COLOR);
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
	glVertexAttribPointer(ARK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ARK_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo for color 
	glGenBuffers(1, &vbo_leaf_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_leaf_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leafColor), leafColor, GL_STATIC_DRAW);
	glVertexAttribPointer(ARK_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ARK_ATTRIBUTE_COLOR);
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

	glVertexAttribPointer(ARK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(ARK_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(ARK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(ARK_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	//initializeCircle(0.5f, centerCoordinates, CIRCLE_POINTS, circleData);
	initCylinder(0.04f, centerCoordinates, CIRCLE_POINTS, 1.0f, cylinderData);
	generateTree(6);

	// vao and vbo related code
	glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderData), cylinderData, GL_STATIC_DRAW);
			glVertexAttribPointer(ARK_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ARK_ATTRIBUTE_POSITION);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glGenBuffers(1, &vbo_normal);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderNormals), cylinderNormals, GL_STATIC_DRAW);
			glVertexAttribPointer(ARK_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(ARK_ATTRIBUTE_NORMAL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Clear the screen using below color
	glClearColor(0.7f, 1.0f, 1.0f, 1.0f);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Print OpenGL info
	printGLInfo();

	perspectiveProjectionMatrix = mat4::identity();
	//warm up resize call
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void printGLInfo(void)
{
	// Local variable declarations
	GLint numExtensions=0;

	//Code
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); // GLSL Graphics library Shading Language
	
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	fprintf(gpFile, "Number of Supported Extensions : %d \n", numExtensions);

	for (int i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
}

void resize(int width, int height)
{
	// Code
	if (height == 0)
	{
		height = 1;  // To avoid devided by 0 in future code
	}
	glViewport(0, 0, width, height);

	perspectiveProjectionMatrix = vmath::perspective(45.0, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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

void initCylinder(GLfloat radius, GLfloat* center, int countOfPoints, GLfloat height, GLfloat* cylinderVertices)
{
	GLfloat* circle = NULL;
	circle = (GLfloat*)malloc(countOfPoints * 3 * sizeof(GLfloat));
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
	GLfloat heightCenter[] = { center[0], center[1], center[2] + height };
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

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Actual window color blue using this funcion 
	
	// Use the shader program object
	glUseProgram(shaderProgramObject);
	viewMatrix = mat4::identity();

	viewMatrix = lookat(vec3(xRotate,0.0f,zTranslate), vec3(0.0f,0.0f,-1.0f), vec3(0.0f,1.0f,0.0f));

	push(modelMatrix);
		glUniform1i(lightingEnabledUniform, 1);
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		// ambient material
		MaterialAmbient[0] = 0.19125; // r
		MaterialAmbient[1] = 0.0735;  // g
		MaterialAmbient[2] = 0.0225;  // b
		MaterialAmbient[3] = 1.0f;    // a
		glUniform3fv(kaUniform, 1, MaterialAmbient);

		// diffuse material
		MaterialDiffuse[0] = 0.7038;  // r
		MaterialDiffuse[1] = 0.27048; // g
		MaterialDiffuse[2] = 0.0828;  // b
		MaterialDiffuse[3] = 1.0f;    // a
		glUniform3fv(kdUniform, 1, MaterialDiffuse);

		// specular material
		MaterialSpecular[0] = 0.256777; // r
		MaterialSpecular[1] = 0.137622; // g
		MaterialSpecular[2] = 0.086014; // b
		MaterialSpecular[3] = 1.0f;     // a
		glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

		// shininess
		MaterialShininess = 0.1 * 128;
		glUniform1f(materialShinenessUniform,MaterialShininess);

		translationMatrix = translate(0.0f, -4.0f, -10.0f); // glTradslatef replaced by this line
		modelMatrix = translationMatrix;
	//	modelMatrix *= scale(4.0f,4.0f,4.0f);
		rotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
		modelMatrix = modelMatrix * rotationMatrix;

			drawTree(5);
					
	modelMatrix = pop();

	push(modelMatrix);

		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		// ambient material
		MaterialAmbient[0] = 0.0;  // r
		MaterialAmbient[1] = 0.0;  // g
		MaterialAmbient[2] = 0.0;  // b
		MaterialAmbient[3] = 1.0f; // a
		glUniform3fv(kaUniform, 1, MaterialAmbient);

		// diffuse material
		MaterialDiffuse[0] = 0.5;  // r
		MaterialDiffuse[1] = 0.5;  // g
		MaterialDiffuse[2] = 0.0;  // b
		MaterialDiffuse[3] = 1.0f; // a
		glUniform3fv(kdUniform, 1, MaterialDiffuse);

		// specular material
		MaterialSpecular[0] = 0.60; // r
		MaterialSpecular[1] = 0.60; // g
		MaterialSpecular[2] = 0.50; // b
		MaterialSpecular[3] = 1.0f; // a
		glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);

		// shininess
		MaterialShininess = 0.25 * 128;
		glUniform1f(materialShinenessUniform,MaterialShininess);

		modelMatrix *= translate(0.0f, -4.0f, -10.0f);
		modelMatrix *= scale(7.0f, 1.0f, 4.0f);

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

		// *** bind vao ***
		glBindVertexArray(gVao_sphere);

		// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		// *** unbind vao ***
		glBindVertexArray(0);
	modelMatrix = pop();

	push(modelMatrix);
		modelMatrix *= translate(-1.2f,-1.4f,-10.0f);
		modelMatrix *= rotate(35.0f,0.0f,0.0f,1.0f);
		drawLeaf();
	modelMatrix = pop();
	// unUse the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);

}

void push(mat4 matrix)
{
	if(top==15)
	{
		fprintf(gpFile, "Stack is Full\n\n");
	}
	//matrixStack[top]=matrix;  This is shallow copy

	// for deep copy you need to copy all the data of matrix to matrix stack of top
	memcpy(matrixStack[top], matrix, sizeof(matrix));
	top++;
}

mat4 pop()
{
	if (top == 0)
	{
		fprintf(gpFile, "Stack is Empty\n\n");
	}
	top = top - 1;
	return(matrixStack[top]);
}

void drawTree(int height)
{
	// function declarations
	mat4 pop();
	void traverseTree(struct tree*, int);

	srand(0);
	push(modelMatrix);
	{
		// Transformations
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
		glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS-1)*4);
		glBindVertexArray(0);		
		modelMatrix *= translate(0.0f, 0.0f, (float)height * 0.2f);	
		traverseTree(root, height - 1);
	}
	modelMatrix = pop();
}

void traverseTree(struct tree* node, int height)
{
	push(modelMatrix);
	{
		if (node->branch1 != NULL && height != 0)
		{
			float rotateAngle = (float)(rand() % 40);
			srand(0);
			modelMatrix *= rotate(rotateAngle, 0.0f,1.0f,0.0f);
			// Transformations
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
			glBindVertexArray(vao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS-1)*4);
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
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);
			glBindVertexArray(vao);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, (CIRCLE_POINTS-1)*4);
			glBindVertexArray(0);
			modelMatrix *= translate(0.0f, 0.0f, (float)height * 0.2f);	
			traverseTree(node->branch2, height - 1);
		}
	}
	modelMatrix = pop();
}
void drawLeaf()
{

	push(modelMatrix);
		//modelMatrix *= translate(0.0f, 0.0f, -7.0f);
		modelMatrix *= scale(0.1f, 0.1f, 0.1f);
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

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

		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

		glBindVertexArray(vao_leaf);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

		glBindVertexArray(0);
	modelMatrix = pop();

	push(modelMatrix);
		modelMatrix *= translate(-0.05f, 0.07f, 0.0f);
		modelMatrix *= rotate(35.0f, 0.0f, 0.0f, 1.0f);
		modelMatrix *= scale(0.03f, 0.06f, 0.03f);
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

		glBindVertexArray(vao_leaf);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

		glBindVertexArray(0);
	modelMatrix = pop();

	push(modelMatrix);
		modelMatrix *= translate(0.05f, 0.07f, 0.0f);
		modelMatrix *= rotate(325.0f, 0.0f, 0.0f, 1.0f);
		modelMatrix *= scale(0.03f, 0.06f, 0.03f);
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
		glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

		glBindVertexArray(vao_leaf);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 22);

		glBindVertexArray(0);
	modelMatrix = pop();
}
void update(void)
{
	const float radian = M_PI / 180.0f;
	static float angle=0.0f;
	// Code
	xRotate = cameraRadius * cos(M_PI * (angle / 180.0f));

	zTranslate = cameraRadius * sin(M_PI * (angle / 180.0f));

	angle += 0.1f;
	if(angle >= 360.0f)
		angle -= 360.0f;
}
void uninitialize(void)
{
	// Function declarations
	void ToggleFullScreen(void);

	// Code
	if (gbFullScreen)
	{
		ToggleFullScreen();
	}
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	// Deletion and uninitialization of vbo
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	// Deletion and uninitialization of vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	// Shader uninitialization
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLsizei numAttachedShaders;

		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;

			if (shaderObjects)
			{
				free(shaderObjects);
				shaderObjects = NULL;

				glUseProgram(0);
				
				glDeleteProgram(shaderProgramObject);
				shaderProgramObject = 0;
			}
		}
	}
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
		fprintf(gpFile, "Log file successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
