// Pierce Moeller, Shane Wilson, Jaqueline Flores-Valdivia, Gwendolyn Wiborg
// Nathaniel Tauzer, Naquesha Carmon
/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


// 'SavedFiles\\ + .txt' characters
const int FILESIZE = 15;


int main()
{
	system("cls");

	char input;
	bool animate = false;
	Node* current = newCanvas();
	char animateYN = 'N';

	List undoList;
	List redoList; 
	List clips;

	do
	{
		// Displays canvas and options then asks for user input
		displayCanvas(current->item);

		animateYN = animate ? 'Y' : 'N';


		// Displays redo and play options
		if (redoList.count > 0 && clips.count >= 2)
		{
			clearLine(MENU, CLEAR);
			printf("<A>nimate: %c / <U>ndo: %d / Red<O>: %d / Cl<I>p: %.*d / <P>lay\n", animateYN, undoList.count, redoList.count, 1, clips.count);
		}
		// Displays play option
		else if (clips.count >= 2)
		{
			clearLine(MENU, CLEAR);
			printf("<A>nimate: %c / <U>ndo: %d / Cl<I>p: %.*d / <P>lay\n", animateYN, undoList.count, 1, clips.count);
		}
		// Displays redo option
		else if (redoList.count > 0)
		{
			clearLine(MENU, CLEAR);
			printf("<A>nimate: %c / <U>ndo: %d / Red<O>: %d / Cl<I>p: %.*d\n", animateYN, undoList.count, redoList.count, 1, clips.count);
		}
		// Displays menu one
		else
		{
			clearLine(MENU, CLEAR);
			printf("<A>nimate: %c / <U>ndo:%2d / Cl<I>p:%2d\n", animateYN, undoList.count, clips.count);
		}

		cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";
		cin >> input;

		// Ignores enter key
		cin.ignore();

		// Clears menu options
		clearLine(MENU + 1, CLEAR);
		clearLine(MENU, CLEAR);

		// Takes uppercase/lowercase user input
		switch (toupper(input))
		{
		// Animate
		case 'A':
			animate = !animate;
			break;
		
		// Clip
		case 'I':
		{
			Node* temp = newCanvas(current);
			addNode(clips, temp);

			break;
		}

		// Redo
		case 'O':
			restore(redoList, undoList, current);
			break;
		
		// Play
		case 'P':
			play(clips);
			break;
		
		// Edit
		case 'E':
			// For undo option
			addUndoState(undoList, redoList, current);
			editCanvas(current->item);

			break;
		
		// Move
		case 'M':
			int rowValue;
			int colValue;

			cout << "Enter column units to move: ";
			cin >> colValue;

			cout << "Enter row units to move: ";
			cin >> rowValue;

			// For undo option
			addUndoState(undoList, redoList, current);
			moveCanvas(current->item, rowValue, colValue);

			break;
		
		// Replace
		case 'R':
			char oldCh;
			char newCh;

			cout << "Enter character to replace: ";
			cin.get(oldCh);
			cin.ignore();

			cout << "Enter character to replace with: ";
			cin.get(newCh);
			cin.ignore();

			// For undo option
			addUndoState(undoList, redoList, current);
			replace(current->item, oldCh, newCh);

			break;
		
		// Draw
		case 'D':

			menuTwo(current, undoList, redoList, clips, animate);

			break;
		
		// Clear
		case 'C':

			// For undo option
			addUndoState(undoList, redoList, current);
			initCanvas(current->item);

			break;
			
		// Undo
		case 'U':

			// Undoes the users last choice
			restore(undoList, redoList, current);
			break;
		
		// Load
		case 'L':
		{
			char filename[FILENAMESIZE - FILESIZE];
			char filenamePath[FILENAMESIZE];
			char choice = 0;

			cout << "<C>anvas or <A>nimation ? ";
			cin >> choice;
			cin.ignore();
			clearLine(MENU, CLEAR);

			// Get user input for file name to open
			cout << "Enter the filename (don't enter 'txt): ";
			cin.getline(filename, FILENAMESIZE - 15);
			clearLine(MENU + 1, CLEAR);

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Canvas
			if (toupper(choice) == 'C')
			{
				snprintf(filenamePath, FILENAMESIZE, "SavedFiles\\%s.txt", filename);
				addUndoState(undoList, redoList, current);

				loadCanvas(current->item, filenamePath);
			}

			// Animation
			if (toupper(choice) == 'A')
			{
				snprintf(filenamePath, FILENAMESIZE, "SavedFiles\\%s", filename);

				loadClips(clips, filenamePath);
			}

			break;
		}
		
		// Save
		case 'S':
		{
			char filename[FILENAMESIZE - FILESIZE];
			char filenamePath[FILENAMESIZE];
			char choice = 0;

			cout << "<C>anvas or <A>nimation ? ";
			cin >> choice;
			cin.ignore();
			clearLine(MENU, CLEAR);

			// Get user input for file name to open
			cout << "Enter the filename (don't enter 'txt): ";
			cin.getline(filename, FILENAMESIZE - 15);
			clearLine(MENU + 1, CLEAR);

			if (cin.fail())
			{
				cin.clear();
				cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			}

			// Canvas
			if (toupper(choice) == 'C')
			{
				snprintf(filenamePath, FILENAMESIZE, "SavedFiles\\%s.txt", filename);

				bool savingClip = saveCanvas(current->item, filenamePath);

				if (savingClip)
				{
					cout << "Clips saved!\n";
					system("pause");
					clearLine(MENU + 2, CLEAR);
				}

			}

			// Animation
			if (toupper(choice) == 'A')
			{
				snprintf(filenamePath, FILENAMESIZE, "SavedFiles\\%s", filename);

				saveClips(clips, filenamePath);
			}

			break;
		}
		
		// Quit
		case 'Q':
		{
			delete current->item;
			deleteList(redoList);
			deleteList(undoList);
			deleteList(clips);
			break;
		}
		}

	// Exits loop when user enters 'Q'/'q'
	} while (toupper(input) != 'Q');

	return 0;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	// Checks if the elements in the array match the character to be replaced
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			// Replaces with specified replacement character
			if (canvas[i][j] == oldCh)
			{
				canvas[i][j] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	cout << "Press <ESC> to stop editing ";

	char input;
	int row = 0, col = 0;

	gotoxy(row, col);
	input = _getch();

	// Loops until user enters <ESC> key
	while (input != ESC)
	{

		// Checks if user pressed arrow keys
		if (input == SPECIAL)
		{
			// Moves cursor with the corresponding arrow key
			gotoxy(row, col);
			input = _getch();

			switch (input)
			{
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}
				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}
				break;
			case RIGHTARROW:
				if (col < MAXCOLS - 1)
				{
					col++;
				}
				break;
			case DOWNARROW:
				if (row < MAXROWS - 1)
				{
					row++;
				}
				break;
			}
		}
		else if (input >= startASCII && input <= lastASCII)
		{
			canvas[row][col] = input;
			cout << input;
		}
		else if (input == '\0')
		{
			input = _getch();
		}
		gotoxy(row, col);
		input = _getch();
	}
}

