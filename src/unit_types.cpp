/* $Id$ */
/*
   Copyright (C) 2003 - 2011 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 *  @file
 *  Handle unit-type specific attributes, animations, advancement.
 */

#include "global.hpp"

#include "unit_types.hpp"

#include "foreach.hpp"
#include "game_config.hpp"
#include "gettext.hpp"
#include "loadscreen.hpp"
#include "log.hpp"
#include "map.hpp"


static lg::log_domain log_config("config");
#define ERR_CF LOG_STREAM(err, log_config)
#define WRN_CF LOG_STREAM(warn, log_config)
#define LOG_CONFIG LOG_STREAM(info, log_config)
#define DBG_CF LOG_STREAM(debug, log_config)

static lg::log_domain log_unit("unit");
#define DBG_UT LOG_STREAM(debug, log_unit)

namespace{
	//Static tokens are replacements for string literals in code
	//They allow for fast comparison operations.
	static const config::t_token z_gender("gender", false);
	static const config::t_token z_type("type", false);
	static const config::t_token z_id("id", false);
	static const config::t_token z_name("name", false);
	static const config::t_token z_random_gender("random_gender", false);
	static const config::t_token z_x("x", false);
	static const config::t_token z_y("y", false);
	static const config::t_token z_variation("variation", false);
	static const config::t_token z_canrecruit("canrecruit", false);
	static const config::t_token z_role("role", false);
	static const config::t_token z_ai_special("ai_special", false);
	static const config::t_token z_side("side", false);
	static const config::t_token z_underlying_id("underlying_id", false);
	static const config::t_token z_overlays("overlays", false);
	static const config::t_token z_variables("variables", false);
	static const config::t_token z_facing("facing", false);
	static const config::t_token z_flying("flying", false);
	static const config::t_token z_modification("modifications", false);
	static const config::t_token z_race("race", false);
	static const config::t_token z_undead_variation("undead_variation", false);
	static const config::t_token z_max_attacks("max_attacks", false);
	static const config::t_token z_attacks_left("attacks_left", false);
	static const config::t_token z_alpha("alpha", false);
	static const config::t_token z_zoc("zoc", false);
	static const config::t_token z_description("description", false);
	static const config::t_token z_cost("cost", false);
	static const config::t_token z_profile("profile", false);
	static const config::t_token z_small_profile("small_profile", false);
	static const config::t_token z_max_hitpoints("max_hitpoints", false);
	static const config::t_token z_max_moves("max_moves", false);
	static const config::t_token z_max_experience("max_experience", false);
	static const config::t_token z_advances_to("advances_to", false);
	static const config::t_token z_null("null", false);
	static const config::t_token z_ai("ai", false);
	static const config::t_token z_formula("formula", false);
	static const config::t_token z_loop_formula("loop_formula", false);
	static const config::t_token z_priority("priority", false);
	static const config::t_token z_vars("vars", false);
	static const config::t_token z_guardian("guardian", false);
	static const config::t_token z_hitpoints("hitpoints", false);
	static const config::t_token z_goto_x("goto_x", false);
	static const config::t_token z_goto_y("goto_y", false);
	static const config::t_token z_experience("experience", false);
	static const config::t_token z_resting("resting", false);
	static const config::t_token z_unrenamable("unrenamable", false);
	static const config::t_token z_lawful("lawful", false);
	static const config::t_token z_neutral("neutral", false);
	static const config::t_token z_chaotic("chaotic", false);
	static const config::t_token z_liminal("liminal", false);
	static const config::t_token z_upkeep("upkeep", false);
	static const config::t_token z_full("full", false);
	static const config::t_token z_flag_rgb("flag_rgb", false);
	static const config::t_token z_language_name("language_name", false);
	//	static const config::t_token z_halo("halo", false);
	//	static const config::t_token z_ellipse("ellipse", false);
	//	static const config::t_token z_usage("usage", false);
	static const config::t_token z_generate_name("generate_name", false);
	static const config::t_token z_availability("availability", false);
	static const config::t_token z_musthave("musthave", false);
	//	static const config::t_token z_trait("trait", false);
	static const config::t_token z_random_traits("random_traits", false);
	static const config::t_token z_male("male", false);
	static const config::t_token z_female("female", false);
	static const config::t_token z_unit_image("unit_image", false);
	static const config::t_token z_duration("duration", false);
	static const config::t_token z_prev_type("prev_type", false);
	static const config::t_token z_forever("forever", false);
	static const config::t_token z_abilities("abilities", false);
	static const config::t_token z_speaker("speaker", false);
	static const config::t_token z_has_weapon("has_weapon", false);
	static const config::t_token z_defense("defense", false);
	static const config::t_token z_visible("visible", false);
	static const config::t_token z_viewing_side("viewing_side", false);
	static const config::t_token z_filter_vision("filter_vision", false);
	static const config::t_token z_adjacent("adjacent", false);
	static const config::t_token z_is_enemy("is_enemy", false);
	static const config::t_token z_count("count", false);
	static const config::t_token z_find_in("find_in", false);
	static const config::t_token z_lua_function("lua_function", false);
	static const config::t_token z_status("status", false);
	static const config::t_token z_loyal("loyal", false);
	static const config::t_token z_free("free", false);
	static const config::t_token z_active_on("active_on", false);
	static const config::t_token z_apply_to("apply_to", false);
	static const config::t_token z_icon("icon", false);
	static const config::t_token z_strict_amla("strict_amla", false);
	static const config::t_token z_require_amla("require_amla", false);
	static const config::t_token z_times("times", false);
	//	static const config::t_token z_empty("", false);
	static const config::t_token z_modifications("modifications", false);
	static const config::t_token z_level("level", false);
	static const config::t_token z_attack("attack", false);
	static const config::t_token z_resistance("resistance", false);
	static const config::t_token z_healable("healable", false);
	static const config::t_token z_unhealable("unhealable", false);
	static const config::t_token z_waypoints("waypoints", false);
	static const config::t_token z_moves("moves", false);
	static const config::t_token z_alignment("alignment", false);
	static const config::t_token z_placement("placement", false);
	static const config::t_token z_do_not_list("do_not_list", false);
	static const config::t_token z_event("event", false);
	static const config::t_token z_any("any", false);
	static const config::t_token z_die("die", false);
	static const config::t_token z_flies("flies", false);
	static const config::t_token z_inherit("inherit", false);
	static const config::t_token z_variation_name("variation_name", false);
	static const config::t_token z_ignore_race_traits("ignore_race_traits", false);
	static const config::t_token z_hide_help("hide_help", false);
	static const config::t_token z_yes("yes", false);
	static const config::t_token z_no("no", false);
	static const config::t_token z_slowed("slowed", false);
	static const config::t_token z_poisoned("poisoned", false);
	static const config::t_token z_petrified("petrified", false);
	static const config::t_token z_uncovered("uncovered", false);
	static const config::t_token z_not_moved("not_moved", false);
	static const config::t_token z_this_unit("this_unit", false);
	static const config::t_token z_and("and", false);
	static const config::t_token z_or("or", false);
	static const config::t_token z_not("not", false);
	static const config::t_token z_filter("filter", false);
	static const config::t_token z_recall("recall", false);
	static const config::t_token z_ability("ability", false);
	static const config::t_token z_movement_cost("movement_cost", false);
	static const config::t_token z_movement_costs("movement_costs", false);
	static const config::t_token z_filter_wml("filter_wml", false);
	static const config::t_token z_filter_adjacent("filter_adjacent", false);
	static const config::t_token z_standing("standing", false);
	static const config::t_token z_ghosted("ghosted", false);
	static const config::t_token z_disabled_ghosted("disabled_ghosted", false);
	static const config::t_token z_image("image", false);
	static const config::t_token z_movement("movement", false);
	static const config::t_token z_die_sound("die_sound", false);
	static const config::t_token z_movement_type("movement_type", false);
	static const config::t_token z_filter_location("filter_location", false);
	static const config::t_token z__disabled_("_disabled_", false);
	static const config::t_token z_idling("idling", false);
	static const config::t_token z_selected("selected", false);
	static const config::t_token z__disabled_selected_("_disabled_selected_", false);
	static const config::t_token z_offense("offense", false);
	static const config::t_token z_max_value("max_value", false);
	static const config::t_token z_advance("advance", false);
	static const config::t_token z_max_times("max_times", false);
	static const config::t_token z_fearless("fearless", false);
	static const config::t_token z_healthy("healthy", false);
	static const config::t_token z_effect("effect", false);
	static const config::t_token z_unit_type("unit_type", false);
	static const config::t_token z_unit_gender("unit_gender", false);
	static const config::t_token z_portrait("portriat", false);
	static const config::t_token z_small_portrait("small_portriat", false);
	static const config::t_token z_new_attack("new_attack", false);
	static const config::t_token z_remove_attacks("remove_attacks", false);
	static const config::t_token z_wesnoth("wesnoth", false);
	static const config::t_token z_attack_list("attack_list", false);
	static const config::t_token z_effect_description("effect_description", false);
	static const config::t_token z_increase("increase", false);
	static const config::t_token z_increase_total("increase_total", false);
	static const config::t_token z_set("set", false);
	static const config::t_token z_set_total("set_total", false);
	static const config::t_token z_violate_maximum("violate_maximum", false);
	static const config::t_token z_HP("HP", false);
	static const config::t_token z_remove("remove", false);
	static const config::t_token z_replace("replace", false);
	static const config::t_token z_heal_full("heal_full", false);
	static const config::t_token z_add("add", false);
	static const config::t_token z_value("value", false);
	static const config::t_token z_new_ability("new_ability", false);
	static const config::t_token z_remove_ability("remove_ability", false);
	static const config::t_token z_image_mod("image_mod", false);
	static const config::t_token z_new_animation("new_animation", false);
	static const config::t_token z_female_name("female_name", false);
	static const config::t_token z_male_name("male_name", false);
	static const config::t_token z_ignore_global_traits("ignore_global_traits", false);
	static const config::t_token z_range("range", false);
	static const config::t_token z_damage("damage", false);
	static const config::t_token z_number("number", false);
	static const config::t_token z_specials("specials", false);
	static const config::t_token z_description_inactive("description_inactive", false);
	static const config::t_token z_name_inactive("name_inactive", false);
	static const config::t_token z_advance_from("advance_from", false);
	static const config::t_token z_hides("hides", false);
	static const config::t_token z_Unit("Unit", false);
	static const config::t_token z_attack_weight("attack_weight", false);
	static const config::t_token z_defense_weight("defense_weight", false);
	static const config::t_token z_accuracy("accuracy", false);
	static const config::t_token z_parry("parry", false);
	static const config::t_token z_special("special", false);
	static const config::t_token z_set_name("set_name", false);
	static const config::t_token z_set_description("set_description", false);
	static const config::t_token z_set_type("set_type", false);
	static const config::t_token z_remove_specials("remove_specials", false);
	static const config::t_token z_set_specials("set_specials", false);
	static const config::t_token z_increase_damage("increase_damage", false);
	static const config::t_token z_increase_attacks("increase_attacks", false);
	static const config::t_token z_attacks("attacks", false);
	static const config::t_token z_usage("usage", false);
	static const config::t_token z_num_traits("num_traits", false);
	static const config::t_token z_movetype("movetype", false);
	static const config::t_token z_base_unit("base_unit", false);
	static const config::t_token z_all("all", false);
	static const config::t_token z_random("random", false);
	static const config::t_token z_type_adv_tree("type_adv_tree", false);
	static const config::t_token z_unit("unit", false);
	static const config::t_token z_not_living("not_living", false);
	static const config::t_token z_increase_parry("increase_parry", false);
	static const config::t_token z_increase_accuracy("increase_accuracy", false);
	static const config::t_token z_mode("mode", false);
	static const config::t_token z_append("append", false);
	static const config::t_token z_advancefrom("advancefrom", false);

}
attack_type::attack_type(const config& cfg) :
	aloc_(),
	dloc_(),
	attacker_(false),
	unitmap_(NULL),
	other_attack_(NULL),
	cfg_(cfg),
	description_(cfg[z_description].t_str()),
	id_(cfg[z_name].token()),
	type_(cfg[z_type].token()),
	icon_(cfg[z_icon].token()),
	range_(cfg[z_range].token()),
	damage_(cfg[z_damage]),
	num_attacks_(cfg[z_number]),
	attack_weight_(cfg[z_attack_weight].to_double(1.0)),
	defense_weight_(cfg[z_defense_weight].to_double(1.0)),
	accuracy_(cfg[z_accuracy]),
	parry_(cfg[z_parry])

