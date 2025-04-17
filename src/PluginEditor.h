/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class NewProjectAudioProcessor;

//==============================================================================
/**
 */
class NewProjectAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  NewProjectAudioProcessorEditor(NewProjectAudioProcessor &);
  ~NewProjectAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  NewProjectAudioProcessor &_audioProcessor;

  const char *_selectedFontFileName{nullptr};
  float _fontHeight{13.0f};

  juce::ComboBox _fontCombo;
  juce::Label _fontSizeLabel;
  juce::Font _font;

  juce::Label _testText, _fontDetails;
  juce::TextEditor _edit;
  juce::Label _frameLabel;
  int _counter = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};
