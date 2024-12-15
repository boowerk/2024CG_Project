#pragma once

#include "stdio.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class FrameBuffer;

extern void TogglePlayMode();

class Window
{
public:
	Window(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool GetShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* GetMouseButton() { return mouseButton; }
	bool* GetKeys() { return keys; }

	GLfloat getXChange();
	GLfloat getYChange();

	GLfloat GetScrollYChange();

	GLFWwindow* GetGLFWwindow() { return mainWindow; }

	void swapBuffers() { glfwSwapBuffers(mainWindow); }
	void SetSceneBuffer(FrameBuffer* sceneBuffer);
	void ToggleWireframeMode();

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	bool keys[1024];

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseButton[8];
	bool mouseRightButtonClicked; // 마우스 오른쪽 버튼이 클릭되었는지
	
	bool isWireframe;

	GLfloat scrollY;

	void CreateCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouseButton(GLFWwindow* window, int button, int action, int mods);
	static void handleMousePos(GLFWwindow* window, double xPos, double yPos);
	static void handleScroll(GLFWwindow* window, double xoffset, double yoffset);

	FrameBuffer* sceneBuffer;
};