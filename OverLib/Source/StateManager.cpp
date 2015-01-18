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

#include "OverLib/StateManager.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;
using namespace OverLib;


StateObject::StateObject(Context* context): Object(context)
{
    SubscribeToEvent(this, E_STATEPRESTART, HANDLER(StateObject, HandleStatePreStart));
    SubscribeToEvent(this, E_STATESTART, HANDLER(StateObject, HandleStateStart));
    SubscribeToEvent(this, E_STATEEND, HANDLER(StateObject, HandleStateEnd));
    SubscribeToEvent(this, E_STATEPOSTEND, HANDLER(StateObject, HandleStatePostEnd));
    SubscribeToEvent(this, E_STATELOADINGUPDATE, HANDLER(StateObject, HandleLoadingUpdate));
}

void StateObject::HandleStatePreStart(StringHash eventType, VariantMap& eventData)
{
}

void StateObject::HandleStateStart(StringHash eventType, VariantMap& eventData)
{
}

void StateObject::HandleStateEnd(StringHash eventType, VariantMap& eventData)
{
}

void StateObject::HandleStatePostEnd(StringHash eventType, VariantMap& eventData)
{
}

void StateObject::HandleLoadingUpdate(StringHash eventType, VariantMap& eventData)
{
}

void StateObject::PostLoadingUpdate(const String& msg)
{
    GetSubsystem<OverLib::StateManager>()->PostLoadingUpdate(msg);
}

void StateObject::PostLoadingComplete()
{
    GetSubsystem<OverLib::StateManager>()->PostLoadingComplete();
}


StateManager::StateManager(Context* context)
    : Object(context)
    , internalState_(NO_TRANSITION)
{
}

StateManager::StateManager(Context* context, Object* loadingState)
    : Object(context)
    , loadingState_(loadingState)
    , internalState_(NO_TRANSITION)
{
    SetState(loadingState_);
}

StateManager::~StateManager()
{
}

Object* StateManager::GetState()
{
    return state_;
}

void StateManager::SetState(Object* state)
{
    if (internalState_ != NO_TRANSITION) {
        throw "Tried to set state while already setting a state";
    }
    internalState_ = LOADINGSCREEN;

    if (loadingState_) {
        loadingState_->SendEvent(E_STATEPRESTART);
    }
    if (state_) {
        state_->SendEvent(E_STATEEND);
    }
    if (loadingState_) {
        loadingState_->SendEvent(E_STATESTART);
    }
    if (state_) {
        state_->SendEvent(E_STATEPOSTEND);
    }
    if (state) {
        state->SendEvent(E_STATEPRESTART);
    }

    using namespace StateLoadingStart;
    VariantMap& eventData = context_->GetEventDataMap();
    eventData[P_OLDSTATE] = state_;
    eventData[P_NEWSTATE] = state;
    SendEvent(E_STATELOADINGSTART, eventData);

    state_ = state;
}

void StateManager::PostLoadingComplete()
{
    if (internalState_ != LOADINGSCREEN) {
        throw "Tried to post done loading to switch to final state while not loading";
    }
    internalState_ = NO_TRANSITION;

    if (loadingState_) {
        loadingState_->SendEvent(E_STATEEND);
    }
    if (state_) {
        state_->SendEvent(E_STATESTART);
    }
    if (loadingState_) {
        loadingState_->SendEvent(E_STATEPOSTEND);
    }

    using namespace StateLoadingEnd;
    VariantMap& eventData = context_->GetEventDataMap();
    eventData[P_NEWSTATE] = state_;
    SendEvent(E_STATELOADINGEND, eventData);
}

void StateManager::SetLoadingState(Object* loadingState)
{
    if (internalState_ != NO_TRANSITION) {
        throw "Tried to set loading state while already setting a state";
    }
    loadingState_ = loadingState;
}

Object* StateManager::GetLoadingState()
{
    return loadingState_;
}

void StateManager::PostLoadingUpdate(String msg)
{
    if (loadingState_) {
        using namespace StateLoadingUpdate;
        VariantMap& eventData = context_->GetEventDataMap();
        eventData[P_MESSAGE] = msg;
        loadingState_->SendEvent(E_STATELOADINGUPDATE, eventData);
    }
}
