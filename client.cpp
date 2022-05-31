#include <iostream>
#include "./hexapawn.h"

// To include template functions
#include "./hptp_tf.cpp"

using namespace std;

void Client::connect( const char* ip, int port )
{
	this -> socket -> start_client( ip, port );
}

Client::Client( const char* ip, int port )
{
	// Instantiate the hsocket for comunication
	this -> socket = new hsocket;

	// Connect to the server
	this -> connect( ip, port );

	// Instantiate the game board
	this -> board = new Board;
}

Client::~Client()
{
	// Free the heap
	delete this -> socket;
	delete this -> board;
}

void Client::recive_board()
{
	*( this -> board ) = this -> socket -> recive_packet<Board>();
}

void Client::init_game( char color )
{
	this -> color = color;
	recive_board();
}

void Client::print_board()
{
	for ( int i = 0; i < this -> board -> rows; i++ )
	{
		for ( int j = 0; j < this -> board -> columns; j++ )
			cout << this -> board -> board[ i * this -> board -> rows + j ] << " ";
		printf( "\n" );
	}
}

bool Client::send_move( Move move )
{
	move.color = this -> color;
	// Send the move to the server
	this -> socket -> send_packet<Move>( move );

	// Return responte of move validity from the server
	return ( this -> socket -> recive_packet<bool>() );
}

int Client::get_game_status()
{
	return ( this -> socket -> recive_packet<int>() );
}

int main( int argc, char *argv[] )
{
	// Is the game on
	int game_status = 0;
	Client client( "127.0.0.1", 80 );
	client.init_game( 'w' );

	// Repeat until someone wins
	while ( game_status == 0 )
	{
		// Prints the board
		client.print_board();

		// Variable to store move
		Move move;

		// Get a move from the user
		do
		{
			// Get input from user for move
			printf( "Move Initial X:\n" );
			cin >> move.start.x;
			printf( "Move Initial Y:\n" );
			cin >> move.start.y;
			printf( "Move Final X:\n" );
			cin >> move.end.x;
			// No need to get end y becouse it is start y + 1
			move.end.y = move.start.y + 1;
		} while ( !client.send_move( move ) );

		// Get the status of the game
		game_status = client.get_game_status();

		// Recive the updated board
		client.recive_board();
	}
	client.print_board();

	// Print message on game status
	if ( game_status == 1 ) // Player won the game
		printf( "You Won\n" );
	else if ( game_status == 2 ) // Player lost the game
		printf( "You Lost\n" );

	return 0;
}