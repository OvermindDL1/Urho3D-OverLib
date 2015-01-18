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

#include "Urho3D-OverLib/AttributeEditor.hpp"

#include <functional>

#include <Urho3D/Core/Attribute.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/ListView.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

using namespace OverLib::AttributeEditor;

const unsigned int ATTR_HEIGHT = 19;
const unsigned int ATTRNAME_WIDTH = 320;
const char* STRIKED_OUT = "——";

SharedPtr<UIElement> OverLib::AttributeEditor::CreateAttributeEditor(Context* context, Urho3D::XMLFile* style, const AttributeInfo& info, unsigned int index, unsigned int subIndex)//, EventHandler* handler)
{
    switch (info.type_) {
    case VAR_BOOL:
        return CreateBoolAttributeEditor(context, style, info, index, subIndex);//, handler);
    case VAR_STRING:
        //case VAR_BUFFER:
        return CreateStringAttributeEditor(context, style, info, index, subIndex);//, handler);
    case VAR_FLOAT:
    case VAR_VECTOR2:
    case VAR_VECTOR3:
    case VAR_VECTOR4:
    case VAR_QUATERNION:
    case VAR_COLOR:
    case VAR_INTVECTOR2:
    case VAR_INTRECT:
    case VAR_INT:
        return CreateNumAttributeEditor(context, style, info, index, subIndex);//, handler);
    default:
        LOGWARNING("Failed creating an attribute editor for: " + info.name_);
        return SharedPtr<UIElement>();
    }
}

SharedPtr<UIElement> OverLib::AttributeEditor::CreateBoolAttributeEditor(Context* context, Urho3D::XMLFile* style, const AttributeInfo& info, unsigned int index, unsigned int subIndex)//, EventHandler* handler)
{
    SharedPtr<UIElement> parent(CreateAttributeEditorParent(context, style, info.name_, index, subIndex));

    SharedPtr<CheckBox> edit(new CheckBox(context));
    parent->AddChild(edit);
    edit->SetName(info.name_);
    edit->SetStyleAuto();
    edit->SetVar("Index", index);
    edit->SetVar("SubIndex", subIndex);
    //edit->SubscribeToEvent(edit, E_TOGGLED, handler);

    return parent;
}

SharedPtr<UIElement> OverLib::AttributeEditor::CreateNumAttributeEditor(Context* context, Urho3D::XMLFile* style, const AttributeInfo& info, unsigned int index, unsigned int subIndex)//, EventHandler* handler)
{
    SharedPtr<UIElement> parent(CreateAttributeEditorParent(context, style, info.name_, index, subIndex));
    VariantType type = info.type_;

    unsigned int numCoords = type - VAR_FLOAT + 1;
    if (type == VAR_QUATERNION)
        numCoords = 3;
    else if (type == VAR_COLOR || type == VAR_INTRECT)
        numCoords = 4;
    else if (type == VAR_INTVECTOR2)
        numCoords = 2;
    else if (type == VAR_INT)
        numCoords = 1;

    if (type == VAR_INT && info.enumNames_) {
        SharedPtr<DropDownList> list(new DropDownList(context));
        parent->AddChild(list);
        list->SetName(info.name_);
        list->SetStyleAuto();
        list->SetFixedHeight(ATTR_HEIGHT - 2);
        list->SetResizePopup(true);
        list->SetPlaceholderText(STRIKED_OUT);
        list->SetVar("Index", index);
        list->SetVar("SubIndex", subIndex);
        list->SetLayout(Urho3D::LM_HORIZONTAL, 0, IntRect(4, 1, 4, 1));

        for (int i = 0; info.enumNames_[i]; ++i) {
            SharedPtr<Text> choice(new Text(context));
            list->AddItem(choice);
            choice->SetStyle("EditorEnumAttributeText");
            choice->SetText(info.enumNames_[i]);
        }
    } else {
        SharedPtr<UIElement> cont(new UIElement(context));
        parent->AddChild(cont);
        cont->SetDefaultStyle(style);
        cont->SetName(info.name_);
        cont->SetVar("Index", index);
        cont->SetVar("SubIndex", subIndex);
        cont->SetLayout(LM_HORIZONTAL);
        cont->SetFixedHeight(ATTR_HEIGHT);

        for (unsigned int i = 0; i < numCoords; ++i) {
            SharedPtr<LineEdit> edit = CreateAttributeLineEdit(context, style, info.name_ + "_" + String(i), index, subIndex);
            edit->SetVar("Coordinate", i);

            // TODO: CreateDragSlider(edit);

            cont->AddChild(edit);
            //if(i==0) edit->SubscribeToEvent(edit, E_TEXTFINISHED, handler);
        }
    }

    return parent;
}

