
#include "ShadowMapSetup.h"
#include "ShadowMapRenderingView.h"
#include "ShadowRenderingView.h"

// Core stuff
#include <Core/Engine.h>
#include <Display/Camera.h>
#include <Display/Frustum.h>
#include <Display/InterpolatedViewingVolume.h>
#include <Display/ViewingVolume.h>
#include <Renderers/TextureLoader.h>
#include <Resources/ResourceManager.h>
#include <Scene/DirectionalLightNode.h>
#include <Scene/SceneNode.h>

// Acceleration extension
#include <Renderers/AcceleratedRenderingView.h>
#include <Scene/ASDotVisitor.h>

// OpenGL extension
#include "ShadowMapRenderer.h"
//#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/OpenGL/ShaderLoader.h>
#include <Resources/GLSLResource.h>

// SDL extension
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

// Resources extensions
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Resources/ColladaResource.h>

// Logging
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>

// HUD
#include <Display/HUD.h>

namespace OpenEngine {
namespace Utils {

using namespace Core;
using namespace Devices;
using namespace Display;
using namespace Logging;
using namespace Renderers::OpenGL;
using namespace Renderers;
using namespace Resources;
using namespace Scene;

class ExtShadowRenderingView
    : public ShadowRenderingView
    , public AcceleratedRenderingView {
public:
    ExtShadowRenderingView(Viewport& viewport, Viewport& shadowMapViewport)
        : IRenderingView(viewport)
        , ShadowRenderingView(viewport, shadowMapViewport)
        , AcceleratedRenderingView(viewport) {}
};

class ExtShadowMapRenderingView
    : public ShadowMapRenderingView
    , public AcceleratedRenderingView {
public:
    ExtShadowMapRenderingView(Viewport& viewport)
        : IRenderingView(viewport)
        , ShadowMapRenderingView(viewport)
        , AcceleratedRenderingView(viewport) {}
};

class TextureLoadOnInit
    : public IListener<RenderingEventArg> {
    TextureLoader& tl;
public:
    TextureLoadOnInit(TextureLoader& tl) : tl(tl) { }
    void Handle(RenderingEventArg arg) {
        if (arg.renderer.GetSceneRoot() != NULL)
            tl.Load(*arg.renderer.GetSceneRoot());
    }
};

class QuitHandler : public IListener<KeyboardEventArg> {
    IEngine& engine;
public:
    QuitHandler(IEngine& engine) : engine(engine) {}
    void Handle(KeyboardEventArg arg) {
        if (arg.sym == KEY_ESCAPE) engine.Stop();
    }
};

ShadowMapSetup::ShadowMapSetup(std::string title)
    : title(title)
    , engine(new Engine())
    , frame(new SDLFrame(800,600,32))
    , viewport(new Viewport(*frame))
    , renderer(new ShadowMapRenderer(viewport))
    , input(new SDLInput())
    , scene(new SceneNode())
    , camera(new Camera(*(new InterpolatedViewingVolume(*(new ViewingVolume())))))
    , frustum(new Frustum(*camera))

    , textureloader(new TextureLoader(*renderer))
    , hud(new HUD())

    , shadowMapFrame(new SDLFrame(800,600,32))
    , shadowMapViewport(new Viewport(*shadowMapFrame))
    , shadowMapCamera(new Camera(*(new ViewingVolume())))
    , shadowMapFrustum(new Frustum(*shadowMapCamera))
    , shadowMapRenderingview(new ExtShadowMapRenderingView(*shadowMapViewport))
    , shadowRenderingview(new ExtShadowRenderingView(*viewport, *shadowMapViewport))
{
    //frustum debug test
    //frustum->VisualizeClipping(true);
    //shadowMapFrustum->VisualizeClipping(true);

    //frustum->SetFar(1200);
    //shadowMapFrustum->SetFar(1300);
    //shadowMapFrustum->SetNear(800);
    //shadow
    shadowMapViewport->SetViewingVolume(shadowMapFrustum);
    shadowMapCamera->SetPosition(Vector<3, float>(0.0,160.0,0.0));
    shadowMapCamera->LookAt(0,0,0);

    // create a logger to std out
    Logger::AddLogger(new StreamLogger(&std::cout));
    // configure modules needing process time
    engine->InitializeEvent().Attach(*frame);
    engine->InitializeEvent().Attach(*renderer);
    engine->InitializeEvent().Attach(*input);
    engine->ProcessEvent().Attach(*frame);
    engine->ProcessEvent().Attach(*renderer);
    engine->ProcessEvent().Attach(*input);
    engine->DeinitializeEvent().Attach(*frame);
    engine->DeinitializeEvent().Attach(*renderer);
    engine->DeinitializeEvent().Attach(*input);
    // add plug-ins
    ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());
    ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());
    ResourceManager<IShaderResource>::AddPlugin(new GLSLPlugin());
    // populate the default scene
    scene->AddNode(new DirectionalLightNode());
    // setup the rendering system
    renderer->ProcessEvent().Attach(*shadowRenderingview);
    renderer->SetSceneRoot(scene);
    viewport->SetViewingVolume(frustum);
    renderer->InitializeEvent().Attach(*(new TextureLoadOnInit(*textureloader)));
    // bind default keys
    input->KeyEvent().Attach(*(new QuitHandler(*engine)));
    
