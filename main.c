#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DECK_SIZE 52
#define ERR 0
#define END 1
#define AGN 2

const unsigned short p = 24419;
const unsigned short q = 29243;
const unsigned int m = 714084817;
const unsigned char iterations = 6;

unsigned char parity(unsigned long long number);
unsigned char bbs(unsigned long long* x);
void swap(unsigned char* a, unsigned char* b);
void shuffle(unsigned char* deck, unsigned long long* x);

typedef struct lista {
    unsigned char info;
    struct lista* next;
    struct lista* prev;
} Lista;

void add_front(Lista** head, unsigned char info);
void add_back(Lista** head, unsigned char info);
unsigned char remove_front(Lista** head);
unsigned char remove_back(Lista** head);
unsigned char look_front(Lista* head);
unsigned char look_back(Lista* head);
unsigned char list_size(Lista* head);
bool list_is_empty(Lista* head);

void push(Lista** stack, unsigned char info);
unsigned char pop(Lista** stack);
unsigned char peek(Lista* stack);
bool stack_is_empty(Lista* stack);

void clearScreen();
void startScreen();
void winScreen(unsigned char player);
void printScreen(unsigned char size1, unsigned char size2, unsigned char curr1, unsigned char curr2, unsigned char o1, unsigned char o2, unsigned char sredina);
void printTurn(unsigned char turn);
void printMenuTake();
void printMenuPlace();

void switchTurn(unsigned char* turn);
unsigned long long getSeed();
bool checkWin(Lista* zatvoren, Lista* otvoren);
void freeAll(Lista* otvoreni1, Lista* zatvoreni1, Lista* otvoreni2, Lista* zatvoreni2, Lista* sredina);
void flip(Lista** otvoreni, Lista** zatvoreni);

