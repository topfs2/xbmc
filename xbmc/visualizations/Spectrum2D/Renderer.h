#pragma once
#include "Objects.h"
#include "Cameras.h"

class IRenderer
{
public:
  virtual ~IRenderer() { }

  virtual void render(Scene *scene, ICamera *camera/*, renderTarget, forceClear*/) = 0;
};

#include <GL/glew.h>

class GLRenderer : public IRenderer
{
public:
  virtual void render(Scene *scene, ICamera *camera/*, renderTarget, forceClear*/)
  {
    if (!(scene && camera)) {
      return;
    }

    InitializeGL();
    InitializeOrthographc((OrthographicCamera *)camera); // TODO Handle PerspectiveCamera

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderObject((Object3D *)scene);
  }

private:
  static void ApplyBasicMaterial(IMaterial *material)
  {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  }

  static void RenderPlane(PlaneGeometry *plane)
  {
    glBegin(GL_QUADS);
      glVertex3f(plane->width * -0.5f, plane->height * -0.5f, 0.0f);
      glVertex3f(plane->width *  0.5f, plane->height * -0.5f, 0.0f);
      glVertex3f(plane->width *  0.5f, plane->height *  0.5f, 0.0f);
      glVertex3f(plane->width * -0.5f, plane->height *  0.5f, 0.0f);
    glEnd();
  }

  static void RenderMesh(Mesh *mesh)
  {
    ApplyBasicMaterial(mesh->material);
    RenderPlane((PlaneGeometry *)mesh->geometry);
  }

  static void RenderObject(Object3D *object)
  {
    glPushMatrix();
    glTranslatef(object->position[0], object->position[1], object->position[2]);

    switch (object->type())
    {
      case CONSTANTS::Mesh:
        RenderMesh((Mesh *)object);
    }

    for (unsigned int i = 0; i < object->children.size(); i++) {
      RenderObject(object->children[i]);
    }

    glPopMatrix();
  }

  static void InitializeGL()
  {
    glShadeModel(GL_SMOOTH);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  static void InitializeOrthographc(OrthographicCamera *camera)
  {
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();

	  glOrtho(camera->left, camera->right, camera->top, camera->bottom, camera->near, camera->far);
	  glMatrixMode(GL_MODELVIEW);
  }
};
