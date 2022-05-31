#include <iostream>
#include <cstring> // for strcpy
#include "./hexapawn.h"
#include <stdlib.h> // for random
using namespace std;

// To include template functions
#include "./hptp_tf.cpp"

// Struct to store informations about pawns colors
struct
{
	char blank;
	char server;
	char client;
} colors_t;

bool Server::check_move( Move move )
{
	// Set the move color
	char color = ( move.color == colors_t.server ) ? colors_t.server : colors_t.client;
	// Set the enemy color
	char opposite_color = ( move.color == colors_t.server ) ? colors_t.client : colors_t.server;

	// Check if move is legal
	if ( ( ( move.end.y >= 0 && move.end.y < this -> board -> rows ) && ( move.start.y >= 0 && move.start.y < this -> board -> rows ) ) && 
		( ( move.end.x >= 0 && move.end.x < this -> board -> columns ) && ( move.start.x >= 0 && move.start.x < this -> board -> columns ) ) )
	{
		if ( ( move.end.y >= 0 && move.end.y < this -> board -> rows ) && ( move.end.y == move.start.y + 1 || move.end.y == move.start.y - 1 ) )
		{
			if ( ( this -> board -> board )[ move.start.y * this -> board -> columns + move.start.x ] == color )
			{
				if ( ( move.start.x == move.end.x + 1 || move.start.x == move.end.x - 1 ) && 
					( this -> board -> board )[ move.end.y * this -> board -> columns + move.end.x ] == opposite_color )
					return 1;
				else if ( ( move.start.x == move.end.x ) && 
					( this -> board -> board )[ move.end.y * this -> board -> columns + move.end.x ] == colors_t.blank )
					return 1;
				else // Illegal move
					return 0;
			}
			else // Illegal move
				return 0;
		}
		else // Illegal move
			return 0;
	}
	else // Illegal move
		return 0;
}

void Server::make_move( Move move )
{
	( this -> board -> board )[ move.start.y * this -> board -> columns + move.start.x ] = colors_t.blank;
	( this -> board -> board )[ move.end.y * this -> board -> columns + move.end.x ] = move.color;
}

Server::Server( const char* ip, int port )
{
	// Instantiate the hsocket for comunication
	this -> socket = new hsocket;
	this -> socket -> start_server( ip, port );

	// Instantiate the game board
	this -> board = new Board;

	// Server starts as black
	colors_t.blank = ' ';
	colors_t.server = 'b';
	colors_t.client = 'w';
}

Server::~Server()
{
	// Free the heap
	delete this -> socket;
	delete this -> board;
}

int Server::check_game()
{
	// Counter to count how many pawns are left
	int pawns_left = 0;
	// Counter to count how many pawns are stuck
	int pawns_stuck = 0;

	// Check all servers pawns
	for ( int i = 0; i < this -> board -> rows; i++ )
		for ( int j = 0; j < this -> board -> columns; j++ )
			if ( this -> board -> board[ i * this -> board -> columns + j ] == colors_t.server )
			{
				pawns_left += 1;
				if ( this -> board -> board[ i * this -> board -> columns + j ] == 0 )
					pawns_stuck += 1;
				if ( this -> board -> board[ ( i - 1 ) * this -> board -> columns + j ] != colors_t.blank )
				{
					if ( j >= 0 && j < this -> board -> columns - 2 )
					{
						if ( this -> board -> board[ ( i - 1 ) * this -> board -> columns + j + 1 ] != colors_t.client )
							pawns_stuck += 1;
					}
					else if ( j <= this -> board -> columns - 1 )
					{
						if ( this -> board -> board[ ( i - 1 ) * this -> board -> columns + j - 1 ] != colors_t.client )
							pawns_stuck += 1;
					}
				}
			}

	// If all server pawns are stuck send player message that he won
	if ( pawns_left == pawns_stuck )
		return 1;

	// Reset the variables
	pawns_left = 0;
	pawns_stuck = 0;

	// Check all clients pawns
	for ( int i = 0; i < this -> board -> rows; i++ )
		for ( int j = 0; j < this -> board -> columns; j++ )
			if ( this -> board -> board[ i * this -> board -> columns + j ] == colors_t.client )
			{
				pawns_left += 1;
				if ( this -> board -> board[ i * this -> board -> columns + j ] == 0 )
					pawns_stuck += 1;
				if ( this -> board -> board[ ( i + 1 ) * this -> board -> columns + j ] != colors_t.blank )
				{
					if ( j >= 0 && j < this -> board -> columns - 2 )
					{
						if ( this -> board -> board[ ( i + 1 ) * this -> board -> columns + j + 1 ] != colors_t.server )
							pawns_stuck += 1;
					}
					else if ( j <= this -> board -> columns - 1 )
					{
						if ( this -> board -> board[ ( i + 1 ) * this -> board -> columns + j - 1 ] != colors_t.server )
							pawns_stuck += 1;
					}
				}
			}

	// If all client pawns are stuck send player message that he lost
	if ( pawns_left == pawns_stuck )
		return 2;
	
	// Nobody won
	return 0;
}

