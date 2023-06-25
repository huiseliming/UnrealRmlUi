// Fill out your copyright notice in the Description page of Project Settings.


#include "SRmlWidget.h"

#include "SlateOptMacros.h"
#include "RmlInterface/UnrealRmlRenderInterface.h"
#include "RmlInterface/UnrealRmlSystemInterface.h"

THIRD_PARTY_INCLUDES_START
#include "tinyxml2.h"
THIRD_PARTY_INCLUDES_END

TSharedPtr<SRmlWidget> SRmlWidget::FocusedRmlWidget;
static int32 GContextIndexCounter = 0;
static TMap<FKey, Rml::Input::KeyIdentifier> GRmlKeyToUEKey =
{
    { EKeys::Zero, Rml::Input::KeyIdentifier::KI_0 },
    { EKeys::Zero, Rml::Input::KeyIdentifier::KI_0 },
    { EKeys::One, Rml::Input::KeyIdentifier::KI_1 },
    { EKeys::Two, Rml::Input::KeyIdentifier::KI_2 },
    { EKeys::Three, Rml::Input::KeyIdentifier::KI_3 },
    { EKeys::Four, Rml::Input::KeyIdentifier::KI_4 },
    { EKeys::Five, Rml::Input::KeyIdentifier::KI_5 },
    { EKeys::Six, Rml::Input::KeyIdentifier::KI_6 },
    { EKeys::Seven, Rml::Input::KeyIdentifier::KI_7 },
    { EKeys::Eight, Rml::Input::KeyIdentifier::KI_8 },
    { EKeys::Nine, Rml::Input::KeyIdentifier::KI_9 },

    { EKeys::A, Rml::Input::KeyIdentifier::KI_A },
    { EKeys::B, Rml::Input::KeyIdentifier::KI_B },
    { EKeys::C, Rml::Input::KeyIdentifier::KI_C },
    { EKeys::D, Rml::Input::KeyIdentifier::KI_D },
    { EKeys::E, Rml::Input::KeyIdentifier::KI_E },
    { EKeys::F, Rml::Input::KeyIdentifier::KI_F },
    { EKeys::G, Rml::Input::KeyIdentifier::KI_G },
    { EKeys::H, Rml::Input::KeyIdentifier::KI_H },
    { EKeys::I, Rml::Input::KeyIdentifier::KI_I },
    { EKeys::J, Rml::Input::KeyIdentifier::KI_J },
    { EKeys::K, Rml::Input::KeyIdentifier::KI_K },
    { EKeys::L, Rml::Input::KeyIdentifier::KI_L },
    { EKeys::M, Rml::Input::KeyIdentifier::KI_M },
    { EKeys::N, Rml::Input::KeyIdentifier::KI_N },
    { EKeys::O, Rml::Input::KeyIdentifier::KI_O },
    { EKeys::P, Rml::Input::KeyIdentifier::KI_P },
    { EKeys::Q, Rml::Input::KeyIdentifier::KI_Q },
    { EKeys::R, Rml::Input::KeyIdentifier::KI_R },
    { EKeys::S, Rml::Input::KeyIdentifier::KI_S },
    { EKeys::T, Rml::Input::KeyIdentifier::KI_T },
    { EKeys::U, Rml::Input::KeyIdentifier::KI_U },
    { EKeys::V, Rml::Input::KeyIdentifier::KI_V },
    { EKeys::W, Rml::Input::KeyIdentifier::KI_W },
    { EKeys::X, Rml::Input::KeyIdentifier::KI_X },
    { EKeys::Y, Rml::Input::KeyIdentifier::KI_Y },
    { EKeys::Z, Rml::Input::KeyIdentifier::KI_Z },

    { EKeys::NumPadZero, Rml::Input::KeyIdentifier::KI_NUMPAD0 },
    { EKeys::NumPadOne, Rml::Input::KeyIdentifier::KI_NUMPAD1 },
    { EKeys::NumPadTwo, Rml::Input::KeyIdentifier::KI_NUMPAD2 },
    { EKeys::NumPadThree, Rml::Input::KeyIdentifier::KI_NUMPAD3 },
    { EKeys::NumPadFour, Rml::Input::KeyIdentifier::KI_NUMPAD4 },
    { EKeys::NumPadFive, Rml::Input::KeyIdentifier::KI_NUMPAD5 },
    { EKeys::NumPadSix, Rml::Input::KeyIdentifier::KI_NUMPAD6 },
    { EKeys::NumPadSeven, Rml::Input::KeyIdentifier::KI_NUMPAD7 },
    { EKeys::NumPadEight, Rml::Input::KeyIdentifier::KI_NUMPAD8 },
    { EKeys::NumPadNine, Rml::Input::KeyIdentifier::KI_NUMPAD9 },

    { EKeys::F1 , Rml::Input::KeyIdentifier::KI_F1 },
    { EKeys::F2 , Rml::Input::KeyIdentifier::KI_F2 },
    { EKeys::F3 , Rml::Input::KeyIdentifier::KI_F3 },
    { EKeys::F4 , Rml::Input::KeyIdentifier::KI_F4 },
    { EKeys::F5 , Rml::Input::KeyIdentifier::KI_F5 },
    { EKeys::F6 , Rml::Input::KeyIdentifier::KI_F6 },
    { EKeys::F7 , Rml::Input::KeyIdentifier::KI_F7 },
    { EKeys::F8 , Rml::Input::KeyIdentifier::KI_F8 },
    { EKeys::F9 , Rml::Input::KeyIdentifier::KI_F9 },
    { EKeys::F10, Rml::Input::KeyIdentifier::KI_F10 },
    { EKeys::F11, Rml::Input::KeyIdentifier::KI_F11 },
    { EKeys::F12, Rml::Input::KeyIdentifier::KI_F12 },

    { EKeys::Enter, Rml::Input::KeyIdentifier::KI_RETURN },
    { EKeys::SpaceBar, Rml::Input::KeyIdentifier::KI_SPACE },
    { EKeys::Escape, Rml::Input::KeyIdentifier::KI_ESCAPE },

    { EKeys::Multiply, Rml::Input::KeyIdentifier::KI_MULTIPLY },
    { EKeys::Add, Rml::Input::KeyIdentifier::KI_ADD },
    { EKeys::Subtract, Rml::Input::KeyIdentifier::KI_SUBTRACT },
    { EKeys::Decimal, Rml::Input::KeyIdentifier::KI_DECIMAL },
    { EKeys::Divide, Rml::Input::KeyIdentifier::KI_DIVIDE },

    { EKeys::PageUp, Rml::Input::KeyIdentifier::KI_PRIOR },
    { EKeys::PageDown, Rml::Input::KeyIdentifier::KI_NEXT },
    { EKeys::End, Rml::Input::KeyIdentifier::KI_END },
    { EKeys::Home, Rml::Input::KeyIdentifier::KI_HOME },

    { EKeys::Left, Rml::Input::KeyIdentifier::KI_LEFT },
    { EKeys::Right, Rml::Input::KeyIdentifier::KI_RIGHT },
    { EKeys::Down, Rml::Input::KeyIdentifier::KI_DOWN },
    { EKeys::Up, Rml::Input::KeyIdentifier::KI_UP },

    { EKeys::Insert, Rml::Input::KeyIdentifier::KI_INSERT },
    { EKeys::Delete, Rml::Input::KeyIdentifier::KI_DELETE },
    { EKeys::BackSpace, Rml::Input::KeyIdentifier::KI_BACK },

    { EKeys::LeftShift, Rml::Input::KeyIdentifier::KI_LSHIFT },
    { EKeys::RightShift, Rml::Input::KeyIdentifier::KI_RSHIFT },
    { EKeys::LeftControl, Rml::Input::KeyIdentifier::KI_LCONTROL },
    { EKeys::RightControl, Rml::Input::KeyIdentifier::KI_RCONTROL },

    { EKeys::NumLock, Rml::Input::KeyIdentifier::KI_NUMLOCK  },
    { EKeys::ScrollLock, Rml::Input::KeyIdentifier::KI_SCROLL  },
};

