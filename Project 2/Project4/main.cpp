//----------------------------------------------------------------------------
//                  CS 215 - Project 2 - MiniMud
//----------------------------------------------------------------------------
// Author:  Tyler Patrick
// Date:  03/06/2019
// Description:  Project 2
// Assistance: I received help from the following: Josiah Jaddock
//-----------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "gameutils.h"
using namespace std;
// ------------------------------------
//              printlogo
//-------------------------------------
void printlogo()
{
	cout << "+-----------------------------------------------------+ \n";
	cout << "|                       Mini Mud                      | \n";
	cout << "|                    TYLARSOFTGAMES                   | \n";
	cout << "+-----------------------------------------------------+ \n";
}//printlogo
/// ------------------------------------
//             getInputFileName
//-------------------------------------
string getInputFileName()
{
	cout << "Enter name of game to play: ";
	string gameName;
	cin >> gameName;
	gameName.append(".txt");
	return(gameName);
}//getInputFileName
// ------------------------------------
//                 initItems
//-------------------------------------

void initItems(item items[], int & numitems)
{
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		items[i].shortDesc = "";
		items[i].longDesc = "";

	}
	numitems = 0;
}//initItems
// ------------------------------------
//                initrooms
//-------------------------------------

void initrooms(room rooms[], int & numRooms)
{
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		rooms[i].north = NO_EXIT;
		rooms[i].east = NO_EXIT;
		rooms[i].south = NO_EXIT;
		rooms[i].west = NO_EXIT;
		rooms[i].longDesc = "";
		rooms[i].shortDesc = "";
		initItems(rooms[i].items, rooms[i].numItems);
	}
}//initrooms
// ------------------------------------
//               InitwinData
//-------------------------------------
	void initWinData(winDataStruct & windata)
	{
		windata.winMessage = "";
		windata.winRoom = NO_EXIT;
		initItems(windata.winItem, windata.numWinItems);
    }//initWinData
	// ------------------------------------
	//                 removeItem
	//-------------------------------------
	item removeItem(item items[], int index, int & numItems)
	{
		if (index > numItems || (index < 0))
		{
			string errormsg = "removeItem: invalid index = ";
			errormsg.append(to_string(index));
			gameAbort(errormsg);
		}
		item remove = items[index];
		numItems--;
		for (int i = index; i < numItems; i++)
		{
			items[i] = items[i + 1];
		}
		items[numItems].longDesc = "";
		items[numItems].shortDesc = "";
		return remove;


	}//removeItem
// ------------------------------------
//                 AddItem
//-------------------------------------
	void addItem(item items[], item newItem, int & numItems)
	{
		if (numItems >= MAX_ITEMS)
		{
			gameAbort("addItem: Maximum number of items exceeded!");
		}
		items[numItems] = newItem;
		numItems++;
	}//addItem
// ------------------------------------
//                 AddItem
//-------------------------------------
	void addItem(string shortDesc, string longDesc, item items[], int & numItems)
	{
		item newItem;
		newItem.shortDesc = shortDesc;
		newItem.longDesc = longDesc;

		addItem(items, newItem, numItems);
	}//addItem