int main() {

    unsigned char code;
    unsigned char turn;
    unsigned char currCard[2] = {0, 0};
    bool pobeda;
    Lista* zatvoreni[2];
    Lista* otvoreni[2];
    Lista* sredina;

    do {
        code = 0;
        turn = 0;
        pobeda = false;
        currCard[0] = 0;
        currCard[1] = 0;
        zatvoreni[0] = NULL;
        zatvoreni[1] = NULL;
        otvoreni[0] = NULL;
        otvoreni[1] = NULL;
        sredina = NULL;

        startScreen();
        getchar();

        unsigned char deck[DECK_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,1,2,3,4,5,6,7,8,9,10,11,12,13,1,2,3,4,5,6,7,8,9,10,11,12,13,1,2,3,4,5,6,7,8,9,10,11,12,13};

        unsigned long long seed = getSeed();
        shuffle(deck, &seed);

        for (size_t i = 0; i < DECK_SIZE / 2; ++i) {
            add_back(&zatvoreni[0], deck[i]);
        }

        for (size_t i = DECK_SIZE / 2; i < DECK_SIZE; ++i) {
            add_back(&zatvoreni[1], deck[i]);
        }

        while (!pobeda) {
            code = 0;
            int izbor;
            bool greska = false;
            bool turnEnd = false;

            if (zatvoreni[turn] == NULL) flip(&(otvoreni[turn]), &(zatvoreni[turn]));

            do {
                do {
                    do {
                        clearScreen();
                        printScreen(list_size(zatvoreni[0]), list_size(zatvoreni[1]), currCard[0], currCard[1], peek(otvoreni[0]), peek(otvoreni[1]), peek(sredina));
                        printf("\n");
                        printTurn(turn);
                        printf("\n");
                        printMenuTake();
                        scanf("%d", &izbor);
                    } while (izbor < 1 || izbor > 4);

                    switch (izbor) {
                        case 1: {
                            if (!list_is_empty(zatvoreni[turn])) {
                                currCard[turn] = remove_back(&(zatvoreni[turn]));
                                greska = false;
                            } else {
                                greska = true;
                            }
                            break;
                        }
                        case 2: {
                            if (!stack_is_empty(otvoreni[turn])) {
                                currCard[turn] = pop(&(otvoreni[turn]));
                                greska = false;
                            } else {
                                greska = true;
                            }
                            break;
                        }
                        case 3: {
                            greska = false;
                            code = AGN;
                            break;
                        }
                        case 4: {
                            greska = false;
                            code = END;
                            break;
                        }
                    }
                    if (code) break;

                } while (greska);

                if (code) break;

                do {
                    do {
                        clearScreen();
                        printScreen(list_size(zatvoreni[0]), list_size(zatvoreni[1]), currCard[0], currCard[1], peek(otvoreni[0]), peek(otvoreni[1]), peek(sredina));
                        printf("\n");
                        printTurn(turn);
                        printf("\n");
                        printMenuPlace();
                        scanf("%d", &izbor);
                    } while (izbor < 1 || izbor > 4);

                    switch (izbor) {
                        case 1: {
                            if (peek(sredina) % 13 == currCard[turn] - 1) {
                                push(&sredina, currCard[turn]);
                                greska = false;
                            } else {
                                greska = true;
                            }
                            break;
                        }
                        case 2: {
                            unsigned char temp = currCard[turn];
                            switchTurn(&turn);

                            if (peek(otvoreni[turn]) % 13 == temp - 1) {
                                push(&(otvoreni[turn]), temp);
                                switchTurn(&turn);
                                greska = false;
                            } else {
                                switchTurn(&turn);
                                greska = true;
                            }
                            break;
                        }
                        case 3: {
                            push(&(otvoreni[turn]), currCard[turn]);
                            greska = false;
                            turnEnd = true;
                            break;
                        }
                        case 4: {
                            greska = false;
                            code = END;
                            break;
                        }
                    }
                    if (code) break;
                } while (greska);

                if (code) break;
                currCard[turn] = 0;
            } while (!turnEnd);

            if (code) break;

            pobeda = checkWin(zatvoreni[turn], otvoreni[turn]);
            switchTurn(&turn);
            clearScreen();
        }

        freeAll(otvoreni[0], zatvoreni[0], otvoreni[1], zatvoreni[1], sredina);

    } while (code == AGN);

    if (code != END) {
        clearScreen();
        switchTurn(&turn);
        winScreen(turn + 1);
    }

    return 0;
}


unsigned char parity(unsigned long long number) {
    unsigned char count = 0;
    while (number) {
        count += number & 1;
        number = number >> 1;
    }
    return count % 2;
}

unsigned char bbs(unsigned long long* x) {
    unsigned char bit;
    unsigned char result = 0;

    for (size_t i = 0; i < iterations; i++) {
        *x = (unsigned long long)((__uint128_t)(*x) * (*x) % m);
        bit = parity(*x);
        result = result << 1;
        result = result | bit;
    }

    return result;
}

void swap(unsigned char* a, unsigned char* b) {
    unsigned char t = *a;
    *a = *b;
    *b = t;
}

void shuffle(unsigned char* deck, unsigned long long* x) {
    unsigned char rand;

    for (size_t i = 0; i < DECK_SIZE; i++) {
        do {
            rand = bbs(x);
        } while (rand >= DECK_SIZE);
        swap(&deck[i], &deck[rand]);
    }
}

void add_front(Lista** head, unsigned char info) {
    Lista* element = calloc(1, sizeof(Lista));
    element->info = info;
    element->prev = NULL;
    element->next = *head;

    if (*head != NULL) (*head)->prev = element;
    *head = element;
}

void add_back(Lista** head, unsigned char info) {
    Lista* element = calloc(1, sizeof(Lista));
    element->info = info;
    element->next = NULL;

    if (*head == NULL) {
        element->prev = NULL;
        *head = element;
    } else {
        Lista* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = element;
        element->prev = temp;
    }
}