Rml::Input::KeyIdentifier ToRmlKey(FKey InKey)
{
    const Rml::Input::KeyIdentifier* RmlKeyResult = GRmlKeyToUEKey.Find(InKey);
    return RmlKeyResult ? *RmlKeyResult : Rml::Input::KeyIdentifier::KI_UNKNOWN;
}

int ToRmlKeyModifierState(const FModifierKeysState& InModifierKeysState)
{
    int RmlKeyModifier = 0;
    RmlKeyModifier |= InModifierKeysState.IsControlDown() ? Rml::Input::KeyModifier::KM_CTRL     : 0;
    RmlKeyModifier |= InModifierKeysState.IsShiftDown()   ? Rml::Input::KeyModifier::KM_SHIFT    : 0;
    RmlKeyModifier |= InModifierKeysState.IsAltDown()     ? Rml::Input::KeyModifier::KM_ALT      : 0;
    RmlKeyModifier |= InModifierKeysState.IsCommandDown() ? Rml::Input::KeyModifier::KM_META     : 0;
    RmlKeyModifier |= InModifierKeysState.AreCapsLocked() ? Rml::Input::KeyModifier::KM_CAPSLOCK : 0;
    return RmlKeyModifier;
}


SRmlWidget::~SRmlWidget()
{
    if (Context)
    {
        Rml::RemoveContext(Context->GetName());
        Context = nullptr;
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SRmlWidget::Construct(const FArguments& InArgs)
{
    bAutoloadFileChanges = InArgs._bAutoloadFileChanges;
    /*
    ChildSlot
    [
         Populate the widget
    ];
    */
    if (Context == nullptr)
    {
        FVector2D SlateLocalSize = GetPaintSpaceGeometry().GetLocalSize();
        Rml::Vector2i RmlCurrentSize((int)SlateLocalSize.X, (int)SlateLocalSize.Y);
        Context = Rml::CreateContext("Ctx_" + std::to_string(GContextIndexCounter++), RmlCurrentSize);
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SRmlWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    if (Context)
    {
        FVector2D SlateLocalSize = AllottedGeometry.GetLocalSize();
        Rml::Vector2i RmlCurrentSize((int)SlateLocalSize.X, (int)SlateLocalSize.Y);
        Rml::Vector2i RmlLastSize = Context->GetDimensions();
        if (RmlCurrentSize != RmlLastSize)
        {
            Context->SetDimensions(RmlCurrentSize);
        }
        Context->Update();
    }
    //if (bAutoloadFileChanges.Get())
    //{
    //	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    //	if (!DocumentFilePaths.IsEmpty())
    //	{
    //		if (PlatformFile.FileExists(*DocumentFilePaths[0]))
    //		{
    //			IFileManager::Get().GetTimeStamp(*DocumentFilePaths[0]);
    //		}
    //	}
    //}
}

int32 SRmlWidget::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    check(IsInGameThread());
    if (!Context) return LayerId;
    CUnrealRmlRenderInterface* RenderInterface = (CUnrealRmlRenderInterface*)Rml::GetRenderInterface();
    RenderInterface->BeginRender(
        //Args,
        AllottedGeometry, 
        //MyCullingRect, 
        OutDrawElements, 
        LayerId//,
        //InWidgetStyle,
        //bParentEnabled
    );
    Context->Render();
    RenderInterface->EndRender();
    return LayerId + 1;
}

FReply SRmlWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    return Context->ProcessKeyDown(ToRmlKey(InKeyEvent.GetKey()), ToRmlKeyModifierState(InKeyEvent.GetModifierKeys())) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    return Context->ProcessKeyUp(ToRmlKey(InKeyEvent.GetKey()), ToRmlKeyModifierState(InKeyEvent.GetModifierKeys())) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
    TCHAR Ch = InCharacterEvent.GetCharacter();
    if (Ch == TEXT('\r'))
    {
        return Context->ProcessTextInput(Rml::Character(TEXT('\n'))) ? FReply::Unhandled() : FReply::Handled();
    }
    if (!FChar::IsPrint(Ch)) return FReply::Unhandled();
    return Context->ProcessTextInput(Rml::Character(Ch)) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    auto MousePos = MouseEvent.GetScreenSpacePosition();
    MousePos = MyGeometry.GetAccumulatedRenderTransform().Inverse().TransformPoint(MousePos);
    return Context->ProcessMouseMove(MousePos.X, MousePos.Y, ToRmlKeyModifierState(MouseEvent.GetModifierKeys())) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return Context->ProcessMouseButtonDown(
        ToRmlKey(MouseEvent.GetEffectingButton()),
        ToRmlKeyModifierState(MouseEvent.GetModifierKeys())) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    return Context->ProcessMouseButtonUp(
        ToRmlKey(MouseEvent.GetEffectingButton()),
        ToRmlKeyModifierState(MouseEvent.GetModifierKeys())) ? FReply::Unhandled() : FReply::Handled();
}

FReply SRmlWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    auto ModifierState = MouseEvent.GetModifierKeys();
    return Context->ProcessMouseWheel(
        -MouseEvent.GetWheelDelta(),
        ToRmlKeyModifierState(ModifierState)) ? FReply::Unhandled() : FReply::Handled();
}

