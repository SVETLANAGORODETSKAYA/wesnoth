/* $Id$ */
/*
   Copyright (C) 2003 - 2008 by David White <dave@whitevine.net>
                 2008 by Ignacio R. Morelle <shadowm2006@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include "global.hpp"

#include "addon_management.hpp"
#include "config.hpp"
#include "construct_dialog.hpp"
#include "dialogs.hpp"
#include "filesystem.hpp"
#include "foreach.hpp"
#include "formatter.hpp"
#include "game_display.hpp"
#include "game_preferences.hpp"
#include "gettext.hpp"
#include "gui/dialogs/addon_connect.hpp"
#include "gui/dialogs/language_selection.hpp"
#include "gui/dialogs/mp_method_selection.hpp"
#include "gui/widgets/window.hpp"
#include "log.hpp"
#include "marked-up_text.hpp"
#include "network.hpp"
#include "serialization/parser.hpp"
#include "serialization/string_utils.hpp"
#include "widgets/menu.hpp"
#include "wml_exception.hpp"
#include "wml_separators.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <stdexcept>

#define DEFAULT_CAMPAIGND_PORT				15003

#define ERR_CFG LOG_STREAM(err , config)
#define LOG_CFG LOG_STREAM(info, config)
#define WRN_CFG LOG_STREAM(warn, config)
#define ERR_FS  LOG_STREAM(err , filesystem)
#define ERR_NET LOG_STREAM(err , network)
#define LOG_NET LOG_STREAM(info, network)

namespace {
	void setup_addon_dirs()
	{
		make_directory(get_user_data_dir() + "/data");
		make_directory(get_addon_campaigns_dir());
	}
} // end unnamed namespace 1

void get_addon_info(const std::string& addon_name, config& cfg)
{
	const std::string parentd = get_addon_campaigns_dir();

	// Cope with old-style or new-style file organization
	const std::string exterior = parentd + "/" + addon_name + ".pbl";
	const std::string interior = parentd + "/" + addon_name + "/_server.pbl";
	const std::string pbl_file = (file_exists(exterior)? exterior : interior);

	scoped_istream stream = istream_file(pbl_file);
	read(cfg, *stream);
}

void set_addon_info(const std::string& addon_name, const config& cfg)
{
	const std::string parentd = get_addon_campaigns_dir();

	scoped_ostream stream = ostream_file(parentd + "/" + addon_name + ".pbl");
	write(*stream, cfg);
}

void remove_local_addon(const std::string& addon)
{
	const std::string addon_dir = get_addon_campaigns_dir() + "/" + addon;

	delete_directory(addon_dir);
	if (file_exists(addon_dir + ".cfg"))
		delete_directory(addon_dir + ".cfg");
}

std::vector<std::string> available_addons()
{
	std::vector<std::string> res;
	std::vector<std::string> files, dirs;
	const std::string parentd = get_addon_campaigns_dir();
	get_files_in_dir(parentd,&files,&dirs);

	for(std::vector<std::string>::const_iterator i = dirs.begin(); i != dirs.end(); ++i) {
		const std::string external_cfg_file = *i + ".cfg";
		const std::string internal_cfg_file = *i + "/_main.cfg";
		const std::string external_pbl_file = *i + ".pbl";
		const std::string internal_pbl_file = *i + "/_server.pbl";
		if((std::find(files.begin(),files.end(),external_cfg_file) != files.end() || file_exists(parentd + "/" + internal_cfg_file)) &&
		   (std::find(files.begin(),files.end(),external_pbl_file) != files.end() || (file_exists(parentd + "/" + internal_pbl_file)))) {
			res.push_back(*i);
		}
	}

	return res;
}

std::vector<std::string> installed_addons()
{
	std::vector<std::string> res;
	const std::string parentd = get_addon_campaigns_dir();
	std::vector<std::string> files, dirs;
	get_files_in_dir(parentd,&files,&dirs);

	for(std::vector<std::string>::const_iterator i = dirs.begin(); i != dirs.end(); ++i) {
		const std::string external_cfg_file = *i + ".cfg";
		const std::string internal_cfg_file = *i + "/_main.cfg";
		if(std::find(files.begin(),files.end(),external_cfg_file) != files.end() || file_exists(parentd + "/" + internal_cfg_file)) {
			res.push_back(*i);
		}
	}

	return res;
}

namespace {
	const char escape_char = '\x01'; //!< Binary escape char.
} // end unnamed namespace 2

static bool needs_escaping(char c) {
	switch(c) {
		case '\x00':
		case escape_char:
		case '\x0D': //Windows -- carriage return
		case '\xFE': //Parser code -- textdomain or linenumber&filename
			return true;
		default:
			return false;
	}
}

static inline bool IsCR(const char& c)
{
	return c == '\x0D';
}

static std::string strip_cr(std::string str, bool strip)
{
	if(!strip)
		return str;
	std::string::iterator new_end = std::remove_if(str.begin(), str.end(), IsCR);
	str.erase(new_end, str.end());
	return str;
}

static std::string encode_binary(const std::string& str)
{
	std::string res;
	res.resize(str.size());
	size_t n = 0;
	for(std::string::const_iterator j = str.begin(); j != str.end(); ++j) {
		if(needs_escaping(*j)) {
			res.resize(res.size()+1);
			res[n++] = escape_char;
			res[n++] = *j + 1;
		} else {
			res[n++] = *j;
		}
	}

	return res;
}

static std::string unencode_binary(const std::string& str)
{
	std::string res;
	res.resize(str.size());

	size_t n = 0;
	for(std::string::const_iterator j = str.begin(); j != str.end(); ++j) {
		if(*j == escape_char && j+1 != str.end()) {
			++j;
			res[n++] = *j - 1;
			res.resize(res.size()-1);
		} else {
			res[n++] = *j;
		}
	}

	return res;
}

namespace {
	void append_default_ignore_patterns(std::pair<std::vector<std::string>, std::vector<std::string> >& patterns)
	{
		std::vector<std::string>& files = patterns.first;
		std::vector<std::string>& dirs  = patterns.second;

		/* Don't upload dot-files/dirs, which are hidden files in
		   UNIX platforms */
		files.push_back(".*");
		dirs.push_back(".*");

		files.push_back("*~");
		files.push_back("*-bak");
		files.push_back("*.pbl");
		files.push_back("*.ign");
		files.push_back("_info.cfg");
		files.push_back("*.exe");
		files.push_back("*.bat");
		files.push_back("*.cmd");
		files.push_back("*.com");
		files.push_back("*.scr");
		files.push_back("*.sh");
		files.push_back("*.js");
		files.push_back("*.vbs");
		files.push_back("*.o");
		/* Remove junk created by certain file manager ;) */
		files.push_back("Thumbs.db");
    }
}

