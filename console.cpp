#include "store.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace freshcart;

// Read a whole line: reject letters, trailing junk, decimals in integer fields,
// negatives and out-of-range values, then allow another attempt.
bool readInteger(const std::string& prompt, int low, int high, int& value) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) return false;
        std::istringstream input(line);
        int candidate;
        if ((input >> candidate) && (input >> std::ws).eof()
            && candidate >= low && candidate <= high) {
            value = candidate;
            return true;
        }
        std::cout << "Invalid input. Enter a whole number from " << low << " to " << high << ".\n";
    }
}
bool readCash(Money minimum, Money& amount) {
    std::string line;
    while (true) {
        std::cout << "Cash received (EGP): ";
        if (!std::getline(std::cin, line)) return false;
        // Parse decimal text without floating-point rounding or exponent notation.
        const auto dot = line.find('.');
        const std::string whole = line.substr(0, dot);
        const std::string fraction = dot == std::string::npos ? "" : line.substr(dot + 1);
        bool digits = !whole.empty() && whole.size() <= 7 && fraction.size() <= 2;
        for (char c : whole) if (c < '0' || c > '9') digits = false;
        for (char c : fraction) if (c < '0' || c > '9') digits = false;
        if (digits) {
            Money parsed = std::stoll(whole) * 100;
            if (!fraction.empty()) parsed += std::stoi(fraction) * (fraction.size() == 1 ? 10 : 1);
            if (parsed >= minimum && parsed <= 100000000) {
                amount = parsed;
                return true;
            }
        }
        std::cout << "Invalid cash. Enter " << money(minimum)
                  << " to 1000000.00, with at most two decimal places.\n";
    }
}
void showProducts(const Store& store) {
    std::cout << "\nID   Product                Price (EGP)  Stock\n";
    for (const auto& p : store.products) {
        std::cout << p.id << "  " << std::left << std::setw(23) << p.name
                  << std::setw(13) << money(p.price) << p.stock << '\n';
    }
}
void showCart(const Store& store, const Cart& cart) {
    const auto totals = calculate(store, cart);
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        if (cart.quantities[i] > 0)
            std::cout << store.products[i].name << " x " << cart.quantities[i]
                      << " = EGP " << money(store.products[i].price * cart.quantities[i]) << '\n';
    }
    std::cout << "Subtotal: EGP " << money(totals.subtotal)
              << " | Discount: EGP " << money(totals.discount)
              << " | Total: EGP " << money(totals.total)
              << '\n' << totals.status << '\n';
}
int main() {
    Store store = createStore();
    Cart cart;
    int choice = 0;
    std::cout << "Welcome to FreshCart - Mini Supermarket Billing System\n"
              << "Session-only data. Prices are sample prices in EGP.\n";
    // Requirement: do-while keeps menu running; switch handles the choices.
    do {
        std::cout << "\n1. Show products\n2. Set cart quantity\n3. View cart / member status\n"
                  << "4. Checkout\n5. Sales summary\n6. Exit\n";
        if (!readInteger("Choose (1-6): ", 1, 6, choice)) break;
        switch (choice) {
        case 1: showProducts(store); break;
        case 2: {
            showProducts(store);
            int id, quantity;
            if (!readInteger("Product ID: ", 101, 112, id)) return 0;
            const int index = id - 101;
            if (!readInteger("New TOTAL quantity (0 removes): ", 0,
                             store.products[index].stock, quantity)) return 0;
            std::string error;
            if (setQuantity(store, cart, index, quantity, error))
                std::cout << "Cart updated.\n";
            else std::cout << error << '\n';
            break;
        }
        case 3: {
            int member;
            if (!readInteger("Registered member? (1=yes, 0=no): ", 0, 1, member)) return 0;
            cart.member = member == 1;
            showCart(store, cart);
            break;
        }
        case 4: {
            showCart(store, cart);
            const auto totals = calculate(store, cart);
            if (totals.items == 0) {
                std::cout << "Cannot checkout an empty cart.\n";
            } else {
                Money paid;
                if (!readCash(totals.total, paid)) return 0;
                Sale sale;
                std::string error;
                if (checkout(store, cart, paid, sale, error)) std::cout << '\n' << sale.receipt;
                else std::cout << error << '\n';
            }
            break;
        }
        case 5:
            std::cout << "Completed sales: " << store.sales.size()
                      << " | Revenue: EGP " << money(totalRevenue(store)) << '\n';
            break;
        case 6:
            std::cout << "Goodbye! Session data is now cleared.\n";
            break;
        default:
            std::cout << "Invalid menu choice. Try again.\n";
            break;
        }
    } while (choice != 6);
    return 0;
}