{
	if (description_.empty())
		description_ = egettext(id().c_str());

	if(icon_.empty()){
		if (id() != n_token::t_token::z_empty())
			icon_ = config::t_token("attacks/" + static_cast<std::string const &>(id()) + ".png");
		else {
			static const config::t_token default_icon("attacks/blank-attack.png", false);			
			icon_ = default_icon;
		}
	}
}

attack_type::~attack_type()
{
}

config::t_token attack_type::accuracy_parry_description() const
{
	if(accuracy_ == 0 && parry_ == 0) {
		return n_token::t_token::z_empty(); }

	std::ostringstream s;
	s << utils::signed_percent(accuracy_);

	if(parry_ != 0) {
		s << "/" << utils::signed_percent(parry_);
	}

	return config::t_token( s.str() );
}

bool attack_type::matches_filter(const config& cfg,bool self) const
{
	const std::vector<config::t_token>& filter_range = utils::split_token(cfg[z_range]);
	const config::t_token& filter_damage = cfg[z_damage];
	const std::vector<config::t_token> filter_name = utils::split_token(cfg[z_name]);
	const std::vector<config::t_token> filter_type = utils::split_token(cfg[z_type]);
	const config::t_token filter_special = cfg[z_special];

	if(filter_range.empty() == false && std::find(filter_range.begin(),filter_range.end(),range()) == filter_range.end())
			return false;

	if(filter_damage.empty() == false && !in_ranges(damage(), utils::parse_ranges(filter_damage))) {
		return false;
	}

	if(filter_name.empty() == false && std::find(filter_name.begin(),filter_name.end(),id()) == filter_name.end())
		return false;

	if(filter_type.empty() == false && std::find(filter_type.begin(),filter_type.end(),type()) == filter_type.end())
		return false;

	if(!self && filter_special.empty() == false && !get_special_bool(filter_special,true))
		return false;

	return true;
}