FCursorReply SRmlWidget::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
    CUnrealRmlSystemInterface* SystemInterface = (CUnrealRmlSystemInterface*)Rml::GetSystemInterface();
    return FCursorReply::Cursor(SystemInterface->GetCachedMouseCursorType());
}

FReply SRmlWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    //UE_LOG(LogTemp, Warning, TEXT("OnFocusReceived"));
    FocusedRmlWidget = StaticCastSharedRef<SRmlWidget>(this->AsShared());
    return SCompoundWidget::OnFocusReceived(MyGeometry, InFocusEvent);
}

void SRmlWidget::OnFocusLost(const FFocusEvent& InFocusEvent)
{
    SCompoundWidget::OnFocusLost(InFocusEvent);
    DeactivateIME();
    FocusedRmlWidget.Reset();
    //UE_LOG(LogTemp, Warning, TEXT("OnFocusLost"));
}

Rml::ElementDocument* SRmlWidget::LoadDocument(const FString& InDocumentPath, bool bUnloadOldDocuments)
{
    if (bUnloadOldDocuments)
    {
        if (Rml::ElementDocument* BodyDocument = Context->GetDocument("body"))
            Context->UnloadDocument(BodyDocument);
        DocumentFilePaths.Empty();
    }
    DocumentFilePaths.AddDefaulted();
    if (FPaths::IsRelative(InDocumentPath))
    {
        DocumentFilePaths.Last() = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Rml"), InDocumentPath);
    }
    else
    {
        DocumentFilePaths.Last() = InDocumentPath;
    }
    DocumentFilePaths.Last() = FPaths::ConvertRelativePathToFull(DocumentFilePaths.Last());
    
    Rml::ElementDocument* Document = Context->LoadDocument(TCHAR_TO_UTF8(*DocumentFilePaths.Last()));

    if (Document)
    {
        Document->Show();
    }
    return Document;
}

