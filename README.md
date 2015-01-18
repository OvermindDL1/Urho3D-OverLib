# Urho3D-OverLib
Urho3D Extra library helpers

These are various repeated bits of code I use in my projects, not everything,
but it may be added to over time.  These are designed to be generic, fast,
little-to-no overhead, and useful.  Pull requests are always welcome.

## How to use
Just git clone or copy this project in to your project and in your
CMakeLists.txt file add a line of `add_subdirectory(OverLib)` then just add the
library and headers to your project via the normal CMake way, such as in:
```cmake
target_link_libraries (${TARGET_NAME} OverLib)
include_directories(${OverLib_SOURCE_DIR}/include)
```

## Information and Disclaimer
Everything defined in OverLib is in use by projects and has been tested to have
no faults with the use that it was designed for, but if any bugs are found then
please send pull requests with fixes.  If any new features are wanted then also
please send pull requests with the new features.  On occasion this library may
be updated as the root project grows as well, for example the next update may be
an Input Mapper as that exists in the multiple parent projects and needs to be
made more generic as well.

## Sections of library

### OverLib
The OverLib class contains a few basic helpers, mostly designed for applications.
It can either by multi-subclassed by your Application subclass such as in:
```cpp
class URHO3D_API OverAssault_Application : public Urho3D::Application, public OverLib::OverLib
{
    // Urho3D Type Information System
    OBJECT(OverAssault_Application);
```
Or it can be used directly as it has static members.

The members it has at the time of the writing of this Readme are:

```cpp
template <class T> static T* GetOrCreateSubSystem(Urho3D::Context* context);
```
This will either get a given subsystem or create it if it does not exist if it
has a constructor that just takes a Context.  Plan to add C++ Move support for
perfect forwarding if a need comes up for it

```cpp
template <class T> static void SendBackgroundLoadResource(Urho3D::Context* context, const Urho3D::String& name, bool sendEventOnFailure = true, Urho3D::Resource* caller = 0);
```
This is a wrapper for the normal Urho3D `ResourceCache::BackgroundLoadResource`
method that will background load the given resource if it is not already loaded,
or will just load the existing resource if it already is.  Unlike the normal
Urho3D method this will always send the events of success/fail as appropriate
even if it loaded an already existing resource, thus your code for background
resource loading stays more clean and in one spot.

### AttributeEditor
This namespace is primarily a partial porting of the AttributeEditor code from
the Urho3D Editor to C++, its static functions in OverLib::AttrributeEditor are:
```cpp
Urho3D::SharedPtr<Urho3D::UIElement> CreateAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::UIElement> CreateBoolAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::UIElement> CreateNumAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::UIElement> CreateStringAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::UIElement> CreateAttributeEditorParent(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::String& name, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::LineEdit> CreateAttributeLineEdit(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::String& name, unsigned int index, unsigned int subIndex);

void SetValue(Urho3D::UIElement* toElement, const Urho3D::Variant& fromValue);
Urho3D::Variant GetValue(const Urho3D::Variant& toValueWithDefault, const Urho3D::UIElement* fromElement);
```
They will return a properly named set of UI Elements that you can bind and
listen to as normal, for example I use it like:
```cpp
                Settings *settings = GetSubsystem<Settings>();
                XMLFile *style = GetSubsystem<UI>()->GetRoot()->GetDefaultStyle();
                const Vector<AttributeInfo>& attrs = *settings->GetAttributes();
                for(int i=0; i<attrs.Size(); ++i)
                {
                    if(attrs[i].mode_ & AM_NOEDIT) continue;
                    SharedPtr<UIElement> elem(AttributeEditor::CreateAttributeEditor(
                            context_,
                            style,
                            attrs[i],
                            i, 1
                    ));
                    if(elem) optionsList->AddItem(elem);
```
Then I load/save from/to it as in:
```cpp
void StateMainMenu::LoadFromSettings()
{
    Settings* settings = GetSubsystem<Settings>();
    const Vector<AttributeInfo>& attrs = *settings->GetAttributes();
    for(int i=0; i<attrs.Size(); ++i)
    {
        const AttributeInfo &attr = attrs[i];
        if(attr.mode_ & AM_NOEDIT) continue;
        const Variant &value = settings->GetAttribute(i);
        UIElement *elem = uiOptions_->GetChild(attr.name_, true);
        if(!elem)
        {
            LOGWARNING("Unable to find the UI Element to set: " + attr.name_);
            continue;
        }
        AttributeEditor::SetValue(elem, value);
    }
}

void StateMainMenu::SaveToSettings()
{
    Settings *settings = GetSubsystem<Settings>();
    const Vector<AttributeInfo>& attrs = *settings->GetAttributes();
    for(int i=0; i<attrs.Size(); ++i)
    {
        const AttributeInfo &attr = attrs[i];
        if(attr.mode_ & AM_NOEDIT) continue;
        UIElement *elem = uiOptions_->GetChild(attr.name_, true);
        if(!elem)
        {
            LOGWARNING("Unable to find the UI Element to get: " + attr.name_);
            continue;
        }
        const Variant &origValue = settings->GetAttribute(i);
        const Variant &value = AttributeEditor::GetValue(origValue, elem);
        if(origValue != value) settings->SetAttribute(i, value);
    }

    Graphics *graphics = GetSubsystem<Graphics>();
    if(graphics) graphics->SetMode(
        settings->windowResolution_.x_,
        settings->windowResolution_.y_,
        settings->windowFullscreen_,
        settings->windowBorderless_,
        settings->windowResizable_,
        settings->windowVsync_,
        settings->windowTripleBuffer_,
        settings->GetWindowMultiSampleLevel());

    settings->SaveSettingsFile();
}
```
NOTE: This is *NOT* complete, lacking a couple of attribute editors, but it was
complete enough for my use and may be expanded later.  As always pull requests
are welcome.

