/* $Id$ */
/*
   Copyright (C) 2003 by David White <davidnwhite@optusnet.com.au>
   Part of the Battle for Wesnoth Project http://wesnoth.whitevine.net

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#include "SDL.h"

#include "log.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

namespace {

struct logd {
	char const *name_;
	int severity_;
};

class null_streambuf : public std::streambuf
{
	virtual int overflow(int c) { return std::char_traits< char >::not_eof(c); }
public:
	null_streambuf() {}
};

} // anonymous namespace

static std::vector< logd > log_domains;
static std::ostream null_ostream(new null_streambuf);
static int indent = 0;

namespace lg {

logger err("error", 0), warn("warning", 1), info("info", 2);
log_domain general("general");

log_domain::log_domain(char const *name) : domain_(log_domains.size())
{
	logd d = { name, 0 };
	log_domains.push_back(d);
}

bool set_log_domain_severity(std::string const &name, int severity)
{
	std::vector< logd >::iterator
		it = log_domains.begin(),
		it_end = log_domains.end();
	if (name == "all") {
		for(; it != it_end; ++it)
			it->severity_ = severity;
	} else {
		for(; it != it_end; ++it)
			if (name == it->name_) break;
		if (it == it_end)
			return false;
		it->severity_ = severity;
	}
	return true;
}

std::ostream &logger::operator()(log_domain const &domain, bool show_names)
{
	logd const &d = log_domains[domain.domain_];
	if (severity_ > d.severity_)
		return null_ostream;
	else {
		if (show_names)
			std::cerr << name_ << " " << d.name_ << ": ";
		return std::cerr;
	}
};

scope_logger::scope_logger(log_domain const &domain, const std::string& str)
	: ticks_(SDL_GetTicks()), str_(str), output_(info(domain, false))
{
	do_indent();
	output_ << "BEGIN: " << str_ << "\n";
	++indent;
}

scope_logger::~scope_logger()
{
	const int ticks = SDL_GetTicks() - ticks_;
	--indent;
	do_indent();
	output_ << "END: " << str_ << " (took " << ticks << "ms)\n";
}

void scope_logger::do_indent()
{
	for(int i = 0; i != indent; ++i)
		output_ << "  ";
}

} // namespace lg
