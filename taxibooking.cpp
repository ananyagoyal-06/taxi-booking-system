#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int absolute(int x) {
    return (x < 0) ? -x : x;
}
class Person {
protected:
    string name;
public:
    Person(string n = "Unknown") : name(n) {}
    virtual void display() const { cout << "Name: " << name << endl; }
    virtual ~Person() {} 
};

class Customer : public Person {
    int pickup, drop;
public:
    Customer(string n, int p, int d) : Person(n), pickup(p), drop(d) {}
    int getPickup() const { return pickup; }
    int getDrop() const { return drop; }
    void display() const override {
        cout << "Customer Name: " << name << ", Pickup: " << pickup << ", Drop: " << drop << endl;
    }
};

class Taxi {
    static int count; 
    const int id;     
    int currentLocation;
    bool isAvailable;
    double totalEarnings;
public:
    Taxi(int loc = 0) : id(++count), currentLocation(loc), isAvailable(true), totalEarnings(0.0) {}

    inline bool available() const { return isAvailable; }

    // Function overloading
    void updateLocation(int newLoc) { currentLocation = newLoc; }
    void updateLocation(int newLoc, bool availability) { currentLocation = newLoc; isAvailable = availability; }

    friend void showTaxiDetails(const Taxi& t);

    void book(double fare, int drop) {
        totalEarnings += fare;
        currentLocation = drop;
        isAvailable = true;
    }

    int getLocation() const { return currentLocation; }
    double getEarnings() const { return totalEarnings; }
    int getId() const { return id; }
};

int Taxi::count = 0;

void showTaxiDetails(const Taxi& t) {
    cout << "Taxi ID: " << t.id
         << " | Location: " << t.currentLocation
         << " | Earnings: " << t.totalEarnings
         << " | Status: " << (t.isAvailable ? "Available" : "Booked") << endl;
}


template <typename T>
T calculateFare(T distance, T ratePerKm) {
    return distance * ratePerKm;
}

class Booking {
    Customer* cust;
    Taxi* taxi;
    double fare;
public:
    Booking() { cust = nullptr; taxi = nullptr; fare = 0; }
    Booking(Customer* c, Taxi* t, double f) : cust(c), taxi(t), fare(f) {}

    void display() const {
        if (cust && taxi)
            cout << "Booking: " << cust->getPickup() << " -> " << cust->getDrop()
                 << " | Taxi ID: " << taxi->getId() << " | Fare: Rs." << fare << endl;
    }
};

class TaxiSystem {
    Taxi* taxis[10];      
    Booking history[50];  
    int taxiCount;
    int bookingCount;
    const double ratePerKm = 10.0;

public:
    TaxiSystem(int count = 4) {
        taxiCount = count;
        bookingCount = 0;
        for (int i = 0; i < taxiCount; i++)
            taxis[i] = new Taxi(i * 10);
    }

    void bookTaxi() {
        try {
            string name;
            int pickup, drop;
            cout << "\nEnter Customer Name: ";
            cin >> name;
            cout << "Enter Pickup (0–100): ";
            cin >> pickup;
            cout << "Enter Drop (0–100): ";
            cin >> drop;

            if (pickup < 0 || drop < 0 || pickup > 100 || drop > 100)
                throw runtime_error("Invalid pickup/drop point!");

            // Find nearest available taxi
            Taxi* nearest = nullptr;
            int minDist = 9999;
            for (int i = 0; i < taxiCount; i++) {
                if (taxis[i]->available()) {
                    int dist = absolute(taxis[i]->getLocation() - pickup);
                    if (dist < minDist) {
                        minDist = dist;
                        nearest = taxis[i];
                    }
                }
            }

            if (!nearest)
                throw runtime_error("No taxi available!");

            double distance = absolute(drop - pickup);
            double fare = calculateFare(distance, ratePerKm); 

            Customer* c = new Customer(name, pickup, drop);
            Booking b(c, nearest, fare);
            history[bookingCount++] = b;

            nearest->book(fare, drop);
            cout << "\nTaxi " << nearest->getId() << " booked successfully for " << name
                 << ". Fare: Rs." << fare << endl;

            saveToFile(name, pickup, drop, nearest->getId(), fare);
            delete c; 
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    void showTaxis() {
        cout << "\n--- Taxi Details ---\n";
        for (int i = 0; i < taxiCount; i++)
            showTaxiDetails(*taxis[i]);
    }

    void showHistory() {
        cout << "\n--- Booking History ---\n";
        if (bookingCount == 0) {
            cout << "No bookings yet.\n";
            return;
        }
        for (int i = 0; i < bookingCount; i++)
            history[i].display();
    }


    void saveToFile(string name, int pickup, int drop, int taxiId, double fare) {
        ofstream fout("bookings.txt", ios::app);
        if (fout.is_open()) {
            fout << "Customer: " << name
                 << " | Pickup: " << pickup
                 << " | Drop: " << drop
                 << " | Taxi ID: " << taxiId
                 << " | Fare: Rs." << fare << "\n";
            fout.close();
        }
    }

    ~TaxiSystem() {
        for (int i = 0; i < taxiCount; i++)
            delete taxis[i];
    }
};

int main() {
    TaxiSystem system(4);
    int choice;

    while (true) {
        cout << "\n===== TAXI BOOKING SYSTEM =====";
        cout << "\n1. Book Taxi";
        cout << "\n2. Show Taxi Details";
        cout << "\n3. Show Booking History";
        cout << "\n4. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.bookTaxi(); break;
            case 2: system.showTaxis(); break;
            case 3: system.showHistory(); break;
            case 4:
                cout << "\nThank you for using Taxi Booking System!\n";
                return 0;
            default:
                cout << "\nInvalid choice!\n";
        }
    }
}
