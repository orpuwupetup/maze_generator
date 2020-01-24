#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>
#include <stack>

using namespace std;

enum CellType {CORRIDOR, WALL, START, END, SOLUTION, NOT_SET};
enum RowType {FIRST, STANDARD, LAST, NO_ROW};
enum Direction {UP, DOWN, LEFT, RIGHT};

const int NO_SET_ASSIGNED = -1;

// representation of single labirynth node
struct Cell
{
    int x, y, currentSet = NO_SET_ASSIGNED;
    CellType type = NOT_SET;
    set<Direction> possibleDirections;
    set<Direction> searchedDirections;
    Direction parentDirection;

    Cell() {}
    Cell(int a, int b)
    {
        this -> x = a;
        this -> y = b;
    }
};

// some aliases for code reading convenience
typedef vector<Cell> Row;
typedef vector<Row> Labirynth;

// User input related function declarations
void getUserInput(int *height, int *width);
bool checkInput(int height, int width);

// Output related function declarations
void getUserInput(int *height, int *width);
bool checkInput(int height, int width);
void showLabirynth(Labirynth &labirynth);
void showRow(Row row);
void showSolution(Labirynth &labirynth);

// Labirynth creating related function declarations
int getNewSetNumber(int currentSetNumber, set<int> &usedNumbers);
Labirynth generateLabirynth(int height, int width);
Labirynth initLabirynth(int height, int width);
RowType getRowType(int currentIndex, int maxIndex);
void setRow(Row &row, Row &previousRow, Row &nextRow, RowType rowType, bool shortestPossibleLabirynth = false);

// Labirynth solving related function declarations
void checkDirections(Cell &cell, Labirynth &labirynth);
void solveLabirynth(Labirynth &labirynth, vector<char> &writtenSolution);

// General utility related function declarations
bool generateRandomBool();
int getRandomIntFromRange(int rangeStart, int rangeEnd);

int main()
{
    // I think everything here is self explanatory
    srand(time(NULL));

    int h,w;
    getUserInput(&h, &w);

    Labirynth labirynth = generateLabirynth(h, w);

    showLabirynth(labirynth);

    showSolution(labirynth);

    return 0;
}

void showSolution(Labirynth &labirynth)
{
    // ask user if he wants to see solution for the generated maze
    char confirm;
    cout << "Czy chcesz zobaczyc rozwiazanie? (Y - tak, N - nie)" << endl;
    cin >> confirm;

    if (confirm == 'y' || confirm == 'Y' )
    {
        // if yes, solve maze and show solution
        vector<char> writtenSolution;
        solveLabirynth(labirynth, writtenSolution);

        showLabirynth(labirynth);

        cout << "\n\n Ruchy potrzebne do przejscia przez labirynt: ";
        for(int i = writtenSolution.size()-1; i >= 0; i--){
            cout << writtenSolution[i];
        }
        cout << endl;
    }
}

