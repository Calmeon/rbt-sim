#pragma once

class Car {
    int v;              // velocity in m/s
    int v_used;         // "action points" used
    int space;          // space ocuppied
    int destination;    // which exit will car use
    int starting_from;  // at which entry car appeared
    bool is_tail;       // true - tail, false - head
   public:
    // head constructor
    Car(int v, int space, int destination, int starting_from);
    // tail constructor (copy constructor)
    Car(Car &head);
    ~Car();

    int get_v();
    void set_v(int v);
    int get_v_used();
    void set_v_used(int v);
    int get_v_available();
    int get_space();
    bool get_is_tail();
    int get_starting_from();
    int get_destination();
};