SharedPtr< UIElement > OverLib::AttributeEditor::CreateStringAttributeEditor(Context* context, Urho3D::XMLFile* style, const AttributeInfo& info, unsigned int index, unsigned int subIndex)//, EventHandler* handler)
{
    SharedPtr<UIElement> parent(CreateAttributeEditorParent(context, style, info.name_, index, subIndex));

    SharedPtr<LineEdit> edit(CreateAttributeLineEdit(context, style, info.name_, index, subIndex));
    parent->AddChild(edit);
    //edit->SubscribeToEvent(edit, E_TEXTFINISHED, handler);

    return parent;
}

SharedPtr<UIElement> OverLib::AttributeEditor::CreateAttributeEditorParent(Context* context, Urho3D::XMLFile* style, const String& name, unsigned int index, unsigned int subIndex)
{
    SharedPtr<UIElement> parent(new UIElement(context));
    parent->SetDefaultStyle(style);
    parent->SetName("Edit" + String(index) + "_" + String(subIndex));
    parent->SetVar("Index", index);
    parent->SetVar("SubIndex", subIndex);
    parent->SetLayout(LM_HORIZONTAL);
    parent->SetFixedHeight(ATTR_HEIGHT);
    //list->AddItem(parent);

    SharedPtr<Text> text(new Text(context));
    parent->AddChild(text);
    text->SetStyle("EditorAttributeText");
    text->SetText(name);
    text->SetFixedWidth(ATTRNAME_WIDTH);

    return parent;
}

SharedPtr<LineEdit> OverLib::AttributeEditor::CreateAttributeLineEdit(Context* context, XMLFile* style, const String& name, unsigned int index, unsigned int subIndex)
{
    SharedPtr<LineEdit> edit(new LineEdit(context));
    edit->SetName(name);
    edit->SetDragDropMode(DD_TARGET);
    edit->SetDefaultStyle(style);
    edit->SetStyle("EditorAttributeEdit");
    edit->SetFixedHeight(ATTR_HEIGHT - 2);
    edit->SetVar("Index", index);
    edit->SetVar("SubIndex", subIndex);

    return edit;
}

void SetLineEditValueNumType(LineEdit* edit, const Urho3D::Variant& fromValue, int i, const String& name)
{
    switch (fromValue.GetType()) {
    case VAR_FLOAT:      edit->SetText(String(fromValue.GetFloat()));                break;
    case VAR_VECTOR2:    edit->SetText(String(fromValue.GetVector2().Data()[i]));    break;
    case VAR_VECTOR3:    edit->SetText(String(fromValue.GetVector3().Data()[i]));    break;
    case VAR_VECTOR4:    edit->SetText(String(fromValue.GetVector4().Data()[i]));    break;
    case VAR_QUATERNION: edit->SetText(String(fromValue.GetQuaternion().Data()[i])); break;
    case VAR_COLOR:      edit->SetText(String(fromValue.GetColor().Data()[i]));      break;
    case VAR_INTVECTOR2: edit->SetText(String(fromValue.GetIntVector2().Data()[i])); break;
    case VAR_INTRECT:    edit->SetText(String(fromValue.GetIntRect().Data()[i]));    break;
    case VAR_INT:        edit->SetText(String(fromValue.GetInt()));                  break;
    default: LOGWARNING("Unable to set number type: " + name + String(i));
    }
}

