#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <queue>

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

class FoodItem {
public:
    FoodItem(const std::string& name, int quantity, int daysToExpiration, const User& owner)
        : name(name), quantity(quantity), daysToExpiration(daysToExpiration), owner(owner) {}

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

private:
    std::string name;
    int quantity;
    int daysToExpiration;
    User owner;
};

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

class Restaurant : public User {
public:
    Restaurant(const std::string& username, const std::string& password)
        : User(username, password, "restaurant") {}

    void addFoodItem(const std::string& name, int quantity, int daysToExpiration, std::vector<FoodItem>& foodItems, std::queue<Notification>& notifications) const {
        foodItems.emplace_back(name, quantity, daysToExpiration, *this);

        // Check for expiration and send notifications
        time_t currentTime = time(nullptr);
        time_t expirationTime = currentTime + daysToExpiration * 24 * 60 * 60;

        if (expirationTime <= currentTime) {
            notifications.push(Notification("Your " + name + " is expired!", *this));
        }
    }
};

class FoodApp {
public:
    void run() {
        bool loggedIn = false;
        User currentUser("", "", ""); // Initialize with empty user
        std::queue<Notification> notifications;
        std::vector<User> users;
        std::vector<FoodItem> foodItems;

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
                            viewFoodItems(foodItems, currentUser);
                        } else {
                            std::cout << "Only people can view food items." << std::endl;
                        }
                        break;
                    case 3:
                        if (currentUser.getUserType() == "people") {
                            viewExpiringItems(foodItems, currentUser);
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
                        currentUser = User("", "", ""); // Clear user data
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
            currentUser = *it; // Set the current user
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

    void addFoodItem(const User& currentUser, std::vector<FoodItem>& foodItems, std::queue<Notification>& notifications) {
        std::string name;
        int quantity, daysToExpiration;
        std::cout << "Enter food item name: ";
        std::cin >> name;
        std::cout << "Enter quantity: ";
        std::cin >> quantity;
        std::cout << "Enter days to expiration: ";
        std::cin >> daysToExpiration;

        if (currentUser.getUserType() == "restaurant") {
            const Restaurant* restaurant = static_cast<const Restaurant*>(&currentUser);
            restaurant->addFoodItem(name, quantity, daysToExpiration, foodItems, notifications);
            std::cout << "Food item added successfully." << std::endl;
        } else {
            std::cout << "Error: Only restaurants can add food items." << std::endl;
        }
    }

    void viewFoodItems(const std::vector<FoodItem>& foodItems, const User& currentUser) {
        std::cout << "Food Items:" << std::endl;
        for (const FoodItem& item : foodItems) {
            if (item.getOwner().getUsername() == currentUser.getUsername()) {
                std::cout << "Name: " << item.getName() << ", Quantity: " << item.getQuantity() << ", Expiration in " << item.getDaysToExpiration() << " days" << std::endl;
            }
        }
    }

    void viewExpiringItems(const std::vector<FoodItem>& foodItems, const User& currentUser) {
        std::cout << "Expiring Food Items:" << std::endl;
        time_t currentTime = time(nullptr);

        for (const FoodItem& item : foodItems) {
            if (item.getOwner().getUsername() == currentUser.getUsername()) {
                time_t expirationTime = currentTime + item.getDaysToExpiration() * 24 * 60 * 60;
                if (expirationTime <= currentTime) {
                    std::cout << "Name: " << item.getName() << ", Quantity: " << item.getQuantity() << ", Expiration in " << item.getDaysToExpiration() << " days" << std::endl;
                }
            }
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
};

int main() {
    FoodApp app;
    app.run();
    return 0;
}


