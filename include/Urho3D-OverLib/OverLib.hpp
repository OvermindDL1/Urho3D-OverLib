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
#include <Urho3D/Resource/ResourceCache.h>

namespace Urho3D
{

namespace OverLib
{

class StateManager;

class OverLib
{
public:
    /// Construct.
    OverLib(Urho3D::Context* context);

public:
    // TODO: If adding C++11+ then add perfect forwarding to the constructor
    /// Helper to create a subsystem
    template <class T> T* GetOrCreateSubSystem()
    {
        return OverLib::GetOrCreateSubSystem<T>(_context);
    }

public:
    template <class T> static T* GetOrCreateSubSystem(Urho3D::Context* context);

    template <class T> static void SendBackgroundLoadResource(Urho3D::Context* context, const Urho3D::String& name, bool sendEventOnFailure = true, Urho3D::Resource* caller = 0);

private:
    Urho3D::Context* _context;
};

}

#include <Urho3D/Core/Context.h>
#include <Urho3D/Resource/Resource.h>
#include <Urho3D/Resource/ResourceEvents.h>

template <class T> T* OverLib::OverLib::GetOrCreateSubSystem(Urho3D::Context* context)
{
    // Return existing SubSystem if possible
    T* ss = context->GetSubsystem<T>();
    if (!ss)
    {
        ss = new T(context);
        context->RegisterSubsystem(ss);
    }
    return ss;
}

template <class T> void OverLib::OverLib::SendBackgroundLoadResource(Urho3D::Context* context, const Urho3D::String& name, bool sendEventOnFailure, Urho3D::Resource* caller)
{
    Urho3D::ResourceCache* cache = context->GetSubsystem<Urho3D::ResourceCache>();
    if(!cache->BackgroundLoadResource<T>(name, sendEventOnFailure, caller))
    {
        Urho3D::Resource* resource = cache->GetResource<T>(name);

        if(!resource && sendEventOnFailure)
        {
            using namespace Urho3D::LoadFailed;

            Urho3D::VariantMap& eventData = context->GetEventDataMap();
            eventData[P_RESOURCENAME] = name;
            cache->SendEvent(Urho3D::E_LOADFAILED, eventData);
        }

        if(resource)
        {
            using namespace Urho3D::ResourceBackgroundLoaded;

            Urho3D::VariantMap& eventData = context->GetEventDataMap();
            eventData[P_RESOURCENAME] = name;
            eventData[P_SUCCESS] = !!resource;
            eventData[P_RESOURCE] = resource;
            cache->SendEvent(Urho3D::E_RESOURCEBACKGROUNDLOADED, eventData);
        }
    }
}

}