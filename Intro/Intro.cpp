// header files
#include <windows.h>
#include <stdio.h> // for file I/O functions
#include <stdlib.h> // for exit()
#include "Intro.h"

// OpenGL header files
#include <GL/glew.h> // this must be above gl.h
#include <GL/gl.h>

#include "vmath.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// OpenGL libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
BOOL gbWindowIsActive = FALSE, gbFullscreen = FALSE;
FILE *gpFile = NULL;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// programmable pipeline related global variables
enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXTURE0
};

GLuint shaderProgramObject, mvpMatrixUniform, textureSamplerUniform;
GLuint vao_cube, vbo_cube_position, vbo_cube_texcoord;

GLfloat angleSaturn = 0.0f, angleAstro = 0.0f, angleMedi = 0.0f, angleComp = 0.0f, speed = 0.05f;
BOOL astro = FALSE, medi = FALSE, comp = FALSE;

vmath::mat4 perspectiveProjectionMatrix;

GLuint texture_saturn, texture_astro, texture_medi, texture_comp, texture_cgp;

// entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // function declarations
    int initialize(void);
    void display(void);
    void update(void);
    void uninitialize(void);

    // variable declarations
    HWND hwnd;
    WNDCLASSEX wcex;
    MSG msg;
    TCHAR szAppName[] = TEXT("Intro");
    BOOL bDone = FALSE;
    int iRetVal = 0, iHorzRes = 0, iVertRes = 0;

    // code
    if (fopen_s(&gpFile, "Log.txt", "w"))
    {
        MessageBox(NULL, TEXT("Creation of logfile failed. Exiting..."), TEXT("File I/O Error"), MB_OK);
        exit(0);
    }

    fprintf(gpFile, "Logfile is successfully created\n");

    // initialization of WNDCLASSEX structure
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = szAppName;
    wcex.lpszMenuName = NULL;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

    // register WNDCLASSEX
    RegisterClassEx(&wcex);

    iHorzRes = GetSystemMetrics(SM_CXSCREEN);
    iVertRes = GetSystemMetrics(SM_CYSCREEN);

    // create the window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, "Intro",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        iHorzRes == 0 ? CW_USEDEFAULT : (iHorzRes - WIN_WIDTH) / 2,
        iVertRes == 0 ? CW_USEDEFAULT : (iVertRes - WIN_HEIGHT) / 2,
        WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

    ghwnd = hwnd;

    // initialize
    iRetVal = initialize();
    if (iRetVal == -1)
    {
        fprintf(gpFile, "ChoosePixelFormat failed\n");
        uninitialize();
    }
    else if (iRetVal == -2)
    {
        fprintf(gpFile, "SetPixelFormat failed\n");
        uninitialize();
    }
    else if (iRetVal == -3)
    {
        fprintf(gpFile, "Creating OpenGL context failed\n");
        uninitialize();
    }
    else if (iRetVal == -4)
    {
        fprintf(gpFile, "Making OpenGL context as current context failed\n");
        uninitialize();
    }
    else if (iRetVal == -5)
    {
        fprintf(gpFile, "GLEW initialization failed\n");
        uninitialize();
    }
    else
        fprintf(gpFile, "Initialization successful\n");

    // show the window
    ShowWindow(hwnd, iCmdShow);

    // foregrounding and focusing the window
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // game loop
    while (!bDone)
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
            if (gbWindowIsActive)
            {
                // render the scene
                display();

                // update the scene
                update();
            }
        }
    }

    uninitialize();

    return (int)msg.wParam;
}

// callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // function declarations
    void ToggleFullscreen(void);
    void resize(int, int);

    // code
    switch (iMsg)
    {
    case WM_CHAR:
        if (wParam == 'F' || wParam == 'f')
            ToggleFullscreen();

        break;

    case WM_KEYDOWN:
        if (wParam == 27)
        {
            fprintf(gpFile, "Escape key has been pressed\n");
            DestroyWindow(hwnd);
        }

        break;

    case WM_SETFOCUS:
        fprintf(gpFile, "Game is about to begin/resume...\n");
        gbWindowIsActive = TRUE;
        break;

    case WM_KILLFOCUS:
        fprintf(gpFile, "Game is about to pause...\n");
        gbWindowIsActive = FALSE;
        break;

    case WM_ERASEBKGND:
        return 0;

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

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(void)
{
    // variable declarations
    static DWORD dwStyle;
    static WINDOWPLACEMENT wp;
    MONITORINFO mi;

    // code
    wp.length = sizeof(WINDOWPLACEMENT);

    if (!gbFullscreen)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghwnd, &wp) &&
                GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                    mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowCursor(FALSE);
                gbFullscreen = TRUE;
                fprintf(gpFile, "Game is now running in fullscreen mode\n");
            }
        }
    }
    else
    {
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(ghwnd, &wp);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        ShowCursor(TRUE);
        gbFullscreen = FALSE;
        fprintf(gpFile, "Game is now running in windowed mode\n");
    }
}

