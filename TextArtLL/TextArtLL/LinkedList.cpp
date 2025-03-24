#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
{
    // Assign memory for the new canvas node and create a pointer to it
    Node* canvas = new Node;
    // Initialize the item in the new canvas node
    initCanvas(canvas->item);
    // Set the next pointer in the new canvas to NULL
    canvas->next = NULL;

    // Return a pointer to the new canvas node
    return canvas;
}

Node* newCanvas(Node* oldNode)
{
    // Assign memory for the new canvas node and create a pointer to it
    Node* copy = new Node;

    // Copy the contents of the item in the old canvas node to the item in the new canvas node
    for (int row = 0; row < MAXROWS; row++)
    {
        for (int col = 0; col < MAXCOLS; col++)
        {
            copy->item[row][col] = oldNode->item[row][col];
        }
    }

    // Return a pointer to the new canvas node
    return copy;
}


void play(List& clips)
{
    // loops as long as the ESCAPE key is not currently being pressed
    while (!(GetKeyState(VK_ESCAPE) & 0x8000))
    {
        // Start the animation by calling the playRecursive function with the head node and the number of clips 
        playRecursive(clips.head, clips.count);
    }
}


void playRecursive(Node* head, int count)
{
    // Base Case: if the list is empty or reached the beginning of the list
    if (head == NULL || count == 0)
    {
        return;
    }

    // Traverse the list recursively to get to the end
    playRecursive(head->next, count - 1);

    // Display the current clip
    displayCanvas(head->item);

    // Tells user they can exit the function by holding ESC
    printf("Hold <ESC> to stop       Clip: %2d     ", count);

    // Pause for 100 milliseconds to slow down animation
    Sleep(100);
}


void addUndoState(List& undoList, List& redoList, Node*& current)
{
    // Adds a node to the undo list 
    addNode(undoList, newCanvas(current));

    // Deletes the redo list
    deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node*& current)
{
    // Adds the current canvas node to the redo list
    addNode(redoList, current);

    // Takes a node from the front of the undo list and makes it the current canvas node
    current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
{
    // Add the node to the front of the list
    nodeToAdd->next = list.head;
    list.head = nodeToAdd;
    list.count++;
}


Node* removeNode(List& list)
{
    if (list.count == NULL)
    {
        // If the list is empty, return is NULL
        return NULL;
    }

    Node* removedNode = list.head;
    list.head = list.head->next;
    list.count--;

    // Detaches the removed node from the list
    removedNode->next = NULL;

    return removedNode;
}


void deleteList(List& list)
{

    while (list.head != NULL)
    {
        Node* temp = new Node;
        temp = list.head->next;
        delete list.head;
        list.head = temp;
        list.count--;
    }
}


bool loadClips(List& clips, char filename[])
{
    // Erase the existing list of clips
    deleteList(clips);

    // Const int MAX_FILE_NUMBER = 16; // Maximum file number in the sequence
    const char PREFIX = '-';

    // Attempt to load the first file in the sequence
    char fullFilename[FILENAMESIZE];
    bool clipsFound = true;


    // Attempt to load the rest of the files in the sequence
    int fileNumber = 1;
    while (clipsFound)
    {
        Node* newNode = newCanvas();
        // Form the full filename for the current file in the sequence
        snprintf(fullFilename, FILENAMESIZE, "%s%c%d.txt", filename, PREFIX, fileNumber);
        clipsFound = loadCanvas(newNode->item, fullFilename);

        if (!clipsFound && fileNumber == 1)
        {
            // If we couldn't load the first file, return false
            cerr << "ERROR: File cannot be read.\n";
            system("pause");
            return false;
            delete newNode;
        }

        // Attempt to load the canvas from the file
        if (clipsFound)
        {
            // If the canvas was successfully loaded, create a new node in the list and add the canvas to it
            addNode(clips, newNode);
            fileNumber++;
        }

        if (!clipsFound && fileNumber > 1)
        {
            // If the canvas couldn't be loaded, assume we've reached the end of the sequence and break out of the loop
            cout << "Clips loaded!\n";
            system("pause");
            clearLine(MENU + 2, CLEAR);
        }
    }

    // If we've added any nodes to the list, return true
    return true;
}


bool saveClips(List& clips, char filename[])
{
  
    char filenamePath[FILENAMESIZE];
    bool savingClip = false;
    int i = clips.count;


    for (Node* current = clips.head; i > 0; current = current->next)
    {
        // Concatenates 'file name + '-clip number' + '.txt''
        snprintf(filenamePath, FILENAMESIZE, "%s-%d.txt", filename, i--);

        // Saves clip to the file
        savingClip = saveCanvas(current->item, filenamePath);

        // Returns false if clips can't save
        if (savingClip == false)
        {
            return false;
        }
    }

    if (savingClip)
    {
        cout << "Clips saved!\n";
        system("pause");
        clearLine(MENU + 2, CLEAR);
    }

    return true;
}