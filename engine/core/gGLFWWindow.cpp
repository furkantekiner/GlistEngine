/*
 * gGLFWWindow.cpp
 *
 *  Created on: May 6, 2020
 *      Author: noyan
 */

#include "gGLFWWindow.h"
#include "gAppManager.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "stb/stb_image.h"

const int gGLFWWindow::CURSORMODE_NORMAL = GLFW_CURSOR_NORMAL;
const int gGLFWWindow::CURSORMODE_HIDDEN = GLFW_CURSOR_HIDDEN;
const int gGLFWWindow::CURSORMODE_DISABLED = GLFW_CURSOR_DISABLED;

GLFWwindow* gGLFWWindow::currentwindow = nullptr;


gGLFWWindow::gGLFWWindow() {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	window = nullptr;
	cursor = new GLFWcursor*[6];
	scalex = 1.0f;
	scaley = 1.0f;

#endif
}

gGLFWWindow::~gGLFWWindow() {
}


void gGLFWWindow::initialize(int width, int height, int windowMode, bool isResizable) {
	gBaseWindow::initialize(width, height, windowMode, isResizable);
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	// Create glfw
	glfwInit();


	// Configure glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        // define something for simulator
    #elif TARGET_OS_IPHONE
        // define something for iphone
    #else
        #define TARGET_OS_OSX 1
        // define something for OSX
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //case_mac
    #endif
#endif

	// All hints available at https://www.glfw.org/docs/latest/window.html#window_hints


	// Create window
	int currentrefreshrate = 60;
    if(windowMode == G_WINDOWMODE_GAME) {
    	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    	width = mode->width;
    	height = mode->height;
    	currentrefreshrate = mode->refreshRate;
    } else if(windowMode == G_WINDOWMODE_FULLSCREEN || windowMode == G_WINDOWMODE_FULLSCREENGUIAPP) {
    	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    	glfwWindowHint(GLFW_RESIZABLE, isResizable);
    } else {
    	glfwWindowHint(GLFW_RESIZABLE, isResizable);
    }

    window = glfwCreateWindow(width, height, title.c_str(),
			(windowMode == G_WINDOWMODE_GAME?glfwGetPrimaryMonitor():NULL), NULL);

	if(window == NULL) {
	    std::cout << "Failed to create GLFW window" << std::endl;
	    glfwTerminate();
	    return;
	}
    currentwindow = window;

	if(windowMode == G_WINDOWMODE_GAME) {
	   	GLFWmonitor* monitor = glfwGetWindowMonitor(window);
	   	glfwSetWindowMonitor(window, monitor, 0, 0, width, height, currentrefreshrate);
	   	glViewport(0, 0, width, height);
	}

	glfwGetFramebufferSize(window, &width, &height);
	this->width = width;
	this->height = height;

	// Fix mouse movement for HiDPI (scaled) displays.
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	// Currently, this is not updated from anywhere, we might need to update this value inside framebuffer_size_callback in some rare edge case.
	this->scalex = (float) width / (float) windowWidth;
	this->scaley = (float) height / (float) windowHeight;

	GLFWimage images[1];
	std::string iconpath = gGetImagesDir() + "gameicon/icon.png";
	images[0].pixels = stbi_load(iconpath.c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);

	cursor[0] = glfwCreateStandardCursor(0x00036001);
	cursor[1] = glfwCreateStandardCursor(0x00036002);
	cursor[2] = glfwCreateStandardCursor(0x00036003);
	cursor[3] = glfwCreateStandardCursor(0x00036004);
	cursor[4] = glfwCreateStandardCursor(0x00036005);
	cursor[5] = glfwCreateStandardCursor(0x00036006);
	glfwSetCursor(window, cursor[0]);

	glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);
	glewExperimental = GL_TRUE;
	glewInit();

