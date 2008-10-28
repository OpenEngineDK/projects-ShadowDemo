
#include <fstream>

// Simple setup of a rendering engine
#include <Utils/SimpleSetup.h>

#include <Display/FollowCamera.h>

// Rendering structures
//#include <Renderers/IRenderNode.h>
//#include <Renderers/OpenGL/RenderingView.h>
//#include <Renderers/IRenderNode.h>
#include <Renderers/RenderStateNode.h>
#include <Renderers/OpenGL/LightRenderer.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/ResourceManager.h>

// Scene structures
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
//#include <Scene/VertexArrayTransformer.h>
//#include <Scene/DisplayListTransformer.h>
#include <Scene/PointLightNode.h>
#include <Scene/DotVisitor.h>
//AccelerationStructures extension
//#include <Scene/CollectedGeometryTransformer.h>
//#include <Scene/QuadTransformer.h>
//#include <Scene/BSPTransformer.h>
//#include <Scene/ASDotVisitor.h>
//#include <Renderers/AcceleratedRenderingView.h>

#include <Utils/MoveHandler.h>
//#include <Utils/Statistics.h>

//FixedTimeStepPhysics extension
#include <Physics/FixedTimeStepPhysics.h>
#include <Physics/RigidBox.h>


// Additional namespaces
using namespace OpenEngine::Core;
//using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Physics;
using namespace OpenEngine::Display;

using OpenEngine::Renderers::OpenGL::RenderingView;


using OpenEngine::Utils::SimpleSetup;

// Configuration structure to pass around to the setup methods
struct Config {
    SimpleSetup           setup;
    PointLightNode*       lightNode;
    FollowCamera*         camera;
    ISceneNode*           renderingScene;
    ISceneNode*           dynamicScene;
    Config()
        : setup(SimpleSetup("ShadowDemo"))
        , lightNode(NULL)
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
void SetupRendering(Config&);


int main() {
    Config config;
    SetupResources(config);
    SetupCamera(config);
    SetupLight(config);
    SetupRendering(config);
    SetupDevices(config);
    SetupScene(config);

    ofstream out("test.dot", ios::out); // create output file
    DotVisitor dot;                        // create dot visitor
    dot.Write(*config.renderingScene, &out);           // build and write the graph
    out.close();                           // close your file
    
    config.setup.SetScene(*config.renderingScene);
    config.setup.GetEngine().Start();
    return 0;
}

void SetupResources(Config& config) {
    config.setup.AddDataDirectory("projects/ShadowDemo/data/");
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
    LightRenderer* lr = new LightRenderer(*config.setup.GetCamera());
    //config.setup.GetRenderer().InitializeEvent().Attach(*lr);
    config.setup.GetRenderer().PreProcessEvent().Attach(*lr);
    //config.setup.GetRenderer().DeinitializeEvent().Attach(*lr);
}

void SetupCamera(Config& config) {
    config.camera        = new FollowCamera( *config.setup.GetCamera() );
    config.setup.SetCamera(*config.camera);

    config.camera->SetPosition(Vector<3, float>(200, 200, 200));
    config.camera->LookAt(0, 0, 0);
}

void SetupRendering(Config& config) {
    RenderStateNode* rsn = new RenderStateNode();
    rsn->EnableOption(RenderStateNode::LIGHTING);
    config.renderingScene = rsn;

    //    GLfloat am[] = {0.0, 0.0, 0.0, 1.0};
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, am);
}


void SetupDevices(Config& config) {
    // Register movement handler to be able to move the camera
    MoveHandler* move_h = new MoveHandler(*config.camera, config.setup.GetMouse());
    config.setup.GetKeyboard().KeyEvent().Attach(*move_h);

    // Keyboard bindings to the rigid box and camera
//     KeyboardHandler* keyHandler = new KeyboardHandler(config.setup.GetEngine(),
//                                                       config.camera,
//                                                       config.physicBody,
//                                                       config.physics);
//     config.setup.GetKeyboard().KeyEvent().Attach(*keyHandler);

//     config.setup.GetEngine().InitializeEvent().Attach(*keyHandler);
//     config.setup.GetEngine().ProcessEvent().Attach(*keyHandler);
//    config.setup.GetEngine().DeinitializeEvent().Attach(*keyHandler);

    config.setup.GetEngine().InitializeEvent().Attach(*move_h);
    config.setup.GetEngine().ProcessEvent().Attach(*move_h);
    config.setup.GetEngine().DeinitializeEvent().Attach(*move_h);
}


void SetupScene(Config& config) {

    // Create scene nodes
    config.dynamicScene = new SceneNode();

    config.renderingScene->AddNode(config.dynamicScene);

    // Attach light node
    TransformationNode* light_tran = new TransformationNode();
    light_tran->SetPosition(Vector<3, float>(0, 200, 0));
    light_tran->AddNode(config.lightNode);
    config.dynamicScene->AddNode(light_tran);

    ISceneNode* current = config.dynamicScene;

    // Position of the vehicle
    //Vector<3,float> position(0, 0, 0);

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
        //mod_tran->SetPosition(position);
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
    FacePtr f1(new Face(Vector<3, float>(-200,0,200), Vector<3, float>(200,0,200),
                        Vector<3, float>(0,0,-200), Vector<3, float>(0,1,0),
                        Vector<3, float>(0,1,0), Vector<3, float>(0,1,0)));
    FaceSet* tri_faces = new FaceSet();
    tri_faces->Add(f1);
    GeometryNode* tri = new GeometryNode(tri_faces);
    current->AddNode(tri);
    
}
