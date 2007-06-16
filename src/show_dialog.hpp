/* $Id$ */
/*
   Copyright (C) 2003 by David White <davidnwhite@verizon.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef SHOW_DIALOG_HPP_INCLUDED
#define SHOW_DIALOG_HPP_INCLUDED

class config;
class CVideo;
class display;

#include "cursor.hpp"
#include "font.hpp"
#include "network.hpp"
#include "tooltips.hpp"

#include "widgets/menu.hpp"

#include "SDL.h"

#include <string>
#include <vector>

namespace gui
{

extern const int ButtonHPadding;
extern const int ButtonVPadding;
enum DIALOG_RESULT {
	CREATE_ITEM =-5,
	DELETE_ITEM=-4,
	ESCAPE_DIALOG=-3, //special return used by WML event dialogs
	CONTINUE_DIALOG=-2,
	CLOSE_DIALOG=-1
	/* results (0..N) reserved for standard button indeces */
};

bool in_dialog();

struct dialog_manager : private cursor::setter, private font::floating_label_context {
	dialog_manager();
	~dialog_manager();

private:
	bool reset_to;
};

class dialog_frame {
public:
	//Static members
	static const std::string default_style;
	static const int title_border_w, title_border_h;

	int vertical_padding() const;

	struct dimension_measurements {
		dimension_measurements();
		SDL_Rect interior, exterior, title, button_row;
	};
	dialog_frame(CVideo &video, const std::string& title="",
                 const std::string* dialog_style=NULL, bool blur=false, std::vector<button*>* buttons=NULL,
                 surface_restorer* restorer=NULL, button* help_button=NULL);
	~dialog_frame();

	dimension_measurements layout(int x, int y, int w, int h);
	dimension_measurements layout(SDL_Rect const& frame_area);
	void set_layout(dimension_measurements &new_dim) { dim_ = new_dim; }
	dimension_measurements get_layout() const { return dim_; }

	void draw();

	//called by draw
	void draw_border();
	void draw_background();

	//also called by layout with null param
	SDL_Rect draw_title(CVideo *video);

private:
	std::string title_;
	CVideo &video_;
	const std::string *dialog_style_;
	bool blur_;
	std::vector<button*>* buttons_;
	button* help_button_;
	surface_restorer* restorer_;
	dimension_measurements dim_;
	surface top_, bot_, left_, right_, top_left_, bot_left_, top_right_, bot_right_, bg_;
	bool have_border_;
};

//frame_measurements draw_dialog_frame(int x, int y, int w, int h, CVideo &video, const std::string* dialog_style=NULL, surface_restorer* restorer=NULL);

//SDL_Rect draw_dialog_background(int x, int y, int w, int h, CVideo &video, const std::string& dialog_style);

//given the location of a dialog, will draw its title.
//Returns the area the title takes up
//SDL_Rect draw_dialog_title(int x, int y, CVideo* disp, const std::string& text, label** label_widget);

//function to draw a dialog on the screen. x,y,w,h give the dimensions of the client area
//of the dialog. 'title' is the title of the dialog. The title will be displayed at the
//top of the dialog above the client area. 'dialog_style' if present gives the style of
//the dialog to use.
//'buttons' contains pointers to standard dialog buttons such as 'ok' and 'cancel' that
//will appear on the dialog. If present, they will be located at the bottom of the dialog,
//below the client area.
//if 'restorer' is present, it will be set to a restorer that will reset the screen area
//to its original state after the dialog is drawn.
//void draw_dialog(int x, int y, int w, int h, CVideo &video, const std::string& title,
 //                const std::string* dialog_style=NULL, std::vector<button*>* buttons=NULL,
 //                surface_restorer* restorer=NULL, button* help_button=NULL, label** label_widget);
//void draw_dialog(frame_measurements &fm, CVideo &video, const std::string& title,
 //                const std::string* dialog_style=NULL, std::vector<button*>* buttons=NULL,
 //                surface_restorer* restorer=NULL, button* help_button=NULL, label** label_widget);

class dialog_button_action
{
public:
	virtual ~dialog_button_action() {}

	typedef DIALOG_RESULT RESULT;

	virtual RESULT button_pressed(int menu_selection) = 0;
};

struct dialog_button_info
{
	dialog_button_info(dialog_button_action* handler, const std::string& label) : handler(handler), label(label)
	{}

	dialog_button_action* handler;
	std::string label;
};

enum DIALOG_TYPE { MESSAGE, OK_ONLY, YES_NO, OK_CANCEL, CANCEL_ONLY, CLOSE_ONLY, NULL_DIALOG };

struct check_item {
	check_item(const std::string& label, bool checked) : label(label), checked(checked) {}
	std::string label;
	bool checked;
};

//an interface for a 'preview pane'. A preview pane is shown beside a dialog created
//by 'show_dialog' and shows information about the selection.
class preview_pane : public widget {
public:
	preview_pane(CVideo &video) : widget(video, false) {}
	virtual ~preview_pane() { tooltips::clear_tooltips(location()); }

	virtual bool show_above() const { return false; }
	virtual bool left_side() const = 0;
	virtual void set_selection(int index) = 0;
	virtual handler_vector handler_members() { return widget::handler_members(); }
};

//if a menu is given, then returns -1 if the dialog was cancelled, and the
//index of the selection otherwise. If no menu is given, returns the index
//of the button that was pressed
int show_dialog(display &screen, surface image,
				const std::string& caption, const std::string& message,
				DIALOG_TYPE type=MESSAGE,
				const std::vector<std::string>* menu_items=NULL,
				const std::vector<preview_pane*>* preview_panes=NULL,
				const std::string& text_widget_label="",
				std::string* text_widget_text=NULL,
				const int text_widget_max_chars = 256,
				std::vector<check_item>* options=NULL, int xloc=-1, int yloc=-1,
				const std::string* dialog_style=NULL,
				std::vector<dialog_button_info>* buttons=NULL,
				const std::string& help_topic="",
				const menu::sorter* sorter=NULL,
				menu::style* menu_style=NULL
			 );

void show_error_message(display &disp, std::string const &message);

void popup_transient(display& screen,
		     const std::string& caption, const std::string& message);

network::connection network_send_dialog(display& disp, const std::string& msg, config& cfg, network::connection connection_num=0);
network::connection network_receive_dialog(display& disp, const std::string& msg, config& cfg, network::connection connection_num=0);
network::connection network_connect_dialog(display& disp, const std::string& msg, const std::string& hostname, int port);

void check_quit(CVideo &video);

}

#endif
