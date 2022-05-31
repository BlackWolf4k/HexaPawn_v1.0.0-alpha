#include "./hptp.h"

// Contains the board and relative informations
struct Board
{
	int rows;
	int columns;
	char board[9];
};

// Conains the coordinates of a point in a 2-dimensional system
struct Coordinates
{
	int x;
	int y;
};

// Contains the coordinates of the beginning and end of a move
struct Move
{
	Coordinates start;
	Coordinates end;
	char color;
};

class Server
{
	private:
		// Socket for comunications
		hsocket* socket;
		// Actual board
		Board* board;
	private:
		// Check if a move from is possible to do
		bool check_move( Move move );
		// Make a move on the board
		void make_move( Move move );
	public:
		Server( const char* ip, int port );
		~Server();
		// Check if the game is in stall or someone won
		int check_game();
		// Send the board
		void send_board();
		// Initialize the game
		void init_game();
		// Print the board
		void print_board();
		// Recive the move from the client
		// Doesn't exit untill the move send is correct
		void recive_move();
		// Decide move to do for server
		void decide_move();
		// Send to the client the status of the game ( 0 = going, 1 = lost, 2 = win )
		void send_game_status();
};

class Client
{
	private:
		// Socket for comunications
		hsocket* socket;
		// Actual board
		Board* board;
		// Color of the client
		char color;
	private:
		// Connects to the server
		void connect( const char* ip, int port );
	public:
		Client( const char* ip, int port );
		~Client();
		// Recive the board from the server
		void recive_board();
		// Initialize the game
		void init_game( char color );
		// Print the board to the screen
		void print_board();
		// Send the move to the server
		// Wait from server response if move was valid
		bool send_move( Move move );
		// Get the status of the game ( 0 = going, 1 = win, 2 = lost )
		int get_game_status();
};