TArray<Rml::ElementDocument*> SRmlWidget::LoadDocuments(const TArray<FString>& InDocumentPaths, bool bUnloadOldDocuments)
{
    if (bUnloadOldDocuments)
    {
        if (Rml::ElementDocument* BodyDocument = Context->GetDocument("body"))
            Context->UnloadDocument(BodyDocument);
        DocumentFilePaths.Empty();
    }
    TArray<Rml::ElementDocument*> Documents;
    for (auto& DocumentPath : InDocumentPaths)
    {
        DocumentFilePaths.AddDefaulted();
        if (FPaths::IsRelative(DocumentPath))
        {
            DocumentFilePaths.Last() = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Rml"), DocumentPath);
        }
        else
        {
            DocumentFilePaths.Last() = DocumentPath;
        }
        DocumentFilePaths.Last() = FPaths::ConvertRelativePathToFull(DocumentFilePaths.Last());
        Documents.Add(Context->LoadDocument(TCHAR_TO_UTF8(*DocumentFilePaths.Last())));
        if (Documents.Last())
        {
            Documents.Last()->Show();
        }
    }
    return Documents;
}

void SRmlWidget::ReloadDocuments()
{
    Rml::Factory::ClearStyleSheetCache();
    Rml::Factory::ClearTemplateCache();
    TArray<FString> TempDocumentFilePaths = DocumentFilePaths;
    LoadDocuments(TempDocumentFilePaths);
}

