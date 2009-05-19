/* $Id$ */
/*
   Copyright (C) 2003 - 2009 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef SERVER_PLAYER_NETWORK_HPP_INCLUDED
#define SERVER_PLAYER_NETWORK_HPP_INCLUDED

#include "../network.hpp"
#include "player.hpp"
#include "simple_wml.hpp"

#include <boost/function.hpp>

namespace wesnothd {

namespace chat_message {
	/**
	 * Function to ensure a text message is within the allowed length
	 */
	void truncate_message(const simple_wml::string_span& str,
		simple_wml::node& message);
} // end chat_message namespace

typedef std::map<network::connection,player> player_map;
typedef std::vector<network::connection> connection_vector;

/** Convenience function for finding a user by name
 * @param all_players map of all players
 * @param name        name of the player to look for
 * @return            an iterator pointing to the player, or to end() if the
 *                    name was not found.
 */
player_map::const_iterator find_user(const player_map& all_players,
									 const simple_wml::string_span& name);

/**
 * Send a wml document to a single player
 * @param data        the document to send
 * @param sock        the socket id to send to
 * @param packet_type the packet type, if empty the root node name is used
 */
void send_to_one(simple_wml::document& data,
				 const network::connection sock,
				 std::string packet_type = "");

/**
 * Send a wml document to a vector of players. More efficient than calling
 * send_to_one many times.
 * @param data        the document to send
 * @param vec         the vector of player socket ids to send to
 * @param exclude     if nonzero, do not send to this player
 * @param packet_type the packet type, if empty the root node name is used
 */
void send_to_many(simple_wml::document& data,
				  const connection_vector& vec,
				  const network::connection exclude = 0,
				  std::string packet_type = "");
/**
 * A more powerful version of send_to_many, allowing the use of a predicate
 * connection->bool. The document will be sent only to those sockets for which
 * the except_pred predicate evaluates to false.
 * @param data        the document to send
 * @param vec         the vector of player socket ids to send to
 * @param except_pred a general exclusion predicate
 * @param exclude     if nonzero, do not send to this player
 * @param packet_type the packet type, if empty the root node name is used
 */
void send_to_many(simple_wml::document& data,
				  const connection_vector& vec,
				  boost::function<bool (network::connection)> except_pred,
				  const network::connection exclude = 0,
				  std::string packet_type = "");
} //end namespace wesnothd

#endif