std::pair<bool, config::t_token> attack_type::apply_modification(const config& cfg)
{
	if(!matches_filter(cfg,0)) {
		return std::make_pair(false, n_token::t_token::z_empty()); }

	const config::t_token& set_name = cfg[z_set_name];
	const t_string& set_desc = cfg[z_set_description];
	const config::t_token& set_type = cfg[z_set_type];
	const config::t_token& del_specials = cfg[z_remove_specials];
	const config &set_specials = cfg.child(z_set_specials);
	const config::t_token& increase_damage = cfg[z_increase_damage];
	const config::t_token& increase_attacks = cfg[z_increase_attacks];
	const config::t_token& set_attack_weight = cfg[z_attack_weight];
	const config::t_token& set_defense_weight = cfg[z_defense_weight];
	const config::t_token& increase_accuracy = cfg[z_increase_accuracy];
	const config::t_token& increase_parry = cfg[z_increase_parry];

	std::stringstream desc;

	if(set_name.empty() == false) {
		id_ = set_name;
		cfg_[z_name] = id();
	}

	if(set_desc.empty() == false) {
		description_ = set_desc;
		cfg_[z_description] = description_;
	}

	if(set_type.empty() == false) {
		type_ = set_type;
		cfg_[z_type] = type_;
	}

	if(del_specials.empty() == false) {
		const std::vector<config::t_token>& dsl = utils::split_token(del_specials);
		if (config &specials = cfg_.child(z_specials))
		{
			config new_specials;
			foreach (const config::any_child &vp, specials.all_children_range()) {
				std::vector<config::t_token>::const_iterator found_id =
					std::find(dsl.begin(), dsl.end(), vp.cfg[z_id]);
				if (found_id == dsl.end()) {
					new_specials.add_child(vp.key, vp.cfg);
				}
			}
			cfg_.clear_children(z_specials);
			cfg_.add_child(z_specials,new_specials);
		}
	}

	if (set_specials) {
		const config::t_token &mode = set_specials[z_mode];
		if (mode != z_append) {
			cfg_.clear_children(z_specials);
		}
		config &new_specials = cfg_.child_or_add(z_specials);
		foreach (const config::any_child &value, set_specials.all_children_range()) {
			new_specials.add_child(value.key, value.cfg);
		}
	}

	if(increase_damage.empty() == false) {
		damage_ = utils::apply_modifier(damage_, increase_damage, 0);
		if (damage_ < 0) {
			damage_ = 0;
		}
		cfg_[z_damage] = damage_;

		int inc_damage = lexical_cast<int>(increase_damage);
		desc << utils::signed_value(inc_damage) << " "
			 << _n("damage", "damage", inc_damage);
	}

	if(increase_attacks.empty() == false) {
		num_attacks_ = utils::apply_modifier(num_attacks_, increase_attacks, 1);
		cfg_[z_number] = num_attacks_;

		int inc_attacks = lexical_cast<int>(increase_attacks);
		desc << utils::signed_value(inc_attacks) << " "
			 << _n("strike", "strikes", inc_attacks);
	}

	if(increase_accuracy.empty() == false) {
		accuracy_ = utils::apply_modifier(accuracy_, increase_accuracy, 1);
		cfg_[z_accuracy] = accuracy_;

		int inc_acc = lexical_cast<int>(increase_accuracy);
		// Help xgettext with a directive to recognise the string as a non C printf-like string
		// xgettext:no-c-format
		desc << utils::signed_value(inc_acc) << _("% accuracy");
	}

	if(increase_parry.empty() == false) {
		parry_ = utils::apply_modifier(parry_, increase_parry, 1);
		cfg_[z_parry] = parry_;

		int inc_parry = lexical_cast<int>(increase_parry);
		// xgettext:no-c-format
		desc << utils::signed_value(inc_parry) << _("% parry");
	}

	if(set_attack_weight.empty() == false) {
		attack_weight_ = lexical_cast_default<double>(set_attack_weight,1.0);
		cfg_[z_attack_weight] = attack_weight_;
	}

	if(set_defense_weight.empty() == false) {
		defense_weight_ = lexical_cast_default<double>(set_defense_weight,1.0);
		cfg_[z_defense_weight] = defense_weight_;
	}

	return std::make_pair(true, config::t_token(desc.str() ));
}

// Same as above, except only update the descriptions
std::pair<bool, config::t_token>  attack_type::describe_modification(const config& cfg)
{
	if(!matches_filter(cfg,0)) {
		return std::make_pair(false, n_token::t_token::z_empty()); }

	const config::t_token& increase_damage = cfg[z_increase_damage];
	const config::t_token& increase_attacks = cfg[z_increase_attacks];

	std::stringstream desc;

	if(increase_damage.empty() == false) {
		int inc_damage = lexical_cast<int>(increase_damage);
		desc << utils::signed_value(inc_damage) << " "
			 << _n("damage","damage", inc_damage);
	}

	if(increase_attacks.empty() == false) {
		int inc_attacks = lexical_cast<int>(increase_attacks);
		desc << utils::signed_value(inc_attacks) << " "
			 << _n("strike", "strikes", inc_attacks);
	}

	return std::make_pair(true, config::t_token(desc.str() ));
}

unit_movement_type::unit_movement_type(const config& cfg, const unit_movement_type* parent) :
	moveCosts_(),
	defenseMods_(),
	parent_(parent),
	cfg_()
{
	//the unit_type give its whole cfg, we don't need all that.
	//so we filter to keep only keys related to movement_type
	//FIXME: This helps but it's still not clean, both cfg use a "name" key

	const t_string& name = cfg[z_name];
	if (!name.empty())
		cfg_[z_name]= cfg[z_name];

	const t_string& flies = cfg[z_flies];
	if (!flies.empty())
		cfg_[z_flies]= cfg[z_flies];

	if (const config &movement_costs = cfg.child(z_movement_costs))
		cfg_.add_child(z_movement_costs, movement_costs);

	if (const config &defense = cfg.child(z_defense))
		cfg_.add_child(z_defense, defense);

	if (const config &resistance = cfg.child(z_resistance))
		cfg_.add_child(z_resistance, resistance);
}

unit_movement_type::unit_movement_type(): moveCosts_(), defenseMods_(), parent_(NULL), cfg_()
{}