### StateManager
This is a high-level StateManager, its purpose is not to handle a stack of
states (though you can do that with its interfaces yourself), but rather is an
Urho3D Event style state manager, primarily to handle timing, transitions, and
loading screens between states.
The events it defines are:
```cpp
/// %StatePreStart is sent to a state when it is about to load, it is to let
/// it preload its assets and prepare its scene
EVENT(E_STATEPRESTART, StatePreStart)
{
}

/// %StateStart is sent when it is time to add its rendering data to the scene
/// and to fully activate.  This should be fast
EVENT(E_STATESTART, StateStart)
{
}

/// %StateEnd is sent when it needs to stop its rendering, such as removing
/// nodes and whatever else, this should be fast
EVENT(E_STATEEND, StateEnd)
{
}

/// %StatePostEnd is sent when the state is being removed, allowing it to
/// clean up assets and whatever else it may need
EVENT(E_STATEPOSTEND, StatePostEnd)
{
}

/// %StateLoadingUpdate is received primarily just by the Loading State,
/// it receives a message posted by the loading state between its PreStart
/// and its Start to allow it to display the state of its loading.
EVENT(E_STATELOADINGUPDATE, StateLoadingUpdate)
{
    PARAM(P_MESSAGE, Message); // String
}

/// %StateLoadingStart is sent globally to announce a state change has
/// started
EVENT(E_STATELOADINGSTART, StateLoadingStart)
{
    PARAM(P_OLDSTATE, OldState); // Old StateObject
    PARAM(P_NEWSTATE, NewState); // New StateObject
}

/// %StateLoadingEnd is sent globally to announce a state change has
/// finished and is now fully loaded and operational
EVENT(E_STATELOADINGEND, StateLoadingEnd)
{
    PARAM(P_NEWSTATE, NewState); // New StateObject
}
```
Those are primarily designed to be used by the State's themselves, which are not
a subclass nor required to subclass anything, they can be any normal Urho3D
Object.  This does, however, contain a helper class to do the grunt work and
binding for you:
```cpp
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
```
If you subclass StateObject then you can optionally inherit any of the Handle
methods to do what you need at whatever times, for example you can start loading
assets in PreStart (background loading is great here!), updating your progress
by calling PosLoadingUpdate with a string/message of how your loading is going,
this could just be a message announcing the percentage complete, or what you are
loading or just loaded, or just leave it blank, however you want to handle it.
Once loading is complete across your frames and you are to a state where you can
actually begin rendering (say that enough of your resources have background
loaded) then call PostLoadingComplete, which will then call your Start callback
and unload what was before.  PostLoadingUpdate and postLoadingComplete only
call these respectively (if you are not using the helper class):
```cpp
// PostLoadingUpdate
GetSubsystem<OverLib::StateManager>()->PostLoadingUpdate(msg);

// PostLoadingComplete
GetSubsystem<OverLib::StateManager>()->PostLoadingComplete();
```

The End event is where you should stop rendering your parts 'now', just a simple
unhooking from the scene or whatever else.  You can then do your background
unloading in the PostEnd event.

The LoadingUpdate event is only passed to a state that is a Loading state.  The
Loading state is like any other normal state, except it is always loaded (though
not Start'ed until time) in the background and is switched to whenever an old
state is unloaded but the new state has not yet finished loading its starting
assets, it receives a message from the loading state that you can display to
update the user of the loading progress if you wish, but is otherwise a normal
state like any other so you can render models, animations, just display a splash
image, whatever you wish.

The StateManager itself is defined as such:
```cpp
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
}
```
It can be constructed with no loading state (a clear screen will display when
states are being changed but there is no loading state defined) or by including
a loading state, and the loading state can be changed at any point by calling
`SetLoadingState` with it.  To change the active state itself then just call
SetState with the new state, it will then follow these steps:

1. Verifies no state transition is already in progress, or it throws.
2. PreStarts the Loading state if it exists.
3. Ends the old state if it exists.
4. Starts the loading state if it exists.
5. PostEnds the old state if it exists.
6. PreStarts the new state if it exists.

A this point it then returns and normal rendering proceeds as normal.  The state
that is starting up can call PostLoadingUpdate to pass the loading status to the
loading state so it can display updates if both wish. Once the new state calls
PostLoadingComplete (irrespective of if PostLoadingUpdate is ever called or not)
then the following happens:

1. If it is not currently state transitioning (say if PostLoadingComplete is
called without GetState ever having been called), then it throws.
2. Ends the loading state.
3. Starts the new state
4. PostEnds the loading state

In general the loading state does its asset loading in its constructor so it is
always and quickly available, unless of course your loading state is a more
complex scene, then load the resources immediately.
