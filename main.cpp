#include <fstream>
#include "ShadowMapSetup.h"
#include <Display/FollowCamera.h>
#include <Scene/RenderStateNode.h>
#include <Renderers/OpenGL/LightRenderer.h>

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

#include "ShadowMapBuilder.h"

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Physics;
using namespace OpenEngine::Display;

using OpenEngine::Renderers::OpenGL::RenderingView;
using OpenEngine::Utils::ShadowMapSetup;

// Configuration structure to pass around to the setup methods
struct Config {
    ShadowMapSetup        setup;
    PointLightNode*       lightNode;
    FollowCamera*         camera;
    ISceneNode*           renderingScene;
    ISceneNode*           dynamicScene;
    Config()
        : setup(ShadowMapSetup("ShadowDemo"))
        , lightNode(NULL)
        , camera(NULL)
        , renderingScene(NULL)
        , dynamicScene(NULL)
    {
        
    }
};

void SetupResources(Config&);
void SetupShadow(Config&);
void SetupLight(Config&);
void SetupCamera(Config&);
void SetupDevices(Config&);
void SetupShaders(Config&);
void SetupScene(Config&);

int main(int argc, char** argv) {
    Config config;
    SetupResources(config);
    SetupCamera(config);

    SetupLight(config);
    SetupShadow(config);
    SetupDevices(config);
    SetupShaders(config);
    SetupScene(config);

    //    ofstream out("test.dot", ios::out); // create output file
    //DotVisitor dot;                        // create dot visitor
    //dot.Write(*config.renderingScene, &out);           // build and write the graph
    //out.close();                           // close your file
    
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

    config.camera->SetPosition(Vector<3, float>(0, 100, -200));
    config.camera->LookAt(0,0,0);
    //    config.camera->LookAt(0, 0, 0);
}

void SetupShadow(Config& config) {
    //ShadowMapBuilder* sb = new ShadowMapBuilder(config.camera);
    //config.setup.GetRenderer().InitializeEvent().Attach(*sb);
    //config.setup.GetRenderer().PreProcessEvent().Attach(*sb);
}

void SetupLight(Config& config) {
    // Set up a light node
    PointLightNode* pln = new PointLightNode();
    pln->active = true;
    /*    pln->constAtt = 1.0;
    pln->linearAtt = 0.0;
    pln->quadAtt = 0.0;
    pln->ambient = Vector<4, float>(0,0,0,1);
    pln->diffuse = Vector<4, float>(0,0,0,1);
    pln->specular = Vector<4, float>(0,0,0,1);*/
    config.lightNode = pln;
    LightRenderer* lr = new LightRenderer(*config.camera);
    config.setup.GetRenderer().InitializeEvent().Attach(*lr);
    config.setup.GetRenderer().PreProcessEvent().Attach(*lr);
    config.setup.GetRenderer().DeinitializeEvent().Attach(*lr);

    RenderStateNode* rsn = new RenderStateNode();
    rsn->EnableOption(RenderStateNode::LIGHTING);
    config.renderingScene = rsn;

}

void SetupDevices(Config& config) {
    MoveHandler* move_h = new MoveHandler(*config.setup.GetShadowMapCamera(), config.setup.GetMouse());
    config.setup.GetKeyboard().KeyEvent().Attach(*move_h);
    config.setup.GetEngine().InitializeEvent().Attach(*move_h);
    config.setup.GetEngine().ProcessEvent().Attach(*move_h);
    config.setup.GetEngine().DeinitializeEvent().Attach(*move_h);
}

void SetupShaders(Config& config) {
    //GLSLResource* res = new GLSLResource("shaders/test.frag");
}

void SetupScene(Config& config) {
    // Create scene nodes
    config.dynamicScene = new SceneNode();

    config.renderingScene->AddNode(config.dynamicScene);

    // Attach light node
    TransformationNode* light_tran = new TransformationNode();
    light_tran->SetPosition(Vector<3, float>(200, 200, 0));
    light_tran->AddNode(config.lightNode);
    config.dynamicScene->AddNode(light_tran);

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
            config.camera->Follow(mod_tran);
        }
        
        current->AddNode(mod_tran);
        logger.info << "Successfully loaded " << mod_str << logger.end;
    }
    mfile->close();
    delete mfile;

    //Ground
    /*FacePtr f1(new Face(Vector<3, float>(-200,0,200), Vector<3, float>(200,0,200),
                        Vector<3, float>(0,0,-200), Vector<3, float>(0,1,0),
                        Vector<3, float>(0,1,0), Vector<3, float>(0,1,0)));
    FaceSet* tri_faces = new FaceSet();
    tri_faces->Add(f1);
    GeometryNode* tri = new GeometryNode(tri_faces);
    current->AddNode(tri);
    */
}