static std::pair<std::vector<std::string>, std::vector<std::string> > read_ignore_patterns(const std::string& addon_name)
{
	const std::string parentd = get_addon_campaigns_dir();
	const std::string exterior = parentd + "/" + addon_name + ".ign";
	const std::string interior = parentd + "/" + addon_name + "/_server.ign";

	std::pair<std::vector<std::string>, std::vector<std::string> > patterns;
	std::string ign_file;
	LOG_CFG << "inserting default ignore patterns...\n";
	append_default_ignore_patterns(patterns);
	LOG_CFG << "searching for .ign file for '" << addon_name << "'...\n";
	if (file_exists(interior)) {
		ign_file = interior;
	} else if (file_exists(exterior)) {
		ign_file = exterior;
	} else {
		LOG_CFG << "no .ign file found for '" << addon_name << "'\n";
		return patterns; // just default patterns
	}
	LOG_CFG << "found .ign file: " << ign_file << '\n';
	std::istream *stream = istream_file(ign_file);
	std::string line;
	while (std::getline(*stream, line)) {
		const size_t l = line.size();
		if (line[l - 1] == '/') { // directory; we strip the last /
			patterns.second.push_back(line.substr(0, l - 1));
		} else { // file
			patterns.first.push_back(line);
		}
	}
	return patterns;
}

static void archive_file(const std::string& path, const std::string& fname, config& cfg)
{
	cfg["name"] = fname;
	const bool is_cfg = (fname.size() > 4 ? (fname.substr(fname.size() - 4) == ".cfg") : false);
	cfg["contents"] = encode_binary(strip_cr(read_file(path + '/' + fname),is_cfg));
}

static void archive_dir(const std::string& path, const std::string& dirname, config& cfg, std::pair<std::vector<std::string>, std::vector<std::string> >& ignore_patterns)
{
	cfg["name"] = dirname;
	const std::string dir = path + '/' + dirname;

	std::vector<std::string> files, dirs;
	get_files_in_dir(dir,&files,&dirs);
	for(std::vector<std::string>::const_iterator i = files.begin(); i != files.end(); ++i) {
		bool valid = true;
		for(std::vector<std::string>::const_iterator p = ignore_patterns.first.begin(); p != ignore_patterns.first.end(); ++p) {
			if (utils::wildcard_string_match(*i, *p)) {
				valid = false;
				break;
			}
		}
		if (valid) {
			archive_file(dir,*i,cfg.add_child("file"));
		}
	}

	for(std::vector<std::string>::const_iterator j = dirs.begin(); j != dirs.end(); ++j) {
		bool valid = true;
		for(std::vector<std::string>::const_iterator p = ignore_patterns.second.begin(); p != ignore_patterns.second.end(); ++p) {
			if (utils::wildcard_string_match(*j, *p)) {
				valid = false;
				break;
			}
		}
		if (valid) {
			archive_dir(dir,*j,cfg.add_child("dir"),ignore_patterns);
		}
	}
}

void archive_addon(const std::string& addon_name, config& cfg)
{
	const std::string parentd = get_addon_campaigns_dir();

	std::pair<std::vector<std::string>, std::vector<std::string> > ignore_patterns;
	// External .cfg may not exist; newer campaigns have a _main.cfg
	const std::string external_cfg = addon_name + ".cfg";
	if (file_exists(parentd + "/" + external_cfg)) {
		archive_file(parentd, external_cfg, cfg.add_child("file"));
	}
	ignore_patterns = read_ignore_patterns(addon_name);
	archive_dir(parentd, addon_name, cfg.add_child("dir"), ignore_patterns);
}

static void unarchive_file(const std::string& path, const config& cfg)
{
	write_file(path + '/' + cfg["name"].str(), unencode_binary(cfg["contents"]));
}

static void unarchive_dir(const std::string& path, const config& cfg)
{
	std::string dir;
	if (cfg["name"].empty())
		dir = path;
	else
		dir = path + '/' + cfg["name"].str();

	make_directory(dir);

	const config::child_list& dirs = cfg.get_children("dir");
	for(config::child_list::const_iterator i = dirs.begin(); i != dirs.end(); ++i) {
		unarchive_dir(dir,**i);
	}

	const config::child_list& files = cfg.get_children("file");
	for(config::child_list::const_iterator j = files.begin(); j != files.end(); ++j) {
		unarchive_file(dir,**j);
	}
}

void unarchive_addon(const config& cfg)
{
	const std::string parentd = get_addon_campaigns_dir();
	setup_addon_dirs();
	unarchive_dir(parentd, cfg);
}

