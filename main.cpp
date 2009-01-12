#include <fstream>
#include "ShadowMapSetup.h"
#include <Display/FollowCamera.h>
#include <Scene/RenderStateNode.h>
#include <Renderers/OpenGL/LightRenderer.h>
#include <Display/Frustum.h>
#include <Devices/SDLInput.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>
#include <Resources/GLSLResource.h>

// Scene structures
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/PointLightNode.h>
#include <Scene/DotVisitor.h>
#include <Utils/MoveHandler.h>

//FixedTimeStepPhysics extension
#include <Physics/FixedTimeStepPhysics.h>
#include <Physics/RigidBox.h>

#include "KeyboardHandler.h"

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Physics;
using namespace OpenEngine::Display;

using OpenEngine::Renderers::OpenGL::ShadowRenderingView;
using OpenEngine::Utils::ShadowMapSetup;

// Configuration structure to pass around to the setup methods
struct Config {
    ShadowMapSetup        setup;
    TransformationNode*   lightTrans;
    FollowCamera*         camera;
    ISceneNode*           renderingScene;
    ISceneNode*           dynamicScene;
    Config()
        : setup(ShadowMapSetup("ShadowDemo"))
        , lightTrans(NULL)
        , camera(NULL)
        , renderingScene(NULL)
        , dynamicScene(NULL)
    {
        
    }
};

void SetupResources(Config&);
void SetupLight(Config&);
void SetupCamera(Config&);
void SetupDevices(Config&);
void SetupScene(Config&);

int main(int argc, char** argv) {
    Config config;
    SetupResources(config);
    SetupCamera(config);
    SetupLight(config);
    SetupDevices(config);
    SetupScene(config);

    //ofstream out("test.dot", ios::out);      // create output file
    //DotVisitor dot;                          // create dot visitor
    //dot.Write(*config.renderingScene, &out); // build and write the graph
    //out.close();                             // close your file
    
    config.setup.SetScene(*config.renderingScene);
    config.setup.GetEngine().Start();
    return 0;
}

void SetupResources(Config& config) {
    config.setup.AddDataDirectory("projects/ShadowDemo/data/");
}

void SetupCamera(Config& config) {
    config.camera        = new FollowCamera( *config.setup.GetCamera() );
    config.setup.SetCamera(*config.camera);

    config.camera->SetPosition(Vector<3, float>(400, 200, 400));
    config.camera->LookAt(0, 0, 0);
}

void SetupLight(Config& config) {
    // Set up a light node
    PointLightNode* pln = config.setup.GetShadowLightNode();
    pln->active = true;

    //draw something at the pos of the light node
    FacePtr face1(new Face(Vector<3, float>(0,0,0), Vector<3, float>(-10,0,10),
                        Vector<3, float>(-10,0,-10), Vector<3, float>(0,1,0),
                        Vector<3, float>(0,1,0), Vector<3, float>(0,1,0)));
    FacePtr face2(new Face(Vector<3, float>(0,0,0), Vector<3, float>(10,0,-10),
                        Vector<3, float>(10,0,10), Vector<3, float>(0,1,0),
                        Vector<3, float>(0,1,0), Vector<3, float>(0,1,0)));


    Vector<4,float> color = Vector<4,float>(1.0,0.0,0.0,0.0);

    face1->colr[0] = color;
    face1->colr[1] = color;
    face1->colr[2] = color;
    face2->colr[0] = color;
    face2->colr[1] = color;
    face2->colr[2] = color;

    FaceSet* faceSet = new FaceSet();
    faceSet->Add(face1);
    faceSet->Add(face2);
    GeometryNode* geom = new GeometryNode(faceSet);
    TransformationNode* light_geom_lift = new TransformationNode();
    light_geom_lift->SetPosition(Vector<3, float>(0, 10, 0));

    light_geom_lift->AddNode(geom);    

    pln->AddNode(light_geom_lift);

    // Attach light node
    TransformationNode* light_tran = new TransformationNode();
    light_tran->SetPosition(Vector<3, float>(0, 100, 200));
        light_tran->AddNode(pln);
    config.lightTrans = light_tran;

    LightRenderer* lr = new LightRenderer(*config.camera);
    config.setup.GetRenderer().InitializeEvent().Attach(*lr);
    config.setup.GetRenderer().PreProcessEvent().Attach(*lr);
    config.setup.GetRenderer().DeinitializeEvent().Attach(*lr);

    RenderStateNode* rsn = new RenderStateNode();
    rsn->EnableOption(RenderStateNode::LIGHTING);
    config.renderingScene = rsn;
}

void SetupDevices(Config& config) {
    MoveHandler* move_h = new MoveHandler(*config.camera, config.setup.GetMouse());
    config.setup.GetKeyboard().KeyEvent().Attach(*move_h);
    config.setup.GetEngine().InitializeEvent().Attach(*move_h);
    config.setup.GetEngine().ProcessEvent().Attach(*move_h);
    config.setup.GetEngine().DeinitializeEvent().Attach(*move_h);

    //To control light
    KeyboardHandler* key_h = new KeyboardHandler(config.lightTrans);
    config.setup.GetKeyboard().KeyEvent().Attach(*key_h);
}

void SetupScene(Config& config) {
    // Create scene nodes
    config.dynamicScene = new SceneNode();

    config.renderingScene->AddNode(config.dynamicScene);

    config.dynamicScene->AddNode(config.lightTrans);

    ISceneNode* current = config.dynamicScene;

    // Add models from models.txt to the scene
    ifstream* mfile = File::Open("projects/ShadowDemo/models.txt");
    
    bool firstModel = true;
    
    while (!mfile->eof()) {
        string mod_str;
        getline(*mfile, mod_str);

        // Check the string
        if (mod_str[0] == '#' || mod_str == "") continue;

        // Load the model
        IModelResourcePtr mod_res = ResourceManager<IModelResource>::Create(mod_str);
        mod_res->Load();
        if (mod_res->GetSceneNode() == NULL) continue;

        ISceneNode* mod_node = mod_res->GetSceneNode();
        mod_res->Unload();

        TransformationNode* mod_tran = new TransformationNode();
        mod_tran->AddNode(mod_node);
 
        if(firstModel){
            //config.camera->Follow(mod_tran);
            mod_tran->Move(0, 30, 190);
            firstModel = false;
        }else{
            mod_tran->Move(0, -50, 230);
        }
        
        current->AddNode(mod_tran);
        logger.info << "Successfully loaded " << mod_str << logger.end;
    }
    mfile->close();
    delete mfile;
}