int initialize(void)
{
    // function declarations
    void resize(int, int);
    void PrintGLInfo(void);
    void uninitialize(void);
    BOOL LoadGLTexture(GLuint*, TCHAR[]);

    // variable declarations
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;

    // code
    // initialization of PIXELFORMATDESCRIPTOR structure
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32; // can also use 24

    // get DC
    ghdc = GetDC(ghwnd);

    // choose pixel format
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

    if (iPixelFormatIndex == 0)
    {
        return -1;
    }

    // set the chosen pixel format
    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
        return -2;

    // create OpenGL rendering context
    ghrc = wglCreateContext(ghdc);
    if (ghrc == NULL)
        return -3;

    // make rendering context as current context
    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
        return -4;

    // GLEW initialization
    if (glewInit() != GLEW_OK)
        return -5;

    // print OpenGL info
    PrintGLInfo();

    // vertex shader
    const GLchar *vertexShaderSourceCode =
        "#version 400 core" \
        "\n" \
        "in vec4 a_position;" \
        "in vec2 a_texcoord;" \
        "uniform mat4 u_mvpMatrix;" \
        "out vec2 a_texcoord_out;" \
        "void main(void)" \
        "{" \
            "gl_Position = u_mvpMatrix * a_position;" \
            "a_texcoord_out = a_texcoord;" \
        "}";

    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
    glCompileShader(vertexShaderObject);

    GLint status, infoLogLength;
    char *log = NULL;
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
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
                fprintf(gpFile, "Vertex shader compilation log: %s\n", log);
                free(log);
                uninitialize();
            }
        }
    }

    // fragment shader
    const GLchar *fragmentShaderSourceCode =
        "#version 400 core" \
        "\n" \
        "in vec2 a_texcoord_out;" \
        "uniform sampler2D u_textureSampler;" \
        "out vec4 fragColor;" \
        "void main(void)" \
        "{" \
            "fragColor = texture(u_textureSampler, a_texcoord_out);" \
        "}";

    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderObject, 1, (char**)&fragmentShaderSourceCode, NULL);
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
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
                fprintf(gpFile, "Fragment shader compilation log: %s\n", log);
                free(log);
                uninitialize();
            }
        }
    }
    
    // shader program object
    shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);
    
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "a_position");
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXTURE0, "a_texcoord");
    
    glLinkProgram(shaderProgramObject);

    status = 0;
    infoLogLength = 0;
    log = NULL;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            log = (char*)malloc(infoLogLength);
            if (log != NULL)
            {
                GLsizei written;
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
                fprintf(gpFile, "Shader program linking log: %s\n", log);
                free(log);
                uninitialize();
            }
        }
    }
    
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
    
    // declaration of vertex data arrays
    const GLfloat cubePosition[] =
    {
        // top face
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,

        // bottom face
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,

        // front face
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,

        // back face
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // right face
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,

        // left face
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f
    };

    const GLfloat cubeTexcoords[] =
    {
        0.0, 1.0,
        1.0, 1.0, 
        1.0, 0.0, 
        0.0, 0.0
    };
    
    // VAO and VBO related code for cube
    glGenVertexArrays(1, &vao_cube);
    glBindVertexArray(vao_cube);
    
    glGenBuffers(1, &vbo_cube_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePosition), cubePosition, GL_STATIC_DRAW);
    
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &vbo_cube_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoord);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
    
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vbo_cube_position);

    // depth-related changes
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (LoadGLTexture(&texture_saturn, MAKEINTRESOURCE(IDBITMAP_SATURN)) == FALSE)
        return -6;

    if (LoadGLTexture(&texture_astro, MAKEINTRESOURCE(IDBITMAP_ASTRO)) == FALSE)
        return -7;

    if (LoadGLTexture(&texture_medi, MAKEINTRESOURCE(IDBITMAP_MEDI)) == FALSE)
        return -8;

    if (LoadGLTexture(&texture_comp, MAKEINTRESOURCE(IDBITMAP_COMP)) == FALSE)
        return -9;

    // if (LoadGLTexture(&texture_cgp, MAKEINTRESOURCE(IDBITMAP_CGP)) == FALSE)
    //     return -10;

    glEnable(GL_TEXTURE_2D);
    
    perspectiveProjectionMatrix = vmath::mat4::identity();

    // warm-up call to resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    return 0;
}