    renderer->PreProcessEvent().Attach(*shadowMapRenderingview);
}

/**
 * Get the engine.
 * The engine can not be replaced.
 * @see IEngine
 */
IEngine& ShadowMapSetup::GetEngine() const {
    return *engine;
}

/**
 * Get the frame.
 * The default frame is will have the dimensions 800x600 at color depth 32.
 * The frame can not be replaced.
 * In order to change the frame use the set-methods defined in IFrame.
 * @see IFrame
 */
IFrame& ShadowMapSetup::GetFrame() const {
    return *frame;
}

/**
 * Get the renderer.
 * The renderer will automatically be supplied with a rendering
 * view that uses the full frame as its viewport.
 * Additionally it will always render the current scene supplied
 * by SetScene().
 * The renderer itself is not replaceable.
 */
IRenderer& ShadowMapSetup::GetRenderer() const {
    return *renderer;
}

/**
 * Get the mouse.
 * The mouse structure is not replaceable.
 */
IMouse& ShadowMapSetup::GetMouse() const {
    return *input;
}

/**
 * Get the keyboard.
 * The keyboard structure is not replaceable.
 */
IKeyboard& ShadowMapSetup::GetKeyboard() const {
    return *input;
}

/**
 * Get the joystick.
 * The joystick structure is not replaceable.
 */
IJoystick& ShadowMapSetup::GetJoystick() const {
    return *input;
}

/**
 * Get the current scene.
 * The default scene consists of a SceneNode with a single
 * DirectionalLightNode beneath it.
 * If you wish to get rid of the light node simply delete the
 * scene and attache a new one that suits you purpose with
 * SetScene().
 * The current scene during renderer initialization will be
 * searched for textures to load.
 */
ISceneNode* ShadowMapSetup::GetScene() const {
    return scene;
}

/**
 * Set the current scene.
 * This will automatically update the renderer to use the new
 * scene.
 * Ownership of the scene remains with the caller but it is
 * assumed to be non-null as long as it is active.
 * If replacing a scene it is the callers responsibility to clean
 * up the old scene if needed by using GetScene().
 * When setting a new scene it will automatically be searched for
 * textures to load.
 * @param scene New active scene.
 */
void ShadowMapSetup::SetScene(ISceneNode& scene) {
    this->scene = &scene;
    renderer->SetSceneRoot(this->scene);
    textureloader->Load(scene);

    OpenGL::ShaderLoader* shaderLoader =
        new OpenGL::ShaderLoader(*textureloader, scene);
    engine->InitializeEvent().Attach(*shaderLoader);
}

/**
 * Get the current camera.
 * The default camera is placed in origin (0,0,0) following the
 * z-axis in the negative direction (0,0,-1).
 */
Camera* ShadowMapSetup::GetCamera() const {
    return camera;
}

Camera* ShadowMapSetup::GetShadowMapCamera() const {
    return shadowMapCamera;
}

PointLightNode* ShadowMapSetup::GetShadowLightNode() {
    return shadowMapRenderingview->GetLightNode();
}

Frustum* ShadowMapSetup::GetFrustum() {
    return frustum;
}

Frustum* ShadowMapSetup::GetShadowMapFrustum() {
    return shadowMapFrustum;
}

/**
 * Set the current camera.
 * Ownership of the camera remains with the caller.
 */
void ShadowMapSetup::SetCamera(Camera& volume) {
    camera = &volume;
    delete frustum;
    frustum = new Frustum(*camera);
    viewport->SetViewingVolume(frustum);
}
/**
 * Set a camera by viewing volume.
 * The non-camera class is used to create a *new* camera that wraps
 * the viewing volume. The new camera is created on the heap and the
 * caller is responsible for any needed clean-up of this structure.
 *
 * @param volume Volume to wrap in a *new* camera.
 */
void ShadowMapSetup::SetCamera(IViewingVolume& volume) {
    camera = new Camera(volume);
    viewport->SetViewingVolume(camera);
}

/**
 * Add a data directory to the file search path.
 * This path will be searched when loading file resources.
 * By default we load all the resource plug-ins we can.
 *
 * @param dir Directory with file resources
 */
void ShadowMapSetup::AddDataDirectory(string dir) {
    DirectoryManager::AppendPath(dir);
}

HUD& ShadowMapSetup::GetHUD() const {
    return *hud;
}

/**
 * Enable various run-time debugging features.
 * This includes
 * - visualization of the frustum,
 * - export the scene to a dot-graph file (scene.dot)
 * - add FPS to the HUD (not implemented yet)
 *
 * @todo: add cairo fps to the hud
 */
void ShadowMapSetup::EnableDebugging() {
    // Visualization of the frustum
    frustum->VisualizeClipping(true);
    scene->AddNode(frustum->GetFrustumNode());

    // @todo: this should be on initialize
    // Output a dot-graph of the scene
    ofstream dotfile("scene.dot", ofstream::out);
    if (!dotfile.good()) {
        logger.error << "Can not open 'scene.dot' for output"
                     << logger.end;
    } else {
        ASDotVisitor dot;
        dot.Write(*scene, &dotfile);
        logger.info << "Saved physics graph to 'scene.dot'"
                    << logger.end
                    << "To create a SVG image run: "
                    << "dot -Tsvg scene.dot > scene.svg"
                    << logger.end;
    }
}

} // NS Utils
} // NS OpenEngine