namespace {
	//! Strip the ".cfg" extension and replace "_" with " " for display.
	//! @param files      List of files in the add-ons directory.
	//! @param dirs       List of subdirectories in the add-ons directory.
	//! @param parent_dir Path to the add-ons directory.
	void prepare_addons_list_for_display(std::vector<std::string>& files,
	                                     std::vector<std::string>& dirs,
	                                     const std::string& parent_dir)
	{
		std::vector<std::string>::iterator i = files.begin();
		while(i != files.end())
		{
			const std::string::size_type pos = i->rfind(".cfg", i->size());
			if(pos == std::string::npos) {
				i = files.erase(i);
			} else {
				i->erase(pos);
				// remove it from the directory list too
				for(std::vector<std::string>::iterator j = dirs.begin(); j != dirs.end() ; ++j) {
					if (*i == *j) {
						dirs.erase(j);
						break;
					}
				};
				std::replace(i->begin(), i->end(), '_', ' ');
				i++;
			}
		}
		// process items of type Addon/_main.cfg too
		i = dirs.begin();
		while(i != dirs.end())
		{
			if (file_exists(parent_dir + *i + "/_main.cfg")) {
				std::replace(i->begin(), i->end(), '_', ' ');
				files.push_back(*i);
				i++;
			} else {
				i = dirs.erase(i);
			}
		}
	}
	
	//! Creates a more human-readable representation of a file size.
	//! @param size_str File size string, as obtained from a config object.
	//! @return         Representation of file size in the biggest byte multiply
	//!                 possible.
	static std::string format_file_size(const std::string& size_str)
	{
		double size = lexical_cast_default<double>(size_str,0.0);
	
		const double k = 1024;
		if(size > 0.0) {
			std::string size_postfix = _("B");	// bytes
			if(size > k) {
				size /= k;
				size_postfix = _("KB");			// kilobytes
				if(size > k) {
					size /= k;
					size_postfix = _("MB");		// megabytes
					if(size > k) {
						size /= k;
						size_postfix = _("GB");	// gigabytes
					}
				}
			}
	
			std::ostringstream stream;
#ifdef _MSC_VER
			// Visual C++ makes 'precision' set the number of decimal places.
			// Other platforms make it set the number of significant figures
			stream.precision(1);
			stream << std::fixed << size << size_postfix;
#else
			if (size < 100) stream.precision(3);
			else size = static_cast<int>(size);
			stream << size << size_postfix;
#endif
			return stream.str();
		} else {
			return "";
		}
	}
	
	//! Return a short string describing an add-on's type.
	//! @param type Numerical add-on type.
	//! @return     A string, translated to the current locale.
	std::string get_translatable_addon_type(ADDON_TYPE type)
	{
		switch (type) {
			case ADDON_SP_CAMPAIGN:
				return _("addon_type^Campaign");
			case ADDON_SP_SCENARIO:
				return _("addon_type^Scenario");
			case ADDON_MP_ERA:
				return _("addon_type^MP Era");
			case ADDON_MP_FACTION:
				return _("addon_type^MP Faction");
			case ADDON_MP_MAPS:
				return _("addon_type^MP Map-pack");
			case ADDON_MP_SCENARIO:
				return _("addon_type^MP scenario");
			case ADDON_MP_CAMPAIGN:
				return _("addon_type^MP campaign");
			case ADDON_MEDIA:
				return _("addon_type^Resources");
			default:
				return _("addon_type^(unknown)");
		}
	}
	
	//! Checks if an add-on's dependencies are met.
	//! @param disp    Object to be used for displaying interactive messages.
	//! @param deplist List of dependencies (add-on identifiers).
	//! @return        true if dependencies are met; false otherwise.
	bool addon_dependencies_met(game_display& disp, const std::vector<std::string>& deplist)
	{
		const std::vector<std::string>& installed = installed_addons();
		std::vector<std::string>::const_iterator i;
		std::string missing = "";
		size_t count_missing = 0;
	
		foreach(const std::string& i, deplist) {
			if (std::find(installed.begin(), installed.end(), i) == installed.end()) {
				missing += "\n" + i;
				++count_missing;
			}
		}
		// If there are any, display a message.
		// TODO: Somehow offer to automatically download
		// the missing dependencies.
		if (!missing.empty()) {
			const std::string msg_title    = _("Dependencies");
			const std::string msg_entrytxt = _n("This add-on depends upon the following add-on which you have not installed yet:",
												"This add-on depends upon the following add-ons which you have not installed yet:",
												count_missing);
			if (gui::dialog(disp, msg_title,
							msg_entrytxt +
							"\n" + missing +
							"\n" + _("Do you still want to download it?"), gui::OK_CANCEL).show())
				return false;
		}
		return true;
	}
	
	void upload_addon_to_server(game_display& disp, const std::string& addon, network::connection sock)
	{
		config request_terms;
		request_terms.add_child("request_terms");
		network::send_data(request_terms, sock, true);
		config data;
		sock = network::receive_data(data,sock,5000);
		if(!sock) {
			gui::show_error_message(disp, _("Connection timed out"));
			return;
		} else if(data.child("error")) {
			std::string error_message = _("The server responded with an error: \"$error|\"");
			utils::string_map symbols;
			symbols["error"] = (*data.child("error"))["message"].str();
			error_message = utils::interpolate_variables_into_string(error_message, &symbols);
			gui::show_error_message(disp, error_message);
			return;
		} else if(data.child("message")) {
			const int res = gui::dialog(disp,_("Terms"),(*data.child("message"))["message"],gui::OK_CANCEL).show();
			if(res != 0) {
				return;
			}
		}
	
		config cfg;
		get_addon_info(addon,cfg);
	
		std::string passphrase = cfg["passphrase"];
		// generate a random passphrase and write it to disk
		// if the .pbl file doesn't provide one already
		if(passphrase.empty()) {
			passphrase.resize(8);
			for(size_t n = 0; n != 8; ++n) {
				passphrase[n] = 'a' + (rand()%26);
			}
			cfg["passphrase"] = passphrase;
			set_addon_info(addon,cfg);
		}
	
		cfg["name"] = addon;
	
		config addon_data;
		archive_addon(addon,addon_data);
	
		data.clear();
		data.add_child("upload",cfg).add_child("data",addon_data);
	
		LOG_NET << "uploading add-on...\n";
		// @todo Should be enabled once the campaign server can be recompiled.
		network::send_data(data, sock, true);
	
		sock = dialogs::network_send_dialog(disp,_("Sending add-on"),data,sock);
		if(!sock) {
			return;
		} else if(data.child("error")) {
			gui::show_error_message(disp, _("The server responded with an error: \"") +
									(*data.child("error"))["message"].str() + '"');
		} else if(data.child("message")) {
			/* GCC-3.3 needs a temp var otherwise compilation fails */
			gui::message_dialog dlg(disp,_("Response"),(*data.child("message"))["message"]);
			dlg.show();
		}
	}
	