void SRmlWidget::SaveDocumentToFile(const FString& InFilePath)
{
    check(IsInGameThread());
    tinyxml2::XMLDocument XMLDocument;
    tinyxml2::XMLDeclaration* XMLDeclaration = XMLDocument.NewDeclaration();
    tinyxml2::XMLElement* rml_XMLElement = XMLDocument.NewElement("rml");
    check(rml_XMLElement == XMLDocument.InsertEndChild(rml_XMLElement));
    tinyxml2::XMLElement* head_XMLElement = XMLDocument.NewElement("head");
    check(head_XMLElement == rml_XMLElement->InsertEndChild(head_XMLElement));
    tinyxml2::XMLElement* body_XMLElement = XMLDocument.NewElement("body");
    check(body_XMLElement == rml_XMLElement->InsertEndChild(body_XMLElement));

    Rml::ElementDocument* RmlDocument = Context->GetDocument(0);
    Rml::SharedPtr<const Rml::ElementDefinition> ElementDefinition = RmlDocument->GetStyleSheet()->GetElementDefinition(RmlDocument);


    if (RmlDocument)
    {
        //Rml::ElementList ElementList;
        //RmlDocument->GetElementsByTagName(ElementList, "body");
        Rml::Element* body_RmlElement = RmlDocument;
        for (auto& [String, Variant] : body_RmlElement->GetAttributes())
        {
            body_XMLElement->SetAttribute(String.c_str(), Variant.Get<Rml::String>().c_str());
        }
        struct FLocal 
        {
            static void SaveChildElementToDoc(tinyxml2::XMLDocument& XMLDocument, tinyxml2::XMLElement* ParentXMLElement, Rml::Element* ParentRmlElement)
            {
                Rml::Element* ChildRmlElement = ParentRmlElement->GetFirstChild();
                while (ChildRmlElement)
                {
                    if (Rml::ElementText* RmlElementText = rmlui_dynamic_cast<Rml::ElementText*>(ChildRmlElement))
                    {
                        tinyxml2::XMLText* XMLText = XMLDocument.NewText(RmlElementText->GetText().c_str());
                        ParentXMLElement->InsertEndChild(XMLText);
                    }
                    else
                    {
                        tinyxml2::XMLElement* ChildXMLElement = XMLDocument.NewElement(ChildRmlElement->GetTagName().c_str());
                        for (auto& [String, Variant] : ChildRmlElement->GetAttributes())
                        {
                            ChildXMLElement->SetAttribute(String.c_str(), Variant.Get<Rml::String>().c_str());
                        }
                        Rml::String Style;
                        //Rml::PropertiesIteratorView PropertiesIteratorView = ChildRmlElement->IterateLocalProperties();
                        //while (!PropertiesIteratorView.AtEnd())
                        //{
                        //	Rml::String StylePropertyName = Rml::StyleSheetSpecification::GetPropertyName(PropertiesIteratorView.GetId());
                        //	Rml::String StylePropertyValue = PropertiesIteratorView.GetProperty().ToString();
                        //	Style += StylePropertyName + ": " + StylePropertyValue + ";";
                        //	++PropertiesIteratorView;
                        //}
                        //ChildXMLElement->SetAttribute("style", Style.c_str());

                        //const Rml::PropertyMap& LocalStyleProperties = ChildRmlElement->GetLocalStyleProperties();
                        //int32 NumLocalStyleProperties = LocalStyleProperties.size();
                        //if (NumLocalStyleProperties > 0)
                        //{
                        //	int32 CurrentLocalStylePropertyIndex = 0;
                        //	auto LocalPropertyIterator = LocalStyleProperties.begin();
                        //	do
                        //	{
                        //		Rml::String StylePropertyName = Rml::StyleSheetSpecification::GetPropertyName(LocalPropertyIterator->first);
                        //		Rml::String StylePropertyValue = LocalPropertyIterator->second.ToString();
                        //		Style += StylePropertyName + ": " + StylePropertyValue + ";";
                        //		++LocalPropertyIterator;
                        //		++CurrentLocalStylePropertyIndex;
                        //	} while (CurrentLocalStylePropertyIndex < NumLocalStyleProperties);
                        //	ChildXMLElement->SetAttribute("style", Style.c_str());
                        //}

                        //Rml::SharedPtr<const Rml::ElementDefinition> ElementDefinition = ChildRmlElement->GetStyleSheet()->GetElementDefinition(ChildRmlElement);
                        //const Rml::ElementStyle* ElementStyle = ChildRmlElement->GetStyle();
                        Rml::ElementStyle* ElementStyle = ChildRmlElement->GetStyle();
                        if (ElementStyle)
                        {

                        }
                        const Rml::PropertyMap& LocalStyleProperties = ChildRmlElement->GetLocalStyleProperties();
                        if (LocalStyleProperties.size() > 0)
                        {
                            auto NumLocalStyleProperties = LocalStyleProperties.size();
                            auto LocalPropertyIterator = LocalStyleProperties.begin();
                            for (int32 i = 0; i < NumLocalStyleProperties; i++)
                            {
                                if (i != 0)
                                    ++LocalPropertyIterator;
                                if (LocalPropertyIterator->second.value.GetType() != Rml::Variant::NONE)
                                {
                                    Rml::String StylePropertyName = Rml::StyleSheetSpecification::GetPropertyName(LocalPropertyIterator->first);
                                    Rml::String StylePropertyValue = LocalPropertyIterator->second.ToString();
                                    Style += StylePropertyName + ": " + ";";
                                }
                                //Style += StylePropertyName + ": " + StylePropertyValue + ";";
                            }
                            //UE_LOG(LogTemp, Warning, TEXT("0"));
                            //for (auto& [PropertyId, Property] : LocalStyleProperties)
                            //{
                            //	UE_LOG(LogTemp, Warning, TEXT("1"));
                            //	Rml::String StylePropertyName = Rml::StyleSheetSpecification::GetPropertyName(PropertyId);
                            //	UE_LOG(LogTemp, Warning, TEXT("2"));
                            //	Rml::String StylePropertyValue = ChildRmlElement->GetProperty(PropertyId)->ToString();
                            //	UE_LOG(LogTemp, Warning, TEXT("3"));
                            //	Style += StylePropertyName + ": " + StylePropertyValue + ";";
                            //	UE_LOG(LogTemp, Warning, TEXT("4"));
                            //}
                            //UE_LOG(LogTemp, Warning, TEXT("5"));
                            ChildXMLElement->SetAttribute("style", Style.c_str());
                        }
                        ParentXMLElement->InsertEndChild(ChildXMLElement);
                        FLocal::SaveChildElementToDoc(XMLDocument, ChildXMLElement, ChildRmlElement);
                    }
                    ChildRmlElement = ChildRmlElement->GetNextSibling();
                }
            }
        };
        FLocal::SaveChildElementToDoc(XMLDocument, body_XMLElement, body_RmlElement);
    }
    //tinyxml2::XMLPrinter XMLPrinter;
    //XMLDocument.Accept(&XMLPrinter);
    //UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(XMLPrinter.CStr()));
    tinyxml2::XMLError XMLError = XMLDocument.SaveFile(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectDir(), "a.xml")));
    if (tinyxml2::XML_SUCCESS != XMLError)
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveFile FAILED XMLError : %d"), XMLError);
    }
}

