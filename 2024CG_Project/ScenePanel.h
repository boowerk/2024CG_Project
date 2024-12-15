#pragma once
#include "Panel.h"

#include "GL/glew.h"

class FrameBuffer;
class Model;
class Window;
class CameraBase;
class Entity;
struct ImVec2;

class ScenePanel : Panel
{
public:
	ScenePanel(FrameBuffer* fb, CameraBase* cam, Window* win);

	virtual void Update() override;
	void HandleInput();
	void SetEntity(Entity* e);
	void SetCamera(CameraBase* cam) { camera = cam; }
	void SetIsPlayMode(bool flag) { isPlayMode = flag; }

	GLfloat GetWidth() { return width; }
	GLfloat GetHeight() { return height; }

	~ScenePanel();

private:
	void DrawGizmo(ImVec2 pos);

	FrameBuffer* sceneBuffer;
	Entity* selectedEntity;
	CameraBase* camera;
	Window* mainWindow;

	int currOperation;

	GLfloat width, height;

	bool isPlayMode;
};

