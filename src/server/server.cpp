#include "../global.hpp"

#include "../config.hpp"
#include "../game_config.hpp"
#include "../network.hpp"
#include "../util.hpp"

#include "SDL.h"

#include "game.hpp"
#include "input_stream.hpp"
#include "metrics.hpp"
#include "player.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <ctime>
#include <vector>

#include <signal.h>

namespace {

config construct_error(const std::string& msg)
{
	config cfg;
	cfg.add_child("error")["message"] = msg;
	return cfg;
}

config construct_server_message(const std::string& message, const game& g)
{
	config turn;
	if(g.started()) {
		config& cmd = turn.add_child("turn");
		config& cfg = cmd.add_child("command");
		config& msg = cfg.add_child("speak");
		msg["description"] = "server";
		msg["message"] = message;
	} else {
		config& msg = turn.add_child("message");
		msg["sender"] = "server";
		msg["message"] = message; 
	}

	return turn;
}

void truncate_message(std::string& str)
{
	const size_t max_message_length = 240;
	str.resize(minimum<size_t>(str.size(),max_message_length));
}

}

class server
{
public:
	server(int port, input_stream& input);
	void run();
private:
	void process_command(const std::string& cmd);

	void delete_game(std::vector<game>::iterator i);

	void dump_stats();

	const network::manager net_manager_;
	const network::server_manager server_;

	config login_response_;

	config initial_response_;
	config old_initial_response_;

	config sync_initial_response();

	player_map players_;

	game not_logged_in_;
	game lobby_players_;
	std::vector<game> games_;

	time_t last_stats_;

	input_stream& input_;

	metrics metrics_;

	bool is_ip_banned(const std::string& ip);
	std::string ban_ip(const std::string& mask);
	std::vector<std::string> bans_;
};

server::server(int port, input_stream& input) : net_manager_(5), server_(port), not_logged_in_(players_), lobby_players_(players_), last_stats_(time(NULL)), input_(input)
{
	login_response_.add_child("mustlogin");
	login_response_["version"] = game_config::version;
}

bool server::is_ip_banned(const std::string& ip)
{
	for(std::vector<std::string>::const_iterator i = bans_.begin(); i != bans_.end(); ++i) {
		std::cerr << "comparing for ban '" << *i << "' vs '" << ip << "'\n";
		const std::string::const_iterator itor = std::mismatch(i->begin(),i->end(),ip.c_str()).first;
		if(itor == i->end() && i->size() == ip.size() || itor != i->end() && *itor == '*') {
			std::cerr << "is banned\n";
			return true;
		}

		std::cerr << "not banned\n";
	}

	return false;
}

std::string server::ban_ip(const std::string& mask)
{
	const std::string::const_iterator asterisk = std::find(mask.begin(),mask.end(),'*');
	if(asterisk != mask.end() && asterisk != mask.end()-1) {
		return "'*' may only appear at the end of the mask";
	}

	bans_.push_back(mask);

	return "";
}

config server::sync_initial_response()
{
	config res;
	res.add_child("gamelist_diff",initial_response_.get_diff(old_initial_response_));
	old_initial_response_ = initial_response_;
	return res;
}

void server::dump_stats()
{
	config parent;
	config& stats = parent.add_child("statistics");

	stats["num_players"] = str_cast(players_.size());
	stats["lobby_players"] = str_cast(lobby_players_.nplayers());
	stats["num_games"] = str_cast(games_.size());
	stats["start_interval"] = str_cast(last_stats_);
	last_stats_ = time(NULL);
	stats["end_interval"] = str_cast(last_stats_);

	//write and flush the output stream
	std::cout << parent.write() << std::endl;
}