	void delete_remote_addon(game_display& disp, const std::string& addon, network::connection sock)
	{
		config cfg;
		get_addon_info(addon,cfg);
	
		config msg;
		msg["name"] = addon;
		msg["passphrase"] = cfg["passphrase"];
	
		config data;
		data.add_child("delete",msg);

		network::send_data(data, sock, true);
	
		sock = network::receive_data(data,sock,5000);
		if(!sock) {
			gui::show_error_message(disp, _("Connection timed out"));
		} else if(data.child("error")) {
			gui::show_error_message(disp, _("The server responded with an error: \"") +
									(*data.child("error"))["message"].str() + '"');
		} else if(data.child("message")) {
			/* GCC-3.3 needs a temp var otherwise compilation fails */
			gui::message_dialog dlg(disp,_("Response"),(*data.child("message"))["message"]);
			dlg.show();
		}
	}
	
	void addons_update_dlg(game_display& disp, config& /*cfg*/, const config::child_list& remote_addons_list,
	                       const network::manager& /*net_manager*/, const network::connection& /*sock*/,
	                       bool* /*do_refresh*/)
	{
		std::vector< config* > remote_matches_cfgs;
		std::vector< std::string > safe_matches;
		std::vector< std::string > unsafe_matches;
		std::ostringstream unsafe_list;
		const std::vector< std::string >& all_local = installed_addons();
		// Add-ons that can be published and are outdated will not be offered for update,
		// but a message will be displayed warning about them to the user.
		const std::vector< std::string >& all_publish = available_addons();
		std::vector<addon_version_info> safe_local_versions;
		std::vector<addon_version_info> unsafe_local_versions;
		std::map<std::string, addon_version_info> remote_version_map;
		foreach(config* const remote_addon, remote_addons_list) {
			if(remote_addon == NULL) continue; // shouldn't happen...
			const std::string& name = (*remote_addon)["name"];
			const std::string& version = (*remote_addon)["version"];
			try {
				remote_version_map.insert(std::make_pair(name, addon_version_info(version)));
			} catch(addon_version_info_not_sane_exception const&) {
				ERR_CFG << "remote add-on '" << name << "' has invalid version string '" << version << "', skipping from updates check...\n";
				continue;
			}
			std::vector<std::string>::const_iterator local_match =
					std::find(all_local.begin(), all_local.end(), name);
			try {
				if(local_match != all_local.end()) {
					const addon_version_info& local_version = get_addon_version_info(name);
					if(remote_version_map[name] > local_version) {
						if(std::find(all_publish.begin(), all_publish.end(), name) != all_publish.end()) {
							unsafe_matches.push_back(name);
							unsafe_local_versions.push_back(local_version);
							unsafe_list << '\n';
							unsafe_list << name << " (local: " << local_version << ", remote: " << version << ")";
						} else {
							safe_matches.push_back(name);
							safe_local_versions.push_back(local_version);
							remote_matches_cfgs.push_back(remote_addon);
						}
					}
				}
			} catch(addon_version_info_not_sane_exception const&) {
				ERR_CFG << "local add-on '" << name << "' has invalid version string '" << version << "', skipping from updates check...\n";
				continue;
			}
		}
		if(!unsafe_matches.empty()) {
			const std::string warn_title = _("Outdated add-ons");
			const std::string warn_entrytxt = _n(
				"An outdated local add-on has publishing information attached. It will not be offered for updating.",
				"Some outdated local add-ons have publishing information attached. They will not be offered for updating.",
				unsafe_matches.size());
			gui::dialog(disp, warn_title, warn_entrytxt + unsafe_list.str(), gui::MESSAGE).show();
		}
		
		// column contents
		std::vector<std::string> addons, titles, oldversions, newversions, options, filtered_opts;
		std::vector<int> sizes;
		
		std::string sep(1, COLUMN_SEPARATOR);
		const std::string& heading =
			(formatter() << HEADING_PREFIX << sep << _("Name") << sep << _("Old version") << sep << _("New version") << sep
			             << _("Author") << sep << _("Size")).str();
		options.push_back(heading);
		filtered_opts.push_back(heading);

		assert(safe_matches.size() == safe_local_versions.size());
		assert(safe_matches.size() == remote_matches_cfgs.size());
		for(size_t i = 0; i < safe_matches.size(); ++i) {
			const config& c = *(remote_matches_cfgs[i]);
			const std::string& name = c["name"];
			const std::string& size = c["size"];
			const std::string& sizef = format_file_size(size);
			const std::string& oldver = safe_local_versions[i];
			const std::string& newver = remote_version_map[name];
			
			std::string author = c["author"];
			std::string title = c["title"];
			if(title.empty()) {
				title = name;
				std::replace(title.begin(), title.end(), '_', ' ');
			}

			utils::truncate_as_wstring(title, 20);
			utils::truncate_as_wstring(author, 16);
			
			//add negative sizes to reverse the sort order
			sizes.push_back(-atoi(size.c_str()));

			addons.push_back(name);
			titles.push_back(title);
			oldversions.push_back(oldver);
			newversions.push_back(newver);

			std::string icon = c["icon"];
			if(icon.find("units/") != std::string::npos &&
				icon.find_first_of('~') == std::string::npos) {
				icon.append("~RC(magenta>red)");
			}

			const std::string text_columns =
				title + COLUMN_SEPARATOR +
				oldver + COLUMN_SEPARATOR +
				newver + COLUMN_SEPARATOR +
				author + COLUMN_SEPARATOR +
				sizef + COLUMN_SEPARATOR;
			options.push_back(IMAGE_PREFIX + icon + COLUMN_SEPARATOR + text_columns);
			filtered_opts.push_back(text_columns);
		}

		// Create dialog
		gui::dialog upd_dialog(disp,
			_("Update Add-ons"),
			_("Select an add-on to update:"), gui::OK_CANCEL);

		// Create widgets
		gui::menu::basic_sorter sorter;
		sorter.set_alpha_sort(1).set_alpha_sort(2).set_alpha_sort(3).set_alpha_sort(4).set_position_sort(5,sizes);

		gui::menu::imgsel_style addon_style(gui::menu::bluebg_style);
		addon_style.scale_images(font::relative_size(72), font::relative_size(72));

		gui::menu* addon_menu =
			new gui::menu(disp.video(), options, false, -1,
			              gui::dialog::max_menu_width, &sorter,
			              &addon_style, false);
		gui::dialog_button* update_all_button =
			new gui::dialog_button(disp.video(), _("Update all"),
			                       gui::button::TYPE_PRESS, -255);
// FIXME: dunno if it was because I was testing with 0 elements (besides
// the header) or I'm doing it wrong, but this causes SIGSEGV. Better let
// someone with the patience and knowledge required to deal with filter_textbox
// do it instead
//
// 		gui::filter_textbox* filter =
// 			new gui::filter_textbox(disp.video(),
// 			                        _("Filter: "), options, filtered_opts, 1, upd_dialog, 300);

		// Add widgets
		upd_dialog.add_button(update_all_button, gui::dialog::BUTTON_EXTRA);
		upd_dialog.set_menu(addon_menu);
// 		upd_dialog.set_textbox(filter);

		// Activate
		int index = upd_dialog.show();
		const bool upd_all = index == -255;
// 		index = filter->get_index(index);
		// FIXME: until it is possible to integrate this functionality and download_addons()
		// in a single dialog, we'll resort to a magic result to detect the "all" choice.
		if(index < 0 && !upd_all)
			return;

		if(upd_all) {
		}
	}

