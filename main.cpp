#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;

typedef vector<char> Row;
typedef vector<Row> Labirynth;

enum RowType {FIRST, STANDARD, LAST, NO_ROW};

void getUserInput(int *height, int *width);
bool checkInput(int height, int width);

Labirynth generateLabirynth(int height, int width);
void showLabirynth(Labirynth &labirynth);

RowType getRowType(int currentIndex, int maxIndex);
void setRow(Row &row, RowType rowType);

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
    cout << "Podaj dwie dodatnie liczby calkowite wyznaczajace kolejno wysokosc i szerokosc labiryntu:" << endl;

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
    return height > 0 && width > 0;
}

Labirynth generateLabirynth(int height, int width) {

    Labirynth labirynth(height, Row(width));

    RowType rowType = NO_ROW;

    for(int i=0; i<height; i++) {
        rowType = getRowType(i, height - 1);

        setRow(labirynth[i], rowType);
    }

    return labirynth;
}

RowType getRowType(int currentIndex, int maxIndex) {
    if (currentIndex == 0)
        return FIRST;
    else if (currentIndex == maxIndex)
        return LAST;
    else
        return STANDARD;
}

void setRow(Row &row, RowType rowType) {
    for(int i=0; i<row.size(); i++) {
        switch(rowType) {
            case FIRST : {
                row[i] = 'S';
                break;
            }
            case STANDARD : {
                row[i] = 'm';
                break;
            }
            case LAST : {
                row[i] = 'L';
                break;
            }
        }
    }
}

void showLabirynth(Labirynth &labirynth) {
    cout << endl;

    for (Row row : labirynth) {
        for(int i=0; i<row.size(); i++){
            cout << row[i];
        }
        cout << endl;
    }
}

int getRandomIntFromRange(int rangeStart, int rangeEnd) {
    return rand() % ((rangeEnd - rangeStart) + 1) + rangeStart;
}