void Server::send_board()
{
	this -> socket -> send_packet<Board>( *( this -> board ) );
}

void Server::init_game()
{
	// Set the board values
	this -> board -> rows = 3;
	this -> board -> columns = 3;
	char board_str[9] = { 'w', 'w', 'w', ' ', ' ', ' ', 'b', 'b', 'b' };
	strcpy( this -> board -> board, board_str );

	send_board();
}

void Server::print_board()
{
	for ( int i = 0; i < this -> board -> rows; i++ )
	{
		for ( int j = 0; j < this -> board -> columns; j++ )
			cout << this -> board -> board[ i * this -> board -> rows + j ] << " ";
		printf( "\n" );
	}
}

void Server::recive_move()
{
	// Variable to store move
	Move move;
	
	// Recive moves from client untill move is correct
	int counter = 0;
	do
	{
		// Send message that move was incorrect
		if ( counter > 0 )
			this -> socket -> send_packet<bool>( 0 );
		// Recive the move
		move = this -> socket -> recive_packet<Move>();
		counter++;
	} while( !check_move( move ) );

	// Make the move on the board
	make_move( move );

	print_board(); // to remove

	// Comunicate to the client that the move was good
	this -> socket -> send_packet<bool>( 1 );
}

void Server::decide_move()
{
	// Variable to store server move
	Move move;

	// Find a suitable move
	do
	{
		srand( time( NULL ) );
		// Generate a random position on board
		int position = 0;
		do
		{
			// position = random() % this -> board -> rows * this -> board -> columns;
			move.start.x = random() % this -> board -> columns;
			move.start.y = random() % this -> board -> rows;
		} while ( this -> board -> board[ move.start.y * this -> board -> columns + move.start.x ] != colors_t.server ); // Check if random position has a server pedon
	
		// Change the position into move coordinates
		// move.start.x = position % this -> board -> columns;
		// move.start.y = position / this -> board -> columns;
		move.color = colors_t.server;
		move.end.y = move.start.y - 1;

		// Check possible moves
		if ( move.end.y >= 0 ) // pawn is not at last row
		{
			if ( move.start.x == 0 ) // pawn is at most left column
			{
				move.end.x = move.start.x + random() % 2;
			}
			else if ( move.start.x == this -> board -> columns - 1 ) // pawn is at most right column
			{
				move.end.x = move.start.x - random() % 2;
			}
			else // pawn in every column else
			{
				move.end.x = move.start.x + ( random() % 3 ) - 1;
			}
		}
		// cout << move.start.x << move.start.y << move.end.x << move.end.y << endl;
	} while ( !check_move( move ) );
	
	// Do the move
	make_move( move );
}

void Server::send_game_status()
{
	this -> socket -> send_packet<int>( check_game() );
}

int main()
{
	// Create the server
	Server server( "127.0.0.1", 80 );

	// Initialize the informations about the game
	server.init_game();

	// Keep track of the state of the game
	int game_status = 0;

	// Play the game
	while( game_status == 0 )
	{
		// Recive move from client
		server.recive_move();

		// Check if the game is still on
		game_status = server.check_game();

		// Make move only if game isn't ended yet
		if ( game_status == 0 )
			// Make a move
			server.decide_move();

			// Send the status of the game to the player
			server.send_game_status(); // connection gets stuck

			// Resend the board
			server.send_board();
	}
}