void server::process_command(const std::string& cmd)
{
	const std::string::const_iterator i = std::find(cmd.begin(),cmd.end(),' ');
	const std::string command(cmd.begin(),i);
	if(command == "msg") {
		if(i == cmd.end()) {
			std::cout << "you must type a message" << std::endl;
			return;
		}

		const std::string msg(i+1,cmd.end()); lobby_players_.send_data(construct_server_message(msg,lobby_players_));
		for(std::vector<game>::iterator g = games_.begin(); g != games_.end(); ++g) {
			g->send_data(construct_server_message(msg,*g));
		}

		std::cout << "message '" << msg << "' relayed to players\n";
	} else if(command == "status") {
		std::cout << "STATUS REPORT\n---\n";
		for(player_map::const_iterator i = players_.begin(); i != players_.end(); ++i) {
			const network::connection_stats& stats = network::get_connection_stats(i->first);
			const int time_connected = stats.time_connected/1000;
			const int seconds = time_connected%60;
			const int minutes = (time_connected/60)%60;
			const int hours = time_connected/(60*60);
			std::cout << "'" << i->second.name() << "' @ " << network::ip_address(i->first) << " connected for " << hours << ":" << minutes << ":" << seconds << " sent " << stats.bytes_sent << " bytes, received " << stats.bytes_received << " bytes\n";
		}

		std::cout << "---" << std::endl;
	} else if(command == "metrics") {
		std::cout << metrics_ << std::endl;
	} else if(command == "ban") {

		if(i == cmd.end()) {
			std::cout << "BAN LIST\n---\n";
			for(std::vector<std::string>::const_iterator i = bans_.begin(); i != bans_.end(); ++i) {
				std::cout << *i << "\n";
			}
			std::cout << "---" << std::endl;
		} else {
			const std::string mask(i+1,cmd.end());
			const std::string& diagnostic = ban_ip(mask);
			if(diagnostic != "") {
				std::cout << "Could not ban '" << mask << "': " << diagnostic << std::endl;
			} else {
				std::cout << "Set ban on '" << mask << "'\n";
			}
		}
	} else if(command == "unban") {
		if(i == cmd.end()) {
			std::cout << "You must enter a mask to unban" << std::endl;
			return;
		}

		const std::string mask(i+1,cmd.end());

		const std::vector<std::string>::iterator itor = std::remove(bans_.begin(),bans_.end(),mask);
		if(itor == bans_.end()) {
			std::cout << "there is no ban on '" << mask << "'" << std::endl;
		} else {
			bans_.erase(itor,bans_.end());
			std::cout << "ban removed on '" << mask << "'" << std::endl;
		}
		
	} else if(command == "kick") {
		if(i == cmd.end()) {
			std::cout << "you must enter a nick to kick\n";
			return;
		}

		const std::string nick(i+1,cmd.end());

		for(player_map::const_iterator j = players_.begin(); j != players_.end(); ++j) {
			if(j->second.name() == nick) {
				std::cout << "kicking nick '" << j->second.name() << "'\n";
				throw network::error("",j->first);
			}
		}
	} else {
		std::cout << "command '" << command << "' is not recognized" << std::endl;
	}
}

