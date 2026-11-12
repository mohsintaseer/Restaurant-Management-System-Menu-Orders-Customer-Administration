#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

struct InputEnded {};

struct Order {
    std::string itemName;
    int quantity;
    int unitPrice;
};

struct Customer {
    std::string username;
    std::string password;
    std::string email;
    std::string address;
    long long totalSpent;

    Customer(const std::string& usernameValue,
             const std::string& passwordValue,
             const std::string& emailValue,
             const std::string& addressValue,
             long long totalSpentValue = 0)
        : username(usernameValue),
          password(passwordValue),
          email(emailValue),
          address(addressValue),
          totalSpent(totalSpentValue) {}
};

class CustomerList {
public:
    Customer* find(const std::string& username) {
        for (Customer& customer : customers) {
            if (customer.username == username) {
                return &customer;
            }
        }
        return nullptr;
    }

    const Customer* find(const std::string& username) const {
        for (const Customer& customer : customers) {
            if (customer.username == username) {
                return &customer;
            }
        }
        return nullptr;
    }

    void addToFront(const Customer& customer) {
        customers.push_front(customer);
    }

    void addToBack(const Customer& customer) {
        customers.push_back(customer);
    }

    bool removeFromFront() {
        if (customers.empty()) {
            return false;
        }
        customers.pop_front();
        return true;
    }

    bool removeFromBack() {
        if (customers.empty()) {
            return false;
        }
        customers.pop_back();
        return true;
    }

    bool remove(const std::string& username) {
        for (std::list<Customer>::iterator customer = customers.begin();
             customer != customers.end(); ++customer) {
            if (customer->username == username) {
                customers.erase(customer);
                return true;
            }
        }
        return false;
    }

    std::vector<const Customer*> sortedByUsername() const {
        std::vector<const Customer*> result;
        for (const Customer& customer : customers) {
            result.push_back(&customer);
        }
        std::stable_sort(result.begin(), result.end(),
                         [](const Customer* left, const Customer* right) {
                             return left->username < right->username;
                         });
        return result;
    }

    std::vector<const Customer*> sortedBySpending() const {
        std::vector<const Customer*> result;
        for (const Customer& customer : customers) {
            result.push_back(&customer);
        }
        std::stable_sort(result.begin(), result.end(),
                         [](const Customer* left, const Customer* right) {
                             return left->totalSpent < right->totalSpent;
                         });
        return result;
    }

    void displayFromFront(std::ostream& output) const {
        if (customers.empty()) {
            output << "No customers exist.\n";
            return;
        }
        int number = 1;
        for (const Customer& customer : customers) {
            displayCustomer(output, customer, number++);
        }
    }

    void displayFromBack(std::ostream& output) const {
        if (customers.empty()) {
            output << "No customers exist.\n";
            return;
        }
        int number = 1;
        for (std::list<Customer>::const_reverse_iterator customer =
                 customers.rbegin();
             customer != customers.rend(); ++customer) {
            displayCustomer(output, *customer, number++);
        }
    }

    static void displayCustomer(std::ostream& output,
                                const Customer& customer,
                                int number) {
        output << "Customer " << number << '\n'
               << "Username: " << customer.username << '\n'
               << "Email: " << customer.email << '\n'
               << "Address: " << customer.address << '\n'
               << "Total spent: " << customer.totalSpent << "\n\n";
    }

private:
    std::list<Customer> customers;
};

struct MenuItem {
    std::string name;
    int price;
    long long revenue;
    std::unique_ptr<MenuItem> left;
    std::unique_ptr<MenuItem> right;

    MenuItem(const std::string& itemName, int itemPrice)
        : name(itemName), price(itemPrice), revenue(0) {}
};

class MenuTree {
public:
    bool insert(const std::string& name, int price) {
        return insert(root, name, price);
    }

    MenuItem* find(const std::string& name) {
        MenuItem* current = root.get();
        while (current != nullptr) {
            if (name == current->name) {
                return current;
            }
            current = name < current->name ? current->left.get()
                                           : current->right.get();
        }
        return nullptr;
    }

    bool erase(const std::string& name) {
        return erase(root, name);
    }

    void display(std::ostream& output) const {
        displayInOrder(root.get(), output, false);
    }

    void displayRevenue(std::ostream& output) const {
        displayInOrder(root.get(), output, true);
    }

    void writeReport(std::ostream& output) const {
        writeInOrder(root.get(), output);
    }

private:
    std::unique_ptr<MenuItem> root;

    static bool insert(std::unique_ptr<MenuItem>& node,
                       const std::string& name,
                       int price) {
        if (!node) {
            node.reset(new MenuItem(name, price));
            return true;
        }
        if (name == node->name) {
            return false;
        }
        return insert(name < node->name ? node->left : node->right,
                      name, price);
    }