config::t_token unit_movement_type::name() const
{
	if (!cfg_.has_attribute(z_name) && parent_)
		return parent_->name();
	else
		return cfg_[z_name];
}

int unit_movement_type::resistance_against(const attack_type& attack) const
{
	bool result_found = false;
	int res = 100;

	if (const config &resistance = cfg_.child(z_resistance))
	{
		if (const::config::attribute_value *val = resistance.get(attack.type())) {
			res = *val;
			result_found = true;
		}
	}

	if(!result_found && parent_ != NULL) {
		res = parent_->resistance_against(attack);
	}

	return res;
}

utils::string_map unit_movement_type::damage_table() const
{
	utils::string_map res;
	if(parent_ != NULL)
		res = parent_->damage_table();

	if (const config &resistance = cfg_.child(z_resistance))
	{
		foreach (const config::attribute &i, resistance.attribute_range()) {
			res[i.first] = i.second;
		}
	}

	return res;
}

bool unit_movement_type::is_flying() const
{
	if (!cfg_.has_attribute(z_flies) && parent_)
		return parent_->is_flying();

	return cfg_[z_flies].to_bool();
}

int movement_cost_internal(t_move_cost_cache& move_costs,
		const config& cfg, const unit_movement_type* parent,
		const gamemap& map, t_translation::t_terrain terrain, int recurse_count)
{
	const int impassable = unit_movement_type::UNREACHABLE;

	const t_move_cost_cache::const_iterator i = move_costs.find(terrain);

	if (i != move_costs.end()) return i->second;

	// If this is an alias, then select the best of all underlying terrains.
	const t_translation::t_list& underlying = map.underlying_mvt_terrain(terrain);
	assert(!underlying.empty());

	if (underlying.size() != 1 || underlying.front() != terrain) {
		bool revert = (underlying.front() == t_translation::MINUS ? true : false);
		if (recurse_count >= 100) {
			ERR_CF << "infinite movement_cost recursion: "
				<< t_translation::write_terrain_code(terrain)
				<< " depth " << recurse_count << "\n";
			move_costs.insert(std::pair<t_translation::t_terrain, int>(terrain, impassable));
			return impassable;
		}

		int ret_value = revert ? 0 : impassable;
		for (t_translation::t_list::const_iterator i = underlying.begin();
				i != underlying.end(); ++i)
		{
			if (*i == t_translation::PLUS) {
				revert = false;
				continue;
			} else if (*i == t_translation::MINUS) {
				revert = true;
				continue;
			}
			const int value = movement_cost_internal(move_costs, cfg,
					parent, map, *i, recurse_count + 1);

			if (value < ret_value && !revert) {
				ret_value = value;
			} else if (value > ret_value && revert) {
				ret_value = value;
			}
		}

		move_costs.insert(std::pair<t_translation::t_terrain, int>(terrain, ret_value));
		return ret_value;
	}

	bool result_found = false;
	int res = impassable;

	if (const config& movement_costs = cfg.child(z_movement_costs))	{
		if (underlying.size() != 1) {
			ERR_CF << "Terrain '" << terrain << "' has "
				<< underlying.size() << " underlying names - 0 expected.\n";

			move_costs.insert(std::pair<t_translation::t_terrain, int>(terrain, impassable));
			return impassable;
		}

		const config::t_token& id = map.get_terrain_info(underlying.front()).idt();
		if (const config::attribute_value *val = movement_costs.get(id)) {
			res = *val;
			result_found = true;
		}
	}

	if (!result_found && parent != NULL) {
		res = parent->movement_cost(map, terrain);
	}

	if (res <= 0) {
		WRN_CF << "Terrain '" << terrain << "' has a movement cost of '"
			<< res << "' which is '<= 0'; resetting to 1.\n";
		res = 1;
	}

	move_costs.insert(std::pair<t_translation::t_terrain, int>(terrain, res));
	return res;
}

const defense_range &defense_range_modifier_internal(defense_cache &defense_mods,
		const config& cfg, const unit_movement_type* parent,
		const gamemap& map, t_translation::t_terrain terrain, int recurse_count)
{
	defense_range dummy = { 0, 100 };
	std::pair<defense_cache::iterator, bool> ib =
		defense_mods.insert(defense_cache::value_type(terrain, dummy));
	if (!ib.second) return ib.first->second;

	defense_range &res = ib.first->second;

	// If this is an alias, then select the best of all underlying terrains.
	const t_translation::t_list& underlying = map.underlying_def_terrain(terrain);
	assert(!underlying.empty());

	if (underlying.size() != 1 || underlying.front() != terrain) {
		bool revert = underlying.front() == t_translation::MINUS;
		if(recurse_count >= 90) {
			ERR_CF << "infinite defense_modifier recursion: "
				<< t_translation::write_terrain_code(terrain)
				<< " depth " << recurse_count << "\n";
		}
		if (recurse_count >= 100) {
			return res;
		}

		if (revert) {
			res.max_ = 0;
			res.min_ = 100;
		}

		for (t_translation::t_list::const_iterator i = underlying.begin();
				i != underlying.end(); ++i) {

			if (*i == t_translation::PLUS) {
				revert = false;
				continue;
			} else if (*i == t_translation::MINUS) {
				revert = true;
				continue;
			}
			const defense_range &inh = defense_range_modifier_internal
				(defense_mods, cfg, parent, map, *i, recurse_count + 1);

			if (!revert) {
				if (inh.max_ < res.max_) res.max_ = inh.max_;
				if (inh.min_ > res.min_) res.min_ = inh.min_;
			} else {
				if (inh.max_ > res.max_) res.max_ = inh.max_;
				if (inh.min_ < res.min_) res.min_ = inh.min_;
			}
		}

		goto check;
	}

	if (const config& defense = cfg.child(z_defense))
	{
		const config::t_token& id = map.get_terrain_info(underlying.front()).idt();
		if (const config::attribute_value *val = defense.get(id)) {
			int def = *val;
			if (def >= 0) res.max_ = def;
			else res.max_ = res.min_ = -def;
			goto check;
		}
	}

	if (parent) {
		/* Assign to the reference res to put the value in the defense_cache. */
		res = parent->defense_range_modifier(map, terrain);
		return res;
	}

	check:

	if (res.min_ < 0) {
		WRN_CF << "Defense '" << res.min_ << "' is '< 0' reset to 0 (100% defense).\n";
		res.min_ = 0;
	}
	if (res.max_ > 100) {
		WRN_CF << "Defense '" << res.max_ << "' is '> 100' reset to 100 (0% defense).\n";
		res.max_ = 100;
	}

	return res;
}

int defense_modifier_internal(defense_cache &defense_mods,
	const config &cfg, const unit_movement_type *parent,
	const gamemap &map, t_translation::t_terrain terrain, int recurse_count)
{
	const defense_range &def = defense_range_modifier_internal(defense_mods,
		cfg, parent, map, terrain, recurse_count);
	return (std::max)(def.max_, def.min_);
}

static const unit_race& dummy_race(){
	static unit_race ur;
	return ur;
}


