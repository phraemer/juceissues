/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

#include "conjure_enum.hpp"

enum class component {
  scheme,
  authority,
  userinfo,
  user,
  password,
  host,
  port,
  path = 12,
  test = path,
  query,
  fragment
};

template <typename E>
[[nodiscard]]
constexpr size_t countOf()
{
  return FIX8::conjure_enum<E>::count();
}

template <typename E>
[[nodiscard]]
constexpr auto toString(E e)
{
  return FIX8::conjure_enum<E>::enum_to_string(e, true /* noscope */);
}

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(
    NewProjectAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), _frameLabel("") {
  _edit.onReturnKey = [this] {
    _frameLabel.setText("onReturnKey " + juce::String(_counter++),
                        juce::NotificationType::dontSendNotification);
  };

  _edit.onTextChange = [this] {
    _frameLabel.setText("onTextChange" + juce::String(_counter++),
                        juce::NotificationType::dontSendNotification);
  };

  addAndMakeVisible(_edit);
  addAndMakeVisible(_frameLabel);

  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(800, 332);

  // TESTING CONJURE_ENUM

  auto name{FIX8::conjure_enum<component>::enum_to_string(component::path)};
  auto name_trim{FIX8::conjure_enum<component>::enum_to_string(
      component::path, true)}; // optionally remove scope in result
      
      auto s1 = toString(component::path);
      
      DBG(">>>> " << s1);
  DBG(">>>> " << name.data());
  DBG(">>>> " << name_trim.data());
      
      auto c = countOf<component>();
      
      DBG(">>>> " << c);
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

  _edit.setBounds(bounds.removeFromBottom(25).removeFromLeft(100));
  _frameLabel.setBounds(bounds.removeFromBottom(25).removeFromLeft(100));
}
