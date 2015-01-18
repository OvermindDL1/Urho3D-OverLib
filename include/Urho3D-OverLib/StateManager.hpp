//
// Copyright (c) 2015 OvermindDL1.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Component.h>

#include <memory>

namespace Urho3D
{

namespace OverLib
{

EVENT(E_STATEPRESTART, StatePreStart)
{
}

EVENT(E_STATESTART, StateStart)
{
}

EVENT(E_STATEEND, StateEnd)
{
}

EVENT(E_STATEPOSTEND, StatePostEnd)
{
}

EVENT(E_STATELOADINGUPDATE, StateLoadingUpdate)
{
    PARAM(P_MESSAGE, Message); // String
}

EVENT(E_STATELOADINGSTART, StateLoadingStart)
{
    PARAM(P_OLDSTATE, OldState); // Old StateObject
    PARAM(P_NEWSTATE, NewState); // New StateObject
}

EVENT(E_STATELOADINGEND, StateLoadingEnd)
{
    PARAM(P_NEWSTATE, NewState); // New StateObject
}

/// %StateObject registers event handlers by default
class URHO3D_API StateObject : public Urho3D::Object
{
    OBJECT(StateObject);

public:
    StateObject(Urho3D::Context* context);

protected:
    virtual void HandleStatePreStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    virtual void HandleStateStart(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    virtual void HandleStateEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    virtual void HandleStatePostEnd(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    virtual void HandleLoadingUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

protected:
    void PostLoadingUpdate(const Urho3D::String& msg);
    void PostLoadingComplete();
};

/// %StateManager able to be set as a SubSystem in Urho3D
class URHO3D_API StateManager : public Urho3D::Object
{
    OBJECT(StateManager);

public:
    /// Construct.
    StateManager(Urho3D::Context* context);
    /// Constructor with an initial loading state
    StateManager(Urho3D::Context* context, Urho3D::Object* loadingState);
    /// Destruct.
    ~StateManager();

public:
    void SetState(Urho3D::Object* state);
    Urho3D::Object* GetState();

    void SetLoadingState(Urho3D::Object* loadingState);
    Urho3D::Object* GetLoadingState();

public: // Only for use by the States themselves
    void PostLoadingUpdate(Urho3D::String msg);
    void PostLoadingComplete();

private:
    Urho3D::SharedPtr<Urho3D::Object> state_;
    Urho3D::SharedPtr<Urho3D::Object> loadingState_;

    enum InternalState {
        NO_TRANSITION,
        LOADINGSCREEN

    };

    InternalState internalState_;
};

}

}