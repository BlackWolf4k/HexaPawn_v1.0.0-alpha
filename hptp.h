#pragma once

#include <sys/socket.h> // For sockets
#include <arpa/inet.h> // For sockets
#include <unistd.h> // For close

// Handles the comunications between a server and a client
class hsocket
{
	private:
		// Descriptor of socket
		int socket_descriptor;
		// Buffer for comunication
		char* buffer;
	private:
		// Connection error
		void connection_problem();
		// Start a socket
		void start_socket();
		// Bind a connection
		void bind_connection( const char* ip, int port );
		// Connect to a host
		void estabilish_connection( const char* ip, int port );
		// Start listening a socket
		void listen_connection();
		// Accept a socket
		void accept_connection();
		// Send a packet
		void send_data();
		// Recive a packet
		void recive_data();
		// Send an ack message
		void send_ack( bool value );
		// Recive the ack message
		bool recive_ack();
		// Make check code for transmission
		void make_check( int size );
		// Check a transmission
		bool check( int size );
	public:
		// Create a new hsocket
		hsocket();
		// Destruct the hsocket
		~hsocket();
		// Start the client
		// Given ip and port to connect to
		void start_client( const char* ip, int port );
		// Start the server
		void start_server( const char* ip, int port );
		// Disconnect from a socket
		void disconnect();
		// Recive a packet
		template<class T>
		T recive_packet();
		// Send a packet
		template<class T>
		void send_packet( T packet );
};