#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <ctime>
#include <limits>

using namespace std;

struct MenuItem {
    int id;
    string name;
    double price;
};

class Restaurant {
public:
    int id;
    string name;
    vector<MenuItem> menu;
};

class DeliveryAgent {
public:
    int id;
    string name;
};

struct Order {
    int id;
    string customerName;
    string restaurantName;
    vector<MenuItem> items;
    double total;
    string status;
    string agentName;
    string createdAt;
};

class FoodOrderingSystem {
private:
    vector<Restaurant> restaurants;
    vector<DeliveryAgent> agents;
    map<int, Order> orders;
    queue<int> availableAgents;

    int nextOrderId = 1001;

    void seedData() {
        restaurants = {
            {1, "Campus Cafe", {
                {1, "Veg Sandwich", 65},
                {2, "Paneer Wrap", 95},
                {3, "Masala Dosa", 80},
                {4, "Cold Coffee", 55}
            }},
            {2, "Spice Corner", {
                {5, "Veg Biryani", 120},
                {6, "Dal Rice", 100},
                {7, "Paneer Thali", 160},
                {8, "Lassi", 45}
            }}
        };

        agents = {
            {1, "Aarav"},
            {2, "Neha"},
            {3, "Kabir"}
        };

        for (const auto& agent : agents) {
            availableAgents.push(agent.id);
        }
    }

    string currentTime() {
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        char buffer[32];

        if (local) {
            strftime(buffer, sizeof(buffer),
                     "%Y-%m-%d %H:%M", local);
            return buffer;
        }

        return "Unknown";
    }

    int readInt(const string& message) {
        int value;

        while (true) {
            cout << message;

            if (cin >> value) {
                return value;
            }

            cout << "Invalid input. Enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void showRestaurants() {
        cout << "\n--- Available Restaurants ---\n";

        for (const auto& restaurant : restaurants) {
            cout << restaurant.id << ". "
                 << restaurant.name << '\n';
        }
    }

    void showMenu(const Restaurant& restaurant) {
        cout << "\nMenu: " << restaurant.name << '\n';

        cout << left << setw(8) << "ID"
             << setw(22) << "Item"
             << "Price (Rs.)\n";

        for (const auto& item : restaurant.menu) {
            cout << left << setw(8) << item.id
                 << setw(22) << item.name
                 << fixed << setprecision(2)
                 << item.price << '\n';
        }
    }

    void placeOrder() {
        showRestaurants();

        int restaurantId = readInt("\nEnter restaurant ID: ");

        Restaurant* selected = nullptr;

        for (auto& restaurant : restaurants) {
            if (restaurant.id == restaurantId) {
                selected = &restaurant;
                break;
            }
        }

        if (selected == nullptr) {
            cout << "Restaurant not found.\n";
            return;
        }

        showMenu(*selected);

        Order order;
        order.id = nextOrderId++;
        order.total = 0;
        order.status = "Placed";
        order.restaurantName = selected->name;
        order.createdAt = currentTime();

        cout << "\nEnter customer name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, order.customerName);

        cout << "\nEnter item IDs one by one.\n";
        cout << "Enter 0 when finished.\n";

        while (true) {
            int itemId = readInt("Item ID: ");

            if (itemId == 0) {
                break;
            }

            bool found = false;

            for (const auto& item : selected->menu) {
                if (item.id == itemId) {
                    order.items.push_back(item);
                    order.total += item.price;
                    found = true;

                    cout << item.name
                         << " added to your order.\n";
                    break;
                }
            }

            if (!found) {
                cout << "Invalid item ID. Try again.\n";
            }
        }

        if (order.items.empty()) {
            cout << "No items selected. Order cancelled.\n";
            return;
        }

        if (!availableAgents.empty()) {
            int agentId = availableAgents.front();
            availableAgents.pop();

            for (const auto& agent : agents) {
                if (agent.id == agentId) {
                    order.agentName = agent.name;
                    break;
                }
            }

            order.status = "Confirmed";
        } else {
            order.agentName = "Not assigned";
            order.status = "Awaiting delivery agent";
        }

        orders[order.id] = order;

        cout << "\n========== ORDER RECEIPT ==========\n";
        cout << "Order ID: " << order.id << '\n';
        cout << "Customer: " << order.customerName << '\n';
        cout << "Restaurant: " << order.restaurantName << '\n';

        for (const auto& item : order.items) {
            cout << item.name << " - Rs. "
                 << fixed << setprecision(2)
                 << item.price << '\n';
        }

        cout << "Total: Rs. " << order.total << '\n';
        cout << "Delivery Agent: " << order.agentName << '\n';
        cout << "Status: " << order.status << '\n';
        cout << "Created At: " << order.createdAt << '\n';
        cout << "===================================\n";
    }

    void trackOrder() {
        int id = readInt("Enter order ID: ");

        auto it = orders.find(id);

        if (it == orders.end()) {
            cout << "Order not found.\n";
            return;
        }

        const Order& order = it->second;

        cout << "\n--- Order Tracking ---\n";
        cout << "Order ID: " << order.id << '\n';
        cout << "Customer: " << order.customerName << '\n';
        cout << "Restaurant: " << order.restaurantName << '\n';
        cout << "Total: Rs. " << order.total << '\n';
        cout << "Agent: " << order.agentName << '\n';
        cout << "Status: " << order.status << '\n';
        cout << "Time: " << order.createdAt << '\n';
    }

    void viewOrders() {
        if (orders.empty()) {
            cout << "No orders placed yet.\n";
            return;
        }

        cout << "\n--- All Orders ---\n";

        for (const auto& entry : orders) {
            const Order& order = entry.second;

            cout << "Order ID: " << order.id
                 << " | Customer: " << order.customerName
                 << " | Restaurant: " << order.restaurantName
                 << " | Total: Rs. " << order.total
                 << " | Status: " << order.status
                 << " | Agent: " << order.agentName
                 << '\n';
        }
    }

public:
    FoodOrderingSystem() {
        seedData();
    }

    void run() {
        int choice;

        do {
            cout << "\n====================================\n";
            cout << " ONLINE FOOD ORDERING SYSTEM\n";
            cout << "====================================\n";
            cout << "1. View Restaurants and Menus\n";
            cout << "2. Place an Order\n";
            cout << "3. Track an Order\n";
            cout << "4. View All Orders\n";
            cout << "5. Exit\n";

            choice = readInt("Enter your choice: ");

            switch (choice) {
                case 1:
                    showRestaurants();

                    for (const auto& restaurant : restaurants) {
                        showMenu(restaurant);
                    }
                    break;

                case 2:
                    placeOrder();
                    break;

                case 3:
                    trackOrder();
                    break;

                case 4:
                    viewOrders();
                    break;

                case 5:
                    cout << "Thank you for using our system!\n";
                    break;

                default:
                    cout << "Invalid choice. Try again.\n";
            }

        } while (choice != 5);
    }
};

int main() {
    FoodOrderingSystem system;
    system.run();

    return 0;
}