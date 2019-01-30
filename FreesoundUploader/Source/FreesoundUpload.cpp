/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "FreesoundUpload.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FreesoundUpload::FreesoundUpload ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    textButton3.reset (new TextButton ("new button"));
    addAndMakeVisible (textButton3.get());
    textButton3->setButtonText (TRANS("CC0"));
    textButton3->setConnectedEdges (Button::ConnectedOnLeft);
    textButton3->addListener (this);
    textButton3->setColour (TextButton::buttonColourId, Colours::grey);

    textButton3->setBounds (512, 424, 88, 24);

    textEditor.reset (new TextEditor ("new text editor"));
    addAndMakeVisible (textEditor.get());
    textEditor->setMultiLine (false);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (false);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (true);
    textEditor->setPopupMenuEnabled (true);
    textEditor->setText (TRANS("Description of the sound\n"));

    textEditor->setBounds (256, 328, 344, 88);

    textButton2.reset (new TextButton ("new button"));
    addAndMakeVisible (textButton2.get());
    textButton2->setButtonText (TRANS("AttribNC"));
    textButton2->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnRight);
    textButton2->addListener (this);
    textButton2->setColour (TextButton::buttonColourId, Colours::grey);

    textButton2->setBounds (424, 424, 88, 24);

    textButton4.reset (new TextButton ("new button"));
    addAndMakeVisible (textButton4.get());
    textButton4->setButtonText (TRANS("Attribution"));
    textButton4->setConnectedEdges (Button::ConnectedOnRight);
    textButton4->addListener (this);
    textButton4->setColour (TextButton::buttonColourId, Colours::grey);

    textButton4->setBounds (336, 424, 88, 24);

    license.reset (new Label ("License",
                              TRANS("License:\n")));
    addAndMakeVisible (license.get());
    license->setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
    license->setJustificationType (Justification::centredLeft);
    license->setEditable (false, false, false);
    license->setColour (TextEditor::textColourId, Colours::black);
    license->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    license->setBounds (256, 424, 72, 24);

    textEditor2.reset (new TextEditor ("new text editor"));
    addAndMakeVisible (textEditor2.get());
    textEditor2->setMultiLine (false);
    textEditor2->setReturnKeyStartsNewLine (false);
    textEditor2->setReadOnly (false);
    textEditor2->setScrollbarsShown (true);
    textEditor2->setCaretVisible (true);
    textEditor2->setPopupMenuEnabled (true);
    textEditor2->setText (TRANS("Name"));

    textEditor2->setBounds (256, 264, 344, 24);

    imageButton.reset (new ImageButton ("imageButton"));
    addAndMakeVisible (imageButton.get());
    imageButton->setButtonText (TRANS("new button"));
    imageButton->addListener (this);

    imageButton->setImages (false, true, true,
                            Image(), 1.0f, Colour (0x00000000),
                            Image(), 1.0f, Colour (0x00000000),
                            Image(), 1.0f, Colour (0x00000000));
    imageButton->setBounds (256, 232, 150, 24);

    textEditor3.reset (new TextEditor ("new text editor"));
    addAndMakeVisible (textEditor3.get());
    textEditor3->setMultiLine (false);
    textEditor3->setReturnKeyStartsNewLine (false);
    textEditor3->setReadOnly (false);
    textEditor3->setScrollbarsShown (true);
    textEditor3->setCaretVisible (true);
    textEditor3->setPopupMenuEnabled (true);
    textEditor3->setText (TRANS("Tags"));

    textEditor3->setBounds (256, 296, 344, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

FreesoundUpload::~FreesoundUpload()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    textButton3 = nullptr;
    textEditor = nullptr;
    textButton2 = nullptr;
    textButton4 = nullptr;
    license = nullptr;
    textEditor2 = nullptr;
    imageButton = nullptr;
    textEditor3 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FreesoundUpload::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FreesoundUpload::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FreesoundUpload::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton3.get())
    {
        //[UserButtonCode_textButton3] -- add your button handler code here..
        //[/UserButtonCode_textButton3]
    }
    else if (buttonThatWasClicked == textButton2.get())
    {
        //[UserButtonCode_textButton2] -- add your button handler code here..
        //[/UserButtonCode_textButton2]
    }
    else if (buttonThatWasClicked == textButton4.get())
    {
        //[UserButtonCode_textButton4] -- add your button handler code here..
        //[/UserButtonCode_textButton4]
    }
    else if (buttonThatWasClicked == imageButton.get())
    {
        //[UserButtonCode_imageButton] -- add your button handler code here..
        //[/UserButtonCode_imageButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FreesoundUpload" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.33"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="new button" id="8caedda781085b2c" memberName="textButton3"
              virtualName="" explicitFocusOrder="0" pos="512 424 88 24" bgColOff="ff808080"
              buttonText="CC0" connectedEdges="1" needsCallback="1" radioGroupId="0"/>
  <TEXTEDITOR name="new text editor" id="4ba2e06a3dd738ea" memberName="textEditor"
              virtualName="" explicitFocusOrder="0" pos="256 328 344 88" initialText="Description of the sound&#10;"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTBUTTON name="new button" id="3d176857338643a5" memberName="textButton2"
              virtualName="" explicitFocusOrder="0" pos="424 424 88 24" bgColOff="ff808080"
              buttonText="AttribNC" connectedEdges="3" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="new button" id="64efb54d09cfd024" memberName="textButton4"
              virtualName="" explicitFocusOrder="0" pos="336 424 88 24" bgColOff="ff808080"
              buttonText="Attribution" connectedEdges="2" needsCallback="1"
              radioGroupId="0"/>
  <LABEL name="License" id="4ae3ef4cd6fad27f" memberName="license" virtualName=""
         explicitFocusOrder="0" pos="256 424 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="License:&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="d365dd33316dbe27" memberName="textEditor2"
              virtualName="" explicitFocusOrder="0" pos="256 264 344 24" initialText="Name"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <IMAGEBUTTON name="imageButton" id="656e3f609ade701" memberName="imageButton"
               virtualName="" explicitFocusOrder="0" pos="256 232 150 24" buttonText="new button"
               connectedEdges="0" needsCallback="1" radioGroupId="0" keepProportions="1"
               resourceNormal="" opacityNormal="1.0" colourNormal="0" resourceOver=""
               opacityOver="1.0" colourOver="0" resourceDown="" opacityDown="1.0"
               colourDown="0"/>
  <TEXTEDITOR name="new text editor" id="e66b8a7c160785c1" memberName="textEditor3"
              virtualName="" explicitFocusOrder="0" pos="256 296 344 24" initialText="Tags"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
