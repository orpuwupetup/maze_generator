#include <iostream>

using namespace std;

void getUserInput(int *height, int *width);
bool checkInput(int height, int width);

int main()
{
    int h,w;
    getUserInput(&h, &w);

    cout << "Wysokosc to " << h << ", a szerokosc to: " << w << endl;

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
