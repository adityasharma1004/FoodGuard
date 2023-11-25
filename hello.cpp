#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <queue>
#include <stdexcept>

// Custom exception class for handling invalid input
class InvalidArgumentException : public std::exception {
public:
    explicit InvalidArgumentException(const std::string& message) : message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

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
            notifications.push(Notification("\033[1;31mYour " + name + " is expired!\033[0m", *this));
        }
    }
};

class FoodItemBST {
private:
    struct Node {
        FoodItem item;
        Node* left;
        Node* right;

        Node(const FoodItem& item) : item(item), left(nullptr), right(nullptr) {}
    };

    Node* root;

    // Helper function to insert a food item into the BST
    Node* insert(Node* node, const FoodItem& item) {
        if (node == nullptr) {
            return new Node(item);
        }

        if (item.getName() < node->item.getName()) {
            node->left = insert(node->left, item);
        } else if (item.getName() > node->item.getName()) {
            node->right = insert(node->right, item);
        }

        return node;
    }

    // Helper function to perform an in-order traversal of the BST
    void inOrderTraversal(Node* node, const User& currentUser, std::vector<FoodItem>& result) const {
        if (node == nullptr) {
            return;
        }

        inOrderTraversal(node->left, currentUser, result);

        if (node->item.getOwner().getUsername() == currentUser.getUsername()) {
            result.push_back(node->item);
        }

        inOrderTraversal(node->right, currentUser, result);
    }

public:
    FoodItemBST() : root(nullptr) {}

    // Public function to insert a food item into the BST
    void insert(const FoodItem& item) {
        root = insert(root, item);
    }

    // Public function to get all food items associated with a user through in-order traversal
    std::vector<FoodItem> getFoodItems(const User& currentUser) const {
        std::vector<FoodItem> result;
        inOrderTraversal(root, currentUser, result);
        return result;
    }
};

class FoodApp {
private:
    FoodItemBST foodItemBST; // Add a BST instance to the FoodApp

    bool loggedIn;
    User currentUser;
    std::queue<Notification> notifications;
    std::vector<User> users;
    std::vector<FoodItem> foodItems;

public:
    FoodApp() : loggedIn(false), currentUser("", "", "") {}

