#pragma once

class Car {
    int v;              // velocity in m/s
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
    int get_space();
    bool get_is_tail();
    void print();
};
