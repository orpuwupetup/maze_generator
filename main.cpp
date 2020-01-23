#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>

using namespace std;

enum CellType {CORRIDOR, WALL, START, END, NOT_SET};
enum RowType {FIRST, STANDARD, LAST, NO_ROW};

const int NO_SET_ASSIGNED = -1;

struct Cell
{
    int x, y, currentSet = NO_SET_ASSIGNED;
    CellType type = NOT_SET;

    Cell() {}
    Cell(int a, int b)
    {
        this -> x = a;
        this -> y = b;
    }
};

typedef vector<Cell> Row;
typedef vector<Row> Labirynth;

void getUserInput(int *height, int *width);
bool checkInput(int height, int width);

int getNewSetNumber(int currentSetNumber, set<int> &usedNumbers);
bool generateRandomBool();
Labirynth generateLabirynth(int height, int width);
Labirynth initLabirynth(int height, int width);
void showLabirynth(Labirynth &labirynth);
void showRow(Row row);

RowType getRowType(int currentIndex, int maxIndex);
void setRow(Row &row, Row &previousRow, Row &nextRow, RowType rowType, bool shortestPossibleLabirynth = false);

int getRandomIntFromRange(int rangeStart, int rangeEnd);

int main()
{
    srand(time(NULL));

    int h,w;
    getUserInput(&h, &w);

    cout << "Wysokosc to " << h << ", a szerokosc to: " << w << endl;

    Labirynth labirynth = generateLabirynth(h, w);

    showLabirynth(labirynth);


    return 0;
}

void getUserInput(int *height, int *width)
{
    cout << "Podaj dwie dodatnie, nieparzyste liczby, calkowite, nie mniejsze niz 3, wyznaczajace kolejno wysokosc i szerokosc labiryntu:" << endl;

    bool isInputCorrect = false;
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
    return height > 0 && width > 0 && height % 2 == 1 && width % 2 == 1 && height >= 3 && width >= 3;
}

Labirynth generateLabirynth(int height, int width)
{

    Labirynth labirynth = initLabirynth(height, width);

    RowType rowType = NO_ROW;

    for(int i=1; i<height; i+=2)
    {
        rowType = getRowType(i, height - 2);

        setRow(labirynth[i], labirynth[i-1], labirynth[i+1], rowType, height == 3);
    }

    return labirynth;
}

