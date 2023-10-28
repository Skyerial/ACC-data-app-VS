//////////////////////////////////////////////////////////////////////////////
// Author: Daniel Oppenhuizen
// License: None
// 
// Implementation of BroadcastingClient class
//////////////////////////////////////////////////////////////////////////////

#include "socket/BroadcastingProtocol.h"

#include <sstream>

void BroadcastingProtocol::construct_conn_msg(const ConnectionMessage& msg, std::string& msg_string)
{
	std::stringstream ss;

	ss << static_cast<unsigned char>(msg.register_command);
	ss << static_cast<unsigned char>(msg.protocol);
	ss << static_cast<unsigned char>(msg.display_name_length);
	ss << static_cast<unsigned char>(0);
	ss << msg.display_name;
	ss << static_cast<unsigned char>(msg.connection_password_length);
	ss << static_cast<unsigned char>(0);
	ss << msg.connection_password;
	ss << static_cast<unsigned char>(msg.real_time_update);
	ss << static_cast<unsigned char>(msg.cmd_password_length);
	ss << static_cast<unsigned char>(0);
	ss << msg.cmd_password;

	msg_string = ss.str();
}

void BroadcastingProtocol::process_incoming_msg(char(&message)[1024])
{
	// First find out what kind of message we are dealing with
	// This is shown by the first byte of the message
	auto message_type = static_cast<InboundMessageTypes>(message[0]);
	switch (message_type)
	{
	case REGISTRATION_RESULT:
		break;
	default:
		break;
	}
}