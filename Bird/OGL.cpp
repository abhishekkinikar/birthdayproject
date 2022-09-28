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
HGLRC ghlrc = NULL;//H = Handle GL = OpenGL, RC = RenderingContext

BOOL gbFullScreen = FALSE;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Programmable Pipeline related global variables
GLuint shaderProgramObject;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE = 0
};

GLuint VAO_triangle1;
GLuint VAO_triangle2;

GLuint VBO_triangle1;
GLuint VBO_triangle2;

GLuint VAO1_triangle1;
GLuint VAO1_triangle2;

GLuint VBO1_triangle1;
GLuint VBO1_triangle2;


GLuint VAO2_triangle1;
GLuint VAO2_triangle2;

GLuint VBO2_triangle1;
GLuint VBO2_triangle2;


GLuint VAO3_triangle1;
GLuint VAO3_triangle2;

GLuint VBO3_triangle1;
GLuint VBO3_triangle2;


GLuint VAO4_triangle1;
GLuint VAO4_triangle2;

GLuint VBO4_triangle1;
GLuint VBO4_triangle2;


GLuint VAO5_triangle1;
GLuint VAO5_triangle2;

GLuint VBO5_triangle1;
GLuint VBO5_triangle2;


GLuint VAO6_triangle1;
GLuint VAO6_triangle2;

GLuint VBO6_triangle1;
GLuint VBO6_triangle2;


GLuint VAO7_triangle1;
GLuint VAO7_triangle2;

GLuint VBO7_triangle1;
GLuint VBO7_triangle2;

GLuint VAO8_triangle1;
GLuint VAO8_triangle2;

GLuint VBO8_triangle1;
GLuint VBO8_triangle2;

GLuint mvpMattrixUniform;

mat4  perspectiveProjectionMatrix;

GLfloat birdWings = -0.1f;
GLfloat angleTriangle1;
GLfloat angleTriangle2;

