#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "vmath.h"
using namespace vmath;

// OpenGL libraries
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

//Macro
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define TESSEL_X 100
#define TESSEL_Z 100
#define QUADSIZE 2.0f
#define X_SHIFT_WATER (QUADSIZE * 2.0f) / TESSEL_X
#define Z_SHIFT_WATER (QUADSIZE * 2.0f) / TESSEL_Z

float water_x[TESSEL_X + 1][TESSEL_Z + 1];
float water_y[TESSEL_X + 1][TESSEL_Z + 1];
float water_z[TESSEL_X + 1][TESSEL_Z + 1];

int freq = 4;
float time = 0.0f;

// global function prototype
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

// Global Veriable declaration
BOOL gbActiveWindow = FALSE;
FILE* gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullScreen = FALSE;

GLuint SPObj_Water;
GLuint SPObj_Boat;
enum {
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXTURE0
};
//Water
GLuint VAO_WATER;
GLuint VBO_POSITION_WATER, VBO_NORMAL;

//Boat
GLuint VAO_BOAT;
GLuint VBO_boat_position;
GLuint VBO_boat_color;

//water
GLuint modelMatrixUniform_water, viewMatrixUniform_water, projectionMatrixUniform_water;

GLuint laUniform_water, ldUniform_water, lsUniform_water;
GLuint lightPositionUniform_water;
GLuint kaUniform_water, kdUniform_water, ksUniform_water;
GLuint materialShininessUniform_water;
GLuint lightingEnabledUnifrom_water;
GLuint yTransUniform_water;

//Boat
GLuint modelMatrixUniform_boat, viewMatrixUniform_boat, projectionMatrixUniform_boat;

GLuint laUniform_boat, ldUniform_boat, lsUniform_boat;
GLuint lightPositionUniform_boat;
GLuint kaUniform_boat, kdUniform_boat, ksUniform_boat;
GLuint materialShininessUniform_boat;
GLuint lightingEnabledUnifrom_boat;
GLuint yTransUniform_boat;

mat4 perspectiveProjectionMatrix;
BOOL bLight = FALSE;

//GLfloat lightAmbient[] = { 0.0f, 0.0f ,0.0f ,1.0f };
//GLfloat lightDiffuse[] = { 1.0f ,1.0f ,1.0f ,1.0f };
//GLfloat lightSpecular[] = { 1.0f ,1.0f ,1.0f ,1.0f };
//GLfloat lightPosition[] = { 100.0f ,100.0f ,100.0f ,1.0f };
//
//GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
//GLfloat materialDiffuse[] = { 1.0f ,1.0f ,1.0f ,1.0f };
//GLfloat materialSpecular[] = { 1.0f ,1.0f ,1.0f ,1.0f };
//GLfloat materialShininess = 50.0f;

GLfloat lightAmbient[] = { 0.1f, 0.1f ,0.1f ,1.0f };
GLfloat lightDiffuse[] = { 1.0f ,1.0f ,1.0f ,1.0f };
GLfloat lightSpecular[] = { 1.0f ,1.0f ,1.0f ,1.0f };
GLfloat lightPosition[] = { 100.0f ,100.0f ,100.0f ,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 0.5f ,0.2f ,0.7f ,1.0f };
GLfloat materialSpecular[] = { 0.7f ,0.7f ,0.7f ,1.0f };
GLfloat materialShininess = 128.0f;

// Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreIntance, LPSTR lpszCmdLine, int iCmdShow ) {
	//function prototype
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

	if (fopen_s(&gpFile, "Log.txt", "w") != 0) {
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

	//create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("PerFragment"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, HWND_DESKTOP, NULL, hInstance, NULL);
	ghwnd = hwnd;

	//show window
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd); //use hwnd here since ghwnd is use for outer functions 
	SetFocus(hwnd);

	// Initialize
	iRateVal = initialize();
	if (iRateVal != 0) {
		fprintf(gpFile, "Initialize() failed \n");
		uninitialize();
	}

	//Game loop
	while (bDone == FALSE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = TRUE;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			if (gbActiveWindow == TRUE) {
				display();
				update();
			}
		}
	}
	uninitialize();
	return ((int)msg.wParam);
}

