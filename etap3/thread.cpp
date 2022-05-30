/*
etap1 - dwa tory, po ktorych poruszaja sie pojazdy:
    --tor1---
    |       |
----|-tor2-------
|   |       |   |
|   |       |   |
|   |       |   |
----|-----------
    |       |
    ---------
     - 1 tor - stała liczba pojazdów poruszających się z tą samą predkością
     - 2 tor - pojazdy z losowym opóźnieniem wjeżdząją na  tor z losową predkością i zjeżdząją po 3 okrążeniach
etap 2 - pojazdy z toru pierwszego mają pierwszeństwo nad pojazdami z toru drugiego

etap 3:

    --tor1---
    |       |
----|-tor2---ABC-
|   |       |   |
|   |       |   |
|   |       |   |
----|-----FED----
    |       |
    ---------

W momencie gdy przed prawym górnym skrzyżowaniem będą czekały już 3 pojazdy żaden inny pojazd z drogi 
podporządkowanej nie może przejechać prawego dolnego skrzyżowania

g++ thread.cpp -o thread -std=c++14 -pthread -lncurses
*/

#include <iostream>
#include <thread>
#include <vector>
#include <ncurses.h>
#include <chrono>
#include <unistd.h>
#include <stdlib.h>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace std::literals::chrono_literals;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

const int FPS = 30;
const int BOARD_WIDTH = 21;
const char EMPTY_PLACE = '.';
const char BG_CHAR = ' ';
const int VELOCITY_FACTOR = 1;
const float VELOCITY_MULTIPLIER = 0.05 * VELOCITY_FACTOR;

const int CARS_NR1 = 10;
const int CARS_NR2 = 10;

const int LAPS_NUMBER = 3;

const int RIGHT_CORNER = BOARD_WIDTH * 3 / 4;
const int LEFT_CORNER = BOARD_WIDTH / 4;

bool car_finished_road = false;
bool do_work = true;

condition_variable up_left_CV, up_right_CV, down_left_CV, down_right_CV;
mutex up_left_mutex, up_right_mutex, down_left_mutex, down_right_mutex;

bool is_up_left_crossroad_busy = false;
bool is_up_right_crossroad_busy = false;
bool is_down_left_crossroad_busy = false;
bool is_down_right_crossroad_busy = false;

// const int RIGHT_CORNER = BOARD_WIDTH * 2 / 3 + 1;
// const int LEFT_CORNER = RIGHT_CORNER / 2;



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

        int rand_time = rand() % 10000 + 1;

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
                for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) // go right
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

                    // RIGHT UP CROSSROAD
                    if (board[LEFT_CORNER - 1][RIGHT_CORNER] == this->id || board[LEFT_CORNER][RIGHT_CORNER] == this->id || board[LEFT_CORNER + 1][RIGHT_CORNER] == this->id)
                    {
                        lock_guard<mutex> lg(up_right_mutex);
                        is_up_right_crossroad_busy = true;
                    }
                    else if (board[LEFT_CORNER - 1][RIGHT_CORNER] == EMPTY_PLACE && board[LEFT_CORNER][RIGHT_CORNER] == EMPTY_PLACE && board[LEFT_CORNER + 1][RIGHT_CORNER] == EMPTY_PLACE)
                    {
                        is_up_right_crossroad_busy = false;
                        up_right_CV.notify_all();
                    }

                    // RIGHT DOWN CROSSROAD
                    if (board[RIGHT_CORNER - 1][RIGHT_CORNER] == this->id || board[RIGHT_CORNER][RIGHT_CORNER] == this->id || board[RIGHT_CORNER + 1][RIGHT_CORNER] == this->id)
                    {
                        lock_guard<mutex> lg(down_right_mutex);
                        is_down_right_crossroad_busy = true;
                    }
                    else if (board[RIGHT_CORNER - 1][RIGHT_CORNER] == EMPTY_PLACE && board[RIGHT_CORNER][RIGHT_CORNER] == EMPTY_PLACE && board[RIGHT_CORNER + 1][RIGHT_CORNER] == EMPTY_PLACE)
                    {
                        is_down_right_crossroad_busy = false;
                        down_right_CV.notify_all();
                    }

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

                    // LEFT UP CROSSROAD
                    if (board[LEFT_CORNER - 1][LEFT_CORNER] == this->id || board[LEFT_CORNER][LEFT_CORNER] == this->id || board[LEFT_CORNER + 1][LEFT_CORNER] == this->id)
                    {
                        lock_guard<mutex> lg(up_left_mutex);
                        is_up_left_crossroad_busy = true;
                    }
                    else if (board[LEFT_CORNER - 1][LEFT_CORNER] == EMPTY_PLACE && board[LEFT_CORNER][LEFT_CORNER] == EMPTY_PLACE && board[LEFT_CORNER + 1][LEFT_CORNER] == EMPTY_PLACE)
                    {
                        is_up_left_crossroad_busy = false;
                        up_left_CV.notify_all();
                    }

                    // LEFT DOWN CROSSROAD
                    if (board[RIGHT_CORNER - 1][LEFT_CORNER] == this->id || board[RIGHT_CORNER][LEFT_CORNER] == this->id || board[RIGHT_CORNER + 1][LEFT_CORNER] == this->id)
                    {
                        lock_guard<mutex> lg(down_left_mutex);
                        is_down_left_crossroad_busy = true;
                    }
                    else if (board[RIGHT_CORNER - 1][LEFT_CORNER] == EMPTY_PLACE && board[RIGHT_CORNER][LEFT_CORNER] == EMPTY_PLACE && board[RIGHT_CORNER + 1][LEFT_CORNER] == EMPTY_PLACE)
                    {
                        is_down_left_crossroad_busy = false;
                        down_left_CV.notify_all();
                    }

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

                for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) // go down
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

                for (int i = 1; i < BOARD_WIDTH; i++) // go right
                {
                    if (!do_work)
                        break;
                    while (board[RIGHT_CORNER][i] != EMPTY_PLACE)
                        usleep(velocity);

                    if (i == LEFT_CORNER)
                    {
                        unique_lock<mutex> ul(down_left_mutex);
                        down_left_CV.wait(ul, []
                                          { return (!is_down_left_crossroad_busy || !do_work); });
                    }
                    else if (i == RIGHT_CORNER)
                    {
                        unique_lock<mutex> ul(down_right_mutex);
                        down_right_CV.wait(ul, []
                                           { return (!is_down_right_crossroad_busy || !do_work); });
                    }

                    board[RIGHT_CORNER][i] = this->id;

                    if (i > 0 && board[RIGHT_CORNER][i - 1] == this->id)
                    {
                        board[RIGHT_CORNER][i - 1] = EMPTY_PLACE;
                    }
                    usleep(velocity);
                }

                for (int i = RIGHT_CORNER - 1; i >= LEFT_CORNER; i--) // go up
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

                for (int i = BOARD_WIDTH - 1 - 1; i >= 0; i--) // go left
                {
                    if (!do_work)
                        break;
                    while (board[LEFT_CORNER][i] != EMPTY_PLACE)
                    {
                        usleep(velocity);
                    }

                    if (i == RIGHT_CORNER)
                    {
                        unique_lock<mutex> ul(up_right_mutex);
                        up_right_CV.wait(ul, []
                                         { return (!is_up_right_crossroad_busy || !do_work); });
                    }
                    else if (i == LEFT_CORNER)
                    {
                        unique_lock<mutex> ul(up_left_mutex);
                        up_left_CV.wait(ul, []
                                        { return (!is_up_left_crossroad_busy || !do_work); });
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
            // sleep(rand() % 5 + 1);
            // int c = rand() % 25 + 65;        // 65-90
            // int random_v = rand() % 75 + 75; // 75-150

            // this->id = c;
            // this->velocity = 1000000 / (random_v * VELOCITY_MULTIPLIER);
            car_finished_road = true;
            break;
        }
        // cout << ". ";
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
                usleep(1000000 / FPS);
            }
            cout << endl;
        }
    }
}

