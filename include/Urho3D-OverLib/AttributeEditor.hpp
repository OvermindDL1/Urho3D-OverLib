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

#include <Urho3D/Container/Ptr.h>

namespace Urho3D
{
class AttributeInfo;
class Context;
class EventHandler;
class LineEdit;
class String;
class UIElement;
class Variant;
class XMLFile;
}

namespace OverLib
{

namespace AttributeEditor
{

Urho3D::SharedPtr<Urho3D::UIElement> CreateAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);//, Urho3D::EventHandler* handler);

Urho3D::SharedPtr<Urho3D::UIElement> CreateBoolAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);//, Urho3D::EventHandler* handler);

Urho3D::SharedPtr<Urho3D::UIElement> CreateNumAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);//, Urho3D::EventHandler* handler);

Urho3D::SharedPtr<Urho3D::UIElement> CreateStringAttributeEditor(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::AttributeInfo& info, unsigned int index, unsigned int subIndex);//, Urho3D::EventHandler* handler);

Urho3D::SharedPtr<Urho3D::UIElement> CreateAttributeEditorParent(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::String& name, unsigned int index, unsigned int subIndex);

Urho3D::SharedPtr<Urho3D::LineEdit> CreateAttributeLineEdit(Urho3D::Context* context, Urho3D::XMLFile* style, const Urho3D::String& name, unsigned int index, unsigned int subIndex);

void SetValue(Urho3D::UIElement* toElement, const Urho3D::Variant& fromValue);
Urho3D::Variant GetValue(const Urho3D::Variant& toValueWithDefault, const Urho3D::UIElement* fromElement);

}

}