void OverLib::AttributeEditor::SetValue(UIElement* toElement, const Urho3D::Variant& fromValue)
{
    switch (fromValue.GetType()) {
    case VAR_BOOL: {
        if (toElement->GetTypeName() == CheckBox::GetTypeNameStatic()) {
            CheckBox* elem = static_cast<CheckBox*>(toElement);
            elem->SetChecked(fromValue.GetBool());
        } else
            LOGWARNING("Failed filling in boolean for: " + toElement->GetName());
        break;
    }

    case VAR_FLOAT:
    case VAR_VECTOR2:
    case VAR_VECTOR3:
    case VAR_VECTOR4:
    case VAR_QUATERNION:
    case VAR_COLOR:
    case VAR_INTVECTOR2:
    case VAR_INTRECT:
    case VAR_INT: {
        VariantType type = fromValue.GetType();
        unsigned int numCoords = type - VAR_FLOAT + 1;
        if (type == VAR_QUATERNION)
            numCoords = 3;
        else if (type == VAR_COLOR || type == VAR_INTRECT)
            numCoords = 4;
        else if (type == VAR_INTVECTOR2)
            numCoords = 2;
        else if (type == VAR_INT)
            numCoords = 1;
        if (type == VAR_INT && toElement->GetTypeName() == DropDownList::GetTypeNameStatic()) {
            DropDownList* list = static_cast<DropDownList*>(toElement);
            list->SetSelection(fromValue.GetInt());
        } else {
            if (toElement->GetTypeName() != UIElement::GetTypeNameStatic()) {
                LOGWARNING("Failed filling in string for: " + toElement->GetName());
            }
            const String& name = toElement->GetName() + "_";
            for (unsigned int i = 0; i < numCoords; ++i) {
                UIElement* elem = toElement->GetChild(name + String(i), true);
                if (!elem || elem->GetTypeName() != LineEdit::GetTypeNameStatic()) {
                    LOGWARNING("Failed filling in number value for: " + name + String(i));
                    break;
                }
                LineEdit* edit = static_cast<LineEdit*>(elem);
                SetLineEditValueNumType(edit, fromValue, i, name);
                edit->ApplyAttributes();
            }
        }
        break;
    }

    case VAR_STRING: {
        if (toElement->GetTypeName() == LineEdit::GetTypeNameStatic()) {
            LineEdit* edit = static_cast<LineEdit*>(toElement);
            edit->SetText(fromValue.GetString());
        } else
            LOGWARNING("Failed filling in string for: " + toElement->GetName());
        break;
    }

//         case VAR_BUFFER:
//             if(toElement->GetTypeName() == LineEdit::GetTypeNameStatic())
//             {
//                 LineEdit *edit = static_cast<LineEdit*>(toElement);
//                 edit->SetText(fromValue.GetBuffer());
//             }
//             else
//                 LOGWARNING("Failed filling in buffer for: "+toElement->GetName());
//             break;

    default:
        LOGWARNING("Failed filling in value for: " + toElement->GetName());
    }
}

Variant OverLib::AttributeEditor::GetValue(const Variant& toValueWithDefault, const UIElement* fromElement)
{
    switch (toValueWithDefault.GetType()) {
    case VAR_BOOL: {
        if (fromElement->GetTypeName() == CheckBox::GetTypeNameStatic()) {
            const CheckBox* elem = static_cast<const CheckBox*>(fromElement);
            return elem->IsChecked();
        } else
            LOGWARNING("Failed acquiring boolean from: " + fromElement->GetName());
        break;
    }

    case VAR_FLOAT:
    case VAR_VECTOR2:
    case VAR_VECTOR3:
    case VAR_VECTOR4:
    case VAR_QUATERNION:
    case VAR_COLOR:
    case VAR_INTVECTOR2:
    case VAR_INTRECT:
    case VAR_INT: {
        VariantType type = toValueWithDefault.GetType();
        unsigned int numCoords = type - VAR_FLOAT + 1;
        if (type == VAR_QUATERNION)
            numCoords = 3;
        else if (type == VAR_COLOR || type == VAR_INTRECT)
            numCoords = 4;
        else if (type == VAR_INTVECTOR2)
            numCoords = 2;
        else if (type == VAR_INT)
            numCoords = 1;
        if (type == VAR_INT && fromElement->GetTypeName() == DropDownList::GetTypeNameStatic()) {
            const DropDownList* list = static_cast<const DropDownList*>(fromElement);
            return Variant(list->GetSelection());
        } else {
            if (fromElement->GetTypeName() != UIElement::GetTypeNameStatic()) {
                LOGWARNING("Failed filling in string for: " + fromElement->GetName());
            }
            const String& name = fromElement->GetName() + "_";
            String output("");
            for (unsigned int i = 0; i < numCoords; ++i) {
                UIElement* elem = fromElement->GetChild(name + String(i), true);
                if (!elem || elem->GetTypeName() != LineEdit::GetTypeNameStatic()) {
                    LOGWARNING("Failed filling in number value for: " + name + String(i));
                    break;
                }
                LineEdit* edit = static_cast<LineEdit*>(elem);
                if (output.Length() > 0) output.Append(' ');
                output.Append(edit->GetText());
            }
            return Variant(toValueWithDefault.GetType(), output);
        }
        break;
    }

    case VAR_STRING: {
        if (fromElement->GetTypeName() == LineEdit::GetTypeNameStatic()) {
            const LineEdit* edit = static_cast<const LineEdit*>(fromElement);
            return edit->GetText();
        } else
            LOGWARNING("Failed acquiring string from: " + fromElement->GetName());
        break;
    }

    default:
        LOGWARNING("Unsupported type of '" + String((int)toValueWithDefault.GetType()) + "' of: " + fromElement->GetName());
    }
    return toValueWithDefault;
}
