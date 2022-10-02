// Header files
#include<windows.h>
#include "OGL.h"
#include<stdio.h>//for file IO operation
#include<stdlib.h>

//This must be befor including GL.h
#include <GL/glew.h>
//OpenGL Header Files
#include <GL/GL.h>
#include "vmath.h"
using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

// Global Funtion declarations
FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HWND ghwnd = NULL;
HDC ghDC = NULL;
HGLRC ghlrc = NULL;

BOOL gbFullScreen = FALSE;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Programmable Pipeline related global variables
GLuint SPObj_Boat;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
};



GLuint VAO_BOAT;
GLuint VBO_boat_position;
GLuint VBO_boat_texcoord;

GLuint VAO_newboat;
GLuint VBO_boat_newposition;
GLuint VBO_boat_newtexcoord;

GLuint modelMatrixUniform_boat;
GLuint viewMatrixUniform_boat;
GLuint projectionMatrixUniform_boat;
GLuint TSU_boat;

mat4  perspectiveProjectionMatrix;
mat4 translationMatrix = mat4::identity();
mat4 modelMatrix = mat4::identity();
mat4 viewMatrix = mat4::identity();


GLfloat anglecube;

GLuint texture_wooden;



// Entry Point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(VOID);
	VOID display(VOID);
	VOID update(VOID);
	VOID uninitialize(VOID);

	// Variable Declarations
	HWND hwnd;
	WNDCLASSEX WndClass;
	MSG msg;
	TCHAR szAppName[] = TEXT("My Window");
	int height, width;
	int centrex, centrey;
	BOOL bDone = FALSE;
	int iRetVal = 0;

	// code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log file failed. Exiting "), TEXT("File I/O"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File is successfully created.\n");
	}
	// Initialization of WNDCLASSEX structure
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.lpszClassName = szAppName;
	WndClass.lpszMenuName = NULL;
	WndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	RegisterClassEx(&WndClass);

	height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	width = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	centrex = width - WIN_WIDTH;
	centrey = height - WIN_HEIGHT;

	// Create the Window 
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName, 
		TEXT("PP: 3D Texture"), 
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 
		centrex / 2, 
		centrey / 2, 
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
		fprintf(gpFile, "Choose pixel formal failed\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format Failed\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL context failed\n");
		uninitialize();
	}
	else if(iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL context as current Context failed\n");
		uninitialize();
	}
	else if(iRetVal == -5)
	{
		fprintf(gpFile, "glewInit() failed\n");
		uninitialize();
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	//Foregrounding and focusing the window

	SetForegroundWindow(hwnd); 
	SetFocus(hwnd);


	//Game Loop 
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
				//Render the Scene
				display();

				//Update the scene
				update();
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	VOID ToggleFullScreen();
	VOID resize(int, int);
	VOID uninitialize(VOID);

	switch (iMsg)
	{
	case WM_SETFOCUS:
		fprintf(gpFile, "Window Focus is Set\n");
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		fprintf(gpFile, "Window Focus is Killed\n");
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE: 
		resize(LOWORD(lParam), HIWORD(lParam));

		break;

	case WM_CLOSE: 
		DestroyWindow(hwnd);

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
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			fprintf(gpFile, "Closed the Window\n");
			if (gpFile)
			{
				fprintf(gpFile, "Log File is SuccessFully Closed.\n");
				fclose(gpFile);
				gpFile = NULL;
			}
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

VOID ToggleFullScreen()
{
	//Variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullScreen == FALSE)
	{
		fprintf(gpFile, "Show Full Screen Window\n");
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
		fprintf(gpFile, "Exit Full Screen Window\n");
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(VOID)
{
	//Function Declarations
	VOID printGLInfo(VOID);
	void uninitialize(void);
	VOID resize(int width, int height);
	BOOL loadGLTexture(GLuint*, TCHAR[]);
	//Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
		
	//Code 

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	//Initialization of PIXELFORMATDESCRIPTOR
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

	//GetDC
	ghDC = GetDC(ghwnd);

	//Choose pixel format
	iPixelFormatIndex = ChoosePixelFormat(ghDC, &pfd);
	if (iPixelFormatIndex == 0)
	{
		return -1;
	}

	//Set the chossen Pixel Format
	if (SetPixelFormat(ghDC, iPixelFormatIndex, &pfd) == FALSE)
	{
		return -2;
	}

	//Create OpenGL REndering COntext
	ghlrc = wglCreateContext(ghDC);
	if (ghlrc == NULL)
	{
		return -3;
	}

	//Make the rendering context as current context.
	if (wglMakeCurrent(ghDC, ghlrc) == FALSE)
	{
		return -4;
	}

	//Glew initialization
	if( glewInit() != GLEW_OK)
	{
		return -5;
	}
	
	//print OpenGL Info
	printGLInfo();

	//Vertex Shader
	//Source code
	const GLchar* vertexShaderSourceCode =
		"#version 440 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec2 a_texcoord;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec2 a_texcoord_out;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_projectionMatrix * u_viewMatrix* u_modelMatrix * a_position;" \
		"a_texcoord_out = a_texcoord;" 
		"}";
	//Object create kel
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Tyala code dila
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL); 
	//Compile kela ithe
	glCompileShader(vertexShaderObject);
	//Error Checking
	GLint status;
	GLint infoLogLength;
	CHAR* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);//
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Vertex Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	
	//fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 440 core" \
		"\n" \
		"in vec2 a_texcoord_out;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_textureSampler, a_texcoord_out);" \
		"}";

	//Object creation
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Tyala 
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL); 
	//Compile
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (CHAR*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "Fragment Shader Compilation Log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}

	//Shader program Object
	//Create shader object
	SPObj_Boat = glCreateProgram();
	//Attach shader object
	glAttachShader(SPObj_Boat, vertexShaderObject);
	glAttachShader(SPObj_Boat, fragmentShaderObject);

	//
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_POSITION, "a_position"); 
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");   
	//Link the shader program object
	glLinkProgram(SPObj_Boat);

	// linking che Error
	status = 0;
	infoLogLength = 0;
	log = NULL;

	modelMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_modelMatrix");
	viewMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_viewMatrix"); // ***Andhar***
	projectionMatrixUniform_boat = glGetUniformLocation(SPObj_Boat, "u_projectionMatrix"); // ***Andhar***
	TSU_boat = glGetUniformLocation(SPObj_Boat, "u_textureSampler"); 
	glGetProgramiv(SPObj_Boat, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(SPObj_Boat, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength >= 0)
		{
			log = (CHAR*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(SPObj_Boat, infoLogLength, &written, log);
				fprintf(gpFile, "Shader program's link log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	//Declaration of vertex data arrays
	/*const GLfloat pyramidPosition[] =
	{
		// front
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// right
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		// back
		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		// left
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f

	};

	const GLfloat pyramidTexcoord[] =
	{
		0.5, 1.0, // front-top
		0.0, 0.0, // front-left
		1.0, 0.0, // front-right

		0.5, 1.0, // right-top
		1.0, 0.0, // right-left
		0.0, 0.0, // right-right

		0.5, 1.0, // back-top
		1.0, 0.0, // back-left
		0.0, 0.0, // back-right

		0.5, 1.0, // left-top
		0.0, 0.0, // left-left
		1.0, 0.0// left-right
	};*/

	const GLfloat cubePosition[] =
	{
		//top
		/*1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,*/

		// bottom
		0.5f, -0.5f, -1.0f,
	   -0.5f, -0.5f, -1.0f,
	   -0.5f, -0.5f,  1.0f,
		0.5f, -0.5f,  1.0f,

		// front
		1.0f, 1.0f, 1.0f,
	   -1.0f, 1.0f, 1.0f,
	   -0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,

		// back
		1.0f, 1.0f, -1.0f,
	   -1.0f, 1.0f, -1.0f,
	   -0.5f, -0.5f, -1.0f,
		0.5f, -0.5f, -1.0f,

		// right
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, -1.0f,

		// left
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-0.5f, -0.5f, -1.0f,
		-0.5f, -0.5f, 1.0f,
	};

	const GLfloat cubeTexcoord[] =
	{
		0.0f, 0.0f, 
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	//new cube
	const GLfloat newcubePosition[] =
	{
		//top
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// bottom
		0.5f, -0.5f, -1.0f,
	   -0.5f, -0.5f, -1.0f,
	   -0.5f, -0.5f,  1.0f,
		0.5f, -0.5f,  1.0f,

		// front
		1.0f, 1.0f, 1.0f,
	   -1.0f, 1.0f, 1.0f,
	   -0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, 1.0f,

		// back
		1.0f, 1.0f, -1.0f,
	   -1.0f, 1.0f, -1.0f,
	   -0.5f, -0.5f, -1.0f,
		0.5f, -0.5f, -1.0f,

		// right
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f,
		0.5f, -0.5f, -1.0f,

		// left
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-0.5f, -0.5f, -1.0f,
		-0.5f, -0.5f, 1.0f,
	};

	const GLfloat newcubeTexcoord[] =
	{
		0.0f, 0.0f, 
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	/*//pyramid
	//VAO and VBO related code
	//VAO
	glGenVertexArrays(1, &VAO_pyramid); //5th step
	glBindVertexArray(VAO_pyramid);
	//VBO for position
	glGenBuffers(1, &VBO_pyramid_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pyramid_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPosition), pyramidPosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for color
	glGenBuffers(1, &VBO_pyramid_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pyramid_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoord), pyramidTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);*/

	//cube
	//VAO and VBO related code
	//VAO
	glGenVertexArrays(1, &VAO_BOAT); //5th step
	glBindVertexArray(VAO_BOAT);
	//VBO for position
	glGenBuffers(1, &VBO_boat_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for texture
	glGenBuffers(1, &VBO_boat_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoord), cubeTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//new cube 
	//cube
	//VAO and VBO related code
	//VAO
	glGenVertexArrays(1, &VAO_newboat); //5th step
	glBindVertexArray(VAO_newboat);
	//VBO for position
	glGenBuffers(1, &VBO_boat_newposition);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_newposition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(newcubePosition), newcubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for texture
	glGenBuffers(1, &VBO_boat_newtexcoord);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_newtexcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(newcubeTexcoord), newcubeTexcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Deprecated
	//glShadeModel(GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//Here Starts OpenGL code
	//Clear the screen using Blue Color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/*if (loadGLTexture(&texture_stone, MAKEINTRESOURCE(IDBITMAP_STONE)) == FALSE)
	{
		fprintf(gpFile, " loadGLTexture() for Stone failed");
		uninitialize();
		return -6;
	}*/
	


	if (loadGLTexture(&texture_wooden, MAKEINTRESOURCE(IDBITMAP_WOODEN)) == FALSE)
	{
		fprintf(gpFile, " loadGLTexture() for kundali failed");
		uninitialize();
		return -7;
	}

	//Enabling Texture
	glEnable(GL_TEXTURE_2D);


	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

BOOL loadGLTexture(GLuint* texture, TCHAR imageResourceId[])
{
	//Variable declarations
	HBITMAP hBitMap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	//code
	hBitMap = (HBITMAP)LoadImage(
		GetModuleHandle(NULL),
		imageResourceId,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION);

	if (hBitMap)
	{
		bResult = TRUE;
		GetObject(hBitMap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Create the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		DeleteObject(hBitMap);
	}
	return bResult;
}


VOID printGLInfo(VOID)
{
	//Local Variable Declarations
	int i;
	GLint numExtension = 0;                                                        

	//Code
	fprintf(gpFile, "OpenGL Vendor : %s\n",glGetString(GL_VENDOR) );
	fprintf(gpFile, "OpenGL Renderer : %s\n",glGetString(GL_RENDERER) );
	fprintf(gpFile, "OpenGL Version : %s\n",glGetString(GL_VERSION) );
	fprintf(gpFile, "GLSL Version  : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION) ); 

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);

	fprintf(gpFile, "Number of Supported Extensions: %d", numExtension );

	for(i = 0; i < numExtension; i++)
	{
		fprintf(gpFile, "%s \n ",glGetStringi(GL_EXTENSIONS, i) );
	}

}

VOID resize(int width, int height)
{
	//Code
	if (height == 0) 
		height = 1;

	glViewport(0, 0, width, height);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

VOID display(VOID)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//use the shader program object
	glUseProgram(SPObj_Boat);

	/*//pyramid
	//Tranformations
	mat4 translationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
	rotationMatrix = rotate(anglepyramid, 0.0f, 1.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix; //order is important
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture_stone);
	//glUniform1i(TSU_boat, 0);

	glBindVertexArray(VAO_pyramid);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 12);

	glBindVertexArray(0);*/

	//cube
	//Tranformations
	

	modelMatrix = vmath::translate(0.0f, 0.0f, -16.0f);
	modelMatrix *= vmath::scale(2.0f, 0.5f, 0.5f);

	glUniformMatrix4fv(modelMatrixUniform_boat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_boat, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_boat, 1, GL_FALSE, perspectiveProjectionMatrix);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_wooden);
	glUniform1i(TSU_boat, 0);
	

	glBindVertexArray(VAO_BOAT);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);


	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D,0);


	//new cube
	
	modelMatrix = vmath::translate(0.0f, 0.0f, -16.0f);
	modelMatrix *= vmath::scale(1.5f, 0.2f, 0.5f);
	
	glUniformMatrix4fv(modelMatrixUniform_boat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_boat, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_boat, 1, GL_FALSE, perspectiveProjectionMatrix);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_wooden);
	glUniform1i(TSU_boat, 0);


	glBindVertexArray(VAO_newboat);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D,0);

	//Unsused shader program object
	glUseProgram(0);

	SwapBuffers(ghDC);

}

VOID update(VOID)
{
	//code
	
	anglecube = anglecube + 1.0f;
	if (anglecube >= 360.0f)
	{
		anglecube = 0.0f;
	}
}

VOID uninitialize(VOID)
{
	//Function declaration
	VOID ToggleFullScreen(void);

	//code
	if (gbFullScreen)
	{
		ToggleFullScreen();
	}

	if (texture_wooden)
	{
		glDeleteTextures(1, &texture_wooden);
		texture_wooden = 0;
	}
	

	if (VBO_boat_position)
	{
		glDeleteBuffers(1, &VBO_boat_position);
		VBO_boat_position = 0;
	}

	if (VBO_boat_texcoord)
	{
		glDeleteBuffers(1, &VBO_boat_texcoord);
		VBO_boat_texcoord = 0;
	}
	//Deletetion and Uninitialization of VAO
	if (VAO_BOAT)
	{
		glDeleteVertexArrays(1, &VAO_BOAT);
		VAO_BOAT = 0;
	}
	

	//Shader unitialization
	if (SPObj_Boat)
	{
		glUseProgram(SPObj_Boat);
		
		GLsizei numAttachedShaders;
		glGetProgramiv(SPObj_Boat, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint* shaderObjects = NULL;
		shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		glGetAttachedShaders(SPObj_Boat, numAttachedShaders, &numAttachedShaders, shaderObjects);
		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(SPObj_Boat, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(SPObj_Boat);
		SPObj_Boat = 0;
	}

	if (wglGetCurrentContext() == ghlrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghlrc)
	{
		wglDeleteContext(ghlrc);
		ghlrc = NULL;
	}
	
	if (ghDC)
	{
		ReleaseDC(ghwnd, ghDC);
		ghDC = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File is SuccessFully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

