#pragma once

class Car {
    int v;              // velocity in m/s
    int v_old;          // velocity in previous step
    int space;          // space ocuppied
    int destination;    // which exit will car use
    int starting_from;  // at which entry car appeared
    Car *head;          // tails have head pointer
    int waiting;        // how many steps is car waiting for lane change

   public:
    // head constructor
    Car(int v, int space, int destination, int starting_from);
    // tail constructor (copy constructor)
    Car(Car *head);
    ~Car();

    int get_v();
    void set_v(int v);
    void save_v_old();
    int get_v_old();
    int get_space();
    bool get_is_tail();
    int get_starting_from();
    int get_destination();
    void set_waiting(int waiting);
    int get_waiting();
};
