#pragma once
#include <string>

struct ConnectionMessage
{
	int register_command;
	int protocol;
	int display_name_length;
	std::string display_name;
	int connection_password_length;
	std::string connection_password;
	int real_time_update;
	int cmd_password_length;
	std::string cmd_password;

	// constructor
	ConnectionMessage()
	{
		register_command = 1;
		protocol = 4;
		display_name = "";
		display_name_length = static_cast<int>(size(display_name));
		connection_password = "asd";
		connection_password_length = static_cast<int>(size(connection_password));
		real_time_update = 250;
		cmd_password = "";
		cmd_password_length = static_cast<int>(size(cmd_password));
	}
};

enum InboundMessageTypes
{
	REGISTRATION_RESULT = 1,
	REALTIME_UPDATE = 2,
	REALTIME_CAR_UPDATE = 3,
	ENTRY_LIST = 4,
	ENTRY_LIST_CAR = 6,
	TRACK_DATA = 5,
	BROADCASTING_EVENT = 7
};

class BroadcastingProtocol
{
public:
	void construct_conn_msg(const ConnectionMessage& msg, std::string& msg_string);
	void process_incoming_msg(char(&message)[1024]);
private:

};