Labirynth initLabirynth(int height, int width)
{
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

RowType getRowType(int currentIndex, int maxIndex)
{
    if (currentIndex == 1)
        return FIRST;
    else if (currentIndex == maxIndex)
        return LAST;
    else
        return STANDARD;
}

// DELETE LATER    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void showRow(Row row)
{
    cout << endl;
    for (int i = 0; i < row.size(); i++)
    {
        Cell cell = row[i];

        if (cell.type == WALL)
        {
            cout << "#";
        }
        else if (cell.type == START)
        {
            cout << "$";
        }
        else if (cell.type == END)
        {
            cout << "@";
        }
        else
        {
            if(cell.currentSet == NO_SET_ASSIGNED)
            {
                cout << " ";
            }
            else
            {
                cout << cell.currentSet;
            }
        }
    }
    cout << endl;
}

void setRow(Row &row, Row &previousRow, Row &nextRow, RowType rowType, bool shortestPossibleLabirynth)
{
    if (rowType == FIRST)
    {
        int entrance = getRandomIntFromRange(1, previousRow.size() - 2);

        previousRow[entrance].type = START;
        previousRow[entrance].currentSet = 1;
        if(entrance %2 == 0)
            row[entrance].type = CORRIDOR;
    }

    // Standard row creation
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

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cout << endl << "after assigning set from previous row \nprevious row is:   " << previousRow[0].x  ;
    showRow(previousRow);
    cout << "current row is:    " << row[0].x ;
    showRow(row);
    cout << "next row is:    " << nextRow[0].x;
    showRow(nextRow);
    cout << endl;

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



    // if cell don't belong to any set, assign it to completely new set
    for(int i = 1; i< row.size() - 1; i+=2)
    {
        row[i].currentSet = getNewSetNumber(row[i].currentSet, usedSetNumbers);
        usedSetNumbers.insert(row[i].currentSet);
    }

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cout << endl << "After assigning each corridor cell new set number, different from any other \nprevious row is:   " << previousRow[0].x  ;
    showRow(previousRow);
    cout << "current row is:    " << row[0].x ;
    showRow(row);
    cout << "next row is:    " << nextRow[0].x;
    showRow(nextRow);
    cout << endl;

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

                for(int j = 1; j< row.size() - 1; j+=2)
                {
                    if (row[j].currentSet == setBeingJoined)
                    {
                        row[j].currentSet = currentSet;
                    }
                }
                row[i+1].type = CORRIDOR;
            }
        }
    }

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    cout << endl << "After joining or dividing adjacent cells\nprevious row is:   " << previousRow[0].x  ;
    showRow(previousRow);
    cout << "current row is:    " << row[0].x ;
    showRow(row);
    cout << "next row is:    " << nextRow[0].x;
    showRow(nextRow);
    cout << endl;

    // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    // Only difference between standard and last row creation
    if (rowType == LAST || shortestPossibleLabirynth)
    {
        int lastSet = row[1].currentSet;
        set<int> setsAlreadyJoined;
        // Knock down walls between cells in different sets
        for(int i = 3; i< row.size() - 1; i+=2)
        {
            if(row[i].currentSet != lastSet) {
                row[i-1].type = CORRIDOR;
                setsAlreadyJoined.insert(row[i].currentSet);

                for (int j = i; j < row.size() - 1; j++)
                {
                    if(setsAlreadyJoined.count(row[j].currentSet))
                        row[j].currentSet = lastSet;
                }
            }
//
//
//            if (row[i + 2].currentSet != row[i].currentSet)
//            {
//                row[i+1].type = CORRIDOR;
//                for (int j = i+2; j < row.size() - 1; j++)
//                {
//                    if (row[j].currentSet == row[i+2].currentSet)
//                        row[j].currentSet = row[i].currentSet;
//                }
//            }
        }

        int exit = getRandomIntFromRange(1, previousRow.size() - 2);
        nextRow[exit].type = END;
        if(exit %2 == 0)
            row[exit].type = CORRIDOR;


        // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        cout << endl << "After knocking walls in last row\nprevious row is:   " << previousRow[0].x  ;
        showRow(previousRow);
        cout << "current row is:    " << row[0].x ;
        showRow(row);
        cout << "next row is:    " << nextRow[0].x;
        showRow(nextRow);
        cout << endl;


        cout << " !!!!!!!!!!!!!!!!!!!!!!!!! END FOR LAST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    } else {

        // randomly generate bottom passages (every set has to have at least one such passage)
        bool setHasBottomCorridor = false;
        set<int> setsWithBottomPassage;

        // make two passages
        for(int l = 0; l<2; l++)
        {
            // through the whole row
            for(int i = 1; i< row.size() - 1; i+=2)
            {
                bool makeBottomPassage = generateRandomBool();

                // in first passage make random corridors to the next row, in next passage, only make one if whole set don't have any connection to the bottom
                if ((l == 0 && makeBottomPassage) || (l == 1 && !setsWithBottomPassage.count(row[i].currentSet)))
                {
                    nextRow[i].type = CORRIDOR;
                    nextRow[i].currentSet = row[i].currentSet;
                    setsWithBottomPassage.insert(row[i].currentSet);
                }

            }
        }

        // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        cout << endl << "After making random bottom passages\nprevious row is:   " << previousRow[0].x  ;
        showRow(previousRow);
        cout << "current row is:    " << row[0].x ;
        showRow(row);
        cout << "next row is:    " << nextRow[0].x;
        showRow(nextRow);
        cout << endl;


        cout << " !!!!!!!!!!!!!!!!!!!!!!!!! END FOR STANDARD !!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        // DELETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}

int getNewSetNumber(int currentSetNumber, set<int> &usedNumbers)
{
    if (currentSetNumber != NO_SET_ASSIGNED)
        return currentSetNumber;
    else
    {
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
    return getRandomIntFromRange(0,100) % 2 == 0;
}

void showLabirynth(Labirynth &labirynth)
{
    cout << endl;

    for (int i=0; i < labirynth.size(); i++)
    {
        for(int j=0; j < labirynth[0].size(); j++)
        {
            switch (labirynth[i][j].type)
            {
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
        cout << i << endl;
    }
}

int getRandomIntFromRange(int rangeStart, int rangeEnd)
{
    return rand() % ((rangeEnd - rangeStart) + 1) + rangeStart;
}