    static bool erase(std::unique_ptr<MenuItem>& node,
                      const std::string& name) {
        if (!node) {
            return false;
        }
        if (name < node->name) {
            return erase(node->left, name);
        }
        if (name > node->name) {
            return erase(node->right, name);
        }
        if (!node->left) {
            node = std::move(node->right);
        } else if (!node->right) {
            node = std::move(node->left);
        } else {
            MenuItem* successor = node->right.get();
            while (successor->left) {
                successor = successor->left.get();
            }
            node->name = successor->name;
            node->price = successor->price;
            node->revenue = successor->revenue;
            erase(node->right, successor->name);
        }
        return true;
    }

    static void displayInOrder(const MenuItem* node,
                               std::ostream& output,
                               bool showRevenue) {
        if (node == nullptr) {
            return;
        }
        displayInOrder(node->left.get(), output, showRevenue);
        output << "* " << node->name << " - " << node->price;
        if (showRevenue) {
            output << " (earned: " << node->revenue << ')';
        }
        output << '\n';
        displayInOrder(node->right.get(), output, showRevenue);
    }

    static void writeInOrder(const MenuItem* node, std::ostream& output) {
        if (node == nullptr) {
            return;
        }
        writeInOrder(node->left.get(), output);
        output << "* " << node->name << ":\nPrice: " << node->price
               << "\nAmount earned: " << node->revenue << "\n\n";
        writeInOrder(node->right.get(), output);
    }
};

class RestaurantSystem {
public:
    RestaurantSystem() : companyRevenue(0), nextReceiptNumber(readReceiptNumber()) {
        seedMenu();
    }

    void run() {
        bool running = true;
        try {
            while (running) {
                std::cout << "\n=== Restaurant Management System ===\n"
                          << "1. Manager menu\n2. Sign up\n3. Log in\n4. Exit\n";
                const int choice = readInteger("Choose an option: ");
                switch (choice) {
                case 1:
                    managerMenu();
                    break;
                case 2:
                    signUp();
                    break;
                case 3:
                    logIn();
                    break;
                case 4:
                    running = false;
                    break;
                default:
                    std::cout << "Choose an option from 1 to 4.\n";
                }
            }
        } catch (const InputEnded&) {
            std::cout << "\nInput closed; exiting.\n";
        }
        writeReports();
        std::cout << "Goodbye!\n";
    }

private:
    CustomerList customers;
    MenuTree menu;
    long long companyRevenue;
    int nextReceiptNumber;

    static std::string readLine(const std::string& prompt) {
        std::string value;
        std::cout << prompt;
        if (!std::getline(std::cin, value)) {
            if (std::cin.eof()) {
                throw InputEnded();
            }
            throw std::runtime_error("Failed to read console input.");
        }
        return value;
    }

    static int readInteger(const std::string& prompt) {
        while (true) {
            const std::string input = readLine(prompt);
            std::istringstream parser(input);
            int value;
            char extra;
            if (parser >> value && !(parser >> extra)) {
                return value;
            }
            std::cout << "Please enter a valid whole number.\n";
        }
    }

    static int readPositiveInteger(const std::string& prompt) {
        while (true) {
            const int value = readInteger(prompt);
            if (value > 0) {
                return value;
            }
            std::cout << "Enter a number greater than zero.\n";
        }
    }

    static bool hasValidPassword(const std::string& password) {
        bool hasUppercase = false;
        bool hasDigit = false;
        for (unsigned char character : password) {
            hasUppercase = hasUppercase || std::isupper(character) != 0;
            hasDigit = hasDigit || std::isdigit(character) != 0;
        }
        return hasUppercase && hasDigit;
    }

    std::string readUniqueUsername(const Customer* currentCustomer = nullptr) {
        while (true) {
            const std::string username = readLine("Username: ");
            const Customer* existing = customers.find(username);
            if (existing == nullptr || existing == currentCustomer) {
                return username;
            }
            std::cout << "That username is already in use.\n";
        }
    }

    std::string readPassword() const {
        while (true) {
            const std::string password =
                readLine("Password (must include an uppercase letter and a number): ");
            if (hasValidPassword(password)) {
                return password;
            }
            std::cout << "Password does not meet the requirements.\n";
        }
    }

    void signUp() {
        std::cout << "\n=== Sign up ===\n";
        const std::string username = readUniqueUsername();
        const std::string password = readPassword();
        const std::string email = readLine("Email: ");
        const std::string address = readLine("Address: ");
        customers.addToBack(Customer(username, password, email, address));
        std::cout << "Account created successfully.\n";
    }