#ifdef _MSC_VER
#pragma warning(push)
//silence "elements of array will be default initialized" warnings
#pragma warning(disable:4351)
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

unit_type::unit_type(const unit_type& o) :
	cfg_(o.cfg_),
	id_(o.id_),
	type_name_(o.type_name_),
	description_(o.description_),
	hitpoints_(o.hitpoints_),
	level_(o.level_),
	movement_(o.movement_),
	max_attacks_(o.max_attacks_),
	cost_(o.cost_),
	usage_(o.usage_),
	undead_variation_(o.undead_variation_),
	image_(o.image_),
	small_profile_(o.small_profile_),
	big_profile_(o.big_profile_),
	flag_rgb_(o.flag_rgb_),
	num_traits_(o.num_traits_),
	variations_(o.variations_),
	race_(o.race_),
	alpha_(o.alpha_),
	abilities_(o.abilities_),
	adv_abilities_(o.adv_abilities_),
	ability_tooltips_(o.ability_tooltips_),
	adv_ability_tooltips_(o.adv_ability_tooltips_),
	zoc_(o.zoc_),
	hide_help_(o.hide_help_),
	advances_to_(o.advances_to_),
	experience_needed_(o.experience_needed_),
	in_advancefrom_(o.in_advancefrom_),
	alignment_(o.alignment_),
	movementType_(o.movementType_),
	possibleTraits_(o.possibleTraits_),
	genders_(o.genders_),
	animations_(o.animations_),
    build_status_(o.build_status_),
	portraits_(o.portraits_)
{
	gender_types_[0] = o.gender_types_[0] != NULL ? new unit_type(*o.gender_types_[0]) : NULL;
	gender_types_[1] = o.gender_types_[1] != NULL ? new unit_type(*o.gender_types_[1]) : NULL;

	for(variations_map::const_iterator i = o.variations_.begin(); i != o.variations_.end(); ++i) {
		variations_[i->first] = new unit_type(*i->second);
	}
}


unit_type::unit_type(config &cfg) :
	cfg_(cfg),
	id_(cfg[z_id].token()),
	type_name_(),
	description_(),
	hitpoints_(0),
	level_(0),
	movement_(0),
	max_attacks_(0),
	cost_(0),
	usage_(),
	undead_variation_(),
	image_(),
	small_profile_(),
	big_profile_(),
	flag_rgb_(),
	num_traits_(0),
	gender_types_(),
	variations_(),
	race_(&dummy_race()),
	alpha_(),
	abilities_(),
	adv_abilities_(),
	ability_tooltips_(),
	adv_ability_tooltips_(),
	zoc_(false),
	hide_help_(false),
	advances_to_(),
	experience_needed_(0),
	in_advancefrom_(false),
	alignment_(),
	movementType_(),
	possibleTraits_(),
	genders_(),
	animations_(),
	build_status_(NOT_BUILT),
	portraits_()
{
	gender_types_[0] = NULL;
	gender_types_[1] = NULL;
}

unit_type::~unit_type()
{
	delete gender_types_[0];
	delete gender_types_[1];

	for(variations_map::iterator i = variations_.begin(); i != variations_.end(); ++i) {
		delete i->second;
	}
}

void unit_type::build_full(const movement_type_map &mv_types,
	const race_map &races, const config::const_child_itors &traits)
{
	if (build_status_ == NOT_BUILT || build_status_ == CREATED)
		build_help_index(mv_types, races, traits);

	config &cfg = cfg_;

	movementType_ = unit_movement_type(cfg);
	alpha_ = ftofxp(1.0);

	foreach (const config &t, traits)
	{
		possibleTraits_.add_child(z_trait, t);
	}
	foreach (config &var_cfg, cfg.child_range(z_variation))
	{
		if (var_cfg[z_inherit].to_bool()) {
			config nvar_cfg(cfg);
			nvar_cfg.merge_with(var_cfg);
			nvar_cfg.clear_children(z_variation);
			var_cfg.swap(nvar_cfg);
		}
		unit_type *ut = new unit_type(var_cfg);
		ut->build_full(mv_types, races, traits);
		variations_.insert(std::make_pair(var_cfg[z_variation_name].token(), ut));
	}

	for (int i = 0; i < 2; ++i) {
		if (gender_types_[i]){
			gender_types_[i]->build_full(mv_types, races, traits);
		}
	}

	const config::attribute_value& align = cfg[z_alignment];
	if(align == z_lawful)
		alignment_ = LAWFUL;
	else if(align == z_chaotic)
		alignment_ = CHAOTIC;
	else if(align == z_neutral)
		alignment_ = NEUTRAL;
	else if(align == "liminal")
		alignment_ = LIMINAL;
	else {
		ERR_CF << "Invalid alignment found for " << id() << ": '" << align << "'\n";
		alignment_ = NEUTRAL;
	}

	if (race_ != &dummy_race())
	{
		if (!race_->uses_global_traits()) {
			possibleTraits_.clear();
		}
		if (cfg[z_ignore_race_traits].to_bool()) {
			possibleTraits_.clear();
		} else {
			foreach (const config &t, race_->additional_traits())
			{
				if (alignment_ != NEUTRAL || t[z_id] != z_fearless)
					possibleTraits_.add_child(z_trait, t);
			}
		}
	}

	// Insert any traits that are just for this unit type
	foreach (const config &trait, cfg.child_range(z_trait))
	{
		possibleTraits_.add_child(z_trait, trait);
	}

	zoc_ = cfg[z_zoc].to_bool(level_ > 0);

	const config::t_token& alpha_blend = cfg[z_alpha];
	if(alpha_blend.empty() == false) {
		alpha_ = ftofxp(atof(alpha_blend.c_str()));
	}

	const config::t_token& move_type = cfg[z_movement_type];

	const movement_type_map::const_iterator it = mv_types.find(move_type);

	if(it != mv_types.end()) {
	    DBG_UT << "setting parent for movement_type " << move_type << "\n";
		movementType_.set_parent(&(it->second));
	}
	else{
	    DBG_UT << "no parent found for movement_type " << move_type << "\n";
	}

	flag_rgb_ = cfg[z_flag_rgb].token();
	game_config::add_color_info(cfg);


	foreach (const config &portrait, cfg_.child_range(z_portrait)) {
		portraits_.push_back(tportrait(portrait));
	}

	// Deprecation messages, only seen when unit is parsed for the first time.

	build_status_ = FULL;
}

