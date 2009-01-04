#ifndef _SHADOW_MAP_SETUP_H_
#define _SHADOW_MAP_SETUP_H_

#include <Meta/Config.h>

// include the main interfaces
#include <Core/IEngine.h>
#include <Devices/IJoystick.h>
#include <Devices/IKeyboard.h>
#include <Devices/IMouse.h>
#include <Display/Camera.h>
#include <Display/IFrame.h>
#include <Renderers/IRenderer.h>
#include <Scene/ISceneNode.h>
#include <Resources/IModelResource.h>
#include <Resources/ITextureResource.h>
#include <Display/HUD.h>

// include all the classes that depend on serialization
#include <Resources/TGAResource.h>
// @todo: what about all the scene nodes?

// other std c++ includes
#include <string>

// forward declarations
namespace OpenEngine {
    namespace Core {
        class Engine;
    }
    namespace Display {
        class Viewport;
        class IViewingVolume;
        class Frustum;
        class SDLFrame;
        class HUD;
    }
    namespace Devices {
        class SDLInput;
    }
    namespace Scene {
        class SceneNode;
    }
    namespace Renderers {
        class TextureLoader;
        namespace OpenGL {
            class ShadowMapRenderer;
            class ShadowRenderingView;
            class ShadowMapRenderingView;
        }
    }
}

namespace OpenEngine {
namespace Utils {

class ShadowMapSetup {
public:

    ShadowMapSetup(std::string title);

    Core::IEngine& GetEngine() const;
    Display::IFrame& GetFrame() const;
    Renderers::IRenderer& GetRenderer() const;

    Devices::IMouse&    GetMouse() const;
    Devices::IKeyboard& GetKeyboard() const;
    Devices::IJoystick& GetJoystick() const;

    Display::HUD& GetHUD() const;

    Scene::ISceneNode* GetScene() const;
    void SetScene(Scene::ISceneNode& scene);

    Display::Camera* GetCamera() const;
    void SetCamera(Display::Camera& volume);
    void SetCamera(Display::IViewingVolume& volume);

    void AddDataDirectory(std::string dir);

    void EnableDebugging();

    Display::Camera* GetShadowMapCamera() const;

private:
    std::string title;
    Core::Engine* engine;
    Display::SDLFrame* frame;
    Display::Viewport* viewport;
    Renderers::OpenGL::ShadowMapRenderer* renderer;
    Devices::SDLInput* input;
    Scene::ISceneNode* scene;
    Display::Camera* camera;
    Display::Frustum* frustum;
    Renderers::OpenGL::ShadowRenderingView* shadowRenderingview;
    Renderers::TextureLoader* textureloader;
    Display::HUD* hud;

    Display::SDLFrame* shadowMapFrame;
    Display::Viewport* shadowMapViewport;
    Display::Camera* shadowMapCamera;
    Display::Frustum* shadowMapFrustum;
    Renderers::OpenGL::ShadowMapRenderingView* shadowMapRenderingview;
};

} // NS Utils
} // NS OpenEngine

#endif // _SHADOW_MAP_SETUP_H_