unsigned char remove_front(Lista** head) {
    if (*head == NULL)
        return ERR;

    Lista* temp = *head;
    unsigned char info = temp->info;
    *head = (*head)->next;

    if (*head != NULL) (*head)->prev = NULL;

    free(temp);
    return info;
}

unsigned char remove_back(Lista** head) {
    if (*head == NULL)
        return ERR;

    Lista* temp = *head;
    unsigned char info;

    while (temp->next != NULL) {
        temp = temp->next;
    }

    if (temp->prev != NULL) temp->prev->next = NULL;
    else *head = NULL;

    info = temp->info;
    free(temp);
    return info;
}

unsigned char look_front(Lista* head) {
    if (head == NULL)
        return ERR;
    return head->info;
}

unsigned char look_back(Lista* head) {
    if (head == NULL)
        return ERR;

    Lista* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp->info;
}

unsigned char list_size(Lista* head) {
    Lista* temp = head;
    unsigned char n = 0;

    while (temp != NULL) {
        n++;
        temp = temp->next;
    }
    return n;
}

bool list_is_empty(Lista* head) {
    return head == NULL;
}

void push(Lista** stack, unsigned char info) {
    Lista* element = calloc(1, sizeof(Lista));
    element->info = info;
    element->next = *stack;
    *stack = element;
}

unsigned char pop(Lista** stack) {
    if (stack == NULL)
        return ERR;

    Lista* temp = *stack;
    unsigned char info = temp->info;

    *stack = temp->next;
    free(temp);
    return info;
}

unsigned char peek(Lista* stack) {
    if (stack == NULL)
        return ERR;
    return stack->info;
}

bool stack_is_empty(Lista* stack) {
    return stack == NULL;
}

void cardToStr(unsigned char card, char* out) {
    if (card == 0) {
        snprintf(out, 3, " ");
        return;
    }

    unsigned char v = card % 13;
    if (v == 0) v = 13;

    switch (v) {
        case 1:  snprintf(out, 3, "A"); break;
        case 11: snprintf(out, 3, "J"); break;
        case 12: snprintf(out, 3, "Q"); break;
        case 13: snprintf(out, 3, "K"); break;
        default: snprintf(out, 3, "%u", v); break;
    }
}

void printScreen(unsigned char size1, unsigned char size2, unsigned char curr1, unsigned char curr2, unsigned char o1, unsigned char o2, unsigned char sredina) {

    char c1[3], c2[3], o1s[3], o2s[3], s[3];

    cardToStr(curr1, c1);
    cardToStr(curr2, c2);
    cardToStr(o1, o1s);
    cardToStr(o2, o2s);
    cardToStr(sredina, s);

    printf("_______________________________________________________________________________________________________________________\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                  _____________            _____________            _____________                                    |\n");
    printf("|                  |           |            |           |            |           |                                    |\n");
    printf("|                  |           |            |%2s         |            |%2s         |                                    |\n", c1, o1s);
    printf("|    Igrac 1 :     |           |            |           |            |           |                                    |\n");
    printf("|                  |     %02u    |            |    %2s     |            |    %2s     |                                    |\n", size1, c1, o1s);
    printf("|                  |           |            |           |            |           |                   sredina          |\n");
    printf("|                  |           |            |        %2s |            |        %2s |                _____________       |\n", c1, o1s);
    printf("|                  |___________|            |___________|            |___________|                |           |       |\n");
    printf("|                                                                                                 |%2s         |       |\n", s);
    printf("|                                                                                                 |           |       |\n");
    printf("|                zatvoreni spilovi          trenutna karta          otvoreni spilovi              |    %2s     |       |\n", s);
    printf("|                                                                                                 |           |       |\n");
    printf("|                  _____________            _____________            _____________                |        %2s |       |\n", s);
    printf("|                  |           |            |           |            |           |                |___________|       |\n");
    printf("|                  |           |            |%2s         |            |%2s         |                                    |\n", c2, o2s);
    printf("|                  |           |            |           |            |           |                                    |\n");
    printf("|                  |     %02u    |            |    %2s     |            |    %2s     |                                    |\n", size2, c2, o2s);
    printf("|    Igrac 2 :     |           |            |           |            |           |                                    |\n");
    printf("|                  |           |            |        %2s |            |        %2s |                                    |\n", c2, o2s);
    printf("|                  |___________|            |___________|            |___________|                                    |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|_____________________________________________________________________________________________________________________|\n");
}

