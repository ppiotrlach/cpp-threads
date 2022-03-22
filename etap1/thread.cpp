#include <iostream>
#include <thread>
#include <vector>
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

const int BOARD_WIDTH = 0.5 * 21;
const char EMPTY_PLACE = '.';
const char BG_CHAR = ' ';
const int VELOCITY_FACTOR = 1;
const float VELOCITY_MULTIPLIER = 0.05 * VELOCITY_FACTOR;

const int CARS_NR2 = 4;
const int CONST_VELOCITY = 500;

const int LAPS_NUMBER = 3;

bool do_work = true;

// const int RIGHT_CORNER = BOARD_WIDTH * 2 / 3 + 1;
// const int LEFT_CORNER = RIGHT_CORNER / 2;

const int RIGHT_CORNER = BOARD_WIDTH * 3 / 4;
const int LEFT_CORNER = BOARD_WIDTH / 4;

void print_roads();
void print_board(char *matrix[BOARD_WIDTH]);

class Car
{
public:
    char id;
    float velocity;
    char **board;

    Car(char id, float velocity, char *matrix[BOARD_WIDTH])
    {
        this->id = id;
        this->velocity = 1000000 / (velocity * VELOCITY_MULTIPLIER);
        this->board = matrix;
    }
    ~Car()
    {
        delete this;
    }

