// Aleksi Nieminen
// 5 point assignment

// Libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>
using namespace std;


// Room count
const int MAX_ROOMS = 300;
const int MIN_ROOMS = 40;

// Room prices (€/night)
const int PRICE_SINGLE = 100;
const int PRICE_DOUBLE = 150;

// Enum class for room type
enum class roomType {
	SINGLE = 1,
	DOUBLE = 2
};


// Room struct, represents each room's data
struct Room {
	int roomNumber; // Unique room index
	roomType type; // Room type enum class (Single/Double)
	bool isReserved; // if room is booked or not
	int reservationId = 0; // 0 means no reservation
	string reserverName; // Name of the person who is reserving
	int nights = 0; // Number of reserved nights, 0 if not being reserved
};


// Empty room setup function, initializes all the fields
Room setupRoom(int number, roomType type) {
	Room r;
	r.roomNumber = number;
	r.type = type;
	r.isReserved = false;
	r.reservationId = 0;
	r.reserverName = "";
	r.nights = 0;
	return r;
}


// Generates a full room list
// Total amount of rooms is even number between 40-300
// First half of rooms must be single, second half must be double
vector<Room> generateRooms(int totalRooms) 
{
	vector<Room> rooms; // vector of room struct

	int half = totalRooms / 2;

	// Generate single rooms, roomNumber from i
	for (int i = 1; i <= half; i++) 
	{
		rooms.push_back(setupRoom(i, roomType::SINGLE));
	}

	// Generate single rooms, roomNumber from i (half+)
	for (int i = half + 1; i <= totalRooms; i++) 
	{
		rooms.push_back(setupRoom(i, roomType::DOUBLE));
	}

	return rooms;
}


// Save generated rooms into a .txt file
// Returns true if successful
bool saveRooms(const vector<Room>& rooms, const string filename = "RoomData.txt") {
	ofstream file(filename);
	if (!file.is_open()) return false;

	for (const Room& r : rooms) 
	{
		file << r.roomNumber << " "
			 << (r.type == roomType::SINGLE ? 1 : 2) << " " // 0/1 to 1/2 format, better if reading directly the .txt file
			 << r.isReserved << " "
			 << r.reservationId << " "
		 	 << r.reserverName << " "
			 << r.nights << "\n";
	}

	return true;
}


// Load roos from a .txt file
// Returns true if successful
bool loadRooms(vector<Room>& rooms, const string& filename = "RoomData.txt")
{
	// return false if not open
	ifstream file(filename);
	if (!file.is_open()) return false;

	rooms.clear();
	int number, typeInt, isReservedInt, reservationId, nights; // first read as integers from the file
	string reserverName;

	while (file >> number >> typeInt >> isReservedInt >> reservationId >> ws) // read integers
	{
		getline(file, reserverName); // read reserver name and nights as a string
		
		// Extract nights from the line ^
		size_t lastSpace = reserverName.find_last_of(' ');
		if (lastSpace != string::npos)
		{
			nights = stoi(reserverName.substr(lastSpace + 1));
			reserverName = reserverName.substr(0, lastSpace);
		}
		else
		{
			nights = 0;
		}

		roomType type = (typeInt == 1) ? roomType::SINGLE : roomType::DOUBLE; // change 1/2 to enum format
		Room r;
		r.roomNumber = number;
		r.type = type;
		r.isReserved = (isReservedInt != 0); // from int to boool format
		r.reservationId = reservationId;
		r.reserverName = reserverName;
		r.nights = nights;
		rooms.push_back(r);
	}

	file.close();
	return true;
}



// Ask user for total room amount in range (40-300)
int askTotalRooms()
{
	int totalRooms;
	while (true)
	{
		cout << "Enter a even total number of rooms (40-300): ";
		if (cin >> totalRooms && totalRooms >= MIN_ROOMS && totalRooms <= MAX_ROOMS && totalRooms % 2 == 0)
		{
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear unnecessary characters
			break;
		}
		else
		{
			// Error, invalid input
			cout << "Invalid input! Enter a valid number." << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear unnecessary characters
		}
	}
	return totalRooms;
}


