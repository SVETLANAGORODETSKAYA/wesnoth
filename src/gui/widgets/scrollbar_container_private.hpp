/* $Id$ */
/*
   Copyright (C) 2009 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_WIDGETS_SCROLLBAR_CONTAINER_PRIVATE_HPP_INCLUDED
#define GUI_WIDGETS_SCROLLBAR_CONTAINER_PRIVATE_HPP_INCLUDED

#include "gui/widgets/scrollbar_container.hpp"

/**
 * @file gui/widgets/scrollbar_containe_private.hpp
 * Helper for header for the scrollbar_container.
 *
 * @note This file should only be included by scrollbar_container.cpp.
 *
 * This file is being used for a small experiment in which some private
 * functions of tscrollbar_container are no longer in tscrollbar_container
 * but moved in a friend class with static functions. The goal is to have
 * less header recompilations, when there's a need to add or remove a private
 * function.  Also non-trivial functions like 'const foo& bar() const' and
 * 'foo& bar()' are wrapped in a template to avoid code duplication (for
 * typing not for the binary) to make maintenance easier.
 */

namespace gui2 {

/**
 * Helper to implement private functions without modifing the header.
 *
 * The class is a helper to avoid recompilation and only has static
 * functions.
 */
struct tscrollbar_container_implementation
{
	/**
	 * Implementation for the wrappers for
	 * [const] twidget* tscrollbar_container::find_widget(
	 * const tpoint&, const bool) [const].
	 *
	 * @param W                   twidget or const twidget.
	 */
	template<class W>
	static W* find_widget(
			typename tconst_duplicator<W, tscrollbar_container>::type&
				scrollbar_container,
			const tpoint& coordinate, const bool must_be_active)
	{

		assert(scrollbar_container.content_
				&& scrollbar_container.content_grid_);

		W* result = scrollbar_container.tcontainer_
				::find_widget(coordinate, must_be_active);

		if(result == scrollbar_container.content_) {
			return scrollbar_container.content_grid_->
					find_widget(coordinate, must_be_active);
		}

		return result;
	}

	/**
	 * Implementation for the wrappers for
	 * [const] twidget* tscrollbar_container::find_widget(
	 * const std::string&, const bool) [const].
	 *
	 * @param W                   twidget or const twidget.
	 */
	template<class W>
	static W* find_widget(
			typename tconst_duplicator<W, tscrollbar_container>::type&
				scrollbar_container,
			const std::string& id, const bool must_be_active)
	{
		// Inherited.
		W* result = scrollbar_container.tcontainer_
				::find_widget(id, must_be_active);

		// Can be called before finalize so test instead of assert for the grid.
		if(!result && scrollbar_container.content_grid_) {
			result = scrollbar_container.
				content_grid_->find_widget(id, must_be_active);
		}

		return result;
	}
};

} // namespace gui2

#endif