void server::run()
{
	config& gamelist = initial_response_.add_child("gamelist");
	old_initial_response_ = initial_response_;

	bool sync_scheduled = false;
	for(int loop = 0;; ++loop) {
		try {
			if(sync_scheduled) {
				//send all players the information that a player has logged
				//out of the system
				lobby_players_.send_data(sync_initial_response());
				sync_scheduled = false;
			}

			//process admin commands
			std::string admin_cmd;
			if(input_.read_line(admin_cmd)) {
				process_command(admin_cmd);
			}
			
			//make sure we log stats every 5 minutes
			if((loop%100) == 0 && last_stats_+5*60 < time(NULL)) {
				dump_stats();
			}

			network::process_send_queue();

			network::connection sock = network::accept_connection();
			if(sock && is_ip_banned(network::ip_address(sock))) {
				std::cerr << "rejected banned user '" << network::ip_address(sock) << "'\n";
				network::send_data(construct_error("You are banned."),sock);
				network::disconnect(sock);
			} else if(sock) {
				network::send_data(login_response_,sock);
				not_logged_in_.add_player(sock);
			}

			config data;
			while((sock = network::receive_data(data)) != NULL) {

				metrics_.service_request();

				//if someone who is not yet logged in is sending
				//login details
				if(not_logged_in_.is_member(sock)) {
					const config* const login = data.child("login");

					//client must send a login first.
					if(login == NULL) {
						network::send_data(construct_error(
						                   "You must login first"),sock);
						continue;
					}

					//check the username is valid (all alpha-numeric or space)
					std::string username = (*login)["username"];
					config::strip(username);
					const int alnum = std::count_if(username.begin(),username.end(),isalnum);
					const int spaces = std::count(username.begin(),username.end(),' ');
					if((alnum + spaces != username.size()) || spaces == username.size() || username.empty()) {
						network::send_data(construct_error(
						                   "This username is not valid"),sock);
						continue;
					}

					if(username.size() > 32) {
						network::send_data(construct_error(
						                   "This username is too long"),sock);
						continue;
					}

					if(username == "server") {
						network::send_data(construct_error(
						                   "The nick 'server' is reserved and can not be used by players"),sock);
						continue;
					}

					//check the username isn't already taken
					player_map::const_iterator p;
					for(p = players_.begin(); p != players_.end(); ++p) {
						if(p->second.name() == username) {
							break;
						}
					}

					if(p != players_.end()) {
						network::send_data(construct_error(
						              "This username is already taken"),sock);
						continue;
					}
					
					config* const player_cfg = &initial_response_.add_child("user");

					const player new_player(username,*player_cfg);

					players_.insert(std::pair<network::connection,player>(sock,new_player));

					//remove player from the not-logged-in list and place
					//the player in the lobby
					not_logged_in_.remove_player(sock);
					lobby_players_.add_player(sock);

					//send the new player the entire list of games and players
					network::send_data(initial_response_,sock);

					//send other players in the lobby the update that the player has joined
					lobby_players_.send_data(sync_initial_response(),sock);

					std::cerr << "'" << username << "' (" << network::ip_address(sock) << ") has logged on\n";

					for(std::vector<game>::iterator g = games_.begin(); g != games_.end(); ++g) {
						g->send_data_observers(construct_server_message(username + " has logged into the lobby",*g));
					}

				} else if(const config* query = data.child("query")) {

					//process queries from clients in here
					std::ostringstream response;
					if((*query)["type"] == "metrics") {
						//a query for server data from a player
						response << metrics_;	
					} else {
						response << "Error: unrecognized query";
					}
					
					network::send_data(construct_server_message(response.str(),lobby_players_),sock);
				} else if(lobby_players_.is_member(sock)) {
					const config* const create_game = data.child("create_game");
					if(create_game != NULL) {

						std::cerr << "creating game...\n";

						//create the new game, remove the player from the
						//lobby and put him/her in the game they have created
						games_.push_back(game(players_));
						lobby_players_.remove_player(sock);
						games_.back().add_player(sock);

						//store the game data here at the moment
						games_.back().level() = *create_game;
						std::stringstream converter;
						converter << games_.back().id();
						games_.back().level()["id"] = converter.str();

						//mark the player as unavailable in the lobby
						const player_map::iterator pl = players_.find(sock);
						if(pl != players_.end()) {
							pl->second.mark_available(false);

							lobby_players_.send_data(sync_initial_response());
						} else {
							std::cerr << "ERROR: Could not find player in map\n";
						}

						continue;
					}

					//see if the player is joining a game
					const config* const join = data.child("join");
					if(join != NULL) {
						const std::string& id = (*join)["id"];
						const int nid = atoi(id.c_str());
						const std::vector<game>::iterator it =
						             std::find_if(games_.begin(),games_.end(),
						                          game_id_matches(nid));
						if(it == games_.end()) {
							//send a response saying the game has been cancelled
							config cfg;
							cfg.add_child("leave_game");
							network::send_data(cfg,sock);

							std::cerr << "attempt to join unknown game\n";
							continue;
						}

						if(it->player_is_banned(sock)) {
							network::send_data(construct_error("You are banned from this game"),sock);
							continue;
						}

						lobby_players_.remove_player(sock);

						//send them the game data
						network::send_data(it->level(),sock);

						it->add_player(sock);

						//mark the player as unavailable in the lobby
						const player_map::iterator pl = players_.find(sock);
						if(pl != players_.end()) {
							pl->second.mark_available(false);

							lobby_players_.send_data(sync_initial_response());
						} else {
							std::cerr << "ERROR: Could not find player in map\n";
						}
					}

					//see if it's a message, in which case we add the name
					//of the sender, and forward it to all players in the lobby
					config* const message = data.child("message");
					if(message != NULL) {
						const player_map::const_iterator p = players_.find(sock);
						assert(p != players_.end());
						(*message)["sender"] = p->second.name();

						truncate_message((*message)["message"]);

						lobby_players_.send_data(data,sock);
					}
				} else {
					std::vector<game>::iterator g;
					for(g = games_.begin(); g != games_.end(); ++g) {
						if(g->is_member(sock))
							break;
					}

					if(g == games_.end()) {
						std::cerr << "ERROR: unknown socket " << games_.size() << "\n";
						continue;
					}

					//if info is being provided about the game state
					if(data.child("info") != NULL) {
						const config& info = *data.child("info");
						if(info["type"] == "termination") {
							g->set_termination_reason(info["condition"]);
						}
					}

					//if the owner is changing the controller for a side
					if(g->is_owner(sock) && data.child("change_controller") != NULL) {
						const config& change = *data.child("change_controller");
						const std::string& result = g->transfer_side_control(change);
						if(result == "") {
							const config& msg = construct_server_message(change["player"] + " takes control of side " + change["side"],*g);
							g->send_data(msg);
						} else {
							const config& msg = construct_server_message(result,*g);
							network::send_data(msg,sock);
						}

						continue;
					}

					//if the owner is banning someone from the game
					if(g->is_owner(sock) && data.child("ban") != NULL) {
						const config& ban = *data.child("ban");
						const std::string& name = ban["username"];
						player_map::iterator pl;
						for(pl = players_.begin(); pl != players_.end(); ++pl) {
							if(pl->second.name() == name) {
								break;
							}
						}

						if(pl->first != sock && pl != players_.end()) {
							g->ban_player(pl->first);
							const config& msg = construct_server_message("You have been banned",*g);
							network::send_data(msg,pl->first);
							config leave_game;
							leave_game.add_child("leave_game");
							network::send_data(leave_game,pl->first);

							g->describe_slots();
							lobby_players_.add_player(pl->first);

							//mark the player as available in the lobby
							pl->second.mark_available(true);

							//send the player who was banned the lobby game list
							network::send_data(initial_response_,pl->first);

							//send all other players in the lobby the update to the lobby
							lobby_players_.send_data(sync_initial_response(),sock);
						} else if(pl == players_.end()) {
							const config& response = construct_server_message(
							             "Ban failed: user '" + name + "' not found",*g);
							network::send_data(response,sock);
						}
					} else if(data.child("ban")) {
						const config& response = construct_server_message(
						             "You cannot ban: not the game creator",*g);
						network::send_data(response,sock);
					}

					//if this is data describing changes to a game.
					else if(g->is_owner(sock) && data.child("scenario_diff")) {
						g->level().apply_diff(*data.child("scenario_diff"));
						g->send_data(data,sock);

						const bool lobby_changes = g->describe_slots();
						if(lobby_changes) {
							lobby_players_.send_data(sync_initial_response());
						}
					}

					//if this is data describing the level for a game.
					else if(g->is_owner(sock) && data.child("side") != NULL) {

						const bool is_init = g->level_init();

						//if this game is having its level data initialized
						//for the first time, and is ready for players to join.
						//We should currently have a summary of the game in g->level().
						//we want to move this summary to the initial_response_, and
						//place a pointer to that summary in the game's description.
						//g->level() should then receive the full data for the game.
						if(!is_init) {

							//if there is no shroud, then tell players in the lobby what the map looks like
							if((*data.child("side"))["shroud"] != "yes") {
								g->level().values["map_data"] = data["map_data"];
								g->level().values["map"] = data["map"];
							}
							
							//update our config object which describes the
							//open games, and notifies the game of where its description
							//is located at
							config& desc = gamelist.add_child("game",g->level());
							g->set_description(&desc);

							//record the full description of the scenario in g->level()
							g->level() = data;
							g->describe_slots();

							//send all players in the lobby the update to the list of games
							lobby_players_.send_data(sync_initial_response());
						} else {

							//we've already initialized this scenario, but clobber its old
							//contents with the new ones given here
							g->level() = data;
						}

						//send all players in the level, except the sender, the new data
						g->send_data(data,sock);
						continue;
					}

					const string_map::const_iterator side = data.values.find("side");
					if(side != data.values.end()) {
						const bool res = g->take_side(sock,data);
						config response;
						if(res) {
							std::cerr << "played joined side\n";
							response["side_secured"] = side->second;

							//update the number of available slots
							const bool res = g->describe_slots();
							if(res) {
								lobby_players_.send_data(sync_initial_response());
							}
						} else {
							response["failed"] = "yes";
						}

						network::send_data(response,sock);
						continue;
					}

					if(data.child("start_game")) {
						//send notification of the game starting immediately.
						//g->start_game() will send data that assumes the [start_game]
						//message has been sent
						g->send_data(data,sock);

						g->start_game();
						lobby_players_.send_data(sync_initial_response());
						continue;
					} else if(data.child("leave_game")) {
						const bool needed = g->is_needed(sock);

						if(needed) {

							//tell all other players the game is over,
							//because a needed player has left
							config cfg;
							cfg.add_child("leave_game");
							g->send_data(cfg);

							//delete the game's description
							config* const gamelist = initial_response_.child("gamelist");
							assert(gamelist != NULL);
							const config::child_itors vg = gamelist->child_range("game");

							const config::child_iterator desc = std::find(vg.first,vg.second,g->description());
							if(desc != vg.second) {
								gamelist->remove_child("game",desc - vg.first);
							}

							//update the state of the lobby to players in it.
							//We have to sync the state of the lobby so we can
							//send it to the players leaving the game
							lobby_players_.send_data(sync_initial_response());

							//set the availability status for all quitting players
							for(player_map::iterator pl = players_.begin(); pl != players_.end(); ++pl) {
								if(g->is_member(pl->first)) {
									pl->second.mark_available(true);
								}
							}

							//put the players back in the lobby and send
							//them the game list and user list again
							g->send_data(initial_response_);
							metrics_.game_terminated(g->termination_reason());
							lobby_players_.add_players(*g);
							games_.erase(g);

							//now sync players in the lobby again, to remove the game
							lobby_players_.send_data(sync_initial_response());
						} else {

							g->remove_player(sock);
							g->describe_slots();
							lobby_players_.add_player(sock);

							//mark the player as available in the lobby
							const player_map::iterator pl = players_.find(sock);
							if(pl != players_.end()) {
								pl->second.mark_available(true);
							} else {
								std::cerr << "ERROR: Could not find player in map\n";
							}

							//send the player who has quit the game list
							network::send_data(initial_response_,sock);

							//send all other players in the lobby the update to the lobby
							lobby_players_.send_data(sync_initial_response(),sock);
						}

						continue;
					} else if(data["side_secured"].empty() == false) {
						continue;
					} else if(data["failed"].empty() == false) {
						std::cerr << "ERROR: failure to get side\n";
						continue;
					}

					config* const turn = data.child("turn");
					if(turn != NULL) {
						g->filter_commands(sock,*turn);

						//notify the game of the commands, and if it changes
						//the description, then sync the new description
						//to players in the lobby
						const bool res = g->process_commands(*turn);
						if(res) {
							lobby_players_.send_data(sync_initial_response());
						}

						//any private 'speak' commands must be repackaged seperate
						//to other commands, and re-sent, since they should only go
						//to some clients.
						const config::child_itors speaks = turn->child_range("command");
						int npublic = 0, nprivate = 0, nother = 0;
						std::string team_name;
						for(config::child_iterator i = speaks.first; i != speaks.second; ++i) {
							config* const speak = (*i)->child("speak");
							if(speak == NULL) {
								++nother;
								continue;
							}

							truncate_message((*speak)["message"]);

							//force the description to be correct to prevent
							//spoofing of messages
							const player_map::const_iterator pl = players_.find(sock);
							assert(pl != players_.end());
							(*speak)["description"] = pl->second.name();

							if((*speak)["team_name"] == "") {
								++npublic;
							} else {
								++nprivate;
								team_name = (*speak)["team_name"];
							}
						}

						//if all there are are messages and they're all private, then
						//just forward them on to the client that should receive them.
						if(nprivate > 0 && npublic == 0 && nother == 0) {
							g->send_data_team(data,team_name,sock);
							continue;
						}

						//at the moment, if private messages are mixed in with other
						//data, then let them go through. It's exceedingly unlikely that
						//this will happen anyway, and if it does, the client should
						//respect not displaying the message.
					}

					//forward data to all players who are in the game,
					//except for the original data sender
					g->send_data(data,sock);

					if(g->started()) {
						g->record_data(data);
					}
				}
			}

			metrics_.no_requests();
			
		} catch(network::error& e) {
			if(!e.socket) {
				std::cerr << "fatal network error: " << e.message << "\n";
				break;
			} else {
				std::cerr << "socket closed: " << e.message << "\n";

				const std::map<network::connection,player>::iterator pl_it = players_.find(e.socket);
				if(pl_it != players_.end()) {
					const config::child_list& users = initial_response_.get_children("user");
					const size_t index = std::find(users.begin(),users.end(),pl_it->second.config_address()) - users.begin();
					if(index < users.size())
						initial_response_.remove_child("user",index);

					players_.erase(pl_it);
				}
				
				not_logged_in_.remove_player(e.socket);
				lobby_players_.remove_player(e.socket);
				for(std::vector<game>::iterator i = games_.begin(); i != games_.end(); ++i) {
					if(i->is_needed(e.socket)) {
						delete_game(i);
						e.socket = 0;
						break;
					} else {
						i->remove_player(e.socket);
					}
				}

				if(e.socket) {
					e.disconnect();
				}

				sync_scheduled = true;

				std::cerr << "done closing socket...\n";
			}

			continue;
		} catch(config::error& e) {
			std::cerr << "error in received data: " << e.message << "\n";
			continue;
		}

		SDL_Delay(20);
	}
}

