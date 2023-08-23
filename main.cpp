#include <iostream>
#include <vector>

class Car {
    int v;  // velocity in m/s
   public:
    Car() {}
    Car(int v) : v(v) {}
    int get_v() { return v; }
};

class Head : public Car {
    // class representing head of the car
    int v;              // velocity in m/s
    int space;          // space ocuppied
    int destination;    // which exit will car use
    int starting_from;  // at which entry car appeared
   public:
    Head(int v, int space, int destination, int starting_from) : Car(v) {
        this->space = space;
        this->destination = destination;
        this->starting_from = starting_from;
    }
    ~Head() {}
    void print() {
        std::cout << "Space: " << space << std::endl;
        std::cout << "Destination: " << destination << std::endl;
        std::cout << "Starting from: " << starting_from << std::endl;
        std::cout << "Velocity: " << get_v() << std::endl;
    }
};

class Tail : public Car {
    Head *head;

   public:
    Tail(Head *head) : Car(head->get_v()), head(head) {}
    ~Tail() {}
    void print() {
        std::cout << "Tail speed: " << get_v() << std::endl;
    }
};

class Rounbabout {
    float island_radius;  // roundabout island radius in m
    int number_of_lanes;
    int max_velocity;  // max velocity in m/s
    float density;     // car denisty on the road <0,1>
   public:
    Rounbabout(
        float island_radius,
        int number_of_lanes = 1,
        int max_velocity = 9,
        float density = 1) {
        this->island_radius = island_radius;
        this->number_of_lanes = number_of_lanes;
        this->max_velocity = max_velocity;
        this->density = density;
    }
    ~Rounbabout() {}
};

int main() {
    Rounbabout *rbt = new Rounbabout(28);

    Head *head = new Head(9, 3, 3, 5);
    head->print();

    Tail *tail = new Tail(head);
    tail->print();

    std::vector<Car *> tab;
    tab.push_back(head);
    tab.push_back(nullptr);
    tab.push_back(tail);

    for (int i =0; i < tab.size();i++){
        if (tab[i] == nullptr){
        std::cout << "null" << std::endl;
        } else {

        std::cout << tab[i]->get_v() << std::endl;
        }
    }

    delete tail;
    delete head;
    delete rbt;
    return 0;
}