void unit_type::build_help_index(const movement_type_map &mv_types,
	const race_map &races, const config::const_child_itors &traits)
{
	if (build_status_ == NOT_BUILT)
		build_created(mv_types, races, traits);

	const config &cfg = cfg_;

	type_name_ = cfg_[z_name];
	description_ = cfg_[z_description];
	hitpoints_ = cfg[z_hitpoints].to_int(1);
	level_ = cfg[z_level];
	movement_ = cfg[z_movement].to_int(1);
	max_attacks_ = cfg[z_attacks].to_int(1);
	cost_ = cfg[z_cost].to_int(1);
	usage_ = cfg_[z_usage].token();
	undead_variation_ = cfg_[z_undead_variation].token();
	image_ = cfg_[z_image].token();
	small_profile_ = cfg_[z_small_profile].token();
	big_profile_ = cfg_[z_profile].token();
	adjust_profile(small_profile_, big_profile_, image_);

	for (int i = 0; i < 2; ++i) {
		if (gender_types_[i])
			gender_types_[i]->build_help_index(mv_types, races, traits);
	}

	const race_map::const_iterator race_it = races.find(cfg[z_race]);
	if(race_it != races.end()) {
		race_ = &race_it->second;
	} else {
		race_ = &dummy_race();
	}

	// if num_traits is not defined, we use the num_traits from race
	num_traits_ = cfg[z_num_traits].to_int(race_->num_traits());

	const std::vector<config::t_token> genders = utils::split_token(cfg[z_gender]);
	for(std::vector<config::t_token>::const_iterator g = genders.begin(); g != genders.end(); ++g) {
		genders_.push_back(string_gender(*g));
	}
	if(genders_.empty()) {
		genders_.push_back(unit_race::MALE);
	}

	if (const config &abil_cfg = cfg.child(z_abilities))
	{
		foreach (const config::any_child &ab, abil_cfg.all_children_range()) {
			const t_string &name = ab.cfg[z_name];
			if (!name.empty()) {
				abilities_.push_back(name);
				ability_tooltips_.push_back(ab.cfg[z_description]);
			}
		}
	}

	foreach (const config &adv, cfg.child_range(z_advancement))
	{
		foreach (const config &effect, adv.child_range(z_effect))
		{
			const config &abil_cfg = effect.child(z_abilities);
			if (!abil_cfg || effect[z_apply_to] != z_new_ability) {
				continue;
			}
			foreach (const config::any_child &ab, abil_cfg.all_children_range()) {
				const t_string &name = ab.cfg[z_name];
				if (!name.empty()) {
					adv_abilities_.push_back(name);
					adv_ability_tooltips_.push_back(ab.cfg[z_description]);
				}
			}
		}
	}

	hide_help_= cfg[z_hide_help].to_bool();

	build_status_ = HELP_INDEX;
}

void unit_type::build_created(const movement_type_map &mv_types,
	const race_map &races, const config::const_child_itors &traits)
{
	gender_types_[0] = NULL;
	gender_types_[1] = NULL;

	config &cfg = cfg_;

	if (config &male_cfg = cfg.child(z_male))
	{
		if (male_cfg[z_inherit].to_bool(true)) {
			config m_cfg(cfg);
			m_cfg.merge_with(male_cfg);
			male_cfg.swap(m_cfg);
		}
		male_cfg.clear_children(z_male);
		male_cfg.clear_children(z_female);
		gender_types_[0] = new unit_type(male_cfg);
	}

	if (config &female_cfg = cfg.child(z_female))
	{
		if (female_cfg[z_inherit].to_bool(true)) {
			config f_cfg(cfg);
			f_cfg.merge_with(female_cfg);
			female_cfg.swap(f_cfg);
		}
		female_cfg.clear_children(z_male);
		female_cfg.clear_children(z_female);
		gender_types_[1] = new unit_type(female_cfg);
	}

	for (int i = 0; i < 2; ++i) {
		if (gender_types_[i]){
			gender_types_[i]->build_created(mv_types, races, traits);}
	}

    const config::t_token& advances_to_val = cfg[z_advances_to];
    if(advances_to_val != z_null && advances_to_val != n_token::t_token::z_empty())
        advances_to_ = utils::split_token(advances_to_val);
    DBG_UT << "unit_type '" << id() << "' advances to : " << advances_to_val << "\n";

	experience_needed_ = cfg[z_experience].to_int(500);

	build_status_ = CREATED;
}

const unit_type& unit_type::get_gender_unit_type(config::t_token const & gender) const
{
	if (gender == z_female) return get_gender_unit_type(unit_race::FEMALE);
	else if (gender == z_male) return get_gender_unit_type(unit_race::MALE);
	else return *this;
}
// const unit_type& unit_type::get_gender_unit_type(std::string gender) const
// {
// 	if (gender == "female") return get_gender_unit_type(unit_race::FEMALE);
// 	else if (gender == "male") return get_gender_unit_type(unit_race::MALE);
// 	else return *this;
// }

const unit_type& unit_type::get_gender_unit_type(unit_race::GENDER gender) const
{
	const size_t i = gender;
	if(i < sizeof(gender_types_)/sizeof(*gender_types_)
	&& gender_types_[i] != NULL) {
		return *gender_types_[i];
	}

	return *this;
}

const unit_type& unit_type::get_variation(const config::t_token& name) const
{
	const variations_map::const_iterator i = variations_.find(name);
	if(i != variations_.end()) {
		return *i->second;
	} else {
		return *this;
	}
}

const t_string unit_type::unit_description() const
{
	if(description_.empty()) {
		return (_("No description available."));
	} else {
		return description_;
	}
}

const std::vector<unit_animation>& unit_type::animations() const {
	if (animations_.empty()) {
		unit_animation::fill_initial_animations(animations_,cfg_);
	}

	return animations_;
}

std::vector<attack_type> unit_type::attacks() const
{
	std::vector<attack_type> res;
	foreach (const config &att, cfg_.child_range(z_attack)) {
		res.push_back(attack_type(att));
	}

	return res;
}


namespace {
	int experience_modifier = 100;
}

unit_type::experience_accelerator::experience_accelerator(int modifier) : old_value_(experience_modifier)
{
	experience_modifier = modifier;
}

unit_type::experience_accelerator::~experience_accelerator()
{
	experience_modifier = old_value_;
}

int unit_type::experience_accelerator::get_acceleration()
{
	return experience_modifier;
}

int unit_type::experience_needed(bool with_acceleration) const
{
	if(with_acceleration) {
		int exp = (experience_needed_ * experience_modifier + 50) /100;
		if(exp < 1) exp = 1;
		return exp;
	}
	return experience_needed_;
}

const char* unit_type::alignment_description(unit_type::ALIGNMENT align, unit_race::GENDER gender)
{
	static const char* aligns[] = { N_("lawful"), N_("neutral"), N_("chaotic"), N_("liminal") };
	static const char* aligns_female[] = { N_("female^lawful"), N_("female^neutral"), N_("female^chaotic"), N_("female^liminal") };
	const char** tlist = (gender == unit_race::MALE ? aligns : aligns_female);

	return (sgettext(tlist[align]));
}

const char* unit_type::alignment_id(unit_type::ALIGNMENT align)
{
	static const char* aligns[] = { "lawful", "neutral", "chaotic", "liminal" };
	return (aligns[align]);
}

bool unit_type::has_ability_by_id(const config::t_token& ability) const
{
	if (const config &abil = cfg_.child(z_abilities))
	{
		foreach (const config::any_child &ab, abil.all_children_range()) {
			if (ab.cfg[z_id] == ability)
				return true;
		}
	}
	return false;
}