	void install_addon(game_display& disp, config& cfg, const config* const addons_tree,
	                   const std::string& addon_id, const std::string& addon_title,
	                   const std::string& addon_type_str, const std::string& addon_uploads_str,
	                   const std::string& addon_version_str,
	                   const network::manager& /*net_manager*/,
	                   const network::connection& sock, bool* do_refresh)
	{
		// Get all dependencies of the addon/campaign selected for download.
		const config * const selected_campaign = addons_tree->find_child("campaign", "name", addon_id);
		assert(selected_campaign != NULL);
		// Get all dependencies which are not already installed.
		// TODO: Somehow determine if the version is outdated.
		std::vector<std::string> dependencies = utils::split((*selected_campaign)["dependencies"]);
		if (!addon_dependencies_met(disp,dependencies)) return;
		// Proceed to download and install
		config request;
		request.add_child("request_campaign")["name"] = addon_id;
		network::send_data(request, sock, true);

		utils::string_map syms;
		syms["addon_title"] = addon_title;
		const std::string& download_dlg_title =
			utils::interpolate_variables_into_string(_("Downloading add-on: $addon_title|..."), &syms);

		network::connection res = dialogs::network_receive_dialog(disp, download_dlg_title, cfg, sock);
		if(!res) {
			return;
		}

		config const * const dlerror = cfg.child("error");
		if(dlerror != NULL) {
			gui::show_error_message(disp, (*dlerror)["message"]);
			return;
		}

		if(!check_names_legal(cfg)) {
			gui::show_error_message(disp, _("The add-on has an invalid file or directory name and can not be installed."));
			return;
		}
		
		// remove any existing versions of the just downloaded add-on,
		// assuming it consists of a dir and a cfg file
		remove_local_addon(addon_id);

		// add revision info to the addon archive
		config* maindir = cfg.find_child("dir", "name", addon_id);
		if(maindir == NULL) {
			LOG_CFG << "downloaded addon '" << addon_id << "' is missing its own directory, creating...\n";
			maindir = &cfg.add_child("dir");
			(*maindir)["name"] = addon_id;
		}

		LOG_CFG << "generating version info for addon '" << addon_id << "'\n";
		config f;
		f["name"] = "_info.cfg";
		std::string s;
		s += "#\n"
				"# Automatically generated by Wesnoth to keep track\n"
				"# of version information on installed add-ons.\n"
				"#\n";
		s += "[info]\n";
		if(!addon_type_str.empty()) {
		s += "    type=\"" + addon_type_str + "\"\n";
		}
		s += "    uploads=\"" + addon_uploads_str + "\"\n";
		s += "    version=\"" + addon_version_str + "\"\n";
		s += "[/info]\n";
		f["contents"] = s;
		maindir->add_child("file", f);
		LOG_CFG << "generated version info, unpacking...\n";
		unarchive_addon(cfg);
		LOG_CFG << "addon unpacked successfully\n";

		std::string warning = "";
		std::vector<config *> scripts = find_scripts(cfg, ".unchecked");
		if(!scripts.empty()) {
			WRN_CFG << "downloaded addon '" << addon_title << "' has unchecked scripts\n";
			warning += "\n";
			warning += _("Unchecked script files found:");
			foreach(const config* i, scripts)
				warning += "\n" + (*i)["name"];
		}

		const std::string& message =
			utils::interpolate_variables_into_string(_("The add-on '$addon_title|' has been successfully installed."), &syms);
		/* GCC-3.3 needs a temp var otherwise compilation fails */
		gui::message_dialog dlg(disp, _("Add-on Installed"), message);
		dlg.show();

		if(do_refresh != NULL)
			*do_refresh = true;
	}