    void drive()
    {
        bool is_after_first_turn = false;
        int random_road = rand() % 4 + 1;

        int rand_time =  rand() % 10000 + 1;

        for (int t = 0; t < rand_time; t++)
        {
            if (!do_work)
            {
                cout << ". ";
                return;
            }
            usleep(100);
        }
        usleep((rand() % 10000 + 1) * 1000);

        while (do_work)
        {
            if (is_after_first_turn || random_road == 1)
            {
                is_after_first_turn = true;
                for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) //go right
                {
                    if (!do_work)
                        break;
                    while (board[0][i] != EMPTY_PLACE) //&& i < RIGHT_CORNER
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
            }
            if (is_after_first_turn || random_road == 2)
            {
                is_after_first_turn = true;
                for (int i = 1; i < BOARD_WIDTH; i++) // go down
                {
                    if (!do_work)
                        break;
                    while (board[i][RIGHT_CORNER] != EMPTY_PLACE)
                        usleep(velocity);

                    board[i][RIGHT_CORNER] = this->id;

                    if (i > 0 && board[i - 1][RIGHT_CORNER] == this->id)
                    {
                        board[i - 1][RIGHT_CORNER] = EMPTY_PLACE;
                    }
                    usleep(velocity);
                }
            }
            if (is_after_first_turn || random_road == 3)
            {
                is_after_first_turn = true;
                for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go left
                {
                    if (!do_work)
                        break;
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
            }
            if (is_after_first_turn || random_road == 3)
            {
                is_after_first_turn = true;
                for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go up
                {
                    if (!do_work)
                        break;
                    while (board[i][LEFT_CORNER] != EMPTY_PLACE)
                    {
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
        cout << ". ";
        return;
    }

    void drive2()
    {
        while (do_work)
        {
            for (int x = 0; x < LAPS_NUMBER; x++)
            {
                if (!do_work)
                    break;

                for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) //go right
                {
                    if (!do_work)
                        break;
                    while (board[i][0] != EMPTY_PLACE)
                    {
                        usleep(velocity);
                    }

                    board[i][0] = this->id;

                    if (i > 0 && board[i - 1][0] == this->id)
                    {
                        board[i - 1][0] = EMPTY_PLACE;
                    }
                    usleep(velocity);
                }

                for (int i = 1; i < BOARD_WIDTH; i++) // go down
                {
                    if (!do_work)
                        break;
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
                    if (!do_work)
                        break;
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
                    if (!do_work)
                        break;
                    while (board[LEFT_CORNER][i] != EMPTY_PLACE)
                    {
                        usleep(velocity);
                    }

                    board[LEFT_CORNER][i] = this->id;

                    if (i < BOARD_WIDTH - 1 && board[LEFT_CORNER][i + 1] == this->id)
                    {
                        board[LEFT_CORNER][i + 1] = EMPTY_PLACE;
                    }
                    usleep(velocity);
                }
            }
            board[LEFT_CORNER][0] = EMPTY_PLACE;
            sleep(rand() % 5 + 1);
            int c = rand() % 25 + 65;        //65-90
            int random_v = rand() % 75 + 75; //75-150

            this->id = c;
            this->velocity = 1000000 / (random_v * VELOCITY_MULTIPLIER);
        }
        cout << ". ";
        return;
    }
};

void print_board(char *matrix[BOARD_WIDTH])
{
    while (do_work)
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

// void spawn_cars_road2(char *matrix[BOARD_WIDTH])
// {
//     int c = 65; //90
//     int random_v;

//     while (true)
//     {
//         c++;
//         if (c > 90)
//             c = 65;

//         random_v = rand() % 150 + 50;
//         // sleep(random_v/200);

//         Car *temp_car = new Car(c, random_v, matrix);
//         thread t_temp(&Car::drive2, temp_car);

//         t_temp.join();
//     }
// }

void print_board_curses(char *matrix[BOARD_WIDTH])
{
    const int WIDTH_MULTIPLIER = 3;

    initscr();
    // start_color();
    // init_pair(1, COLOR_GREEN, COLOR_BLACK); //backgroundprint_board_curses
    // init_pair(2, COLOR_RED, COLOR_BLACK); //roads
    // init_pair(3, COLOR_GREEN, COLOR_BLACK); //cars

    // attron(COLOR_PAIR(1));
    // background characters

    for (int i = 0; i < BOARD_WIDTH; i++)
    {
        if (i == LEFT_CORNER || i == RIGHT_CORNER)
            continue;
        for (int j = 0; j < BOARD_WIDTH * WIDTH_MULTIPLIER; j++)
        {
            // if(j == LEFT_CORNER || j == RIGHT_CORNER) continue;
            mvprintw(i, j, "%c", BG_CHAR); // =     move(0,0);     printw("%d", c);
        }
    }

    // attron(COLOR_PAIR(2));
    //roads, cars
    while (do_work)
    {
        for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) //go right
        {
            mvprintw(0, i * WIDTH_MULTIPLIER, "%c", matrix[0][i]); // =     move(0,0);     printw("%d", c);
            mvprintw(i, 0, "%c", matrix[i][0]);                    // =     move(0,0);     printw("%d", c);
        }

        for (int i = 1; i < BOARD_WIDTH; i++) // go down
        {
            mvprintw(i, RIGHT_CORNER * WIDTH_MULTIPLIER, "%c", matrix[i][RIGHT_CORNER]);
            mvprintw(RIGHT_CORNER, i * WIDTH_MULTIPLIER, "%c", matrix[RIGHT_CORNER][i]);
        }

        for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go left
        {

            mvprintw(BOARD_WIDTH - 1, i * WIDTH_MULTIPLIER, "%c", matrix[BOARD_WIDTH - 1][i]);
            mvprintw(i, (BOARD_WIDTH - 1) * WIDTH_MULTIPLIER, "%c", matrix[i][BOARD_WIDTH - 1]);
        }

        for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go up
        {

            mvprintw(i, LEFT_CORNER * WIDTH_MULTIPLIER, "%c", matrix[i][LEFT_CORNER]);
            mvprintw(LEFT_CORNER, i * WIDTH_MULTIPLIER, "%c", matrix[LEFT_CORNER][i]);
        }
        refresh();
    }
    // attroff(COLOR_PAIR(1));
    endwin();
    return;
}

void ncurses_print()
{
    initscr();

    while (do_work)
    {

        for (int k = 20; k < 120; k++)
        {
            if (k == 119)
                k = 0;
            for (int i = 0; i < 20; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    mvprintw(j, i, "%c", k); // =     move(0,0);     printw("%d", c);
                }
                refresh();
            }
        }
    }
    endwin();
    return;
}

void wait_for_esc()
{
    while (do_work)
    {
        if (cin.get() == 27)
            do_work = false;
    }
    return;
}

int main()
{
    srand(time(0));

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

    thread t_printer(print_board_curses, matrix);

    Car *c1 = new Car('1', 80, matrix);
    thread t_c1(&Car::drive, c1);

    Car *c2 = new Car('2', 80, matrix);
    thread t_c2(&Car::drive, c2);

    Car *c3 = new Car('3', 80, matrix);
    thread t_c3(&Car::drive, c3);

    Car *c4 = new Car('4', 80, matrix);
    thread t_c4(&Car::drive, c4);

    Car *c5 = new Car('5', 80, matrix);
    thread t_c5(&Car::drive, c5);

    Car *ca = new Car('A', 130, matrix);
    thread t_ca(&Car::drive2, ca);

    Car *cb = new Car('B', 120, matrix);
    thread t_cb(&Car::drive2, cb);

    Car *cc = new Car('C', 90, matrix);
    thread t_cc(&Car::drive2, cc);

    Car *cd = new Car('D', 80, matrix);
    thread t_cd(&Car::drive2, cd);

    Car *ce = new Car('E', 100, matrix);
    thread t_ce(&Car::drive2, ce);

    thread t_escape(wait_for_esc);

    t_escape.join();
    t_printer.join();
    t_c1.join();
    t_c2.join();
    t_c3.join();
    t_c4.join();
    t_c5.join();

    t_ca.join();
    t_cb.join();
    t_cc.join();
    t_cd.join();
    t_ce.join();

    while (do_work)
    {
    }

    delete matrix;
    matrix = nullptr;

    return 0;
}