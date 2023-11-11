#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm> // Include the algorithm header
#include <queue>

// Rest of the code remains unchanged

// Define a User class
class User {
public:
    User(const std::string& username, const std::string& password, const std::string& userType)
        : username(username), password(password), userType(userType) {}

    std::string getUsername() const {
        return username;
    }

    std::string getPassword() const {
        return password;
    }

    std::string getUserType() const {
        return userType;
    }

private:
    std::string username;
    std::string password;
    std::string userType;
};

// Define a FoodItem class
class FoodItem {
public:
    FoodItem(const std::string& name, int quantity, int daysToExpiration, const User& owner)
        : name(name), quantity(quantity), daysToExpiration(daysToExpiration), owner(owner), next(nullptr) {}

    std::string getName() const {
        return name;
    }

    int getQuantity() const {
        return quantity;
    }

    int getDaysToExpiration() const {
        return daysToExpiration;
    }

    User getOwner() const {
        return owner;
    }

    FoodItem* getNext() const {
        return next;
    }

    void setNext(FoodItem* nextItem) {
        next = nextItem;
    }

private:
    std::string name;
    int quantity;
    int daysToExpiration;
    User owner;
    FoodItem* next;
};

// Define a Notification class
class Notification {
public:
    Notification(const std::string& message, const User& recipient)
        : message(message), recipient(recipient) {}

    std::string getMessage() const {
        return message;
    }

    User getRecipient() const {
        return recipient;
    }

private:
    std::string message;
    User recipient;
};

// Define a FoodApp class
class FoodApp {
public:
    FoodApp() : foodItems(nullptr) {}

    void run() {
        bool loggedIn = false;
        User currentUser("", "", "");
        std::queue<Notification> notifications;
        std::vector<User> users;

        while (true) {
            if (!loggedIn) {
                std::cout << "1. Login\n2. Signup\n3. Exit\nEnter your choice: ";
                int choice;
                std::cin >> choice;

                switch (choice) {
                    case 1:
                        loggedIn = login(currentUser, users);
                        break;
                    case 2:
                        signup(users);
                        break;
                    case 3:
                        return;
                    default:
                        std::cout << "Invalid choice. Try again." << std::endl;
                }
            } else {
                std::cout << "1. Add Food Item (Restaurant)\n2. View Food Items (People)\n3. View Expiring Items (People)\n4. Notifications (People)\n5. Logout\nEnter your choice: ";
                int choice;
                std::cin >> choice;

                switch (choice) {
                    case 1:
                        if (currentUser.getUserType() == "restaurant") {
                            addFoodItem(currentUser, foodItems, notifications);
                        } else {
                            std::cout << "Only restaurants can add food items." << std::endl;
                        }
                        break;
                    case 2:
                        if (currentUser.getUserType() == "people") {
                            viewFoodItems(currentUser);
                        } else {
                            std::cout << "Only people can view food items." << std::endl;
                        }
                        break;
                    case 3:
                        if (currentUser.getUserType() == "people") {
                            viewExpiringItems(currentUser);
                        } else {
                            std::cout << "Only people can view expiring items." << std::endl;
                        }
                        break;
                    case 4:
                        if (currentUser.getUserType() == "people") {
                            viewNotifications(notifications);
                        } else {
                            std::cout << "Restaurants cannot view notifications." << std::endl;
                        }
                        break;
                    case 5:
                        loggedIn = false;
                        currentUser = User("", "", "");
                        break;
                    default:
                        std::cout << "Invalid choice. Try again." << std::endl;
                }
            }
        }
    }

private:
    bool login(User& currentUser, std::vector<User>& users) {
        std::string username, password;
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        std::cin >> password;

        auto it = std::find_if(users.begin(), users.end(), [username, password](const User& user) {
            return user.getUsername() == username && user.getPassword() == password;
        });

        if (it != users.end()) {
            currentUser = *it;
            std::cout << "Login successful. Welcome, " << username << "!" << std::endl;
            return true;
        } else {
            std::cout << "Login failed. Invalid username or password." << std::endl;
            return false;
        }
    }

    void signup(std::vector<User>& users) {
        std::string username, password, userType;
        std::cout << "Enter username: ";
        std::cin >> username;

        auto it = std::find_if(users.begin(), users.end(), [username](const User& user) {
            return user.getUsername() == username;
        });

        if (it != users.end()) {
            std::cout << "Username is already in use. Please choose a different username." << std::endl;
        } else {
            std::cout << "Enter password: ";
            std::cin >> password;
            std::cout << "Enter user type (people or restaurant): ";
            std::cin >> userType;

            if (userType == "people" || userType == "restaurant") {
                users.emplace_back(username, password, userType);
                std::cout << "Signup successful. You can now log in." << std::endl;
            } else {
                std::cout << "Invalid user type. Please choose 'people' or 'restaurant'." << std::endl;
            }
        }
    }

    void addFoodItem(const User& currentUser, FoodItem*& foodItems, std::queue<Notification>& notifications) {
        std::string name;
        int quantity, daysToExpiration;
        std::cout << "Enter food item name: ";
        std::cin >> name;
        std::cout << "Enter quantity: ";
        std::cin >> quantity;
        std::cout << "Enter days to expiration: ";
        std::cin >> daysToExpiration;

        if (currentUser.getUserType() == "restaurant") {
            FoodItem* newItem = new FoodItem(name, quantity, daysToExpiration, currentUser);
            newItem->setNext(foodItems);
            foodItems = newItem;

            // Check for expiration and send notifications
            time_t currentTime = time(nullptr);
            time_t expirationTime = currentTime + daysToExpiration * 24 * 60 * 60;

            if (expirationTime <= currentTime) {
                notifications.push(Notification("Your " + name + " is expired!", currentUser));
            }
            std::cout << "Food item added successfully." << std::endl;
        } else {
            std::cout << "Error: Only restaurants can add food items." << std::endl;
        }
    }

    void viewFoodItems(const User& currentUser) {
        std::cout << "Food Items:" << std::endl;
        FoodItem* current = foodItems;
        while (current != nullptr) {
            if (current->getOwner().getUsername() == currentUser.getUsername()) {
                std::cout << "Name: " << current->getName() << ", Quantity: " << current->getQuantity() << ", Expiration in " << current->getDaysToExpiration() << " days" << std::endl;
            }
            current = current->getNext();
        }
    }

    void viewExpiringItems(const User& currentUser) {
        std::cout << "Expiring Food Items:" << std::endl;
        time_t currentTime = time(nullptr);
        FoodItem* current = foodItems;

        while (current != nullptr) {
            if (current->getOwner().getUsername() == currentUser.getUsername()) {
                time_t expirationTime = currentTime + current->getDaysToExpiration() * 24 * 60 * 60;
                if (expirationTime <= currentTime) {
                    std::cout << "Name: " << current->getName() << ", Quantity: " << current->getQuantity() << ", Expiration in " << current->getDaysToExpiration() << " days" << std::endl;
                }
            }
            current = current->getNext();
        }
    }

    void viewNotifications(std::queue<Notification>& notifications) {
        std::cout << "Notifications:" << std::endl;
        while (!notifications.empty()) {
            Notification notification = notifications.front();
            std::cout << "Message: " << notification.getMessage() << ", Recipient: " << notification.getRecipient().getUsername() << std::endl;
            notifications.pop();
        }
    }

private:
    FoodItem* foodItems; // Linked list of food items
};

int main() {
    FoodApp app;
    app.run();
    return 0;
}


