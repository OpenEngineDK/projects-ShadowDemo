// OpenGL light preprocessor implementation.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "ShadowMapBuilder.h"
#include "ShadowMapRenderer.h"

#include <Scene/TransformationNode.h>
#include <Scene/GeometryNode.h>
#include <Display/IViewingVolume.h>
#include <Display/Camera.h>
#include <Display/ViewingVolume.h>

#include <Geometry/FaceSet.h>

#include <Logging/Logger.h>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

using OpenEngine::Geometry::FaceSet;
using OpenEngine::Math::Vector;
using OpenEngine::Math::Matrix;


ShadowMapBuilder::ShadowMapBuilder(Display::Camera* cam){
    
    //volume = &vol;
    //volume = new Display::ViewingVolume();
    //camera = cam;//new Display::Camera(*volume);
    camera = new Display::Camera(*(new Display::ViewingVolume()));
    Vector<3, float> lightPos = Vector<3, float>(0, 0, 0);
    camera->Move(lightPos);
}

ShadowMapBuilder::~ShadowMapBuilder() {}
        
void ShadowMapBuilder::VisitTransformationNode(TransformationNode* node) {
    //    logger.info << "ShadMapBuilder: visited TransNode" << logger.end;

    CHECK_FOR_GL_ERROR();
    
    // push transformation matrix to model view stack
    Matrix<4,4,float> m = node->GetTransformationMatrix();
    float f[16];
    m.ToArray(f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(f);
    // traverse sub nodes
    node->VisitSubNodes(*this);
    // pop transformation matrix
    glPopMatrix();

    CHECK_FOR_GL_ERROR();
}

void ShadowMapBuilder::VisitGeometryNode(GeometryNode* node) {
    //    logger.info << "ShadMapBuilder: visited GeomNode" << logger.end;
    

    FaceList::iterator itr;
    FaceSet* faces = node->GetFaceSet();
    if (faces == NULL) return;


    CHECK_FOR_GL_ERROR();
    // for each face ...
    for (itr = faces->begin(); itr != faces->end(); itr++) {
        FacePtr f = (*itr);

        //        ApplyMaterial(f->mat);

        glBegin(GL_TRIANGLES);
        // for each vertex ...
        for (int i=0; i<3; i++) {
            Vector<3,float> v = f->vert[i];
			glVertex3f(v[0],v[1],v[2]);
        }
        glEnd();
        CHECK_FOR_GL_ERROR();
    }

    CHECK_FOR_GL_ERROR();
    
    // traverse sub nodes
    node->VisitSubNodes(*this);

    CHECK_FOR_GL_ERROR();
}    


void ShadowMapBuilder::Handle(RenderingEventArg arg) {
    CHECK_FOR_GL_ERROR();
    //logger.info << "ShadowBuilder" << logger.end;
    // turn off lights
    glDisable(GL_LIGHTING);

    CHECK_FOR_GL_ERROR();

    //ApplyViewingVolume(*volume);
    //Vector<3, float> oldPos = volume->GetPosition();
    //Vector<3, float> lightPos = Vector<3, float>(50, 10, 0);
    //volume->SetPosition(lightPos);

    // rotate the world to compensate for the camera
//     Matrix<4,4,float> matrix = volume->GetViewMatrix();
//     float f[16] = {0};
//     matrix.ToArray(f);
//     glMultMatrixf(f);
//     CHECK_FOR_GL_ERROR();
    // TODO: Tænke over ovenstående

    //gluLookAt(50.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    //glClear(GL_DEPTH_BUFFER_BIT);
    CHECK_FOR_GL_ERROR();

    //ApplyViewingVolume(*volume);
    glPushMatrix();

    //arg.renderer.GetSceneRoot()->Accept(*this);
    
    //volume->SetPosition(oldPos);

    

    //gluLookAt(50.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // TODO: move the camera into the pos of the light source

    //CHECK_FOR_GL_ERROR();

    //TODO: maybe use OE functionality for texture handling

    //Vector<3, float> oldPos = camera->GetPosition();
    //Quaternion<float> oldDir = camera->GetDirection();
    //logger.info << oldPos << logger.end;
    
    
    logger.info << "lightPos: " << camera->GetPosition() << logger.end;
    //camera->SetDirection(lightPos, Vector<3, float>(0,1,0));
    //volume->SetPosition(lightPos);

    ApplyViewingVolume(*camera);
    glFlush();

    // bind the texture used to store the shadow map
    ShadowMapRenderer* shadRend = static_cast<const ShadowMapRenderer*>(&arg.renderer);
    GLuint texName = shadRend->GetShadowMapID();
    glBindTexture(GL_TEXTURE_2D, texName);
    // overwrite the previous shadow map
    // TODO: get resolution from viewing volume
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 800, 600, 0);
    
    //logger.info << "lightPos: " << camera->GetPosition() << logger.end;

    //camera->SetPosition(oldPos);
    //camera->SetDirection(oldDir);

    glEnable(GL_LIGHTING);

    CHECK_FOR_GL_ERROR();
}

    void ShadowMapBuilder::ApplyViewingVolume(Display::IViewingVolume& volume) {
    // Select The Projection Matrix
    glMatrixMode(GL_PROJECTION);
    CHECK_FOR_GL_ERROR();

    // Reset The Projection Matrix
    glLoadIdentity();
    CHECK_FOR_GL_ERROR();

    // Setup OpenGL with the volumes projection matrix
    Matrix<4,4,float> projMatrix = volume.GetProjectionMatrix();
    float arr[16] = {0};
    projMatrix.ToArray(arr);
    glMultMatrixf(arr);
    CHECK_FOR_GL_ERROR();

    // Select the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    CHECK_FOR_GL_ERROR();

    // Reset the modelview matrix
    glLoadIdentity();
    CHECK_FOR_GL_ERROR();

    // Get the view matrix and apply it
    Matrix<4,4,float> matrix = volume.GetViewMatrix();
    float f[16] = {0};
    matrix.ToArray(f);
    glMultMatrixf(f);
    CHECK_FOR_GL_ERROR();

    glPopMatrix();
}


} // NS OpenGL
} // NS OpenEngine
} // NS Renderers
