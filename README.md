# Restaurant Management System

**Suggested project title:** *Restaurant Management System: Menu, Orders & Customer Administration*

A console-based restaurant application written in standard C++17. It supports customer sign-up and login, menu ordering, manager operations, and simple text-file reports.

## Data structures and algorithms

- **Binary search tree (BST):** stores menu items by name and supports insertion, search, deletion, and in-order display.
- **Doubly linked list:** `std::list<Customer>` keeps customers in insertion order and supports front/back operations and deletion.
- **Queue:** `std::queue<Order>` keeps each customer's order lines in FIFO order.
- **Sorting:** customer views use `std::stable_sort` to order customers by username or total spending.

The implementations use standard C++ ownership and containers instead of manual fixed-size arrays, raw owning pointers, or Windows-only APIs.

## Build and run

Requires a C++17 compiler.

### Windows (MinGW-w64)

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic restaurant_management_system.cpp -o restaurant_management_system.exe
.\restaurant_management_system.exe
```

### Linux or macOS

```sh
g++ -std=c++17 -Wall -Wextra -pedantic restaurant_management_system.cpp -o restaurant_management_system
./restaurant_management_system
```

Run the application from the project directory so its text files are created there.

## Main features

- Register customers and log in with username/password.
- Place orders from the menu and view a receipt.
- Update customer account details.
- Add or remove customers and menu items from the manager menu.
- View customer lists in both directions and sort by username or spending.
- View company and per-item revenue.

Set the `RESTAURANT_MANAGER_PASSWORD` environment variable before launching the program to enable the manager menu. If it is unset, manager access is disabled. There are no built-in customer accounts; customers can sign up from the main menu.

## Data files

- `Receipt.txt` — appended order receipts.
- `ReceiptNumber.txt` — next receipt number.
- `Customerdetails.txt` — customer report written when the application exits.
- `Fooditem-details.txt` — menu and item-revenue report written when the application exits.

Customer report files intentionally omit passwords.
