/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(
    NewProjectAudioProcessor &p)
    : AudioProcessorEditor(&p), _audioProcessor(p), _fontCombo(),
      _font(juce::FontOptions{}), _frameLabel("") {
  _edit.onReturnKey = [this] {
    _frameLabel.setText("onReturnKey " + juce::String(_counter++),
                        juce::NotificationType::dontSendNotification);
  };

  _edit.onTextChange = [this] {
    _frameLabel.setText("onTextChange" + juce::String(_counter++),
                        juce::NotificationType::dontSendNotification);
  };

  _selectedFontFileName = BinaryData::namedResourceList[0];

  for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
    auto fontName = BinaryData::namedResourceList[i];
    if (juce::String(fontName).contains("ttf")) {
      _fontCombo.addItem(juce::String(fontName), i + 1);
    }
  }

  auto updateTestText = [this] {
    int fontFileSize = 0;
    auto fontFile =
        BinaryData::getNamedResource(_selectedFontFileName, fontFileSize);

    _font = juce::Font{juce::FontOptions{
        juce::Typeface::createSystemTypefaceFor(fontFile, fontFileSize)}
                           .withHeight(_fontHeight)};

    _testText.setFont(_font);
    _testText.repaint();

    _fontDetails.setText(
        juce::String("Font: ") + _testText.getFont().getTypefaceName() + " " +
            std::to_string(_testText.getFont().getHeight()) + " px",
        juce::NotificationType::sendNotification);
  };

  _fontCombo.onChange = [this, updateTestText] {
    auto selectedItemIndex = _fontCombo.getSelectedItemIndex();
    if (selectedItemIndex > 0) {
      _selectedFontFileName =
          BinaryData::namedResourceList[selectedItemIndex - 1];
      DBG("Selecting Font: " << _selectedFontFileName);
      updateTestText();
    }
  };
  _fontCombo.setSelectedItemIndex(0);
  _fontCombo.onChange();

  _fontSizeLabel.setEditable(true);
  _fontSizeLabel.onTextChange = [this, updateTestText]() {
    auto text = _fontSizeLabel.getText();
    try {
      _fontHeight = std::stof(text.toStdString());
      updateTestText();
    } catch (...) {
      // Ignore
    }
  };

  _fontSizeLabel.setText("13.0", juce::NotificationType::dontSendNotification);

  _testText.setText("Length     Envelope     Dry/Wet",
                    juce::NotificationType::dontSendNotification);
  _testText.setJustificationType(juce::Justification::centred);

  addAndMakeVisible(_testText);
  addAndMakeVisible(_fontDetails);
  addAndMakeVisible(_fontCombo);
  addAndMakeVisible(_fontSizeLabel);
  addAndMakeVisible(_edit);
  addAndMakeVisible(_frameLabel);

  updateTestText();

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(800, 332);

  juce::MessageManager::callAsync([this] { //
    _edit.setText(_audioProcessor.getName());
  });
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor() {}

//==============================================================================
void NewProjectAudioProcessorEditor::paint(juce::Graphics &g) {
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void NewProjectAudioProcessorEditor::resized() {
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..

  auto bounds = getLocalBounds();

  _testText.setBounds(bounds.removeFromTop(25).removeFromLeft(200));
  bounds.removeFromTop(10);
  _fontDetails.setBounds(bounds.removeFromTop(25).removeFromLeft(200));
  _fontCombo.setBounds(bounds.removeFromTop(25).removeFromLeft(400));
  _fontSizeLabel.setBounds(bounds.removeFromTop(25).removeFromLeft(100));

  _edit.setBounds(bounds.removeFromBottom(25).removeFromLeft(100));
  _frameLabel.setBounds(bounds.removeFromBottom(25).removeFromLeft(100));
}