// Startup rooms
// Ask user to load existing .txt file or generate new
vector<Room> startupRooms()
{
	vector<Room> rooms;
	cout << "Do you want to load existing room data or reset room  data? Load [1], Reset [2]: ";
	string choice;
	getline(cin, choice);

	if (choice == "1") // Load existing file
	{
		if (loadRooms(rooms))
		{
			cout << "Rooms loaded successfully from file.\n";
			return rooms;
		}
		else
		{
			cout << "Failed to load rooms. Generating new room data.\n";
		}
	}
	else if (choice == "2") // Reset, generate new rooms
	{
		int totalRooms = askTotalRooms();
		rooms = generateRooms(totalRooms);

		if (saveRooms(rooms))
		{
			cout << "Rooms generated successfully and saved to file.\n";
		}
		else
		{
			cout << "Failed to save rooms to file!\n";
		}
	}

	return rooms;
}

// ---------------------------------------------------------------------------------------- //
// RESERVATION SECTION -- RESERVATION SECTION -- RESERVATION SECTION -- RESERVATION SECTION //
// ---------------------------------------------------------------------------------------- //


// Generate unique reservation ID between 10000-99999, returns ID
int generateReservationId(const vector<Room>& rooms)
{
	int id;
	bool unique;

	do
	{
		id = rand() % 90000 + 10000; // number between 10000-99999
		unique = true;
		for (const Room& r : rooms)
		{
			if (r.reservationId == id) // if other room has same id, id is not false and start over
			{
				unique = false;
				break;
			}
		}
	} while (!unique); // If unique return ID
	return id;
}


// Calculate price with discount (0%, 10%, 20%), returns final price
int calculatePrice(const Room& room)
{
	// Calculate base price
	int basePrice = (room.type == roomType::SINGLE ? PRICE_SINGLE : PRICE_DOUBLE) * room.nights;

	int discountOptions[3] = { 0,10,20 };
	int discountPercent = discountOptions[rand() % 3]; // Get discount rate

	// Calculate final price
	int finalPrice = basePrice * (100 - discountPercent) / 100;

	cout << "Discount of " << discountPercent << "% applied\n";
	cout << "Total price: " << finalPrice << "€\n";

	return finalPrice;
}