//----------------------------------------------------------------------------
//							loadData
//----------------------------------------------------------------------------
void loadData(room rooms[], int & numRooms, winDataStruct & winData) {
	// variable declarations
	ifstream f;				// input file handle
	string tag;				// first word extracted from each line
	string filename;		// name of input file
	string data;			// data read from file using cleanGetline()
	int roomNdx = -1;		// index of the "current room" being read/populated
							// this should always be rumRooms-1 

	// initialize the rooms and winData
	initrooms(rooms, numRooms);
	initWinData(winData);
	// get input file name and open the input file; abort on failure
	filename = getInputFileName();
	f.open(filename);
	if (f.fail() )
	{
		string errormsg = "Unable to open ";
		errormsg.append(filename);
		gameAbort(errormsg);

	}
	// read line-by-line and store data in the game data structures
	f >> tag;
	while (tag != "END:") {
		if (tag == "ROOM:") {
			numRooms++;		// starting a new room
			roomNdx++;		// increment every time numRooms is incremented
			cleanGetline(f, rooms[roomNdx].shortDesc); // short desc on remainder of ROOM: line
			cleanGetline(f, rooms[roomNdx].longDesc);  // long desc on next line by itself (no tag)
		}
		else if (tag == "ITEM:") {
			string shortDesc, longDesc;
			cleanGetline(f, shortDesc);
			cleanGetline(f, longDesc);
			addItem(shortDesc, longDesc, rooms[roomNdx].items, rooms[roomNdx].numItems);
		}
		else if (tag == "WIN_ITEM:") {
			cleanGetline(f, data);
			addItem(data, "", winData.winItem, winData.numWinItems);
		}
		else if (tag == "NORTH:")
			f >> rooms[roomNdx].north;
		else if (tag == "SOUTH:")
			f >> rooms[roomNdx].south;
		else if (tag == "EAST:")
			f >> rooms[roomNdx].east;
		else if (tag == "WEST:")
			f >> rooms[roomNdx].west;
		else if (tag == "WIN_ROOM:")
			f >> winData.winRoom;
		else if (tag == "REMARK:")
			cleanGetline(f, data);		// data is junk, throw it away
		else if (tag == "WIN_TEXT:")
		{


			cleanGetline(f, data);
			winData.winMessage = data;
		}
		else if (tag == "END:");

		else
			gameAbort("Unknown tag in " + filename + ": " + tag);

		// check for read failure and read the next tag
		if (f.fail())
			gameAbort("Failure while reading input file " + filename);
		f >> tag;
	}
} // loadData()
// ------------------------------------
//                 getCmd
//-------------------------------------
string getCmd( string & remainder)
{
	cout << "===> ";
	string input = remainder, cmd;
	if (cin.peek() == '\n') cin.ignore();
	getline(cin, input);
	cmd = split(input, remainder);
	return cmd;
	
}//getCmd
//----------------------------------------------------------------------------
//							makeExitList
//----------------------------------------------------------------------------
string makeExitList(room thisRoom) {
	string exitList = "You can go:";
	if (thisRoom.north != NO_EXIT) exitList += " north,";
	if (thisRoom.south != NO_EXIT) exitList += " south,";
	if (thisRoom.east != NO_EXIT) exitList += " east,";
	if (thisRoom.west != NO_EXIT) exitList += " west.";
	exitList[exitList.length() - 1] = '.'; // change last comma to a period
	return exitList;
} // makeExitList()
// ------------------------------------
//                 doLook
//-------------------------------------
void doLook(room rooms, bool Desc)
{
	if (Desc == 0)
	{
		cout << rooms.longDesc << endl;
	}
	else
		cout << rooms.shortDesc << endl;
	if (rooms.numItems > 0)
	{
		cout << "You notice the following: ";
		for (int i = 0; i < rooms.numItems - 1; i++)
		{
			cout << rooms.items[i].shortDesc << " , ";
		}
		cout << rooms.items[rooms.numItems - 1].shortDesc << "." << endl;
	}
	string list = makeExitList(rooms);
	cout << list << endl;
	
} //dolook()
// ------------------------------------
//                 do look 
//-------------------------------------
void doLook(room rooms)
{
	bool longFlag = 0;
	doLook(rooms, longFlag);
} //doLook
//-------------------------------------
//               startup
//-------------------------------------
void startup(room gameRooms[], int & numRooms, item inv[], int & numItems,winDataStruct & winData)
{
	printlogo();
	loadData(gameRooms, numRooms, winData);
	initItems(inv, numItems);
	cout << "You fall asleep and dream of a place far away...\n";
	doLook(gameRooms[START_ROOM],0);
}

//----------------------------------------------------------------------------
//							doInventory
//----------------------------------------------------------------------------
// Given: the player's inventory (partial array of items)
// This is the game's response to the "inv"/"i" (inventory) command.
// Prints a message telling the player what he/she is holding.
//----------------------------------------------------------------------------
void doInventory(item item[], int numItems) {
	if (numItems == 0)
		cout << "You don't have anything.\n";
	else {
		cout << "You are holding: ";
		for (int i = 0; i < (numItems - 1); i++)		// for all but last item
			cout << item[i].shortDesc << ", ";			// print short desc with comma
		cout << item[numItems - 1].shortDesc << ".\n";  // for last item, use period and newline
	}
} // doInventory()