    Customer* authenticate() {
        const std::string username = readLine("Username: ");
        Customer* customer = customers.find(username);
        if (customer == nullptr) {
            std::cout << "Username not found.\n";
            return nullptr;
        }
        for (int attempt = 1; attempt <= 3; ++attempt) {
            if (readLine("Password: ") == customer->password) {
                return customer;
            }
            std::cout << "Incorrect password (" << attempt << " of 3).\n";
        }
        return nullptr;
    }

    void logIn() {
        Customer* customer = authenticate();
        if (customer == nullptr) {
            return;
        }
        bool loggedIn = true;
        while (loggedIn) {
            std::cout << "\n=== Customer menu ===\n"
                      << "1. Order food\n2. Change username\n3. Change password\n"
                      << "4. Change email\n5. Change address\n6. View details\n"
                      << "7. Log out\n";
            const int choice = readInteger("Choose an option: ");
            switch (choice) {
            case 1:
                placeOrder(*customer);
                break;
            case 2:
                customer->username = readUniqueUsername(customer);
                break;
            case 3:
                customer->password = readPassword();
                break;
            case 4:
                customer->email = readLine("New email: ");
                break;
            case 5:
                customer->address = readLine("New address: ");
                break;
            case 6:
                CustomerList::displayCustomer(std::cout, *customer, 1);
                break;
            case 7:
                loggedIn = false;
                break;
            default:
                std::cout << "Choose an option from 1 to 7.\n";
            }
        }
    }

    void placeOrder(Customer& customer) {
        std::queue<Order> orderQueue;
        bool ordering = true;
        std::cout << "\n=== Menu ===\n";
        menu.display(std::cout);

        while (ordering) {
            const std::string itemName = readLine("Item name (or 'done' to finish): ");
            if (itemName == "done") {
                break;
            }
            MenuItem* item = menu.find(itemName);
            if (item == nullptr) {
                std::cout << "That item is not on the menu.\n";
                continue;
            }
            const int quantity = readPositiveInteger("Quantity: ");
            orderQueue.push(Order{item->name, quantity, item->price});
            if (readInteger("Add another item? (1 yes, 0 no): ") != 1) {
                ordering = false;
            }
        }

        if (orderQueue.empty()) {
            std::cout << "No items were ordered.\n";
            return;
        }

        std::ofstream receipt("Receipt.txt", std::ios::app);
        if (!receipt) {
            std::cerr << "Could not open Receipt.txt for writing.\n";
            return;
        }
        long long total = 0;
        receipt << "Receipt #" << nextReceiptNumber << '\n';
        std::cout << "\n=== Receipt #" << nextReceiptNumber << " ===\n";
        while (!orderQueue.empty()) {
            const Order order = orderQueue.front();
            orderQueue.pop();
            const long long lineTotal =
                static_cast<long long>(order.unitPrice) * order.quantity;
            total += lineTotal;
            MenuItem* item = menu.find(order.itemName);
            if (item != nullptr) {
                item->revenue += lineTotal;
            }
            std::cout << order.itemName << " x " << order.quantity
                      << " - " << lineTotal << '\n';
            receipt << order.itemName << " x " << order.quantity
                    << " - " << lineTotal << '\n';
        }
        customer.totalSpent += total;
        companyRevenue += total;
        std::cout << "Total: " << total << '\n';
        receipt << "Total: " << total << "\n\n";
        ++nextReceiptNumber;
        writeReceiptNumber();
    }

    bool authenticateManager() const {
        const char* managerPassword =
            std::getenv("RESTAURANT_MANAGER_PASSWORD");
        if (managerPassword == nullptr || managerPassword[0] == '\0') {
            std::cerr << "Manager access is disabled. Set the "
                         "RESTAURANT_MANAGER_PASSWORD environment variable.\n";
            return false;
        }
        for (int attempt = 1; attempt <= 3; ++attempt) {
            if (readLine("Manager password: ") == managerPassword) {
                return true;
            }
            std::cout << "Incorrect password (" << attempt << " of 3).\n";
        }
        return false;
    }