//callback function
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// funtion declarations 
	void toggleFullScreen(void);
	void resize(int, int);
	
	switch (iMsg) {
		case WM_SETFOCUS:
	 		gbActiveWindow = TRUE;
	 		break;
		case WM_KILLFOCUS:
	 		gbActiveWindow = FALSE;
	 		break;
		case WM_ERASEBKGND:
			return (0); 
		case WM_CHAR:
			switch (wParam) {
				case 'f':
				case 'F':
					toggleFullScreen();
					break;
				case 'L':
				case 'l':
					bLight = !bLight;
					break;
				default:
					break;
			}
			break;
		case WM_KEYDOWN:
			switch (wParam) {
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
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void toggleFullScreen() {
	// Variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;

	//code
	wp.length = sizeof(WINDOWPLACEMENT);
	if (gbFullScreen == FALSE) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			mi.cbSize = sizeof(MONITORINFO);
			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			ShowCursor(FALSE);
			gbFullScreen = TRUE;
		}
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen = FALSE;
	}
}

int initialize(void) {
	// function prototype
	void printGLInfo(void);
	void uninitialize(void);
	void resize(int width, int height);
	
	//veriable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	//initialization of pixel format discriptor
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

	//GLEW intialization
	if (glewInit() != GLEW_OK)
		return -1;

	// print OpenGL info
	// printGLInfo();

	//Water Related changes
	for (int i = 0; i <= TESSEL_Z; i++) {
		for (int j = 0; j <= TESSEL_X; j++) {
			water_x[i][j] = -QUADSIZE + (j * X_SHIFT_WATER);
			water_y[i][j] = 0.0f;
			water_z[i][j] = -QUADSIZE + (i * Z_SHIFT_WATER);
		}
	}

	//Water
	//Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightEnabled;" \
		"out vec3 transformedNormal;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"void main(void) {" \
			"if(u_lightEnabled == 1) {" \
				"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
				"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
				"transformedNormal = normalMatrix * a_normal;" \
				"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
				"viewerVector = -eyeCoordinates.xyz;" \
			"}" \
			"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
			"lightDirection = vec3(0.0, 0.0, a_position.y);" \
		"}";
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status, infoLoglength;
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Vertex Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormal;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightEnabled;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 fong_ads_color;" \
			"if(u_lightEnabled == 1) {" \
				"vec3 ambient = u_la * u_ka;" \
				"vec3 normalized_transformed_normals = normalize(transformedNormal);" \
				"vec3 normalized_light_direction = normalize(lightDirection);" \
				"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals),0.0);" \
				"vec3 reflectionVector =  reflect(-normalized_light_direction, normalized_transformed_normals);" \
				"vec3 normalized_viewer_vector = normalize(viewerVector);" \
				"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewer_vector),0.0),u_materialShininess);" \
				"fong_ads_color = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				"// fong_ads_color = vec3(1.0, 1.0, 1.0);\n" \
				"fong_ads_color = lightDirection + vec3(0.0, 0.0, 0.5);" \
			"}" \
			"FragColor = vec4(fong_ads_color, 1.0);" \
		"}";
	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Fragment Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	//shader program object
	SPObj_Water = glCreateProgram();
	glAttachShader(SPObj_Water, vertexShaderObject);
	glAttachShader(SPObj_Water, fragmentShaderObject);
	glBindAttribLocation(SPObj_Water, AMC_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(SPObj_Water, AMC_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(SPObj_Water);
	glGetProgramiv(SPObj_Water, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramiv(SPObj_Water, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetProgramInfoLog(SPObj_Water, infoLoglength, &written, log);
				fprintf(gpFile, "Shader Program Link Log : %s\n", log);
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

		//Normal VBO
		// glGenBuffers(1, &VBO_NORMAL);
		// glBindBuffer(GL_ARRAY_BUFFER, VBO_NORMAL);
		// {
		// 	glBufferData(GL_ARRAY_BUFFER, sizeof(Normal), Normal, GL_STATIC_DRAW);
		// 	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		// 	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
		// }
		// glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);


	//Boat

	GLchar* vertexShaderSourceCode_Boat =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec3 a_normal;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightEnabled;" \
		"uniform float y_trans;" \
		"out vec3 transformedNormal;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"void main(void) {" \
			"if(u_lightEnabled == 1) {" \
				"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
				"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
				"transformedNormal = normalMatrix * a_normal;" \
				"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
				"viewerVector = -eyeCoordinates.xyz;" \
			"}" \
			"else" \
			"{" \
			"transformedNormal = a_normal;" \
			"}" \
			"mat4 modelMatrix = u_modelMatrix;" \
			"modelMatrix[3][1] = modelMatrix[3][1] + y_trans;" \
			"gl_Position = u_projectionMatrix * u_viewMatrix * modelMatrix * a_position;" \
		"}";
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode_Boat, NULL);
	glCompileShader(vertexShaderObject);

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Vertex Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}

	GLchar* fragmentShaderSourceCode_Boat =
		"#version 460 core" \
		"\n" \
		"in vec3 transformedNormal;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"uniform int u_lightEnabled;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 fong_ads_color;" \
			"if(u_lightEnabled == 1) {" \
				"vec3 ambient = u_la * u_ka;" \
				"vec3 normalized_transformed_normals = normalize(transformedNormal);" \
				"vec3 normalized_light_direction = normalize(lightDirection);" \
				"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals),0.0);" \
				"vec3 reflectionVector =  reflect(-normalized_light_direction, normalized_transformed_normals);" \
				"vec3 normalized_viewer_vector = normalize(viewerVector);" \
				"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector,normalized_viewer_vector),0.0),u_materialShininess);" \
				"fong_ads_color = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				" fong_ads_color = vec3(transformedNormal);\n" \
			"}" \
			"FragColor = vec4(fong_ads_color, 1.0);" \
		"}";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode_Boat, NULL);
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLoglength, &written, log);
				fprintf(gpFile, "Fragment Shader compilation log : %s \n", log);
				free(log);
				uninitialize();
			}
		}
	}


	//shader program object
	SPObj_Boat = glCreateProgram();
	glAttachShader(SPObj_Boat, vertexShaderObject);
	glAttachShader(SPObj_Boat, fragmentShaderObject);
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(SPObj_Boat, AMC_ATTRIBUTE_NORMAL, "a_normal");
	glLinkProgram(SPObj_Boat);
	glGetProgramiv(SPObj_Boat, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramiv(SPObj_Boat, GL_INFO_LOG_LENGTH, &infoLoglength);
		if (infoLoglength > 0) {
			char *log = (char*)malloc(infoLoglength);
			if (log != NULL) {
				GLsizei written;
				glGetProgramInfoLog(SPObj_Boat, infoLoglength, &written, log);
				fprintf(gpFile, "Shader Program Link Log : %s\n", log);
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

	const GLfloat cubeColor[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

	};

	//cube
	//VAO_WATER and VBO related code
	//VAO_WATER
	glGenVertexArrays(1, &VAO_BOAT); //5th step
	glBindVertexArray(VAO_BOAT);
	//VBO for position
	glGenBuffers(1, &VBO_boat_position);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for color
	glGenBuffers(1, &VBO_boat_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_boat_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	

	//Depth related changes
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Here starts OpenGL code
	//clear the screen using blue color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void printGLInfo() {
	//local variable declarations
	GLint numExtension;

	//code
	fprintf(gpFile, "OpenGL Vendor: %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer: %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version: %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version: %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtension);
	fprintf(gpFile, "Number of supported Extension: %d \n", numExtension);

	for (int i = 0; i < numExtension; i++)
		fprintf(gpFile, " %s \n", glGetStringi(GL_EXTENSIONS,i));
}

void resize(int width, int height) {
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void) {
	//Variable Declaration
	GLfloat water_pos[3 * 6 * TESSEL_X * TESSEL_Z];
	static float boatHeight = 0.0f;

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use the shader program object
	glUseProgram(SPObj_Water);

	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, -1.0f, -6.0f);
	modelMatrix = translationMatrix;

	glUniformMatrix4fv(modelMatrixUniform_water, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_water, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_water, 1, GL_FALSE, perspectiveProjectionMatrix);
	
	if(bLight) {
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
	else {
		glUniform1i(lightingEnabledUnifrom_water, 0);
	}
	

	int k = 0;
	for (int i = 0; i < TESSEL_Z; i++) {
		for (int j = 0; j < TESSEL_X; j++) {
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

	glUseProgram(0);


	//Boat
	glUseProgram(SPObj_Boat);
	//Tranformations
	

	translationMatrix = mat4::identity();
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	translationMatrix = vmath::translate(0.0f, -1.0f, -6.0f);

	scaleMatrix = vmath::scale(2.0f, 0.5f, 0.5f);
	modelMatrix = translationMatrix * scaleMatrix;

	glUniformMatrix4fv(modelMatrixUniform_boat, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_boat, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_boat, 1, GL_FALSE, perspectiveProjectionMatrix);
	boatHeight = boatHeight  + 0.01f;
	glUniform1f(yTransUniform_boat, sinf(boatHeight)* 0.5f);
	

	glBindVertexArray(VAO_BOAT);

	//Here there shouls be drawing code
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);// 0 pasun start kar 3 vertices draw kar
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);


	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void) {
	// code
	time += 0.01f;

	for (int i = 0; i <= TESSEL_Z; i++) {
		for (int j = 0; j <= TESSEL_X; j++) {
			float temp_x = (float)(j + 1) / (float)TESSEL_X;
			temp_x = (temp_x * 2.0f) - 1.0f;

			float temp_z = (float)(i + 1) / (float)TESSEL_Z;
			temp_z = (temp_z * 2.0f) - 1.0f;
			water_y[i][j] = sin(temp_x * freq + time) * sin(temp_z * freq + time) * 0.2f;
		}
	}
}

void uninitialize(void) {
	//function declarations
	void toggleFullScreen(void);

	//code
	if (gbFullScreen)
		toggleFullScreen();
	if (VBO_POSITION_WATER) {
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
	//Deletetion and Uninitialization of VAO_WATER
	if (VAO_BOAT)
	{
		glDeleteVertexArrays(1, &VAO_BOAT);
		VAO_BOAT = 0;
	}

	// if (VBO_NORMAL) {
	// 	glDeleteBuffers(1, &VBO_NORMAL);
	// 	VBO_NORMAL = 0;
	// }
	if (VAO_WATER) {
		glDeleteVertexArrays(1, &VAO_WATER);
		VAO_WATER = 0;
	}
	if (SPObj_Water) {
		GLsizei numAttachedShaders;
		GLuint* shaderObject = NULL;
		glUseProgram(SPObj_Water);
		glGetProgramiv(SPObj_Water, GL_ATTACHED_SHADERS, &numAttachedShaders);
		shaderObject = (GLuint*)malloc(numAttachedShaders * sizeof(GLuint));
		glGetAttachedShaders(SPObj_Water, numAttachedShaders, &numAttachedShaders, shaderObject);
		for (GLsizei i = 0; i < numAttachedShaders; i++) {
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
	if (ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (ghwnd) {
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
	if (gpFile) {
		fprintf(gpFile, "\nLog file succefully closed");
		fclose(gpFile);
		gpFile = NULL;
	}
}