std::vector<config::t_token> unit_type::get_ability_list() const
{
	std::vector<config::t_token> res;

	const config &abilities = cfg_.child(z_abilities);
	if (!abilities) return res;

	foreach (const config::any_child &ab, abilities.all_children_range()) {
		const config::t_token &id = ab.cfg[z_id];
		if (!id.empty())
			res.push_back(id);
	}

	return res;
}

bool unit_type::hide_help() const {
	return hide_help_ || unit_types.hide_help(id(), race_->id());
}

void unit_type::add_advancement(const unit_type &to_unit,int xp)
{
	const config::t_token &to_id =  to_unit.cfg_[z_id];
	const config::t_token &from_id =  cfg_[z_id];

	// Add extra advancement path to this unit type
	LOG_CONFIG << "adding advancement from " << from_id << " to " << to_id << "\n";
	if(std::find(advances_to_.begin(), advances_to_.end(), to_id) == advances_to_.end()) {
		advances_to_.push_back(to_id);
	} else {
		LOG_CONFIG << "advancement from " << from_id
		           << " to " << to_id << " already known, ignoring.\n";
		return;
	}

	if(xp > 0) {
		//xp is 0 in case experience= wasn't given.
		if(!in_advancefrom_) {
			//This function is called for and only for an [advancefrom] tag in a unit_type referencing this unit_type.
			in_advancefrom_ = true;
			experience_needed_ = xp;
			DBG_UT << "Changing experience_needed from " << experience_needed_ << " to " << xp << " due to (first) [advancefrom] of " << to_id << "\n";
		}
		else if(experience_needed_ > xp) {
			experience_needed_ = xp;
			DBG_UT << "Lowering experience_needed from " << experience_needed_ << " to " << xp << " due to (multiple, lower) [advancefrom] of " << to_id << "\n";
		}
		else
			DBG_UT << "Ignoring experience_needed change from " << experience_needed_ << " to " << xp << " due to (multiple, higher) [advancefrom] of " << to_id << "\n";
	}

	// Add advancements to gendered subtypes, if supported by to_unit
	for(int gender=0; gender<=1; ++gender) {
		if(gender_types_[gender] == NULL) continue;
		if(to_unit.gender_types_[gender] == NULL) {
			WRN_CF << to_id << " does not support gender " << gender << "\n";
			continue;
		}
		LOG_CONFIG << "gendered advancement " << gender << ": ";
		gender_types_[gender]->add_advancement(*(to_unit.gender_types_[gender]),xp);
	}

	// Add advancements to variation subtypes.
	// Since these are still a rare and special-purpose feature,
	// we assume that the unit designer knows what they're doing,
	// and don't block advancements that would remove a variation.
	for(variations_map::iterator v=variations_.begin();
	    v!=variations_.end(); ++v) {
		LOG_CONFIG << "variation advancement: ";
		v->second->add_advancement(to_unit,xp);
	}
}

static void advancement_tree_internal(const config::t_token& id, boost::unordered_set<config::t_token>& tree)
{
	const unit_type *ut = unit_types.find(id);
	if (!ut)
		return;

	foreach(const config::t_token& adv, ut->advances_to()) {
		if (tree.insert(adv).second) {
			// insertion succeed, expand the new type
			advancement_tree_internal(adv, tree);
		}
	}
}

boost::unordered_set<config::t_token> unit_type::advancement_tree() const
{
	boost::unordered_set<config::t_token> tree;
	advancement_tree_internal(id(), tree);
	return tree;
}

const std::vector<config::t_token> unit_type::advances_from() const
{
	// currently not needed (only help call us and already did it)
	unit_types.build_all(unit_type::HELP_INDEX);

	std::vector<config::t_token> adv_from;
	foreach (const unit_type_data::unit_type_map::value_type &ut, unit_types.types())
	{
		foreach(const config::t_token& adv, ut.second.advances_to()) {
			if (adv == id())
				adv_from.push_back(ut.second.id());
		}
	}
	return adv_from;
}

unit_type_data::unit_type_data() :
	types_(),
	movement_types_(),
	races_(),
	hide_help_all_(false),
	hide_help_type_(),
	hide_help_race_(),
	unit_cfg_(NULL),
	build_status_(unit_type::NOT_BUILT)
{
}

void unit_type_data::set_config(config &cfg)
{
    DBG_UT << "unit_type_data::set_config, name: " << cfg[z_name] << "\n";

    clear();
    set_unit_config(cfg);

	foreach (const config &mt, cfg.child_range(z_movetype))
	{
		const unit_movement_type move_type(mt);
		movement_types_.insert(
			std::pair<config::t_token,unit_movement_type>(move_type.name(), move_type));
		loadscreen::increment_progress();
	}

	foreach (const config &r, cfg.child_range(z_race))
	{
		const unit_race race(r);
		races_.insert(std::pair<config::t_token,unit_race>(race.id(),race));
		loadscreen::increment_progress();
	}

	foreach (config &ut, cfg.child_range(z_unit_type))
	{
		config::t_token id = ut[z_id];
		if (const config &bu = ut.child(z_base_unit))
		{
			// Derive a new unit type from an existing base unit id
			config merge_cfg = find_config(bu[z_id]);
			ut.clear_children(z_base_unit);
			merge_cfg.merge_with(ut);
			ut.swap(merge_cfg);
		}
		// We insert an empty unit_type and build it after the copy (for performance).
		insert(std::make_pair(id, unit_type(ut)));
		LOG_CONFIG << "added " << id << " to unit_type list (unit_type_data.unit_types)\n";
		loadscreen::increment_progress();
	}

	build_all(unit_type::CREATED);

	if (const config &hide_help = cfg.child(z_hide_help)) {
		hide_help_all_ = hide_help[z_all].to_bool();
		read_hide_help(hide_help);
	}
}

const unit_type *unit_type_data::find(const config::t_token& key, unit_type::BUILD_STATUS status) const
{
	if (key.empty() || key == z_random) return NULL;

	DBG_CF << "trying to find " << key  << " in unit_type list (unit_type_data.unit_types)\n";
    const unit_type_map::iterator itor = types_.find(key);

    //This might happen if units of another era are requested (for example for savegames)
    if (itor == types_.end()){
        /*
        for (unit_type_map::const_iterator ut = types_.begin(); ut != types_.end(); ut++)
            DBG_UT << "Known unit_types: key = '" << ut->first << "', id = '" << ut->second.id() << "'\n";
        */
		return NULL;
    }

    //check if the unit_type is constructed and build it if necessary
    build_unit_type(itor, status);

	return &itor->second;
}

void unit_type_data::check_types(const std::vector<config::t_token>& types) const
{
	foreach(const config::t_token& type, types) {
		if(!find(type)) { throw game::game_error("unknown unit type: " + type); }
	}
}

