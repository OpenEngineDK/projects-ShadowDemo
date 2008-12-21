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

#include <Geometry/FaceSet.h>

#include <Logging/Logger.h>

namespace OpenEngine {
namespace Renderers {
namespace OpenGL {

using OpenEngine::Geometry::FaceSet;
using OpenEngine::Math::Vector;
using OpenEngine::Math::Matrix;


ShadowMapBuilder::ShadowMapBuilder(Display::IViewingVolume& volume)
    : volume(volume) {}

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

    // rotate the world to compensate for the camera
    //Matrix<4,4,float> matrix = volume.GetViewMatrix();
    //float f[16] = {0};
    //matrix.ToArray(f);
    //glMultMatrixf(f);
    //CHECK_FOR_GL_ERROR();
    // TODO: Tænke over ovenstående

    arg.renderer.GetSceneRoot()->Accept(*this);

    //gluLookAt(50.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    // TODO: move the camera into the pos of the light source

    //CHECK_FOR_GL_ERROR();

    //TODO: maybe use OE functionality for texture handling

    //Vector<3, float> oldPos = volume.GetPosition();
    //Vector<3, float> lightPos = Vector<3, float>(50, 10, 0);
    //volume.SetPosition(lightPos);

    // bind the texture used to store the shadow map
    ShadowMapRenderer* shadRend = static_cast<const ShadowMapRenderer*>(&arg.renderer);
    GLuint texName = shadRend->GetShadowMapID();
    glBindTexture(GL_TEXTURE_2D, texName);
    // overwrite the previous shadow map
    // TODO: get resolution from viewing volume
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 800, 600, 0);
    
    //volume.SetPosition(oldPos);
    
    glEnable(GL_LIGHTING);

    CHECK_FOR_GL_ERROR();
}


} // NS OpenGL
} // NS OpenEngine
} // NS Renderers