//----------------------------------------------------------------------------
//							goNorth
//----------------------------------------------------------------------------
// Given:    partial array of rooms
// Modifies: index of current room
// This is the game's response to the player entering the "north"/"n" command.
// When the north exit exists, changes the current room index and performs
// a "look" command with a short room description
void goNorth(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].north == NO_EXIT)
		cout << "You can't go north.\n";
	else {
		currentRoom = rooms[currentRoom].north;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goNorth()
//----------------------------------------------------------------------------
//							goEast
//----------------------------------------------------------------------------
void goEast(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].east == NO_EXIT)
		cout << "You can't go east.\n";
	else {
		currentRoom = rooms[currentRoom].east;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goEast()
//----------------------------------------------------------------------------
//							goWest
//----------------------------------------------------------------------------
void goWest(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].west == NO_EXIT)
		cout << "You can't go west.\n";
	else {
		currentRoom = rooms[currentRoom].west;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goWest()
//----------------------------------------------------------------------------
//							goSouth
//----------------------------------------------------------------------------
void goSouth(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].south == NO_EXIT)
		cout << "You can't go south.\n";
	else {
		currentRoom = rooms[currentRoom].south;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goSouth()
//-------------------------------------------
//                  findItem
//-------------------------------------------
int findItem(item items[],int numItems,string term)
{
	for (int i = 0; i < numItems; i++)
	{
		if (items[i].shortDesc == term)
		{
			return i;
		}
	}
	return NOT_FOUND;
}//findItem
//---------------------------------------
//               doExamine
//---------------------------------------
void doExamine(string object, item roomitems[], int numItemsRoom, item playerinv[], int numItemsPlayer)
{
	int foundIndex = findItem(roomitems, numItemsRoom, object);
	if (foundIndex == -1)
	{
		foundIndex = findItem(playerinv, numItemsPlayer, object);
		if (foundIndex == -1)
			cout << "You see no " << object << endl;
		else
			cout << playerinv[foundIndex].longDesc << endl;
	}
	else
	{
		cout << roomitems[foundIndex].longDesc << endl;
	}
}// doExamine
//-----------------------------------------
//                doTake
//-----------------------------------------
void doTake(string itm, item roomitems[], int & numItemsRoom, item playerInv[], int & numItemsPlayer)
{
	int index = findItem(roomitems, numItemsRoom, itm);
	if ((index != NOT_FOUND) && (numItemsPlayer < MAX_ITEMS))
	{
		item tempItem = removeItem(roomitems, index, numItemsRoom);
		addItem(playerInv, tempItem, numItemsPlayer);
		cout << "You take " << tempItem.shortDesc << endl;
	}
	else if ((index != NOT_FOUND) && (numItemsPlayer >= MAX_ITEMS))
	{
		cout << "Your hands are full you cant take that \n";
	}
	else
	{
		findItem(playerInv, numItemsPlayer, itm);
		if (index == NOT_FOUND)
		{
			cout << "You see no" << itm << endl;
		}
		else
		{
			cout << "You already have the" << itm << endl;
		}
	}//doTake
}
//----------------------------------------------------------
//                     doDrop
//----------------------------------------------------------
void doDrop(string itemname, item roomitems[], int & numRoomItems, item playerinv[], int & numItemsPlayer)
{
	int foundindex = findItem(playerinv, numItemsPlayer, itemname);
	if ((foundindex != NOT_FOUND) && (numRoomItems < MAX_ITEMS))
	{
		item removed = removeItem(playerinv, foundindex, numItemsPlayer);
		addItem(roomitems, removed, numRoomItems);
		cout << "You drop the " << removed.shortDesc << endl;
	}
	else if ((foundindex != NOT_FOUND) && (numRoomItems >= MAX_ITEMS))
	{
		cout << "The room is full of junk; you can't drop that" << endl;

	}
	else
	{
		foundindex = findItem(roomitems, numRoomItems, itemname);

		if (foundindex != -1)
		{
			cout << "You dont have the " << itemname << endl;
		}
		else 
		{
			cout << "You have no " << itemname << endl;
		}


	}
}// doDrop
//-------------------------------------------------------
//                       gameWon
//-------------------------------------------------------
bool gameWon(winDataStruct win, room gamerooms[], int numRooms)
{
	int index;
	for (int i = 0; i < win.numWinItems; i++)
	{
		index = findItem(gamerooms[win.winRoom].items, gamerooms[win.winRoom].numItems, win.winItem[i].shortDesc);
		if (index == -1)
		{
			return false;
		}


	}
	return true;
}//gameWon
//----------------------------------------------------------------------------
//							main
//----------------------------------------------------------------------------
int main() {
	// declare data structures
	room gamerooms[MAX_ROOMS];
	item items[MAX_ITEMS];
	int currentRoom = START_ROOM, numRooms = 0, playerItems;
	winDataStruct gameWin;


	// declare other variables
	string cmd;					// command entered by user (first word of input)
	string remainder;			// remainder of command entered by user
	bool   gameWasWon = false;	// check with gameWon() as needed

	// start up: load game data from file and initialize player inventory
	startup(gamerooms, numRooms, items, playerItems, gameWin);
	// get first command input
	 cmd = getCmd(remainder);

	// repeat until "exit" entered or the game is won
	while (cmd != "exit" && !gameWasWon) {
		if (cmd == "help")
			printHelp();
		// add cases for other commands and invocations
		else if ((cmd == "look") || (cmd == "l"))
			doLook(gamerooms[currentRoom]);
		else if ((cmd == "inv") || (cmd == "i"))
			doInventory(items, playerItems);
		else if ((cmd == "exa") || (cmd == "x"))
			doExamine(remainder, gamerooms[currentRoom].items, gamerooms[currentRoom].numItems, items, playerItems);
		else if ((cmd == "take") || (cmd == "t"))
			doTake(remainder, gamerooms[currentRoom].items, gamerooms[currentRoom].numItems, items, playerItems);
		else if ((cmd == "drop") || (cmd == "d"))
			doDrop(remainder, gamerooms[currentRoom].items, gamerooms[currentRoom].numItems, items, playerItems);
		else if ((cmd == "north") || (cmd == "n"))
			goNorth(currentRoom, gamerooms);
		else if ((cmd == "south") || (cmd == "s"))
			goSouth(currentRoom, gamerooms);
		else if ((cmd == "east") || (cmd == "e"))
			goEast(currentRoom, gamerooms);
		else if ((cmd == "west") || (cmd == "w"))
			goWest(currentRoom, gamerooms);
		else // the user entered an unrecognized command
			cout << "What??\n";

		// check if game was won and print win message or get next user input
		gameWasWon = gameWon(gameWin, gamerooms, numRooms);
		if (gameWasWon)
			cout << gameWin.winMessage << endl;
		else
		cmd = getCmd(remainder);
	}

	system("pause");
	return 0;
}