    void run() {
        while (true) {
            try {
                if (!loggedIn) {
                    handleLoginSignup();
                } else {
                    handleUserActions();
                }
            } catch (const std::exception& e) {
                std::cerr << "\033[1;31mError: " << e.what() << "\033[0m" << std::endl;
            } catch (...) {
                std::cerr << "\033[1;31mAn unknown error occurred.\033[0m" << std::endl;
            }
        }
    }

private:
    void handleLoginSignup() {
        std::cout << "\033[1;32m1. Login\n2. Signup\n3. Exit\033[0m\nEnter your choice: ";
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
                exit(0);
            default:
                throw InvalidArgumentException("\033[1;31mInvalid choice. Try again.\033[0m");
        }
    }

    void handleUserActions() {
        std::cout << "\033[1;32m1. Add Food Item (Restaurant)\n2. View Food Items (People)\n3. View Expiring Items (People)\n4. Notifications (People)\n5. Logout\033[0m\nEnter your choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                if (currentUser.getUserType() == "restaurant") {
                    addFoodItem(currentUser, foodItems, notifications);
                } else {
                    throw InvalidArgumentException("\033[1;31mOnly restaurants can add food items.\033[0m");
                }
                break;
            case 2:
                if (currentUser.getUserType() == "people") {
                    viewFoodItems(currentUser);
                } else {
                    throw InvalidArgumentException("\033[1;31mOnly people can view food items.\033[0m");
                }
                break;
            case 3:
                if (currentUser.getUserType() == "people") {
                    viewExpiringItems(currentUser);
                } else {
                    throw InvalidArgumentException("\033[1;31mOnly people can view expiring items.\033[0m");
                }
                break;
            case 4:
                if (currentUser.getUserType() == "people") {
                    viewNotifications(notifications);
                } else {
                    throw InvalidArgumentException("\033[1;31mRestaurants cannot view notifications.\033[0m");
                }
                break;
            case 5:
                loggedIn = false;
                currentUser = User("", "", ""); // Clear user data
                break;
            default:
                throw InvalidArgumentException("\033[1;31mInvalid choice. Try again.\033[0m");
        }
    }

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
            std::cout << "\033[1;32mLogin successful. Welcome, " << username << "!\033[0m" << std::endl;
            return true;
        } else {
            std::cout << "\033[1;31mLogin failed. Invalid username or password.\033[0m" << std::endl;
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
            throw InvalidArgumentException("\033[1;31mUsername is already in use. Please choose a different username.\033[0m");
        } else {
            std::cout << "Enter password: ";
            std::cin >> password;
            std::cout << "Enter user type (\033[1;34mpeople\033[0m or \033[1;34mrestaurant\033[0m): ";
            std::cin >> userType;

            if (userType == "people" || userType == "restaurant") {
                users.emplace_back(username, password, userType);
                std::cout << "\033[1;32mSignup successful. You can now log in.\033[0m" << std::endl;
            } else {
                throw InvalidArgumentException("\033[1;31mInvalid user type. Please choose 'people' or 'restaurant'.\033[0m");
            }
        }
    }

    void addFoodItem(const User& currentUser, std::vector<FoodItem>& foodItems, std::queue<Notification>& notifications) {
        std::string name;
        int quantity, daysToExpiration;
        std::cout << "Enter food item name: ";
        std::cin >> name;
        if (name.empty()) {
            throw InvalidArgumentException("\033[1;31mFood item name cannot be empty.\033[0m");
        }

        std::cout << "Enter quantity: ";
        std::cin >> quantity;
        if (quantity <= 0) {
            throw InvalidArgumentException("\033[1;31mQuantity must be greater than 0.\033[0m");
        }

        std::cout << "Enter days to expiration: ";
        std::cin >> daysToExpiration;
        if (daysToExpiration <= 0) {
            throw InvalidArgumentException("\033[1;31mDays to expiration must be greater than 0.\033[0m");
        }

        if (currentUser.getUserType() == "restaurant") {
            const Restaurant* restaurant = static_cast<const Restaurant*>(&currentUser);
            restaurant->addFoodItem(name, quantity, daysToExpiration, foodItems, notifications);

            // Insert the food item into the BST
            foodItemBST.insert(FoodItem(name, quantity, daysToExpiration, *restaurant));

            std::cout << "\033[1;32mFood item added successfully.\033[0m" << std::endl;
        } else {
            throw InvalidArgumentException("\033[1;31mError: Only restaurants can add food items.\033[0m");
        }
    }

    void viewFoodItems(const User& currentUser) {
        std::vector<FoodItem> foodItems = foodItemBST.getFoodItems(currentUser);

        std::cout << "\033[1;34mFood Items:\033[0m" << std::endl;
        for (const FoodItem& item : foodItems) {
            std::cout << "\033[1;34mName:\033[0m " << item.getName() << ", \033[1;34mQuantity:\033[0m " << item.getQuantity() << ", \033[1;34mExpiration in\033[0m " << item.getDaysToExpiration() << " days" << std::endl;
        }
    }

    void viewExpiringItems(const User& currentUser) {
        std::cout << "\033[1;34mExpiring Food Items:\033[0m" << std::endl;
        time_t currentTime = time(nullptr);

        std::vector<FoodItem> foodItems = foodItemBST.getFoodItems(currentUser);

        for (const FoodItem& item : foodItems) {
            time_t expirationTime = currentTime + item.getDaysToExpiration() * 24 * 60 * 60;
            if (expirationTime <= currentTime) {
                std::cout << "\033[1;34mName:\033[0m " << item.getName() << ", \033[1;34mQuantity:\033[0m " << item.getQuantity() << ", \033[1;34mExpiration in\033[0m " << item.getDaysToExpiration() << " days" << std::endl;
            }
        }
    }

    void viewNotifications(std::queue<Notification>& notifications) {
        std::cout << "\033[1;34mNotifications:\033[0m" << std::endl;
        while (!notifications.empty()) {
            Notification notification = notifications.front();
            std::cout << "\033[1;34mMessage:\033[0m " << notification.getMessage() << ", \033[1;34mRecipient:\033[0m " << notification.getRecipient().getUsername() << std::endl;
            notifications.pop();
        }
    }
};

int main() {
    FoodApp app;
    app.run();
    return 0;
}
