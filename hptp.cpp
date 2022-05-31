#include "./hptp.h"
#include <iostream> // for perror and exit

#define BUFFER_SIZE 1024

// Clear a memory segment of specified size
void bzero( void* buffer, int size )
{
	for ( int i = 0; i < size; i++ )
		( ( char* )buffer )[i] = 0;
}

/* PRIVATE FUNCTIONS */
void hsocket::connection_problem()
{
	perror( "An error has occurred" ); // Print the occurred error
	exit( 0 ); // Exit
}

void hsocket::start_socket()
{
	/* Needed Socket variables */
	this -> socket_descriptor = 0; // Socket Descriptor

	/* Start a socket */
	this -> socket_descriptor = socket( AF_INET, SOCK_STREAM, 0 );
	// Check if socket started correctly
	if ( this -> socket_descriptor < 0 ) // Error in starting socket
		connection_problem();
}

void hsocket::bind_connection( const char* ip, int port )
{
	int is_connection_estabilished = 0; // Is connection estabilished
	struct sockaddr_in host_address; // Server informations for connection

	/* Initialize the connection to host */
	host_address.sin_family = AF_INET;
	host_address.sin_port = port;
	host_address.sin_addr.s_addr = inet_addr( ip );

	/* Bind the connection to the host */
	is_connection_estabilished = bind( this -> socket_descriptor, ( struct sockaddr* )&host_address, sizeof( host_address ) );
	// Check if binding ended correctly
	if( is_connection_estabilished < 0 ) // Error in binding
		connection_problem(); // Exit the program
}

void hsocket::estabilish_connection( const char* ip, int port )
{
	int is_connection_estabilished = 0; // Is connection estabilished
	struct sockaddr_in host_address; // Server informations for connection

	/* Initialize the connection to host */
	host_address.sin_family = AF_INET;
	host_address.sin_port = port;
	host_address.sin_addr.s_addr = inet_addr( ip );

    is_connection_estabilished = connect( this -> socket_descriptor, ( struct sockaddr* )&host_address, sizeof( host_address ) );
	if( is_connection_estabilished < 0 ) // Error in listening
		connection_problem(); // Exit the program
}

void hsocket::listen_connection()
{
	/* Needed variables for listening */
	int is_connection_estabilished;

	/* Start the listening */
	is_connection_estabilished = listen( this -> socket_descriptor, 1 );
	// Check if listening starting ended correctly
	if( is_connection_estabilished < 0 ) // Error in listening
		connection_problem(); // Exit the program
}

void hsocket::accept_connection()
{
	// Needed variables
	struct sockaddr_in address;
	socklen_t address_size = sizeof( address );

	// Accept the connection
	this -> socket_descriptor = accept( this -> socket_descriptor, ( struct sockaddr* )&address, &address_size );
}

void hsocket::send_data()
{
	// Store result of operation
	int good = 0;

	// Send datas
	good = send( this -> socket_descriptor, this -> buffer, BUFFER_SIZE, 0 );

	// Check if send was sucessfull
	if ( good < 0 )
		connection_problem();

	// Clear the buffer
	bzero( this -> buffer, BUFFER_SIZE );
}

void hsocket::recive_data()
{
	// Clear the buffer
	bzero( this -> buffer, BUFFER_SIZE );

	// Store result of operation
	int good = 0;

	// Recive datas
	good = recv( this -> socket_descriptor, this -> buffer, BUFFER_SIZE, 0 );

	// Check if recivement was sucessfull
	if ( good < 0 )
		connection_problem();
}

void hsocket::send_ack( bool value )
{
	// Store in buffer the ack value
	this -> buffer[0] = value;

	// Send the ack
	send_data();
}

bool hsocket::recive_ack()
{
	// Recive the ack
	recive_data();

	// Return the recive ack value
	return ( ( bool )this -> buffer[0] );
}

void hsocket::make_check( int size )
{
	// Check code
	int check_code = 0;

	// Make check code
	for ( int i = 0; i < size; i++ )
		check_code += this -> buffer[i] & 1;
	
	// Append the check code to the buffer
	*( int* )( this -> buffer + size ) = check_code;
}

bool hsocket::check( int size )
{
	// Check code
	int check_code = *( int* )( this -> buffer + size );

	// Check if the trasmission was correct
	for ( int i = 0; i < size; i++ )
		check_code -= this -> buffer[i] & 1;
	
	return ( check_code == 0 );
}

/* PUBLIC FUNCTIONS */
hsocket::hsocket()
{
	// Alloccate a buffer for comunication
	buffer = new char[BUFFER_SIZE];
}

hsocket::~hsocket()
{
	disconnect();
	// Free the comunication buffer
	delete[] buffer;
}

void hsocket::start_client( const char* ip, int port )
{
	start_socket();
	estabilish_connection( ip, port );
}

void hsocket::start_server( const char* ip, int port )
{
	start_socket();
	bind_connection( ip, port );
	listen_connection();
	accept_connection();
}

void hsocket::disconnect()
{
	close( this -> socket_descriptor );
}