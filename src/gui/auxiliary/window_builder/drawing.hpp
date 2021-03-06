/* $Id$ */
/*
   Copyright (C) 2010 - 2012 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_DRAWING_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_DRAWING_HPP_INCLUDED

#include "config.hpp"
#include "gui/auxiliary/window_builder/control.hpp"

namespace gui2 {

namespace implementation {

struct tbuilder_drawing
	: public tbuilder_control
{
	explicit tbuilder_drawing(const config& cfg);

	twidget* build () const;

	/** The width of the widget. */
	tformula<unsigned> width;

	/** The height of the widget. */
	tformula<unsigned> height;

	/** Config containing what to draw on the widgets canvas. */
	config draw;
};

} // namespace implementation

} // namespace gui2

#endif

