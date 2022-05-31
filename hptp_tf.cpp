#include "./hptp.h"

// Template function, file must be included separetly
template<class T>
T hsocket::recive_packet()
{
	// Variable to store ack
	bool ack = 0;

	// Variable to store the packet
	T packet;

	while ( !ack )
	{
		// Recive the packet
		recive_data();

		// Store a copy of the packet
		packet = *( T* )( this -> buffer );

		// Check the packet
		ack = check( sizeof( T ) );

		// Send the ack
		send_ack( ack );
	}

	// Return the packet
	return packet;
}

template<class T>
void hsocket::send_packet( T packet )
{
	// Ack message to check if the packet was recived correctly
	bool ack = 0;

	// Store the packet in the buffer
	*( T* )( this -> buffer ) = packet;

	// Make check code for the packet
	make_check( sizeof( T ) );

	while ( !ack )
	{
		// Send the packet
		send_data();

		// Wait the response
		ack = recive_ack();
	}
}