const config& unit_type_data::find_config(const config::t_token& key) const
{
	const config &cfg = unit_cfg_->find_child(z_unit_type, z_id, key);

	if (cfg)
		return cfg;

    ERR_CF << "unit type not found: " << key << "\n";
    ERR_CF << *unit_cfg_ << "\n";

    throw config::error("unit type not found: "+ static_cast<std::string const &>(key));
}

void unit_type_data::clear()
{
	types_.clear();
	movement_types_.clear();
	races_.clear();
	build_status_ = unit_type::NOT_BUILT;

	hide_help_all_ = false;
	hide_help_race_.clear();
	hide_help_type_.clear();
}

void unit_type_data::build_all(unit_type::BUILD_STATUS status)
{
	if (int(status) <= int(build_status_)) return;
	assert(unit_cfg_ != NULL);

	for (unit_type_map::iterator u = types_.begin(), u_end = types_.end(); u != u_end; ++u) {
		build_unit_type(u, status);
		loadscreen::increment_progress();
	}
	for (unit_type_map::iterator u = types_.begin(), u_end = types_.end(); u != u_end; ++u) {
		add_advancement(u->second);
	}

	build_status_ = status;
}

unit_type &unit_type_data::build_unit_type(const unit_type_map::iterator &ut, unit_type::BUILD_STATUS status) const
{
	DBG_UT << "Building unit type " << ut->first << ", level " << status << '\n';

	if (int(status) <= int(ut->second.build_status()))
		return ut->second;

	switch (status) {
	case unit_type::CREATED:
		ut->second.build_created(movement_types_, races_, unit_cfg_->child_range(z_trait));
		break;
	case unit_type::HELP_INDEX:
		// Build the data needed to feed the help index.
		ut->second.build_help_index(movement_types_, races_, unit_cfg_->child_range(z_trait));
		break;
	default:
		ut->second.build_full(movement_types_, races_, unit_cfg_->child_range(z_trait));
	}

	return ut->second;
}

void unit_type_data::read_hide_help(const config& cfg)
{
	if (!cfg)
		return;

	hide_help_race_.push_back(boost::unordered_set<config::t_token>());
	hide_help_type_.push_back(boost::unordered_set<config::t_token>());

	std::vector<config::t_token> races = utils::split_token(cfg[z_race]);
	hide_help_race_.back().insert(races.begin(), races.end());

	std::vector<config::t_token> types = utils::split_token(cfg[z_type]);
	hide_help_type_.back().insert(types.begin(), types.end());

	std::vector<config::t_token> trees = utils::split_token(cfg[z_type_adv_tree]);
	hide_help_type_.back().insert(trees.begin(), trees.end());
	foreach(const config::t_token& t_id, trees) {
		unit_type_map::iterator ut = types_.find(t_id);
		if (ut != types_.end()) {
			boost::unordered_set<config::t_token> adv_tree = ut->second.advancement_tree();
			hide_help_type_.back().insert(adv_tree.begin(), adv_tree.end());
		}
	}

	// we call recursively all the imbricated [not] tags
	read_hide_help(cfg.child(z_not));
}

bool unit_type_data::hide_help(const config::t_token& type, const config::t_token& race) const
{
	bool res = hide_help_all_;
	int lvl = hide_help_all_ ? 1 : 0; // first level is covered by 'all=yes'

	// supposed to be equal but let's be cautious
	int lvl_nb = std::min(hide_help_race_.size(), hide_help_type_.size());

	for (; lvl < lvl_nb; ++lvl) {
		if (hide_help_race_[lvl].count(race) || hide_help_type_[lvl].count(type))
			res = !res; // each level is a [not]
	}
	return res;
}

void unit_type_data::add_advancement(unit_type& to_unit) const
{
    const config& cfg = to_unit.get_cfg();

    foreach (const config &af, cfg.child_range(z_advancefrom))
    {
        const config::t_token &from = af[z_unit];
        int xp = af[z_experience];

        unit_type_data::unit_type_map::iterator from_unit = types_.find(from);

		if (from_unit == types_.end()) {
			std::ostringstream msg;
			msg << "unit type '" << from << "' not found when resolving [advancefrom] tag for '"
				<< to_unit.id() << "'";
			throw config::error(msg.str());
		}

        // Fix up advance_from references
        from_unit->second.add_advancement(to_unit, xp);

        DBG_UT << "Added advancement ([advancefrom]) from " << from << " to " << to_unit.id() << "\n";
    }
}

const unit_race *unit_type_data::find_race(const config::t_token &key) const
{
	race_map::const_iterator i = races_.find(key);
	return i != races_.end() ? &i->second : NULL;
}

// This function is only meant to return the likely state of not_living
// for a new recruit of this type. It should not be used to check if
// a particular unit is living or not, use get_state("not_living") for that.
bool unit_type::not_living() const
{
	// If a unit hasn't been modified it starts out as living.
	bool not_living = false;

	// Look at all of the "musthave" traits to see if the not_living
	// status gets changed. In the unlikely event it gets changed
	// multiple times, we want to try to do it in the same order
	// that unit::apply_modifications does things.
	foreach (const config &mod, possible_traits())
	{
		if (mod[z_availability] != z_musthave)
			continue;

		foreach (const config &effect, mod.child_range(z_effect))
		{
			// See if the effect only applies to
			// certain unit types But don't worry
			// about gender checks, since we don't
			// know what the gender of the
			// hypothetical recruit is.
			const config::t_token &ut = effect[z_unit_type];
			if (!ut.empty()) {
				const std::vector<config::t_token> &types = utils::split_token(ut);
				if(std::find(types.begin(), types.end(), id()) == types.end())
					continue;
			}

			// We're only interested in status changes.
			if (effect[z_apply_to] != z_status) {
				continue;
			}
			if (effect[z_add] == z_not_living) {
				not_living = true;
			}
			if (effect[z_remove] == z_not_living) {
				not_living = false;
			}
		}
	}

	return not_living;
}

bool unit_type::has_random_traits() const
{
	if (num_traits() == 0) return false;
	config::const_child_itors t = possible_traits();
	while(t.first != t.second) {
		const config::attribute_value& availability = (*t.first)["availability"];
		if(availability.blank()) return true;
		if(strcmp(availability.str().c_str(), "musthave") != 0) return true;
		++t.first;
	}
	return false;
}

unit_type_data unit_types;

void adjust_profile(config::t_token &small, config::t_token &big, config::t_token const &def)
{
	if (big.empty())
	{
		// No profile data; use the default image.
		small = def;
		big = def;
	}
	else if (small.empty())
	{
		// No small profile; use the current profile for it and
		// try to infer the big one.
		small = big;
		std::string sbig = static_cast<std::string const &>(big);
		std::string::size_type offset = sbig.find('~');
		offset = sbig.find_last_of('/', offset);
		if (offset != std::string::npos) {
			sbig.insert(offset, "/transparent");
		} else {
			sbig = "transparent/" + sbig;
		}
		if (!image::locator(sbig).file_exists()){
			big = small; }
		else {
			big = config::t_token(sbig);
		}
	}
}