void startScreen() {
    printf("_______________________________________________________________________________________________________________________\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                            ________________  _________                                              |\n");
    printf("|                                            \\__    ___/  _  \\ \\_   ___ \\                                             |\n");
    printf("|                                              |    | /  /_\\  \\/    \\  \\/                                             |\n");
    printf("|                                              |    |/    |    \\     \\____                                            |\n");
    printf("|                                              |____|\\____|____/\\________/                                            |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                               Klikni enter da zapocnes...                                           |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|_____________________________________________________________________________________________________________________|\n");
    printf("\n\n");
}

void winScreen(unsigned char player) {
    printf("_______________________________________________________________________________________________________________________\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                            Igrac %hhu je pobednik!!!                                                   |\n", player);
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|                                                                                                                     |\n");
    printf("|_____________________________________________________________________________________________________________________|\n");
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printTurn(unsigned char turn) {
    printf("____________________________\n");
    printf("|                          |\n");
    printf("| Na potezu je : Igrac %hhu   |\n", turn + 1);
    printf("|__________________________|\n");
}

void printMenuTake() {
    printf("1 -- Vuci kartu sa sopstvenog zatvorenog spila\n");
    printf("2 -- Vuci kartu sa sopstvenog otvorenog spila\n");
    printf("3 -- Restartuj igru\n");
    printf("4 -- Izadji iz aplikacije\n");
    printf("\n");
    printf("Unesi izbor: ");
}

void printMenuPlace() {
    printf("1 -- Postavi izvucenu kartu na sredinu\n");
    printf("2 -- Postavi izvucenu kartu kod protivnika\n");
    printf("3 -- Postavi izvucenu kartu na sopstveni otvoreni spil\n");
    printf("4 -- Izadji iz aplikacije\n");
    printf("\n");
    printf("Unesi izbor: ");
}

void switchTurn(unsigned char* turn) {
    *turn = (*turn == 0) ? 1 : 0;
}

unsigned long long getSeed() {
    long long seed;

    do {
        clearScreen();

        printf("_______________________________________________________________________________________________________________________\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                 Unesi klicu za generator pseudoslucajnih brojeva                                    |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                          ( pozitivan broj veci od 1 )                                               |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|                                                                                                                     |\n");
        printf("|_____________________________________________________________________________________________________________________|\n");
        printf("\n");
        printf("Klica : ");
        scanf("%lld", &seed);

    } while (seed <= 1);

    clearScreen();
    return (unsigned long long)seed;
}

bool checkWin(Lista* zatvoren, Lista* otvoren) {
    return list_is_empty(zatvoren) && stack_is_empty(otvoren);
}

void freeAll(Lista* otvoreni1, Lista* zatvoreni1, Lista* otvoreni2, Lista* zatvoreni2, Lista* sredina) {
    while (otvoreni1 != NULL)  pop(&otvoreni1);
    while (otvoreni2 != NULL)  pop(&otvoreni2);
    while (zatvoreni1 != NULL) remove_front(&zatvoreni1);
    while (zatvoreni2 != NULL) remove_front(&zatvoreni2);
    while (sredina != NULL)    pop(&sredina);
}

void flip(Lista** otvoreni, Lista** zatvoreni) {
    while (*otvoreni != NULL) {
        add_back(zatvoreni, pop(otvoreni));
    }
}