void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char temp[MAXROWS][MAXCOLS];

	// Initialize temporary canvas to move
	initCanvas(temp);

	// Shifts the image
	for (int i = 0; i < MAXROWS; i++)
	{
		int newi = i + rowValue;
		for (int j = 0; j < MAXCOLS; j++)
		{
			int newj = j + colValue;

			if (newi >= 0 && newj >= 0 && newi < MAXROWS && newj < MAXCOLS)
			{
				temp[newi][newj] = canvas[i][j];
			}
		}
	}
	copyCanvas(canvas, temp);
}


void initCanvas(char canvas[][MAXCOLS])
{
	// Creates a blank canvas

	// Loops through rows
	for (int x = 0; x < MAXROWS; x++)
	{	// Loops thorugh columns
		for (int y = 0; y < MAXCOLS; y++)
		{	// Enters a space
			canvas[x][y] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	// Returns cursor to top left corner
	int x = 0, y = 0;
	gotoxy(x, y);

	// Creates vertical lines for canvas
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			cout << canvas[x][y];
		}

		printf("|\n");
	}

	// Creates horizontal lines for canvas
	for (int i = 0; i < MAXCOLS; i++)
	{
		printf("-");
	}

	cout << "\n";
}


void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	// Loops through rows
	for (int rows = 0; rows < MAXROWS; rows++)
	{   // Loops through columns
		for (int cols = 0; cols < MAXCOLS; cols++)
		{
			// Copies 'to' array and stores elements into another array 'from'
			to[rows][cols] = from[rows][cols];
		}
	}
}


bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	// Open the file for reading
	ifstream infile;
	infile.open(filename);

	// Check if the file was opened successfully
	if (!infile)
	{
		return false;
	}

	int row = 0;
	char c;
	initCanvas(canvas);
	// Read characters until the end of file or maximum rows reached
	while (row < MAXROWS && infile.get(c))
	{
		int col = 0;
		// Read characters until end of line or maximum columns reached
		while (col < MAXCOLS && c != '\n' && !infile.eof())
		{
			canvas[row][col] = c;
			col++;
			infile.get(c);
		}
		// Skip the rest of the line
		while (c != '\n' && infile.get(c)) {}
		row++;
	}

	// Close the file
	infile.close();

	return true;
}


bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	// Open the file for writing
	ofstream outfile(filename);
	// Check if the file was opened successfully
	if (!outfile)
	{
		return false;
	}

	// Loop through each row of the canvas
	for (int row = 0; row < MAXROWS; row++)
	{
		// Loop through each column of the current row
		for (int col = 0; col < MAXCOLS; col++)
		{
			// Write the character to the file
			outfile.put(canvas[row][col]);
		}
		// Write a newline character at the end of each row
		outfile.put('\n');
	}

	// Close the file
	outfile.close();


	return true;
}