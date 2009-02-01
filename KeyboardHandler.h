#ifndef _KEYBOARD_HANDLER_
#define _KEYBOARD_HANDLER_

#include <Core/IListener.h>
//#include <Core/IEngine.h>
#include <Devices/IKeyboard.h>
#include <Devices/Symbols.h>
//#include <Display/Camera.h>
//#include <Physics/FixedTimeStepPhysics.h>
//#include <Physics/RigidBox.h>
//#include <Math/Matrix.h>
//#include <Utils/Timer.h>
#include <Scene/TransformationNode.h>
#include <Math/Vector.h>

using OpenEngine::Core::IModule;
using OpenEngine::Core::IListener;
/* using OpenEngine::Core::IEngine; */
/* using OpenEngine::Core::InitializeEventArg; */
/* using OpenEngine::Core::ProcessEventArg; */
/* using OpenEngine::Core::DeinitializeEventArg; */
using namespace OpenEngine::Devices;
/* using OpenEngine::Display::Camera; */
/* using OpenEngine::Physics::RigidBox; */
/* using OpenEngine::Physics::FixedTimeStepPhysics; */
using OpenEngine::Math::Vector;
/* using OpenEngine::Utils::Timer; */
using OpenEngine::Scene::TransformationNode;

namespace keys = OpenEngine::Devices;

class KeyboardHandler : /*public IModule,*/ public IListener<KeyboardEventArg> {
private:
    TransformationNode* transformationNode;
/*     bool up, down, left, right, mod; */
/*     float step; */
/*     Camera* camera; */
/*     RigidBox* box; */
/*     FixedTimeStepPhysics* physics; */
/*     IEngine& engine; */
/*     Timer timer; */

public:
 KeyboardHandler(TransformationNode* transformationNode)
     : transformationNode(transformationNode)
    {}


    void Handle(KeyboardEventArg arg) {
        (arg.type == EVENT_PRESS) ? KeyDown(arg) : KeyUp(arg);
    }

    void KeyDown(KeyboardEventArg arg) {
        Vector<3,float> position = transformationNode->GetPosition();
        int scale = 5;

        switch ( arg.sym ) {

        // Move
        case keys::KEY_UP:
            transformationNode->SetPosition(position + Vector<3,float>(0,0,1)*scale);
            break;
        case keys::KEY_DOWN:
            transformationNode->SetPosition(position + Vector<3,float>(0,0,-1)*scale);
            break;
        case keys::KEY_LEFT:
            transformationNode->SetPosition(position + Vector<3,float>(1,0,0)*scale);
            break;
        case keys::KEY_RIGHT:
            transformationNode->SetPosition(position + Vector<3,float>(-1,0,0)*scale);
            break;
        case keys::KEY_o:
            transformationNode->SetPosition(position + Vector<3,float>(0,1,0)*scale);
            break;
        case keys::KEY_l:
            transformationNode->SetPosition(position + Vector<3,float>(0,-1,0)*scale);
            break;

        // Log Camera position 
/*         case keys::KEY_c: { */
/*             Vector<3,float> camPos = camera->GetPosition(); */
/*             logger.info << "Camera Position: " << camPos << logger.end; */
/*             break; */
/*         } */

/*         // Increase/decrease time in Physic */
/*         case keys::KEY_PLUS:  mod = true; step =  0.001f; break; */
/*         case keys::KEY_MINUS: mod = true; step = -0.001f; break; */
        

/*         // Quit on Escape */
/*         case keys::KEY_ESCAPE: */
/*             engine.Stop(); */
/*             break; */
        default: break;
        }
    }

    void KeyUp(KeyboardEventArg arg) {
        switch ( arg.sym ) {
/*         case keys::KEY_UP:    up    = false; break; */
/*         case keys::KEY_DOWN:  down  = false; break; */
/*         case keys::KEY_LEFT:  left  = false; break; */
/*         case keys::KEY_RIGHT: right = false; break; */
/*         case keys::KEY_PLUS:  mod   = false; break; */
/*         case keys::KEY_MINUS: mod   = false; break; */

        default: break;
        }
    }
};

#endif
