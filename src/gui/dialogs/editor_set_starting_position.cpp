/* $Id$ */
/*
   Copyright (C) 2011 - 2012 by Ignacio Riquelme Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-editor"

#include "gui/dialogs/editor_set_starting_position.hpp"

#include "foreach.hpp"
#include "formatter.hpp"
#include "formula_string_utils.hpp"
#include "gettext.hpp"
#ifdef GUI2_EXPERIMENTAL_LISTBOX
	#include "gui/widgets/list.hpp"
#else
	#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "map_location.hpp"

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_editor_set_starting_position
 *
 * == Editor set starting position ==
 *
 * Map editor dialog for setting player starting positions.
 *
 * @begin{table}{dialog_widgets}
 *
 * listbox & & listbox & m &
 *         Listbox displaying player choices. $
 *
 * -player & & control & m &
 *         Widget which shows a player item label. $
 *
 * -location & & control & m &
 *         Widget which shows the coordinates to the current
 *         starting position for a player if it exists. $
 *
 * ok & & button & m &
 *         OK button. $
 *
 * cancel & & button & m &
 *         Cancel button. $
 *
 * @end{table}
 */

REGISTER_DIALOG(editor_set_starting_position)

teditor_set_starting_position::teditor_set_starting_position(unsigned current_player, unsigned maximum_players, const std::vector<map_location>& starting_positions)
	: players_(maximum_players)
	, selection_(std::min(current_player, maximum_players))
	, starting_positions_(starting_positions)
{
	if(starting_positions_.size() != maximum_players) {
		starting_positions_.resize(maximum_players);
	}
}

void teditor_set_starting_position::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& list = find_widget<tlistbox>(&window, "listbox", false);
	window.keyboard_capture(&list);

	std::map<std::string, string_map> data;
	string_map column;

	column["label"] = _("player^None");
	data.insert(std::make_pair("player", column));
	list.add_row(data);

	for(unsigned i = 0; i < starting_positions_.size(); ++i) {
		const map_location& player_pos = starting_positions_[i];

		data.clear();

		utils::string_map symbols;
		symbols["player_number"] = str_cast(i + 1);

		column["label"] = utils::interpolate_variables_into_string(_("Player $player_number"), &symbols);
		data.insert(std::make_pair("player", column));

		if(player_pos.valid()) {
			column["label"] = (formatter() << "(" << player_pos.x + 1 << ", " << player_pos.y + 1 << ")").str();
			data.insert(std::make_pair("location", column));
		}

		list.add_row(data);
	}

	list.select_row(selection_);
}

void teditor_set_starting_position::post_show(twindow& window)
{
	if(get_retval() != twindow::OK) {
		return;
	}

	tlistbox& list = find_widget<tlistbox>(&window, "listbox", false);
	selection_ = list.get_selected_row();
}

}