void solveLabirynth(Labirynth &labirynth, vector<char> &writtenSolution)
{
    Cell startNode;

    // find and save starting node
    for(int i = 0; i< labirynth[0].size() - 1; i++)
    {
        if(labirynth[0][i].type == START) {
            startNode = labirynth[0][i];
            break;
        }
    }

    // create stack for saving solution, and push starting node, and node directly below into it - they are always part the of solution
    stack<Cell> solution;
    solution.push(startNode);
    solution.push(labirynth[startNode.x+1][startNode.y]);

    // set from which direction current node was entered
    solution.top().parentDirection = UP;

    bool noSolutionFound = false;

    // start of maze solving loop
    do
    {
        // check possible directions for current cell
        checkDirections(solution.top(), labirynth);

        // get number of possible directions
        int numberOfDirections = solution.top().possibleDirections.size();

        // if no possible directions left
        if (numberOfDirections == 0)
        {
            // if there are no directions left to follow, it means that algorithm encountered dead and, so we want to pop last cell from solution stack to go back a little bit
            solution.pop();

            // if there is only start node left on the stack, it means that algorithm couldn't find the exit, so we need to break out of the loop
            if (solution.top().type == START)
            {
                cout << endl << endl << "No solution found" << endl;
                noSolutionFound = true;
                break;
            }

            continue;
        }

        // pick at random one direction from all possible ones
        int takenDirectionIndex = getRandomIntFromRange(0,numberOfDirections-1);
        Direction takenDirection;

        // solution for getting element from set at the needed index
        int increment = 0;
        for(Direction dir : solution.top().possibleDirections)
        {
            if (increment == takenDirectionIndex)
            {
                takenDirection = dir;
                break;
            }
            increment ++;
        }

        // erase picked direction from possible directions and add it to directions already searched by this cell
        solution.top().possibleDirections.erase(takenDirection);
        solution.top().searchedDirections.insert(takenDirection);

        /*
            'move' through the maze in the correct direction according to which one was drawn and set parent direction for newly selected cell
            add parent direction to searched directions set as well, to prevent unneeded backtracking
        */
        switch (takenDirection)
        {
        case UP :
        {
            solution.push(labirynth[solution.top().x-1][solution.top().y]);
            solution.top().parentDirection = DOWN;
            solution.top().searchedDirections.insert(DOWN);
            break;
        }
        case LEFT :
        {
            solution.push(labirynth[solution.top().x][solution.top().y-1]);
            solution.top().parentDirection = RIGHT;
            solution.top().searchedDirections.insert(RIGHT);
            break;
        }
        case RIGHT :
        {
            solution.push(labirynth[solution.top().x][solution.top().y+1]);
            solution.top().parentDirection = LEFT;
            solution.top().searchedDirections.insert(LEFT);
            break;
        }
        case DOWN :
        {
            solution.push(labirynth[solution.top().x+1][solution.top().y]);
            solution.top().parentDirection = UP;
            solution.top().searchedDirections.insert(UP);
            break;
        }
        }

        // delete parent direction from possible directions set as well, to prevent unneeded backtracking
        solution.top().possibleDirections.erase(solution.top().parentDirection);

    } while (solution.top().type != END); // if End cell is reached, end the loop

    // pop end cell from solution, so that it will show as @ in stead of as solution cell
    solution.pop();

    char writtenSolutionSign;

    // Add two more moves to written solution, because of former .pop() called on the stack
    writtenSolution.push_back('K');
    writtenSolution.push_back('D');

    // if only one node is left in solution stack, it is start node, leave it, so it also will be shows as $ in stead of as solution cell
    while(solution.size() != 1)
    {
        // mark solution on the labirynth, starting from the end cell and ending on the start cell
        labirynth[solution.top().x][solution.top().y].type = SOLUTION;

        // for the written solution, write opposite direction to the one the parent cell was at
        switch(solution.top().parentDirection) {
            case UP: {
                writtenSolutionSign = 'D';
                break;
            }
            case DOWN: {
                writtenSolutionSign = 'G';
                break;
            }
            case LEFT: {
                writtenSolutionSign = 'P';
                break;
            }
            case RIGHT: {
                writtenSolutionSign = 'L';
                break;
            }
        }

        solution.pop();
        writtenSolution.push_back(writtenSolutionSign);
    }

    // add start node to written solution, because of former .pop()
    writtenSolution.push_back('S');
}

void checkDirections(Cell &cell, Labirynth &labirynth)
{
    /*
        for every direction possible for the cell, check if adjacent cell is CORRIDOR or the END
        and if this direction was not already searched, add it to cells possible directions
        if adjacent cell is END cell, remove all other possible directions, and make the cell go to it,
        as to not make unneeded moves, if this direction is not drawn during random selection
    */
    CellType bottomCellType = labirynth[cell.x + 1][cell.y].type;
    if ((bottomCellType == CORRIDOR || bottomCellType == END) && !cell.searchedDirections.count(DOWN))
    {
        if (bottomCellType == END)
            cell.possibleDirections.clear();
        cell.possibleDirections.insert(DOWN);
        if (bottomCellType == END)
            return;
    }

    CellType topCellType = labirynth[cell.x - 1][cell.y].type;
    if ((topCellType == CORRIDOR || topCellType == END) && !cell.searchedDirections.count(UP))
    {
        if (topCellType == END)
            cell.possibleDirections.clear();
        cell.possibleDirections.insert(UP);
        if (topCellType == END)
            return;
    }

    CellType leftCellType = labirynth[cell.x][cell.y - 1].type;
    if ((leftCellType == CORRIDOR || leftCellType == END) && !cell.searchedDirections.count(LEFT))
    {
        if (leftCellType == END)
            cell.possibleDirections.clear();
        cell.possibleDirections.insert(LEFT);
        if (leftCellType == END)
            return;
    }

    CellType rightCellType = labirynth[cell.x][cell.y + 1].type;
    if ((rightCellType == CORRIDOR || rightCellType == END) && !cell.searchedDirections.count(RIGHT))
    {
        if (rightCellType == END)
            cell.possibleDirections.clear();
        cell.possibleDirections.insert(RIGHT);
        if (rightCellType == END)
            return;
    }
}