void print_board_curses(char *matrix[BOARD_WIDTH])
{

    const int WIDTH_MULTIPLIER = 3;

    curs_set(0);

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
    // roads, cars
    while (do_work)
    {
        // if (is_up_right_crossroad_busy)
        // {
        //     mvprintw(LEFT_CORNER - 1, RIGHT_CORNER * 3 - 1, "%c", 'X'); // =     move(0,0);     printw("%d", c);
        // }
        // else
        // {
        //     mvprintw(LEFT_CORNER - 1, RIGHT_CORNER * 3 - 1, "%c", '|'); // =     move(0,0);     printw("%d", c);
        // }

        for (int i = LEFT_CORNER + 1; i <= RIGHT_CORNER; i++) // go right
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
        // usleep(1000000 / FPS);
    }
    // attroff(COLOR_PAIR(1));

    return;
}

void wait_for_esc()
{
    while (do_work)
    {
        if (getch() == 27)
            do_work = false;
    }
    return;
}

void car2_manager(char *matrix[BOARD_WIDTH])
{
    while (do_work)
    {
        int rand_time = rand() % 25 + 1;
        for (int i = 0; i < rand_time; i++)
        {
            if (!do_work)
                break;
            sleep(1);
        }
        if (!do_work)
            break;

        Car *temp_car = new Car(rand() % 25 + 65, rand() % 80 + 50, matrix);
        thread t_car(&Car::drive2, temp_car);
        t_car.join();
    }
}

int main()
{
    initscr();
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

    thread t_cars1[CARS_NR1];
    thread t_cars2[CARS_NR2];
    thread t_printer(print_board_curses, matrix);

    for (int i = 0; i < CARS_NR1; i++)
    {
        Car *temp_car = new Car('0' + i, 80, matrix);
        t_cars1[i] = thread(&Car::drive, temp_car);
    }

    for (int i = 0; i < CARS_NR2; i++)
    {
        t_cars2[i] = thread(car2_manager, matrix);
    }

    thread t_escape(wait_for_esc);

    t_escape.join();
    t_printer.join();

    for (int i = 0; i < CARS_NR1; i++)
    {
        t_cars1[i].join();
    }

    for (int i = 0; i < CARS_NR2; i++)
    {
        t_cars2[i].join();
    }

    delete matrix;
    matrix = nullptr;

    endwin();
    return 0;
}