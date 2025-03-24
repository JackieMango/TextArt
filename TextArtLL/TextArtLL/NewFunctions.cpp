#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	// Define the menu options
	char menu_options[] = { "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu : " };
	// Display animate on/off
	char animateYN = 'N';
	//menu_options[11] = animate ? 'Y' : 'N';
	char input = 0, choice = 0;
	int height = 0, branchAngle = 0, trunk = 270;

	// Variables for drawing/fill functions
	DrawPoint start;
	DrawPoint endP;
	Point startPoint;
	Point center;
	Point endPoint;
	Point fill;

	// Display the menu
	do {
		// Clear the canvas
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

		// Get user input
		cout << menu_options;
		cin >> choice;

		// Ignores enter key
		cin.ignore();

		// Clears menu options
		clearLine(MENU + 1, CLEAR);
		clearLine(MENU, CLEAR);

		// Process the user's choice
		switch (toupper(choice)) {
			// Animate
		case 'A':

			// Toggle the animate option
			animate = !animate;
			break;

			// Undo
		case 'U':

			restore(undoList, redoList, current);

			break;

			// Clip
		case 'I':

			// Creates temporary canvas to store clips
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

			// Fill
		case 'F':

			cout << "Enter character to fill with from current location / <ESC> to cancel ";
			input = getPoint(fill);

			// Goes back to menu if user enters <ESC>
			if (input == ESC)
			{
				break;
			}

			// For undo option
			addUndoState(undoList, redoList, current);

			fillRecursive(current->item, fill.row, fill.col, current->item[fill.row][fill.col], input, animate);
			break;

			// Line
		case 'L':

			// Draws line from two points
			cout << "Type any letter to choose start point / <ESC> to cancel\n";
			input = getPoint(startPoint);
			start = startPoint;

			// Clears menu options
			clearLine(MENU, CLEAR);

			cout << "Type any letter to choose end point / <ESC> to cancel\n";
			input = getPoint(endPoint);
			endP = endPoint;

			// Goes back to menu if user enters <ESC>
			if (input == ESC)
			{
				break;
			}

			// For undo option
			addUndoState(undoList, redoList, current);

			drawLine(current->item, startPoint, endPoint, animate);

			break;

			// Box
		case 'B':

			cout << "Enter size: ";
			cin >> height;

			// Clears menu options
			clearLine(MENU, CLEAR);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel\n";
			input = getPoint(center);

			// Puts box in center if user enters <C>
			if (toupper(input) == 'C')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}

			// Goes back to menu if user enters <ESC>
			if (input == ESC)
			{
				break;
			}

			// For undo option
			addUndoState(undoList, redoList, current);

			drawBox(current->item, center, height, animate);
			break;

			// Nested Boxes
		case 'N':

			cout << "Enter size of largest box: ";
			cin >> height;

			// Clears menu options
			clearLine(MENU, CLEAR);

			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel\n";
			input = getPoint(center);

			// Puts boxes in center if user enters <C>
			if (toupper(input) == 'C')
			{
				center.row = MAXROWS / 2;
				center.col = MAXCOLS / 2;
			}

			// Goes back to menu if user enters <ESC>
			if (input == ESC)
			{
				break;
			}

			// For undo option
			addUndoState(undoList, redoList, current);

			drawBoxesRecursive(current->item, center, height, animate);
			break;

			// Tree
		case 'T':

			cout << "Enter approximate tree height: ";
			cin >> height;

			// Clears menu options
			clearLine(MENU, CLEAR);

			cout << "Enter branch angle: ";
			cin >> branchAngle;

			// Clears menu options
			clearLine(MENU, CLEAR);

			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel\n";
			input = getPoint(startPoint);

			// Prints tree in bottom center if user enters <C>
			if (toupper(input) == 'C')
			{
				startPoint.row = MAXROWS - 1;
				startPoint.col = MAXCOLS / 2;
			}

			// Goes back to menu if user enters <ESC>
			if (input == ESC)
			{
				break;
			}
			start = startPoint;

			// For undo option
			addUndoState(undoList, redoList, current);

			treeRecursive(current->item, start, height, trunk, branchAngle, animate);

			break;

			// Main Menu
		case 'M':

			return;
		}
	} while (choice != 'M');

}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
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

		else if (input == '\0')
		{
			input = _getch();
		}

		// Writes character user entered at cursor placement
		else if (startASCII <= input && lastASCII >= input)
		{
			pt.row = row;
			pt.col = col;
			cout << input;

			return input;
		}

		gotoxy(row, col);
		input = _getch();
	}

	return input;
}

// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	// Base cases:
	if (row < 0 || col < 0 || row >= MAXROWS || col >= MAXCOLS || canvas[row][col] != oldCh || oldCh == newCh)
	{
		return;
	}

	Point fill(row, col);

	drawHelper(canvas, fill, newCh, animate);

	// Recursive case
	fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
	fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
}

// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// Base Case:
	if (start.row >= MAXROWS || start.col >= MAXCOLS || start.row < 0 || start.col < 0 || height < 3)
	{
		return;
	}

	// Draws tree trunk
	DrawPoint trunk = findEndPoint(start, height / 3, startAngle);
	drawLine(canvas, start, trunk, animate);

	// Recursively draws branches
	treeRecursive(canvas, trunk, height - 2, startAngle + branchAngle, branchAngle, animate);
	treeRecursive(canvas, trunk, height - 2, startAngle - branchAngle, branchAngle, animate);
}

// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	// Check that the height is at least 2
	if (height >= 2)
	{
		drawBox(canvas, center, height, animate);
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
	else
		return;
}