void SRmlWidget::ActivateIME()
{
    if (ITextInputMethodSystem* const TextInputMethodSystem = FSlateApplication::Get().GetTextInputMethodSystem())
    {
        if (!TextInputMethodContext.IsValid())
        {
            TextInputMethodContext = MakeShareable(new FRmlTextInputMethodContext(StaticCastSharedRef<SRmlWidget>(this->AsShared())));
            TextInputMethodChangeNotifier = TextInputMethodSystem->RegisterContext(TextInputMethodContext.ToSharedRef());
            if (TextInputMethodChangeNotifier.IsValid())
            {
                TextInputMethodChangeNotifier->NotifyLayoutChanged(ITextInputMethodChangeNotifier::ELayoutChangeType::Created);
            }
        }
        if (TextInputMethodContext.IsValid())
        {
            TSharedRef<FRmlTextInputMethodContext> TextInputMethodContextRef = TextInputMethodContext.ToSharedRef();
            if (!TextInputMethodSystem->IsActiveContext(TextInputMethodContextRef))
            {
                //UE_LOG(LogTemp, Warning, TEXT("ActivateIME"));
                TextInputMethodSystem->ActivateContext(TextInputMethodContextRef);
            }
        }
    }
}

void SRmlWidget::DeactivateIME()
{
    if (ITextInputMethodSystem* const TextInputMethodSystem = FSlateApplication::Get().GetTextInputMethodSystem())
    {
        if (TextInputMethodContext.IsValid())
        {
            TSharedRef<FRmlTextInputMethodContext> TextInputMethodContextRef = TextInputMethodContext.ToSharedRef();
            if (TextInputMethodSystem->IsActiveContext(TextInputMethodContextRef))
            {
                if (TextInputMethodContextRef->IsComposing())
                {
                    TextInputMethodContextRef->AbortComposition();
                    if (TextInputMethodChangeNotifier.IsValid())
                    {
                        TextInputMethodChangeNotifier->CancelComposition();
                    }
                }
                //UE_LOG(LogTemp, Warning, TEXT("DeactivateIME"));
                TextInputMethodSystem->DeactivateContext(TextInputMethodContextRef);
            }
        }
    }
}