    void managerMenu() {
        if (!authenticateManager()) {
            return;
        }
        bool managing = true;
        while (managing) {
            std::cout << "\n=== Manager menu ===\n"
                      << "1. Add customer to front\n2. Add customer to back\n"
                      << "3. View company revenue\n4. List customers from front\n"
                      << "5. List customers from back\n6. Sort customers by username\n"
                      << "7. Sort customers by spending\n8. Remove front customer\n"
                      << "9. Remove back customer\n10. Remove customer by username\n"
                      << "11. View menu\n12. Remove menu item\n"
                      << "13. Add menu item\n14. View item revenue\n15. Exit\n";
            const int choice = readInteger("Choose an option: ");
            switch (choice) {
            case 1:
                customers.addToFront(readCustomer());
                break;
            case 2:
                customers.addToBack(readCustomer());
                break;
            case 3:
                std::cout << "Company revenue: " << companyRevenue << '\n';
                break;
            case 4:
                customers.displayFromFront(std::cout);
                break;
            case 5:
                customers.displayFromBack(std::cout);
                break;
            case 6:
                displaySortedCustomers(customers.sortedByUsername());
                break;
            case 7:
                displaySortedCustomers(customers.sortedBySpending());
                break;
            case 8:
                std::cout << (customers.removeFromFront()
                                  ? "Front customer removed.\n"
                                  : "No customers to remove.\n");
                break;
            case 9:
                std::cout << (customers.removeFromBack()
                                  ? "Back customer removed.\n"
                                  : "No customers to remove.\n");
                break;
            case 10: {
                const std::string username = readLine("Username to remove: ");
                std::cout << (customers.remove(username)
                                  ? "Customer removed.\n"
                                  : "Customer not found.\n");
                break;
            }
            case 11:
                menu.display(std::cout);
                break;
            case 12: {
                const std::string name = readLine("Menu item to remove: ");
                std::cout << (menu.erase(name)
                                  ? "Menu item removed.\n"
                                  : "Menu item not found.\n");
                break;
            }
            case 13: {
                const std::string name = readLine("New menu item name: ");
                const int price = readPositiveInteger("Price: ");
                std::cout << (menu.insert(name, price)
                                  ? "Menu item added.\n"
                                  : "That menu item already exists.\n");
                break;
            }
            case 14:
                menu.displayRevenue(std::cout);
                break;
            case 15:
                managing = false;
                break;
            default:
                std::cout << "Choose an option from 1 to 15.\n";
            }
        }
    }

    Customer readCustomer() {
        const std::string username = readUniqueUsername();
        const std::string password = readPassword();
        const std::string email = readLine("Email: ");
        const std::string address = readLine("Address: ");
        const int spending = readInteger("Total spent: ");
        return Customer(username, password, email, address, spending);
    }

    static void displaySortedCustomers(
        const std::vector<const Customer*>& sortedCustomers) {
        if (sortedCustomers.empty()) {
            std::cout << "No customers exist.\n";
            return;
        }
        int number = 1;
        for (const Customer* customer : sortedCustomers) {
            CustomerList::displayCustomer(std::cout, *customer, number++);
        }
    }

    int readReceiptNumber() const {
        std::ifstream input("ReceiptNumber.txt");
        int number = 1;
        if (input >> number && number > 0) {
            return number;
        }
        return 1;
    }

    void writeReceiptNumber() const {
        std::ofstream output("ReceiptNumber.txt");
        if (!output) {
            std::cerr << "Could not update ReceiptNumber.txt.\n";
            return;
        }
        output << nextReceiptNumber;
    }

    void writeReports() const {
        std::ofstream customerReport("Customerdetails.txt");
        if (!customerReport) {
            std::cerr << "Could not write Customerdetails.txt.\n";
        } else {
            customerReport << "Customers in list order:\n";
            customers.displayFromFront(customerReport);
            customerReport << "\nCustomers sorted by spending:\n";
            displayCustomerReport(customers.sortedBySpending(), customerReport);
        }

        std::ofstream menuReport("Fooditem-details.txt");
        if (!menuReport) {
            std::cerr << "Could not write Fooditem-details.txt.\n";
        } else {
            menu.writeReport(menuReport);
        }
    }

    static void displayCustomerReport(
        const std::vector<const Customer*>& sortedCustomers,
        std::ostream& output) {
        int number = 1;
        for (const Customer* customer : sortedCustomers) {
            CustomerList::displayCustomer(output, *customer, number++);
        }
    }

    void seedMenu() {
        const std::pair<const char*, int> initialItems[] = {
            {"Biryani", 150},          {"Burger", 200},
            {"Chow-mein", 500},        {"Noodle-Soup", 290},
            {"Fried-Rice", 650},        {"Wonton", 675},
            {"Manchurian", 700},        {"Chilli-Chicken", 999},
            {"Spaghetti", 895},         {"Dumplings", 750},
            {"Nihari", 600},            {"Chicken-Karahi", 890},
            {"Haleem", 250},             {"Cheese-Burger", 250},
            {"Drink-Small", 60},         {"Drink-Large", 200},
            {"Icecream", 150},           {"Chocolate-Cake", 500}
        };
        for (const auto& item : initialItems) {
            menu.insert(item.first, item.second);
        }
    }

};

} // namespace

int main() {
    RestaurantSystem restaurant;
    restaurant.run();
    return 0;
}
