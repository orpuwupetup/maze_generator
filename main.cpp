#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>

using namespace std;

enum CellType {CORRIDOR, WALL, START, END, NOT_SET};
enum RowType {FIRST, STANDARD, LAST, NO_ROW};

struct Cell {
    int x, y, currentSet = -1;
    CellType type = NOT_SET;

    Cell(){}
    Cell(int a, int b) {
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

void getUserInput(int *height, int *width) {
    cout << "Podaj dwie dodatnie, nieparzyste liczby, calkowite, nie mniejsze niz 3, wyznaczajace kolejno wysokosc i szerokosc labiryntu:" << endl;

    bool isInputCorrect = false;
    do {
    cin >> *height >> *width;

    isInputCorrect = checkInput(*height, *width);

    if (!isInputCorrect) {
        cout << "Niepoprawne dane, podaj ponownie wysokosc i szerokosc labiryntu: " << endl;
    }

    } while (!isInputCorrect);
}

bool checkInput(int height, int width) {
    return height > 0 && width > 0 && height % 2 == 1 && width % 2 == 1 && height >= 3 && width >= 3;
}

Labirynth generateLabirynth(int height, int width) {

    Labirynth labirynth = initLabirynth(height, width);

    RowType rowType = NO_ROW;

    for(int i=1; i<height; i+=2) {
        rowType = getRowType(i, height - 2);

        setRow(labirynth[i], labirynth[i-1], labirynth[i+1], rowType, height == 3);
    }

    return labirynth;
}

Labirynth initLabirynth(int height, int width) {
    Labirynth lab = Labirynth(height, Row(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++){
            lab[i][j] = Cell(i, j);
            lab[i][j].type = WALL;
        }
    }

    return lab;
}

RowType getRowType(int currentIndex, int maxIndex) {
    if (currentIndex == 1)
        return FIRST;
    else if (currentIndex == maxIndex)
        return LAST;
    else
        return STANDARD;
}

void setRow(Row &row, Row &previousRow, Row &nextRow, RowType rowType, bool shortestPossibleLabirynth) {
        switch(rowType) {
            case FIRST : {
                int entrance = getRandomIntFromRange(1, previousRow.size() - 2);
                int exit = -1;
                if (shortestPossibleLabirynth) {
                    exit = getRandomIntFromRange(1, nextRow.size() - 2);
                }
                cout << endl << entrance << endl;

                for (int a = 0; a < previousRow.size(); a++) {
                    if (a == entrance) {
                        previousRow[a].type = START;
                        previousRow[a].currentSet = 1;
                    } else {
                        previousRow[a].type = WALL;
                    }

                    if (shortestPossibleLabirynth){
                        if (a == exit)
                            nextRow[a].type = END;
                        else
                            nextRow[a].type = WALL;

                        if (a != 0 && a != nextRow.size() - 1)
                            row[a].type = CORRIDOR;
                    }
                }

                if (shortestPossibleLabirynth)
                    return;
            }
            case STANDARD : {
                row = previousRow;
                set<int> usedSetNumbers;
                usedSetNumbers.clear();

                // if cell in previous row has down passage, assign cell beneath it to the same set
                for(int i = 1; i< row.size() - 1; i+=2) {
                    if(previousRow[i].type == CORRIDOR || previousRow[i].type == START){
                        row[i].currentSet = previousRow[i].currentSet;
                        usedSetNumbers.insert(row[i].currentSet);
                    } else {
                        row[i].currentSet = -1;
                    }
                }

                cout << endl << endl << "row without random cells, just from start: ";
                for (int a = 0; a < row.size(); a++)
                    if(row[a].currentSet != -1)
                        cout << row[a].currentSet;
                cout << endl;

                // if cell don't belong to any set, assign it to completely new set
                for(int i = 1; i< row.size() - 1; i+=2) {
                    row[i].currentSet = getNewSetNumber(row[i].currentSet, usedSetNumbers);
                    usedSetNumbers.insert(row[i].currentSet);
                }

                cout << endl << endl << "row with random cells, except one from start: ";
                for (int a = 0; a < row.size(); a++)
                    if(row[a].currentSet != -1)
                        cout << row[a].currentSet;
                cout << endl << endl;

                for(int i = 1; i< row.size() - 3; i+=2) {
                    if(row[i].currentSet == row[i+2].currentSet){
                        row[i+1].type = WALL;
                    } else {
                        // generate random bool to determine if cells should be separated or not
                        if (generateRandomBool()) {
                            row[i+1].type = WALL;
                        } else {
                            int setToMerge = row[i+2].currentSet;
                            int indexToMerge = i;
                            do {
                                indexToMerge += 2;
                                row[indexToMerge].currentSet = row[i].currentSet;
                                i += 2;
                            } while (row[i+2].currentSet == setToMerge && i < row.size() - 1);
                        }
                    }
                }

                bool setHasBottomCorridor = false;
                int currentSet = -1;
                for(int i = 1; i< row.size() - 3; i+=2) {
                    if (row[i].currentSet != currentSet) {
                        currentSet = row[i].currentSet;
                        setHasBottomCorridor = false;
                    }

                    // generate random bool to determine if cell should have bottom wall or not
                    if (row[i+2].currentSet != currentSet && !setHasBottomCorridor) {
                        nextRow[i].type = CORRIDOR;
                        nextRow[i].currentSet = row[i].currentSet;
                    }else {
                        if(generateRandomBool()) {
                            nextRow[i].type = CORRIDOR;
                            nextRow[i].currentSet = row[i].currentSet;
                            setHasBottomCorridor = true;
                        } else {
                            nextRow[i].type = WALL;
                        }
                    }

                }


                break;
            }
            case LAST : {

                break;
            }
        }
}

int getNewSetNumber(int currentSetNumber, set<int> &usedNumbers) {
    if (currentSetNumber != -1)
        return currentSetNumber;
    else {
        int newSetNumber = currentSetNumber;
        do {
            newSetNumber ++;
        } while (usedNumbers.count(newSetNumber));

        return newSetNumber;
    }
}

bool generateRandomBool() {
    return getRandomIntFromRange(0,100) % 2 == 0;
}

void showLabirynth(Labirynth &labirynth) {
    cout << endl;

    for (int i=0; i < labirynth.size(); i++) {
        for(int j=0; j < labirynth[0].size(); j++){
            switch (labirynth[i][j].type) {
                case WALL : {
                    cout << "#";
                    break;
                }
                case CORRIDOR : {
                    cout << ".";
                    break;
                }
                case START : {
                    cout << "$";
                    break;
                }
                case END : {
                    cout << "@";
                    break;
                }
                case NOT_SET : {
                    cout << " ";
                    break;
                }
                }
        }
        cout << endl;
    }
}

int getRandomIntFromRange(int rangeStart, int rangeEnd) {
    return rand() % ((rangeEnd - rangeStart) + 1) + rangeStart;
}
