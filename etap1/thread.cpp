#include <iostream>
#include <thread>
#include <ncurses.h>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
using namespace std::literals::chrono_literals;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;
//-----------------

const int BOARD_WIDTH = 60;
const char EMPTY_PLACE = '.';
const char BG_CHAR = '~';
const int VELOCITY_MULTIPLIER = 3;


const int RIGHT_CORNER = BOARD_WIDTH * 2 / 3 + 1;
const int LEFT_CORNER = RIGHT_CORNER / 2;


void print_roads();
void print_board(char *matrix[BOARD_WIDTH]);


class Car
{
public:
    char id;
    float velocity;
    char **board;
    int test;

    Car(char id, float velocity, char *matrix[BOARD_WIDTH], int test)
    {
        this->id = id;
        this->velocity = 10000000 /velocity * VELOCITY_MULTIPLIER;
        this->board = matrix;
        this->test = test;
    }

    void drive()
    {
        while (true)
        {
            for (int i = LEFT_CORNER+1; i <= RIGHT_CORNER; i++) //go right
            {
                while (board[0][i] != EMPTY_PLACE ) //&& i < RIGHT_CORNER
                {
                    usleep(velocity);
                }

                board[0][i] = this->id;

                if (i > 0 && board[0][i - 1] == this->id)
                {
                    board[0][i - 1] = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = 1; i < BOARD_WIDTH; i++) // go down
            {
                while (board[i][RIGHT_CORNER] != EMPTY_PLACE)
                    usleep(velocity);

                board[i][RIGHT_CORNER] = this->id;

                if (i > 0 && board[i - 1][RIGHT_CORNER] == this->id)
                {
                    board[i - 1][RIGHT_CORNER] = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go left
            {
                while (board[BOARD_WIDTH - 1][i] != EMPTY_PLACE)
                {
                    usleep(velocity);
                }

                board[BOARD_WIDTH - 1][i] = this->id;

                if (i > 0 && board[BOARD_WIDTH - 1][i + 1] == this->id)
                {
                    board[BOARD_WIDTH - 1][i + 1] = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go up
            {
                while (board[i][LEFT_CORNER] != EMPTY_PLACE ){
                    usleep(velocity);
                    
                }

                board[i][LEFT_CORNER] = this->id;

                if (i < BOARD_WIDTH - 1 && board[i + 1][LEFT_CORNER] == this->id)
                {
                    board[i + 1][LEFT_CORNER] = EMPTY_PLACE;
                }
                usleep(velocity);
            }
        }
    }

    void drive2()
    {
        while (true)
        {
            for (int i = LEFT_CORNER+1; i <= RIGHT_CORNER; i++) //go right
            {
                while (board[i][0] != EMPTY_PLACE ) //&& i < RIGHT_CORNER
                {
                    usleep(velocity);
                }

                board[i][0] = this->id;

                if (i > 0 && board[i-1][0] == this->id)
                {
                    board[i-1][0]  = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = 1; i < BOARD_WIDTH; i++) // go down
            {
                while (board[RIGHT_CORNER][i] != EMPTY_PLACE)
                    usleep(velocity);

                board[RIGHT_CORNER][i] = this->id;

                if (i > 0 && board[RIGHT_CORNER][i - 1] == this->id)
                {
                    board[RIGHT_CORNER][i - 1] = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go left
            {
                while (board[i][BOARD_WIDTH - 1] != EMPTY_PLACE)
                {
                    usleep(velocity);
                }

                board[i][BOARD_WIDTH - 1] = this->id;

                if (i > 0 && board[i + 1][BOARD_WIDTH - 1] == this->id)
                {
                    board[i + 1][BOARD_WIDTH - 1] = EMPTY_PLACE;
                }
                usleep(velocity);
            }

            for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go up
            {
                while (board[LEFT_CORNER][i] != EMPTY_PLACE ){
                    usleep(velocity);
                    
                }

                board[LEFT_CORNER][i] = this->id;

                if (i < BOARD_WIDTH - 1 && board[LEFT_CORNER][i+1] == this->id)
                {
                    board[LEFT_CORNER][i+1] = EMPTY_PLACE;
                }
                usleep(velocity);
            }
        }
    
    }
    
};

void print_board(char *matrix[BOARD_WIDTH])
{
    while (true)
    {
        system("clear");
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            for (int j = 0; j < BOARD_WIDTH; j++)
            {
                cout << matrix[i][j] << " ";
                usleep(3);
            }
            cout << endl;
        }
    }
}

void print_board_curses(char *matrix[BOARD_WIDTH])
{   
    const int WIDTH_MULTIPLIER = 3;



    initscr();
    start_color();
    // init_pair(1, COLOR_GREEN, COLOR_BLACK); //background
    init_pair(1, COLOR_CYAN, COLOR_BLUE); //background
    init_pair(2, COLOR_RED, COLOR_BLACK); //roads
    // init_pair(3, COLOR_GREEN, COLOR_BLACK); //cars


    attron(COLOR_PAIR(1));
    //background characters
    for(int i = 0; i < BOARD_WIDTH; i++){
        for(int j = 0; j < BOARD_WIDTH; j++){
            mvprintw(i, j * WIDTH_MULTIPLIER, "%c", BG_CHAR); // =     move(0,0);     printw("%d", c);
        }
    }


    attron(COLOR_PAIR(2));
    //roads, cars
    while (true)
        {
            for (int i = LEFT_CORNER+1; i <= RIGHT_CORNER; i++) //go right
            {
                mvprintw(0, i * WIDTH_MULTIPLIER, "%c", matrix[0][i]); // =     move(0,0);     printw("%d", c);
                mvprintw(i, 0, "%c", matrix[i][0]); // =     move(0,0);     printw("%d", c);
            }

            for (int i = 1; i < BOARD_WIDTH; i++) // go down
            {
                mvprintw(i, RIGHT_CORNER * WIDTH_MULTIPLIER, "%c", matrix[i][RIGHT_CORNER]);
                mvprintw(RIGHT_CORNER, i * WIDTH_MULTIPLIER, "%c", matrix[RIGHT_CORNER][i]);
            }

            for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go left
            {

                mvprintw(BOARD_WIDTH - 1, i * WIDTH_MULTIPLIER, "%c", matrix[BOARD_WIDTH - 1][i]);
                mvprintw(i, (BOARD_WIDTH - 1)   * WIDTH_MULTIPLIER, "%c", matrix[i][BOARD_WIDTH - 1]);

            }

            for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go up
            {

                mvprintw(i, LEFT_CORNER * WIDTH_MULTIPLIER, "%c", matrix[i][LEFT_CORNER]);
                mvprintw(LEFT_CORNER, i * WIDTH_MULTIPLIER, "%c", matrix[LEFT_CORNER][i]);
            }
            getch();
        }
    attroff(COLOR_PAIR(1));

    endwin();
}


void ncurses_print()
{
    initscr();

    while(true){

    for (int k = 20; k < 120; k++)
    {
        if(k == 119) k = 0;
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 20; j++)
            {
                mvprintw(j, i, "%c", k); // =     move(0,0);     printw("%d", c);
            }
            getch();
        }
    }
    }
    getch();
    endwin();
}

int main()
{
    char **matrix;
    matrix = new char *[BOARD_WIDTH];

    for (int i = 0; i < BOARD_WIDTH; i++)
        matrix[i] = new char[BOARD_WIDTH];

    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            matrix[i][j] = EMPTY_PLACE;
        }
    }

    Car *c3 = new Car('$', 20, matrix, 2);
    thread t_c3(&Car::drive, c3);

    Car *c1 = new Car('X', 40, matrix, 2);
    thread t_c1(&Car::drive, c1);

    Car *c2 = new Car('O', 80, matrix, 2);
    thread t_c2(&Car::drive, c2);

    Car *c4 = new Car('A', 2, matrix, 2);
    thread t_c4(&Car::drive2, c4);

    Car *c5 = new Car('B', 1, matrix, 2);
    thread t_c5(&Car::drive2, c5);

    Car *c6 = new Car('C', 3, matrix, 2);
    thread t_c6(&Car::drive2, c6);

    

    // thread t_printer(print_board, matrix);
    thread t_printer(print_board_curses, matrix);
    // thread t_printer(ncurses_print);
    t_printer.join();

    t_c1.join();
    t_c2.join();
    t_c3.join();
    t_c4.join();
    t_c5.join();
    t_c6.join();

    return 0;
}