void getUserInput(int *height, int *width)
{
    cout << "Podaj dwie dodatnie, nieparzyste liczby, calkowite, nie mniejsze niz 3, wyznaczajace kolejno wysokosc i szerokosc labiryntu:" << endl;

    bool isInputCorrect = false;

    // prompt the user to provide correct height and width of the labirynth, as long as needed for successful input
    do
    {
        cin >> *height >> *width;

        isInputCorrect = checkInput(*height, *width);

        if (!isInputCorrect)
        {
            cout << "Niepoprawne dane, podaj ponownie wysokosc i szerokosc labiryntu: " << endl;
        }

    }
    while (!isInputCorrect);
}

bool checkInput(int height, int width)
{
    // check if input is correct, that both numbers are odd and greater or equal to 3
    return height > 0 && width > 0 && height % 2 == 1 && width % 2 == 1 && height >= 3 && width >= 3;
}

Labirynth generateLabirynth(int height, int width)
{
    Labirynth labirynth = initLabirynth(height, width);

    RowType rowType = NO_ROW;

    for(int i=1; i<height; i+=2)
    {
        rowType = getRowType(i, height - 2);

        // set labirynth each row of the labirynth, each combined of 3 other rows (top walls, passage, and bottom walls)
        setRow(labirynth[i], labirynth[i-1], labirynth[i+1], rowType, height == 3);
    }

    return labirynth;
}

Labirynth initLabirynth(int height, int width)
{
    // init each cell of the labirynth so it will store its coordinates, and so that the initial walls and passages will be in correct places
    Labirynth lab = Labirynth(height, Row(width));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            lab[i][j] = Cell(i, j);
            if (i%2 == 1 && j%2 == 1)
                lab[i][j].type = CORRIDOR;
            else
                lab[i][j].type = WALL;
        }
    }

    return lab;
}

// set row type for future reference
RowType getRowType(int currentIndex, int maxIndex)
{
    if (currentIndex == 1)
        return FIRST;
    else if (currentIndex == maxIndex)
        return LAST;
    else
        return STANDARD;
}

