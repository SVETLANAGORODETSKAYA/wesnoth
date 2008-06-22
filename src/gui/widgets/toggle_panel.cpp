/* $Id$ */
/*
   copyright (C) 2008 by mark de wever <koraq@xs4all.nl>
   part of the battle for wesnoth project http://www.wesnoth.org/

   this program is free software; you can redistribute it and/or modify
   it under the terms of the gnu general public license version 2
   or at your option any later version.
   this program is distributed in the hope that it will be useful,
   but without any warranty.

   see the copying file for more details.
*/

#include "gui/widgets/toggle_panel.hpp"

#include "foreach.hpp"
#include "log.hpp"

#define DBG_G LOG_STREAM_INDENT(debug, gui)
#define LOG_G LOG_STREAM_INDENT(info, gui)
#define WRN_G LOG_STREAM_INDENT(warn, gui)
#define ERR_G LOG_STREAM_INDENT(err, gui)

#define DBG_G_D LOG_STREAM_INDENT(debug, gui_draw)
#define LOG_G_D LOG_STREAM_INDENT(info, gui_draw)
#define WRN_G_D LOG_STREAM_INDENT(warn, gui_draw)
#define ERR_G_D LOG_STREAM_INDENT(err, gui_draw)

#define DBG_G_E LOG_STREAM_INDENT(debug, gui_event)
#define LOG_G_E LOG_STREAM_INDENT(info, gui_event)
#define WRN_G_E LOG_STREAM_INDENT(warn, gui_event)
#define ERR_G_E LOG_STREAM_INDENT(err, gui_event)

#define DBG_G_P LOG_STREAM_INDENT(debug, gui_parse)
#define LOG_G_P LOG_STREAM_INDENT(info, gui_parse)
#define WRN_G_P LOG_STREAM_INDENT(warn, gui_parse)
#define ERR_G_P LOG_STREAM_INDENT(err, gui_parse)

namespace gui2 {

void ttoggle_panel::set_data(const std::map<std::string /* widget id */, std::map<
		std::string /* member id */, t_string /* member value */> >& data)
{
	// typedef boost problem work around.
	typedef std::pair<std::string, std::map<std::string, t_string> > hack ;
	foreach(const hack& item, data) {
		tcontrol* widget = dynamic_cast<tcontrol*>(find_widget(item.first, false));
		if(widget) {
			widget->set_members(item.second);
		}
	}
}

void ttoggle_panel::mouse_enter(tevent_handler&) 
{ 
	DBG_G_E << "Toggle panel: mouse enter.\n"; 

	if(is_selected()) {
		set_state(FOCUSSED_SELECTED);
	} else {
		set_state(FOCUSSED);
	}
}

void ttoggle_panel::mouse_leave(tevent_handler&) 
{ 
	DBG_G_E << "Toggle panel: mouse leave.\n"; 

	if(is_selected()) {
		set_state(ENABLED_SELECTED);
	} else {
		set_state(ENABLED);
	}
}

void ttoggle_panel::mouse_left_button_click(tevent_handler&) 
{ 
	DBG_G_E << "Toggle panel: left mouse button click.\n"; 

	if(is_selected()) {
		set_state(ENABLED);
	} else {
		set_state(ENABLED_SELECTED);
	}

	if(callback_mouse_left_click_) {
		callback_mouse_left_click_(this);
	}
}

void ttoggle_panel::set_active(const bool active)
{
	if(active) {
		if(is_selected()) {
			set_state(ENABLED_SELECTED);
		} else {
			set_state(ENABLED);
		}
	} else {
		if(is_selected()) {
			set_state(DISABLED_SELECTED);
		} else {
			set_state(DISABLED);
		}
	}
}

SDL_Rect ttoggle_panel::get_client_rect() const
{
	const ttoggle_panel_definition::tresolution* conf = 
		dynamic_cast<const ttoggle_panel_definition::tresolution*>(config());
	assert(conf);

	SDL_Rect result = get_rect();
	result.x += conf->left_border;
	result.y += conf->top_border;
	result.w -= conf->left_border + conf->right_border;
	result.h -= conf->top_border + conf->bottom_border;

	return result;
}

tpoint ttoggle_panel::border_space() const
{
	const ttoggle_panel_definition::tresolution* conf = 
		dynamic_cast<const ttoggle_panel_definition::tresolution*>(config());
	assert(conf);

	return tpoint(conf->left_border + conf->right_border,
		conf->top_border + conf->bottom_border);
}

void ttoggle_panel::set_selected(const bool selected)
{
	if(selected == is_selected()) {
		return;
	}

	if(selected) {
		set_state(static_cast<tstate>(state_ + ENABLED_SELECTED));
	} else {
		set_state(static_cast<tstate>(state_ - ENABLED_SELECTED));
	}
}

void ttoggle_panel::set_state(tstate state)
{
	if(state != state_) {
		state_ = state;
		set_dirty(true);
	}
}

} // namespace gui2