	void download_addons(game_display& disp, std::string remote_host, bool update_mode, bool* do_refresh)
	{
		const std::vector<std::string> address_components =
			utils::split(remote_host, ':');
		if(address_components.empty()) {
			return;
		}
		
		const std::string old_host = preferences::campaign_server();
		const int remote_port = lexical_cast_default<int>(address_components.back(),
		                                                  DEFAULT_CAMPAIGND_PORT);
		remote_host = address_components.front();
		preferences::set_campaign_server(remote_host);
		
		try {
			const network::manager net_manager;
			const network::connection sock =
				dialogs::network_connect_dialog(disp, _("Connecting to Add-ons Server..."),
				                                remote_host, remote_port);
			if(!sock) {
				gui::show_error_message(disp, _("Could not connect to host."));
				preferences::set_campaign_server(old_host);
				return;
			}
			
			config cfg;
			cfg.add_child("request_campaign_list");
			network::send_data(cfg, sock, true);
			network::connection res = dialogs::network_receive_dialog(disp, _("Requesting list of add-ons"), cfg, sock);
			if(!res) {
				return;
			}

			config const * const error = cfg.child("error");
			if(error != NULL) {
				gui::show_error_message(disp, (*error)["message"]);
				return;
			}
			
			config const * const addons_tree = cfg.child("campaigns");
			if(addons_tree == NULL) {
				gui::show_error_message(disp, _("An error occurred while communicating with the server."));
				return;
			}
			
			const config::child_list& addon_cfgs = addons_tree->get_children("campaign");
			if(update_mode) {
				addons_update_dlg(disp, cfg, addon_cfgs, net_manager, sock, do_refresh);
				return;
			}
			
			// column contents
			std::vector<std::string> addons, titles, versions, uploads, types, options, options_to_filter;
			std::vector<int> sizes;

			std::string sep(1, COLUMN_SEPARATOR);
			const std::string& heading =
				(formatter() << HEADING_PREFIX << sep << _("Name") << sep << _("Version") << sep
				             << _("Author") << sep << _("Type") << sep << _("Downloads") << sep << _("Size")).str();
			options.push_back(heading);
			options_to_filter.push_back(heading);

			const std::vector< std::string >& publish_options = available_addons();

			std::vector< std::string > delete_options;
			
			foreach(const config* i, addon_cfgs) {
				const config& c = *i;

				const std::string& name = c["name"];
				const std::string& downloads = c["downloads"].str();
				const std::string& size = c["size"];
				const std::string& sizef = format_file_size(size);
				const std::string& type_str = c["type"];
				const ADDON_TYPE type = get_addon_type(type_str);
				const std::string& type_label_str = get_translatable_addon_type(type);

				addons.push_back(name);
				versions.push_back(c["version"]);
				uploads.push_back(c["uploads"]);
				types.push_back(type_str);
				
				if(std::count(publish_options.begin(), publish_options.end(), name) != 0) {
					delete_options.push_back(name);
				}
				
				std::string title = c["title"];
				if(title == "") {
					title = name;
					std::replace(title.begin(),title.end(),'_',' ');
				}
				titles.push_back(title);
				
				std::string version = c["version"], author = c["author"];

				utils::truncate_as_wstring(title, 20);
				utils::truncate_as_wstring(version, 12);
				utils::truncate_as_wstring(author, 16);
				
				//add negative sizes to reverse the sort order
				sizes.push_back(-atoi(size.c_str()));
				
				std::string icon = c["icon"];
				if(icon.find("units/") != std::string::npos &&
				   icon.find_first_of('~') == std::string::npos) {
					//a hack to prevent magenta icons, because they look awful
					icon.append("~RC(magenta>red)");
				}
				const std::string text_columns =
					title + COLUMN_SEPARATOR +
					version + COLUMN_SEPARATOR +
					author + COLUMN_SEPARATOR +
					type_label_str + COLUMN_SEPARATOR +
					downloads + COLUMN_SEPARATOR +
					sizef + COLUMN_SEPARATOR;

				options.push_back(IMAGE_PREFIX + icon + COLUMN_SEPARATOR + text_columns);
				// icon paths shouldn't be filtered!
				options_to_filter.push_back(text_columns);
			}
			
			std::string pub_option_text, del_option_text;

			foreach(const std::string& pub, publish_options) {
				static const std::string publish_icon = "icons/icon-addon-publish.png";
				const std::string text = _("Publish add-on: ") + pub;
				options.push_back(IMAGE_PREFIX + publish_icon + COLUMN_SEPARATOR + font::GOOD_TEXT + text);
				options_to_filter.push_back(text);
			}
			foreach(const std::string& del, delete_options) {
				static const std::string delete_icon = "icons/icon-addon-delete.png";
				const std::string text = _("Delete add-on: ") + del;
				options.push_back(IMAGE_PREFIX + delete_icon + COLUMN_SEPARATOR + font::BAD_TEXT + text);
				options_to_filter.push_back(text);
			}

			if(addons.empty() && publish_options.empty()) {
				gui::show_error_message(disp, _("There are no add-ons available for download from this server."));
				return;
			}
			
			gui::menu::basic_sorter sorter;
			sorter.set_alpha_sort(1).set_alpha_sort(2).set_alpha_sort(3).set_alpha_sort(4).set_numeric_sort(5).set_position_sort(6,sizes);

			gui::dialog addon_dialog(disp, _("Get Add-ons"),
			                         _("Choose the add-on to download."),
			                         gui::OK_CANCEL);
			gui::menu::imgsel_style addon_style(gui::menu::bluebg_style);
			
			//make sure the icon isn't too big
			addon_style.scale_images(font::relative_size(72), font::relative_size(72));
			gui::menu *addon_menu = new gui::menu(disp.video(), options, false, -1,
			                                      gui::dialog::max_menu_width, &sorter,
			                                      &addon_style, false);
			addon_dialog.set_menu(addon_menu);

			gui::filter_textbox* filter = new gui::filter_textbox(disp.video(),
			_("Filter: "), options, options_to_filter, 1, addon_dialog, 300);
			addon_dialog.set_textbox(filter);

			int index = addon_dialog.show();
			index = filter->get_index(index);

			if(index < 0) {
				return;
			}
			
			// Handle deletion option
			if(index >= int(addons.size() + publish_options.size())) {
				const std::string& addon = delete_options[index - int(addons.size() + publish_options.size())];
				delete_remote_addon(disp, addon, sock);
				return;
			}
			
			// Handle publish option
			if(index >= int(addons.size())) {
				const std::string& addon = publish_options[index - int(addons.size())];
				upload_addon_to_server(disp, addon, sock);
				return;
			}
			
			// Handle download
			install_addon(disp, cfg, addons_tree, addons[index], titles[index], types[index],
			              uploads[index], versions[index], net_manager, sock, do_refresh);

		} catch(config::error& e) {
			ERR_CFG << "config::error thrown during transaction with add-on server; \""<< e.message << "\"\n";
			gui::show_error_message(disp, _("Network communication error."));
		} catch(network::error& e) {
			ERR_NET << "network::error thrown during transaction with add-on server; \""<< e.message << "\"\n";
			gui::show_error_message(disp, _("Remote host disconnected."));
		} catch(io_exception& e) {
			ERR_FS << "io_exception thrown while installing an addon; \"" << e.what() << "\"\n";
			gui::show_error_message(disp, _("A problem occurred when trying to create the files necessary to install this add-on."));
		} catch(twml_exception& e) {
			e.show(disp);
		}
	}
	