void setRow(Row &row, Row &previousRow, Row &nextRow, RowType rowType, bool shortestPossibleLabirynth)
{
    // if first row, set starting point at random point at the top of the labirynth
    if (rowType == FIRST)
    {
        int entrance = getRandomIntFromRange(1, previousRow.size() - 2);

        // if entrance is not above valid corridor, move it one place to the right
        if (entrance % 2 == 0) {
            entrance++;
        }

        previousRow[entrance].type = START;
        previousRow[entrance].currentSet = 1;
    }

    // Standard row creation

    // set created to know what set numbers are already in use
    set<int> usedSetNumbers;
    usedSetNumbers.clear();

    // if cell in previous row has down passage, assign cell beneath it to the same set
    for(int i = 1; i< row.size() - 1; i+=2)
    {
        if(previousRow[i].type == CORRIDOR || previousRow[i].type == START)
        {
            row[i].currentSet = previousRow[i].currentSet;
            usedSetNumbers.insert(row[i].currentSet);
        }
        else
        {
            row[i].currentSet = NO_SET_ASSIGNED;
        }
    }

    // if cell don't belong to any set, assign it to its unique set
    for(int i = 1; i< row.size() - 1; i+=2)
    {
        row[i].currentSet = getNewSetNumber(row[i].currentSet, usedSetNumbers);
        usedSetNumbers.insert(row[i].currentSet);
    }

    // Randomly generate passages or leave walls between adjacent row cells
    for(int i = 1; i< row.size() - 3; i+=2)
    {
        // if adjacent cells don't belong to the same set
        if(row[i].currentSet != row[i+2].currentSet)
        {
            bool joinAdjacentCellsSets = generateRandomBool();
            if (joinAdjacentCellsSets)
            {
                // save index and set of both cell to join and current cell for further reference
                int nextCellIndex = i+2;
                const int currentSet = row[i].currentSet;
                const int setBeingJoined = row[nextCellIndex].currentSet;

                // if any cell down the row belong to same set as cell being joined, set its set to new one as well
                for(int j = 1; j< row.size() - 1; j+=2)
                {
                    if (row[j].currentSet == setBeingJoined)
                    {
                        row[j].currentSet = currentSet;
                    }
                }
                // 'break' wall between cells
                row[i+1].type = CORRIDOR;
            }
        }
        // if cells are in same set, leave wall between them to prevent loops in the maze
    }

    /*
    Only difference between standard and last row creation
        For last row, after randomly joining or dividing adjacent cells, knock down walls between cells
        not in the same set, join later cell to first cell set, so that finally, all the cells will be
        in one, big set
    */
    if (rowType == LAST || shortestPossibleLabirynth)
    {
        int lastSet = row[1].currentSet;
        set<int> setsAlreadyJoined;

        for(int i = 3; i< row.size() - 1; i+=2)
        {
            if(row[i].currentSet != lastSet)
            {
                row[i-1].type = CORRIDOR;
                setsAlreadyJoined.insert(row[i].currentSet);

                for (int j = i; j < row.size() - 1; j++)
                {
                    if(setsAlreadyJoined.count(row[j].currentSet))
                        row[j].currentSet = lastSet;
                }
            }
        }

        // add exit point at random place in last row
        int exit = getRandomIntFromRange(1, previousRow.size() - 2);

        // if exit is not below valid corridor, move it one place to the right
        if (exit % 2 == 0) {
            exit++;
        }

        nextRow[exit].type = END;
    }
    else
    {
        /*
            if row is not last, after randomly joining or dividing adjacent cells
            randomly generate bottom passages (every set has to have at least one such passage)
        */
        bool setHasBottomCorridor = false;
        set<int> setsWithBottomPassage;

        // make two loops...
        for(int l = 0; l<2; l++)
        {
            // ...through the whole row
            for(int i = 1; i< row.size() - 1; i+=2)
            {
                bool makeBottomPassage = generateRandomBool();

                /*
                    in first passage make random corridors to the next row, in next passage,
                    only make one if whole set don't have any connection to the bottom
                */
                if ((l == 0 && makeBottomPassage) || (l == 1 && !setsWithBottomPassage.count(row[i].currentSet)))
                {
                    nextRow[i].type = CORRIDOR;
                    nextRow[i].currentSet = row[i].currentSet;
                    setsWithBottomPassage.insert(row[i].currentSet);
                }

            }
        }
    }
}

int getNewSetNumber(int currentSetNumber, set<int> &usedNumbers)
{
    // if cell is signed to the set, return this set number
    if (currentSetNumber != NO_SET_ASSIGNED)
        return currentSetNumber;
    else
    {
        // otherwise, find and return new unique set number
        int newSetNumber = currentSetNumber;
        do
        {
            newSetNumber ++;
        }
        while (usedNumbers.count(newSetNumber));

        return newSetNumber;
    }
}

bool generateRandomBool()
{
    /*
        cant do something like getRandomIntFromRange(0,1) == 1;, because c++ pseudo random
        generator is even less random for the smallest bit, which will be checked in that case
    */
    return getRandomIntFromRange(0,100) % 2 == 0;
}

void showLabirynth(Labirynth &labirynth)
{
    cout << endl;

    // for every cell in the labirynth, show correct sign according to its CellType
    for (int i=0; i < labirynth.size(); i++)
    {
        for(int j=0; j < labirynth[0].size(); j++)
        {
            switch (labirynth[i][j].type)
            {
            case SOLUTION :
            {
                cout << ".";
                break;
            }
            case WALL :
            {
                cout << "#";
                break;
            }
            case CORRIDOR :
            {
                cout << " ";
                break;
            }
            case START :
            {
                cout << "$";
                break;
            }
            case END :
            {
                cout << "@";
                break;
            }
            case NOT_SET :
            {
                cout << " ";
                break;
            }
            }
        }
        cout << endl;
    }
}

int getRandomIntFromRange(int rangeStart, int rangeEnd)
{
    return rand() % ((rangeEnd - rangeStart) + 1) + rangeStart;
}