// Room reservation
void reserveRoom(vector<Room>& rooms)
{
	// Count available rooms
	int emptyTotal = 0;
	int emptySingles = 0;
	int emptyDoubles = 0;

	for (int i = 0; i < rooms.size(); i++)
	{
		if (!rooms[i].isReserved)
		{
			emptyTotal++;
			if (rooms[i].type == roomType::SINGLE) emptySingles++;
			else emptyDoubles++;
		}
	}

	// Print room availability
	cout << "\nRooms available: " << emptyTotal << endl;
	cout << "Single rooms available: " << emptySingles << endl;
	cout << "Double rooms available: " << emptyDoubles << endl;
	// If no rooms available return to main menu
	if (emptyTotal == 0)
	{
		cout << "No rooms available. Returning to main menu...\n";
		return;
	}

	// Option to return to main menu
	cout << "Do you want to continue reservation or return to main menu?\n";
	cout << "Reserve [1], Return [0]: ";
	int choice;
	while (!(cin >> choice) || (choice != 0 && choice != 1))
	{
		cout << "Invalid input! Enter a valid number: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	if (choice == 0) return;

	// Ask for room type
	int typeChoice;
	cout << "\nEnter the room type to reserve. Single [1]. Double [2]: ";
	while (!(cin >> typeChoice) ||
		(typeChoice != 1 && typeChoice != 2) ||
		(typeChoice == 1 && emptySingles == 0) ||
		(typeChoice == 2 && emptyDoubles == 0))
	{
		if (typeChoice == 1 && emptySingles == 0) cout << "\nNo single rooms available: ";
		else if (typeChoice == 2 && emptyDoubles == 0) cout << "\nNo double rooms available: ";
		else cout << "\nInvalid input! Enter a valid number: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	roomType chosenType = (typeChoice == 1) ? roomType::SINGLE : roomType::DOUBLE;
	
	// Ask for nuber of nights
	int nights;
	cout << "Enter number of nights: ";
	while (!(cin >> nights) || nights <= 0)
	{
		cout << "\nInvalid input! Enter a valid number of nights: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	// Ask for reservation name
	string name;
	cout << "Enter reservation name: ";
	getline(cin, name);

	// Find first available room of chosen type
	int roomIndex = -1;
	for (int i = 0; i < rooms.size(); i++)
	{
		if (!rooms[i].isReserved && rooms[i].type == chosenType)
		{
			roomIndex = i;
			break;
		}
	}

	if (roomIndex == -1)
	{
		cout << "Unexpected error: No room found. Returning to main menu...";
		return;
	}

	// Give reservation ID and update room data
	int reservationId = generateReservationId(rooms);
	rooms[roomIndex].isReserved = true;
	rooms[roomIndex].reservationId = reservationId;
	rooms[roomIndex].reserverName = name;
	rooms[roomIndex].nights = nights;

	// Show price with discount
	int finalPrice = calculatePrice(rooms[roomIndex]);

	cout << "\nRoom " << rooms[roomIndex].roomNumber << " reserved successfully.\n";
	cout << "Reservation ID: " << reservationId << endl;
	cout << "Price: " << finalPrice << "€\n";
	cout << "\nReturning to main menu...\n";

	saveRooms(rooms); // Save reservation to .txt file
}


// -------------------------------------------------------------------- //
// SEARCH SECTION -- SEARCH SECTION -- SEARCH SECTION -- SEARCH SECTION //
// -------------------------------------------------------------------- //

// Ask to search by name or booking number
void searchReservation(const vector<Room>& rooms)
{
	int choice;

	cout << "Do you want to search by name or by booking number?\n";
	cout << "Search by name [1], Search by number [2], Return [0]: ";

	while (!(cin >> choice) || (choice != 0 && choice != 1 && choice != 2))
	{
		cout << "Invalid input! Enter a valid number: ";
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	if (choice == 0)
	{
		cout << "Returning to main menu...\n";
		return;
	}

	// SEARCH BY NAME
	if (choice == 1)
	{
		string name;
		cout << "Enter the reservation name: ";
		getline(cin, name);

		bool found = false;

		for (int i = 0; i < rooms.size(); i++)
		{
			if (rooms[i].isReserved && rooms[i].reserverName == name)
			{
				// If found
				found = true;
				cout << "\nReservation found.\n";
				cout << "Room number: " << rooms[i].roomNumber << endl;
				cout << "Room type: " << (rooms[i].type == roomType::SINGLE ? "Single" : "Double") << endl;
				cout << "Reservation ID: " << rooms[i].reservationId << endl;
				cout << "Name: " << rooms[i].reserverName << endl;
				cout << "Nights: " << rooms[i].nights << endl;
			}
		}

		// If not found
		if (!found)
		{
			cout << "\nNo reservation found.\n";
		}

		cout << "Returning to main menu...\n";
		return;
	}

	// SEARCH BY BOOKING NUMBER
	if (choice == 2)
	{
		int id;
		cout << "Enter the reservation ID (10000-99999): ";

		while (!(cin >> id) || id < 10000 || id > 99999)
		{
			cout << "Invalid input! Enter a valid number: ";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		bool found = false;

		for (int i = 0; i < rooms.size(); i++)
		{
			if (rooms[i].isReserved && rooms[i].reservationId == id)
			{
				// If found
				found = true;
				cout << "\nReservation found.\n";
				cout << "Room number: " << rooms[i].roomNumber << endl;
				cout << "Room type: " << (rooms[i].type == roomType::SINGLE ? "Single" : "Double") << endl;
				cout << "Reservation ID: " << rooms[i].reservationId << endl;
				cout << "Name: " << rooms[i].reserverName << endl;
				cout << "Nights: " << rooms[i].nights << endl;

				break;
			}
		}

		if (!found)
		{
			cout << "\nNo reservation found.\n";
		}

		cout << "Returning to main menu...\n";
		return;
	}
}


// Main menu
void mainmenu()
{
	vector<Room> rooms = startupRooms(); // Load or generate rooms

	// Search, reserve or exit
	int choice;
	do
	{
		cout << "\nDo you want to search for a booking or make a new reservation?\n";
		cout << "Search [1], Reserve [2], Exit [0]: ";

		while (!(cin >> choice))
		{
			cout << "Invalid input! Enter a number: ";
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
		}
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		switch (choice)
		{
		case 1:
			searchReservation(rooms); // Call search function
			break;

		case 2:
			reserveRoom(rooms); // Call reservation function
			break;

		case 0:
			cout << "Exiting program...\n"; // Exit
			break;

		default:
			cout << "Invalid input! Enter a valid number.\n";
		}
	} while (choice != 0); // Exit
}


// Run the main menu through void
int main()
{
	mainmenu();
	return 0;
}