void PrintGLInfo(void)
{
    // variable declarations
    GLint numExtensions = 0;

    // code
    fprintf(gpFile, "OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version: %s\n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    fprintf(gpFile, "Number of supported extensions: %d\n", numExtensions);
    for (int i = 0; i < numExtensions; i++)
    {
        fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
    }
}

BOOL LoadGLTexture(GLuint *texture, TCHAR imageResourceId[])
{
    // variable declarations
    HBITMAP hBitmap = NULL;
    BITMAP bmp;
    BOOL bResult = false;

    // code
    hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (hBitmap)
    {
        bResult = TRUE;
        GetObject(hBitmap, sizeof(bmp), &bmp);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // for better performance
        glGenTextures(1, texture);
        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // create the texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
        glGenerateMipmap(GL_TEXTURE_2D);

        // unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // delete the texture object
        DeleteObject(hBitmap);
    }

    return bResult;
}

void resize(int width, int height)
{
    // code
    if (height == 0)
        height = 1; // to avoid division by zero in future

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
    // function declarations
    void drawCube(GLuint texture);

    // code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the shader program object
    glUseProgram(shaderProgramObject);

    // transformations
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
    vmath::mat4 rotationMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewMatrix = vmath::mat4::identity();
    vmath::mat4 modelViewProjectionMatrix = vmath::mat4::identity();

    translationMatrix = vmath::translate(-2.0f, 0.5f, -6.0f);
    scaleMatrix = vmath::scale(-0.5f, -0.5f, -0.5f);
    rotationMatrix = vmath::rotate(angleSaturn, 1.0f, 0.0f, 0.0f);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    glBindVertexArray(vao_cube);

    drawCube(texture_saturn);

    // transformations
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    translationMatrix = vmath::translate(-1.0f, 0.5f, -6.0f);
    rotationMatrix = vmath::rotate(angleAstro, 1.0f, 0.0f, 0.0f);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    drawCube(texture_astro);

    // transformations
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    translationMatrix = vmath::translate(0.0f, 0.5f, -6.0f);
    rotationMatrix = vmath::rotate(angleMedi, 1.0f, 0.0f, 0.0f);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    drawCube(texture_medi);

    // transformations
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    translationMatrix = vmath::translate(1.0f, 0.5f, -6.0f);
    rotationMatrix = vmath::rotate(angleComp, 1.0f, 0.0f, 0.0f);
    modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    drawCube(texture_comp);

    // transformations
    // translationMatrix = vmath::mat4::identity();
    // rotationMatrix = vmath::mat4::identity();
    // modelViewMatrix = vmath::mat4::identity();
    // modelViewProjectionMatrix = vmath::mat4::identity();

    // translationMatrix = vmath::translate(0.0f, -1.5f, -6.0f);
    // modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix;
    
    // modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
    // glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);
    
    // drawCube(texture_cgp);
    
    glBindVertexArray(0);

    // unuse the shader program object
    glUseProgram(0);

    SwapBuffers(ghdc);
}

void drawCube(GLuint texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureSamplerUniform, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
}

void update(void)
{
    // code
    angleSaturn = angleSaturn - speed;
    if (angleSaturn <= -90.0f)
    {
        angleSaturn = -90.0f;
        astro = TRUE;
    }
    
    if (astro == TRUE)
    {
        angleAstro = angleAstro - speed;
        if (angleAstro <= -90.0f)
        {
            angleAstro = -90.0f;
            medi = TRUE;
        }
    }
    
    if (medi == TRUE)
    {
        angleMedi = angleMedi - speed;
        if (angleMedi <= -90.0f)
        {
            angleMedi = -90.0f;
            comp = TRUE;
        }
    }
    
    if (comp == TRUE)
    {
        angleComp = angleComp - speed;
        if (angleComp <= -90.0f)
            angleComp = -90.0f;
    }
}

void uninitialize(void)
{
    // code
    if (gbFullscreen)
    {
        ToggleFullscreen();
    }

    if (texture_saturn)
    {
        glDeleteTextures(1, &texture_saturn);
        texture_saturn = 0;
    }
    
    // deletion and uninitialization of VBO
    if (vbo_cube_texcoord)
    {
        glDeleteBuffers(1, &vbo_cube_texcoord);
        vbo_cube_texcoord = 0;
    }
    if (vbo_cube_position)
    {
        glDeleteBuffers(1, &vbo_cube_position);
        vbo_cube_position = 0;
    }
    
    // deletion and uninitialization of VAO
    if (vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube = 0;
    }

    // shader uninitialization
    if (shaderProgramObject)
    {
        glUseProgram(shaderProgramObject);

        GLsizei numAttachedShaders;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

        GLuint *shaderObjects = NULL;
        shaderObjects = (GLuint*)malloc(numAttachedShaders * sizeof (GLuint));

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

    if (wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL, NULL);
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
        fprintf(gpFile, "Exited game loop\n");
        fprintf(gpFile, "Logfile is successfully closed\n");
        fclose(gpFile);
        gpFile = NULL;
    }        
}