	void uninstall_local_addons(game_display& disp, bool* should_reload_cfg)
	{
		std::vector<std::string> addons;
		std::vector<std::string> addon_dirs;

		const std::string parentdir = get_addon_campaigns_dir() + "/";

		get_files_in_dir(parentdir, &addons, &addon_dirs, FILE_NAME_ONLY);
		prepare_addons_list_for_display(addons, addon_dirs, parentdir);

		if (addons.empty()) {
			gui::show_error_message(disp, _("You have no add-ons installed."));
			return;
		}

		gui::menu::basic_sorter sorter;
		sorter.set_alpha_sort(1);

		int index = 0;
		int res;

		do {
			gui::dialog addon_dialog(disp,
							_("Uninstall add-ons"), _("Choose the add-on to remove."),
							gui::OK_CANCEL);
			gui::menu::imgsel_style &addon_style = gui::menu::bluebg_style;

			gui::menu *addon_menu = new gui::menu(disp.video(), addons, false, -1,
					gui::dialog::max_menu_width, &sorter, &addon_style, false);
			addon_dialog.set_menu(addon_menu);
			index = addon_dialog.show();

			if(index < 0)
				return;

			std::string confirm_message = _("Are you sure you want to remove the add-on '$addon|'?");
			utils::string_map symbols;
			symbols["addon"] = addons.at(index);
			confirm_message = utils::interpolate_variables_into_string(confirm_message, &symbols);
			res = gui::dialog(disp, _("Confirm"), confirm_message, gui::YES_NO).show();
		} while (res != 0);

		// Put underscores back in the name and remove the addon
		std::string filename = addons.at(index);
		std::replace(filename.begin(), filename.end(), ' ', '_');
		bool delete_success = delete_directory(parentdir + filename);
		delete_success |= delete_directory(parentdir + filename + ".cfg");
		// Report results
		if (delete_success)
		{
			std::string message = _("Add-on '$addon|' deleted.");
			utils::string_map symbols;
			symbols["addon"] = addons.at(index);
			message = utils::interpolate_variables_into_string(message, &symbols);
			/* GCC-3.3 needs a temp var otherwise compilation fails */
			gui::dialog dlg(disp, _("Add-on deleted"), message, gui::OK_ONLY);
			dlg.show();

			if(should_reload_cfg != NULL)
				*should_reload_cfg = true;
		}
		else
		{
			/* GCC-3.3 needs a temp var otherwise compilation fails */
			gui::dialog dlg2(disp, _("Error"), _("Add-on could not be deleted -- a file was not found."),
					gui::OK_ONLY);
			dlg2.show();
		}
	}
} // end unnamed namespace 4

#define ADDONS_OPT_DOWNLOAD		0
#define ADDONS_OPT_UNINSTALL	2
#define ADDONS_OPT_UPDATE		3

