// OpenGL light preprocessor implementation.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _SHADOW_MAP_BUILDER_H_
#define _SHADOW_MAP_BUILDER_H_

#include <Renderers/IRenderer.h>
#include <Scene/ISceneNodeVisitor.h>
#include <Core/IListener.h>
#include <Display/Camera.h>
#include <Meta/OpenGL.h>
#include <Display/ViewingVolume.h>

namespace OpenEngine {

    //forward declarations
    namespace Scene {
        class TransformationNode;
        class GeometryNode;
        //        class PointLightNode;
    }

/*     namespace Display { */
/*         class IViewingVolume; */
/*     } */

namespace Renderers {
namespace OpenGL {

using namespace OpenEngine::Geometry;
using OpenEngine::Scene::TransformationNode;
using OpenEngine::Scene::GeometryNode;
//using OpenEngine::Scene::PointLightNode;
//using OpenEngine::Scene::DirectionalLightNode;
//using OpenEngine::Scene::SpotLightNode;
using OpenEngine::Scene::ISceneNodeVisitor;

using OpenEngine::Core::IListener;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Renderers::RenderingEventArg;


/**
 * Renderer using OpenGL
 *
 * @class Renderer Renderer.h Renderers/OpenGL/IRenderer.h
 */
class ShadowMapBuilder: public ISceneNodeVisitor, public IListener<RenderingEventArg> {
 private:
    //    float pos[4], dir[4];
    //    GLint lightCount;
    Display::IViewingVolume* volume;
    Display::Camera* camera;

 public:

    ShadowMapBuilder(Display::Camera* vol); 
    ~ShadowMapBuilder();
        
    void Handle(RenderingEventArg arg);
    
    void VisitTransformationNode(TransformationNode* node);

    void VisitGeometryNode(GeometryNode* node);
    
    void ApplyViewingVolume(Display::IViewingVolume& volume);
    
    //    void VisitPointLightNode(PointLightNode* node);

    //    void VisitSpotLightNode(SpotLightNode* node);
        
};

} // NS OpenGL
} // NS OpenEngine
} // NS Renderers

#endif