mat4 pushMatrix;
mat4 popMatrix;
mat4 matrixStack[16];
int top = 0;

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
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)// _s mhnje secured/safe file opening
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
		TEXT("PP: Birdie"), 
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

	SetForegroundWindow(hwnd); //hwnd or ghwnd donhi vaparla tar chalel pan ghwnd he apan baherchyan sathi kel ahe tar aat madhe vapartana nako.
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

	case WM_SIZE: //Window ch size badalto tevha
		resize(LOWORD(lParam), HIWORD(lParam));

		break;

	case WM_CLOSE: //WMCLose and mag WM_Destroy yeto
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
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * a_position;" \
		"}";
	//Object create kel
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Tyala code dila
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL); // kontya shader object la deych ahe, kiti string ch shader ahe mhnje shareds ch array ahe to , shader ch code de, jevdh array arel mhnj ejevdhe strings ahet tyachi lambi de(NULL ji single string dili ahe tyachi purna lambi ghe.) 
	//Compile kela ithe
	glCompileShader(vertexShaderObject);
	//Error Checking
	GLint status;
	GLint infoLogLength;
	CHAR* log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);//
	if (status == GL_FALSE)//mhnje error ahe
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);//konasathi, Kay PAije, kasha madhe paije
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
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
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
	shaderProgramObject = glCreateProgram();
	//Attach shader object
	glAttachShader(shaderProgramObject, vertexShaderObject);//konala attach karaych, konta attach karaych
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	//
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position"); //Andhaar
	//Link the shader program object
	glLinkProgram(shaderProgramObject);

	// linking che Error
	status = 0;
	infoLogLength = 0;
	log = NULL;

	mvpMattrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix"); //Andhaar
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength >= 0)
		{
			log = (CHAR*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				fprintf(gpFile, "Shader program's link log: %s\n", log);
				free(log);
				uninitialize();
			}
		}
	}
	/*const GLfloat triangleVertices1[] =
	{
		-0.1f, birdWings, 0.0f,
		-0.15f, -0.3f, 0.0f,
		0.15f, -0.3f, 0.0f,
	};

	const GLfloat triangleVertices2[] =
	{
		0.1f, birdWings - 0.02f, 0.0f,
		-0.15f, -0.3f, 0.0f,
		0.15f, -0.3f, 0.0f
	};

	const GLfloat triangleVertices3[] =
	{
		0.35f, -0.3f, 0.0f,
		0.15f, -0.3f, 0.0f,
		0.15f, -0.27f, 0.0f
	};*/
	//Declaration of vertex data arrays
	const GLfloat triangleVertices1[] =
	{
		-0.1f, birdWings, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f,
	};

	const GLfloat triangleVertices2[] =
	{
		0.2f, birdWings - 0.02f, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f
	};

	
	//VAO and VBO related code
	glGenVertexArrays(1, &VAO_triangle1); //5th step
	glBindVertexArray(VAO_triangle1);
	glGenBuffers(1, &VBO_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices1), triangleVertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO_triangle2); //5th step
	glBindVertexArray(VAO_triangle2);
	glGenBuffers(1, &VBO_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices2), triangleVertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//2
	birdWings = birdWings + 0.2f;
	const GLfloat triangleVertices11[] =
	{
		-0.1f, birdWings, 0.0f,
		0.1f, -0.1f, 0.0f,
		0.09f, -0.1f, 0.0f,
	};

	const GLfloat triangleVertices21[] =
	{
		0.2f, birdWings - 0.02f, 0.0f,
		0.1f, -0.1f, 0.0f,
		0.09f, -0.1f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO1_triangle1); //5th step
	glBindVertexArray(VAO1_triangle1);
	glGenBuffers(1, &VBO1_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices11), triangleVertices11, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO1_triangle2); //5th step
	glBindVertexArray(VAO1_triangle2);
	glGenBuffers(1, &VBO1_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices21), triangleVertices21, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	//3
	birdWings = birdWings - 0.2f;
	const GLfloat triangleVertices12[] =
	{
		0.1f, birdWings, 0.0f,
		0.3f, -0.3f, 0.0f,
		0.29f, -0.3f, 0.0f,
	};

	const GLfloat triangleVertices22[] =
	{
		0.4f, birdWings - 0.02f, 0.0f,
		0.3f, -0.3f, 0.0f,
		0.29f, -0.3f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO2_triangle1); //5th step
	glBindVertexArray(VAO2_triangle1);
	glGenBuffers(1, &VBO2_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices12), triangleVertices12, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO2_triangle2); //5th step
	glBindVertexArray(VAO2_triangle2);
	glGenBuffers(1, &VBO2_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices22), triangleVertices22, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	//4
	birdWings = birdWings + 0.2f;
	const GLfloat triangleVertices13[] =
	{
		-0.1f, birdWings, 0.0f,
		-0.3f, -0.12f, 0.0f,
		-0.29f, -0.12f, 0.0f,
	};

	const GLfloat triangleVertices23[] =
	{
		-0.4f, birdWings - 0.02f, 0.0f,
		-0.3f, -0.12f, 0.0f,
		-0.29f, -0.12f, 0.0f
	};

	//VAO and VBO related code
	glGenVertexArrays(1, &VAO3_triangle1); //5th step
	glBindVertexArray(VAO3_triangle1);
	glGenBuffers(1, &VBO3_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices13), triangleVertices13, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO3_triangle2); //5th step
	glBindVertexArray(VAO3_triangle2);
	glGenBuffers(1, &VBO3_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices23), triangleVertices23, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//5
	birdWings = birdWings + 0.42f;
	const GLfloat triangleVertices14[] =
	{
		-0.1f, birdWings, 0.0f,
		0.1f, 0.3f, 0.0f,
		0.09f, 0.3f, 0.0f,
	};

	const GLfloat triangleVertices24[] =
	{
		0.2f, birdWings - 0.02f, 0.0f,
		0.1f, 0.3f, 0.0f,
		0.09f, 0.3f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO4_triangle1); //5th step
	glBindVertexArray(VAO4_triangle1);
	glGenBuffers(1, &VBO4_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices14), triangleVertices14, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO4_triangle2); //5th step
	glBindVertexArray(VAO4_triangle2);
	glGenBuffers(1, &VBO4_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices24), triangleVertices24, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//6
	birdWings = birdWings - 0.1f;
	const GLfloat triangleVertices15[] =
	{
		0.3f, birdWings, 0.0f,
		0.4f, 0.2f, 0.0f,
		0.39f, 0.2f, 0.0f,
	};

	const GLfloat triangleVertices25[] =
	{
		0.6f, birdWings - 0.02f, 0.0f,
		0.4f, 0.2f, 0.0f,
		0.39f, 0.2f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO5_triangle1); //5th step
	glBindVertexArray(VAO5_triangle1);
	glGenBuffers(1, &VBO5_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices15), triangleVertices15, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO5_triangle2); //5th step
	glBindVertexArray(VAO5_triangle2);
	glGenBuffers(1, &VBO5_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices25), triangleVertices25, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//7
	birdWings = birdWings - 0.21f;
	const GLfloat triangleVertices16[] =
	{
		0.41f, birdWings, 0.0f,
		0.55f, -0.01f, 0.0f,
		0.54f, -0.01f, 0.0f,
	};

	const GLfloat triangleVertices26[] =
	{
		0.7f, birdWings - 0.02f, 0.0f,
		0.55f, -0.01f, 0.0f,
		0.54f, -0.01f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO6_triangle1); //5th step
	glBindVertexArray(VAO6_triangle1);
	glGenBuffers(1, &VBO6_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO6_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices16), triangleVertices16, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO6_triangle2); //5th step
	glBindVertexArray(VAO6_triangle2);
	glGenBuffers(1, &VBO6_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO6_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices26), triangleVertices26, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//8
	const GLfloat triangleVertices17[] =
	{
		-0.1f, birdWings, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f,
	};

	const GLfloat triangleVertices27[] =
	{
		0.2f, birdWings - 0.02f, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO_triangle1); //5th step
	glBindVertexArray(VAO_triangle1);
	glGenBuffers(1, &VBO_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices1), triangleVertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO_triangle2); //5th step
	glBindVertexArray(VAO_triangle2);
	glGenBuffers(1, &VBO_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices2), triangleVertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//9
	const GLfloat triangleVertices18[] =
	{
		-0.1f, birdWings, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f,
	};

	const GLfloat triangleVertices28[] =
	{
		0.2f, birdWings - 0.02f, 0.0f,
		0.1f, -0.3f, 0.0f,
		0.09f, -0.3f, 0.0f
	};


	//VAO and VBO related code
	glGenVertexArrays(1, &VAO_triangle1); //5th step
	glBindVertexArray(VAO_triangle1);
	glGenBuffers(1, &VBO_triangle1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices1), triangleVertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &VAO_triangle2); //5th step
	glBindVertexArray(VAO_triangle2);
	glGenBuffers(1, &VBO_triangle2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices2), triangleVertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);



	//glGenVertexArrays(1, &VAO_triangle3); //5th step
	//glBindVertexArray(VAO_triangle3);
	//glGenBuffers(1, &VBO_triangle3);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_triangle3);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices3), triangleVertices3, GL_STATIC_DRAW);
	//glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	if (birdWings >= 0.2f)
	{
		birdWings = 0.1f;
	}
	else
	{
		birdWings = 0.21f;
	}
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

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
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
	fprintf(gpFile, "GLSL Version  : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION) ); //Graphic library shading language.

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
	if (height == 0) //To avoid illegal instruction (divide by 0) in future
		height = 1;

	glViewport(0, 0, width, height);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

VOID display(VOID)
{
	// Function declaration
	void push(mat4);
	mat4 pop();
	//code
	glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
	//use the shader program object
	glUseProgram(shaderProgramObject);

	//Tranformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	push(modelViewMatrix);
		translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
		rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
		modelViewMatrix = translationMatrix * rotationMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		glBindVertexArray(VAO_triangle1);
	
		//Here there shouls be drawing code
		glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

		glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
		translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
		rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
		modelViewMatrix = translationMatrix * rotationMatrix;;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
		glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		glBindVertexArray(VAO_triangle2);

		//Here there shouls be drawing code
		glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

		glBindVertexArray(0);
	modelViewMatrix = pop();

	//2
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO1_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO1_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	//3
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO2_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO2_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	//4
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO3_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO3_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();


	//5
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO4_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO4_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	//6
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO5_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO5_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	//7
	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle1, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO6_triangle1);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();

	push(modelViewMatrix);
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	rotationMatrix = rotate(angleTriangle2, 1.0f, 0.0f, 0.0f);
	modelViewMatrix = translationMatrix * rotationMatrix;;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(VAO6_triangle2);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	glBindVertexArray(0);
	modelViewMatrix = pop();


	//push(modelViewMatrix);
	//	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	//	modelViewMatrix = translationMatrix;
	//	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	//	glUniformMatrix4fv(mvpMattrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	//	glBindVertexArray(VAO_triangle3);

	//	//Here there shouls be drawing code
	//	glDrawArrays(GL_TRIANGLES, 0, 3);// 0 pasun start kar 3 vertices draw kar

	//	glBindVertexArray(0);
	//modelViewMatrix = pop();

	//Unsused shader program object
	glUseProgram(0);

	SwapBuffers(ghDC);
}

