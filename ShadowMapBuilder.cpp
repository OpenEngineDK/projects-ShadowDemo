// OpenGL light preprocessor implementation.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "ShadowMapBuilder.h"
#include <Scene/TransformationNode.h>
#include <Scene/GeometryNode.h>
#include <Display/IViewingVolume.h>

#include <Geometry/FaceSet.h>

//#include <Logging/Logger.h>

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

    FaceList::iterator itr;
    FaceSet* faces = node->GetFaceSet();
    if (faces == NULL) return;

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

    // turn off lights

    glDisable(GL_LIGHTING);

    CHECK_FOR_GL_ERROR();

    // rotate the world to compensate for the camera
    Matrix<4,4,float> matrix = volume.GetViewMatrix();
    float f[16] = {0};
    matrix.ToArray(f);
    glMultMatrixf(f);
    CHECK_FOR_GL_ERROR();

    arg.renderer.GetSceneRoot()->Accept(*this);


    CHECK_FOR_GL_ERROR();
}


} // NS OpenGL
} // NS OpenEngine
} // NS Renderers