void manage_addons(game_display& disp)
{
	int res;
	std::string remote_host;
	const std::string default_host = preferences::campaign_server();
	
	if(gui2::new_widgets) {
		gui2::taddon_connect addon_dlg;

		addon_dlg.set_host_name(default_host);
		addon_dlg.show(disp.video());

		res = addon_dlg.get_retval();
		if(res == gui2::twindow::OK) {
			res = 0;
			remote_host = addon_dlg.host_name();
		}
	} else {
		gui::dialog svr_dialog(disp,
		                       _("Connect to add-ons server"),
		                       _("Type the address of a server to download add-ons from."),
		                       gui::OK_CANCEL);
		svr_dialog.set_textbox(_("Server: "), default_host);
#if 0
		// not ready for production yet
		svr_dialog.add_button(new gui::dialog_button(disp.video(), _("Update add-ons"),
		                      gui::button::TYPE_PRESS, ADDONS_OPT_UPDATE),
		                      gui::dialog::BUTTON_EXTRA_LEFT);
#endif
		svr_dialog.add_button(new gui::dialog_button(disp.video(), _("Uninstall add-ons"),
		                      gui::button::TYPE_PRESS, ADDONS_OPT_UNINSTALL),
		                      gui::dialog::BUTTON_EXTRA);
		res = svr_dialog.show();
		remote_host = svr_dialog.textbox_text();
		bool do_refresh = false;
		switch(res) {
			case ADDONS_OPT_UPDATE:
			case ADDONS_OPT_DOWNLOAD:
				download_addons(disp, remote_host, res==ADDONS_OPT_UPDATE, &do_refresh);
				break;
			case ADDONS_OPT_UNINSTALL:
				uninstall_local_addons(disp, &do_refresh);
				break;
			default:
				return;
		}
		// Signal game_controller to reload WML
		if(do_refresh) {
			throw config_changed_exception();
		}
	}
}

bool operator>(const addon_version_info& l, const addon_version_info& r)
{
	if((!r.sane) || (!l.sane))
		throw addon_version_info_not_sane_exception();

	return (l.vmajor > r.vmajor || (l.vmajor == r.vmajor && (l.vminor > r.vminor || (l.vminor == r.vminor && l.revision > r.revision))));
}

bool operator<(const addon_version_info& l, const addon_version_info& r)
{
	if((!r.sane) || (!l.sane))
		throw addon_version_info_not_sane_exception();

	return (l.vmajor < r.vmajor || (l.vmajor == r.vmajor && (l.vminor < r.vminor || (l.vminor == r.vminor && l.revision < r.revision))));
}

bool operator>=(const addon_version_info& l, const addon_version_info& r)
{
	if((!r.sane) || (!l.sane))
		throw addon_version_info_not_sane_exception();

	return !(l < r);
}

bool operator<=(const addon_version_info& l, const addon_version_info& r)
{
	if((!r.sane) || (!l.sane))
		throw addon_version_info_not_sane_exception();

	return !(l > r);
}

std::string addon_version_info::str(void) const
{
	if(!sane)
		throw addon_version_info_not_sane_exception();

	std::ostringstream out;
	out << vmajor << '.' << vminor << '.' << revision;
	return out.str();
}

addon_version_info& addon_version_info::operator=(const addon_version_info& o)
{
	if(this != &o) {
		this->vmajor = o.vmajor;
		this->vminor = o.vminor;
		this->revision = o.revision;
		// copy o's insanity too
		this->sane = o.sane;
	}
	return *this;
}

void addon_version_info::reset()
{
	vmajor = vminor = revision = 0;
	sane = false;
}

addon_version_info::addon_version_info(const std::string& src_str)
{
	const std::vector<std::string> components = utils::split(src_str, '.');
	try {
		vmajor   = lexical_cast<unsigned>(components.at(0));
		vminor   = lexical_cast<unsigned>(components.at(1));
		revision = components.size() >= 3 ? lexical_cast<unsigned>(components[2]) : 0;
		sane     = true;
	} catch(std::out_of_range const&) {
		reset();
	} catch(bad_lexical_cast const&)  {
		reset();
	}
}

addon_version_info::addon_version_info(const addon_version_info& src_struct)
	: vmajor(src_struct.vmajor), vminor(src_struct.vminor),
	  revision(src_struct.revision), sane(src_struct.sane)
{}

addon_version_info::addon_version_info(unsigned major, unsigned minor, unsigned rev, bool sane_flag)
	: vmajor(major), vminor(minor), revision(rev), sane(sane_flag)
{}

addon_version_info::addon_version_info()
	: vmajor(0), vminor(0), revision(0), sane(true)
{}

namespace {
	std::map< std::string, addon_version_info > version_info_cache;
} // end unnamed namespace 5

void refresh_addon_version_info_cache(void)
{
	if(version_info_cache.empty() != true)
		version_info_cache.clear();

	LOG_CFG << "probing add-ons and refreshing version information cache...\n";

	const std::vector<std::string>& addons = installed_addons();
	if(addons.empty()) {
		LOG_CFG << "no add-ons found\n";
		return;
	}
	static const std::string parentd = get_addon_campaigns_dir();
	std::vector<std::string> addon_info_files;
	foreach(std::string const& dir, addons)
		addon_info_files.push_back(parentd+"/"+dir+"/_info.cfg");

	size_t i = 0;

	foreach(std::string const& info_file, addon_info_files) {
		assert(i < addons.size());

		config cfg;
		std::string const& addon = addons[i];
		scoped_istream stream = istream_file(info_file);

		read(cfg, *stream);
		
		config const* const info_cfg = cfg.child("info");
		if(info_cfg == NULL) {
			++i;
			continue;
		}
		std::string const& version = (*info_cfg)["version"];
		LOG_CFG << "caching add-on version info: " << addon << " [" << version << "]\n";
		version_info_cache.insert(std::make_pair(addon, addon_version_info(version)));
		
		++i;
	}
}

const addon_version_info& get_addon_version_info(const std::string& addon)
{
	static const addon_version_info nil(0,0,0,false);
	std::map< std::string, addon_version_info >::iterator ret = version_info_cache.find(addon);

	if (ret != version_info_cache.end())
		return ret->second;
	else
		return nil;

}