#if defined(WIN32) || defined(LINUX) || defined(APPLE)
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//	    std::cout << "Failed to initialize GLAD" << std::endl;
//	    return -1;
//	}
#endif

	glfwSetWindowUserPointer(window, this);


	// Window specs to OpenGL
	glViewport(0, 0, width, height);


	// Notify OpenGL if the window size changed
	glfwSetFramebufferSizeCallback(window, gGLFWWindow::framebuffer_size_callback);
	glfwSetCharCallback(window, gGLFWWindow::character_callback);
	glfwSetKeyCallback(window, gGLFWWindow::key_callback);
	glfwSetCursorPosCallback(window, gGLFWWindow::mouse_pos_callback);
	glfwSetMouseButtonCallback(window, gGLFWWindow::mouse_button_callback);
	glfwSetCursorEnterCallback(window, gGLFWWindow::mouse_enter_callback);
	glfwSetScrollCallback(window, gGLFWWindow::mouse_scroll_callback);
	glfwSetWindowFocusCallback(window, gGLFWWindow::window_focus_callback);
	glfwSetJoystickCallback(gGLFWWindow::joystick_callback);
#endif
}


bool gGLFWWindow::getShouldClose() {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	return glfwWindowShouldClose(window);
#endif
	return 0;
}

void gGLFWWindow::update() {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	// End window drawing
    glfwSwapBuffers(window);
    glfwPollEvents();
#endif
//    std::cout << "width:" << width << ", height:" << height << std::endl;
}

void gGLFWWindow::close() {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	// Deallocate glfw resources
	glfwTerminate();
#endif
}

void gGLFWWindow::enableVsync(bool vsync) {
	gBaseWindow::enableVsync(vsync);
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	glfwSwapInterval(vsync);
#endif
}

void gGLFWWindow::setCursor(int cursorNo) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	glfwSetCursor(window, cursor[cursorNo]);
#endif
}

void gGLFWWindow::setCursorMode(int cursorMode) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
#endif
}

void gGLFWWindow::setClipboardString(std::string text) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	glfwSetClipboardString(window, text.c_str());
#endif
}

std::string gGLFWWindow::getClipboardString() {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	return glfwGetClipboardString(window);
#endif
}

void gGLFWWindow::setWindowSize(int width, int height) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	if(window != nullptr) {
		glfwSetWindowSize(window, width, height);
		framebuffer_size_callback(window, width, height);
	}
#endif
}

void gGLFWWindow::setWindowResizable(bool isResizable) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	if(window != nullptr) {
		glfwSetWindowAttrib(window, GLFW_RESIZABLE, isResizable);
	}
#endif
}

void gGLFWWindow::setWindowSizeLimits(int minWidth, int minHeight, int maxWidth, int maxHeight) {
#if defined(WIN32) || defined(LINUX) || defined(APPLE)
	if(window != nullptr) {
		if(minWidth == 0) minWidth = GLFW_DONT_CARE;
		if(minHeight == 0) minHeight = GLFW_DONT_CARE;
		if(maxWidth == 0) maxWidth = GLFW_DONT_CARE;
		if(maxHeight == 0) maxHeight = GLFW_DONT_CARE;
		glfwSetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight);
	}
#endif
}

#if defined(WIN32) || defined(LINUX) || defined(APPLE)
void gGLFWWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    (static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->setSize(width, height);
}

void gGLFWWindow::character_callback(GLFWwindow* window, unsigned int keycode) {
	 (static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onCharEvent(keycode);
}

void gGLFWWindow::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_RELEASE && action != GLFW_PRESS) return;
    (static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onKeyEvent(key, action);
}

void gGLFWWindow::window_focus_callback(GLFWwindow* window, int focused) {
	(static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onWindowFocus(focused);
}

void gGLFWWindow::joystick_callback(int jid, int event) {
	(static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(currentwindow)))->onJoystickConnected(jid, glfwJoystickIsGamepad(jid), event == GLFW_CONNECTED);
}

void gGLFWWindow::mouse_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	auto handle = static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window));
	handle->onMouseMoveEvent(xpos * handle->scalex, ypos * handle->scaley);
}

void gGLFWWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);
   (static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onMouseButtonEvent(button, action, xpos, ypos);
}

void gGLFWWindow::mouse_enter_callback(GLFWwindow* window, int entered) {
	(static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onMouseEnterEvent(entered);
}

void gGLFWWindow::mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	(static_cast<gGLFWWindow *>(glfwGetWindowUserPointer(window)))->onMouseScrollEvent(xoffset, yoffset);
}
#endif