void server::delete_game(std::vector<game>::iterator i)
{
	metrics_.game_terminated(i->termination_reason());

	//delete the game's configuration
	config* const gamelist = initial_response_.child("gamelist");
	assert(gamelist != NULL);
	const config::child_itors vg = gamelist->child_range("game");
	const config::child_list::iterator g = std::find(vg.first,vg.second,i->description());
	if(g != vg.second) {
		const size_t index = g - vg.first;
		gamelist->remove_child("game",index);
	}
	
	i->disconnect();
	games_.erase(i);
}

int main(int argc, char** argv)
{
	int port = 15000;

	network::set_default_send_size(4096);

	std::string fifo_path = "/var/run/wesnothd/socket";

	for(int arg = 1; arg != argc; ++arg) {
		const std::string val(argv[arg]);
		if(val.empty()) {
			continue;
		}

		if((val == "--max_packet_size" || val == "-m") && arg+1 != argc) {
			network::set_default_send_size(size_t(atoi(argv[++arg])));
		}
		else if((val == "--port" || val == "-p") && arg+1 != argc) {
			port = atoi(argv[++arg]);
		} else if(val == "--help" || val == "-h") {
			std::cout << "usage: " << argv[0]
				<< " [options]\n"
				<< "  -p, --port     Binds the server to the specified port\n";
			return 0;
		} else if(val == "--version" || val == "-v") {
			std::cout << "Battle for Wesnoth server " << game_config::version
				<< "\n";
			return 0;
		} else if(val == "--fifo") {
			++arg;
			if(arg == argc) {
				std::cerr << "option --fifo requires a path argument\n";
				return 0;
			}

			fifo_path = argv[arg];
		} else if(val[0] == '-') {
			std::cerr << "unknown option: " << val << "\n";
			return 0;
		} else {
			port = atoi(argv[arg]);
		}
	}

	input_stream input(fifo_path);
	
	try {
		server(port,input).run();
	} catch(network::error& e) {
		std::cerr << "caught network error while server was running. aborting.: " << e.message << "\n";
		return -1;
	}

	return 0;
}