void push(mat4 matrix)
{
	if (top == 15)
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

VOID update(VOID)
{
	//code
	/*angleTriangle1 = angleTriangle1 + 1.0f;
	if (angleTriangle1 >= 90.0f)
	{
		angleTriangle1 = 0.0f;
	}
	angleTriangle2 = angleTriangle2 - 1.0f;
	if (angleTriangle2 <= -90.0f)
	{
		angleTriangle2 = 0.0f;
	}*/
	/*if (birdWings >= 0.2f && birdWings <= 0.1f)
	{
		birdWings--;
	}
	else
	{
		birdWings++;
	}*/
}

VOID uninitialize(VOID)
{
	//Function declaration
	VOID ToggleFullScreen(void);

	//code
	if (gbFullScreen)//chukun jar destroy kel fullscreen astana i.e ESC dabun tar adhi normal ho mag marav karan tine khup kashta ghetlet Full screen karayla
	{
		ToggleFullScreen();
	}
	//Deletetion and Uninitialization of VBO
	if (VBO_triangle1)
	{
		glDeleteBuffers(1, &VBO_triangle1);
		VBO_triangle1 = 0;
	}

	if (VBO_triangle2)
	{
		glDeleteBuffers(1, &VBO_triangle2);
		VBO_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO_triangle1)
	{
		glDeleteVertexArrays(1, &VAO_triangle1);
		VAO_triangle1 = 0;
	}

	if (VAO_triangle2)
	{
		glDeleteVertexArrays(1, &VAO_triangle2);
		VAO_triangle2 = 0;
	}
	//2
	if (VBO1_triangle1)
	{
		glDeleteBuffers(1, &VBO1_triangle1);
		VBO1_triangle1 = 0;
	}

	if (VBO1_triangle2)
	{
		glDeleteBuffers(1, &VBO1_triangle2);
		VBO1_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO1_triangle1)
	{
		glDeleteVertexArrays(1, &VAO1_triangle1);
		VAO1_triangle1 = 0;
	}

	if (VAO1_triangle2)
	{
		glDeleteVertexArrays(1, &VAO1_triangle2);
		VAO1_triangle2 = 0;
	}

	//3
	if (VBO2_triangle1)
	{
		glDeleteBuffers(1, &VBO2_triangle1);
		VBO2_triangle1 = 0;
	}

	if (VBO2_triangle2)
	{
		glDeleteBuffers(1, &VBO2_triangle2);
		VBO2_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO2_triangle1)
	{
		glDeleteVertexArrays(1, &VAO2_triangle1);
		VAO2_triangle1 = 0;
	}

	if (VAO2_triangle2)
	{
		glDeleteVertexArrays(1, &VAO2_triangle2);
		VAO2_triangle2 = 0;
	}

	//4
	if (VBO3_triangle1)
	{
		glDeleteBuffers(1, &VBO3_triangle1);
		VBO3_triangle1 = 0;
	}

	if (VBO3_triangle2)
	{
		glDeleteBuffers(1, &VBO3_triangle2);
		VBO3_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO3_triangle1)
	{
		glDeleteVertexArrays(1, &VAO3_triangle1);
		VAO3_triangle1 = 0;
	}

	if (VAO3_triangle2)
	{
		glDeleteVertexArrays(1, &VAO3_triangle2);
		VAO3_triangle2 = 0;
	}

	//5
	if (VBO4_triangle1)
	{
		glDeleteBuffers(1, &VBO4_triangle1);
		VBO4_triangle1 = 0;
	}

	if (VBO4_triangle2)
	{
		glDeleteBuffers(1, &VBO4_triangle2);
		VBO4_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO4_triangle1)
	{
		glDeleteVertexArrays(1, &VAO4_triangle1);
		VAO4_triangle1 = 0;
	}

	if (VAO4_triangle2)
	{
		glDeleteVertexArrays(1, &VAO4_triangle2);
		VAO4_triangle2 = 0;
	}

	//6
	if (VBO5_triangle1)
	{
		glDeleteBuffers(1, &VBO5_triangle1);
		VBO5_triangle1 = 0;
	}

	if (VBO5_triangle2)
	{
		glDeleteBuffers(1, &VBO5_triangle2);
		VBO5_triangle2 = 0;
	}

	//Deletetion and Uninitialization of VAO
	if (VAO5_triangle1)
	{
		glDeleteVertexArrays(1, &VAO5_triangle1);
		VAO5_triangle1 = 0;
	}

	if (VAO4_triangle2)
	{
		glDeleteVertexArrays(1, &VAO4_triangle2);
		VAO4_triangle2 = 0;
	}

	//Shader unitialization
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
		}
		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
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

