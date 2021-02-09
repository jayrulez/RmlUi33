/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef RMLUICONTROLSWIDGETSLIDER_H
#define RMLUICONTROLSWIDGETSLIDER_H

#include "../../Include/RmlUi/Core/EventListener.h"

namespace Rml {
namespace Controls {

class ElementFormControl;

/**
	A generic widget for incorporating sliding functionality into an element.

	@author Peter Curry
 */

class WidgetSlider : public Core::EventListener
{
public:
	enum Orientation
	{
		VERTICAL,
		HORIZONTAL
	};

	WidgetSlider(ElementFormControl* parent);
	virtual ~WidgetSlider();

	/// Initialises the slider's hidden elements.
	bool Initialise();

	/// Updates the key repeats for the increment / decrement arrows.
	void Update();

	/// Sets the position of the bar.
	/// @param[in] bar_position The new position of the bar (0 representing the start of the track, 1 representing the end).
	void SetBarPosition(float bar_position);
	/// Returns the current position of the bar.
	/// @return The current position of the bar (0 representing the start of the track, 1 representing the end).
	float GetBarPosition();

	/// Sets the orientation of the slider.
	/// @param[in] orientation The slider's new orientation.
	void SetOrientation(Orientation orientation);
	/// Returns the slider's orientation.
	/// @return The orientation of the slider.
	Orientation GetOrientation() const;

	/// Sets the dimensions to the size of the slider.
	/// @param[in] dimensions The dimensions to size.
	void GetDimensions(Rml::Core::Vector2f& dimensions) const;

protected:
	/// Lays out and resizes the slider's internal elements.
	/// @param[in] containing_block The padded box containing the slider. This is used to resolve relative properties.
	/// @param[in] slider_length The total length, in pixels, of the slider widget.
	/// @param[in] bar_length The total length of the bar, as a proportion of the track length. If this is -1, the intrinsic length will be used.
	void FormatElements(const Rml::Core::Vector2f& containing_block, float slider_length, float bar_length = -1);
	/// Lays out and positions the bar element.
	/// @param[in] bar_length The total length of the bar, as a proportion of the track length. If this is -1, the intrinsic length will be used.
	void FormatBar(float bar_length = -1);

	/// Returns the widget's parent element.
	Core::Element* GetParent() const;

	/// Handles events coming through from the slider's components.
	void ProcessEvent(Core::Event& event) override;

	/// Called when the slider's bar position is set or dragged.
	/// @param[in] bar_position The new position of the bar (0 representing the start of the track, 1 representing the end).
	/// @return The new position of the bar.
	virtual float OnBarChange(float bar_position) = 0;
	/// Called when the slider is incremented by one 'line', either by the down / right key or a mouse-click on the
	/// increment arrow.
	/// @return The new position of the bar.
	virtual float OnLineIncrement() = 0;
	/// Called when the slider is decremented by one 'line', either by the up / left key or a mouse-click on the
	/// decrement arrow.
	/// @return The new position of the bar.
	virtual float OnLineDecrement() = 0;

private:
	/// Determine the normalized bar position given an absolute position coordinate.
	/// @param[in] absolute_position Absolute position along the axis determined by 'orientation'.
	/// @return The normalized bar position [0, 1]
	float AbsolutePositionToBarPosition(float absolute_position) const;

	void PositionBar();

    ElementFormControl* parent;

	Orientation orientation;

	// The background track element, across which the bar slides.
	Core::Element* track;
	// The bar element. This is the element that is dragged across the trough.
	Core::Element* bar;
	// The two (optional) buttons for incrementing and decrementing the slider.
	Core::Element* arrows[2];

	// A number from 0 to 1, indicating how far along the track the bar is.
	float bar_position;
	// If the bar is being dragged, this is the pixel offset from the start of the bar to where it was picked up.
	float bar_drag_anchor;

	// Set to the auto-repeat timer if either of the arrow buttons have been pressed, -1 if they haven't.
	float arrow_timers[2];
	double last_update_time;
};

}
}

#endif
