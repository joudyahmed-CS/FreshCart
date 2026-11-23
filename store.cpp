#include "store.h"
#include <iomanip>
#include <sstream>

namespace freshcart {
Store createStore() {
    return {{{
        {101, "Whole Milk",      "Dairy",   "1 litre",  4200, 30},
        {102, "Greek Yogurt",    "Dairy",   "200 g",    3500, 24},
        {103, "Cheddar Cheese",  "Dairy",   "250 g",    8500, 20},
        {104, "Sourdough Bread", "Bakery",  "1 loaf",   5500, 18},
        {105, "Butter Croissant","Bakery",  "1 piece",  2800, 32},
        {106, "Red Apples",      "Produce", "1 kg",     6500, 40},
        {107, "Fresh Bananas",   "Produce", "1 kg",     3800, 35},
        {108, "Tomatoes",        "Produce", "1 kg",     2200, 45},
        {109, "Orange Juice",    "Drinks",  "1 litre",  4800, 25},
        {110, "Mineral Water",   "Drinks",  "1.5 litres",1200,60},
        {111, "Basmati Rice",    "Pantry",  "1 kg",     9500, 28},
        {112, "Penne Pasta",     "Pantry",  "500 g",    3200, 36}
    }}, {}};
}
std::string money(Money amount) {
    std::ostringstream out;
    out << (amount < 0 ? "-" : "") << (amount < 0 ? -amount : amount) / 100
        << '.' << std::setfill('0') << std::setw(2)
        << (amount < 0 ? -amount : amount) % 100;
    return out.str();
}
bool setQuantity(const Store& store, Cart& cart, int index,
                 int quantity, std::string& error) {
    error.clear();
    // Requirement: logical operators and two kinds of invalid input.
    if (index < 0 || index >= PRODUCT_COUNT) {
        error = "Choose a product from the catalog.";
        return false;
    }
    if (quantity < 0 || quantity > store.products[index].stock) {
        error = "Quantity must be between 0 and available stock ("
            + std::to_string(store.products[index].stock) + ").";
        return false;
    }
    cart.quantities[index] = quantity;
    return true;
}
Totals calculate(const Store& store, const Cart& cart) {
    Totals result;
    // Requirement: for loop and multiplication/addition calculations.
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        result.subtotal += store.products[i].price * cart.quantities[i];
        result.items += cart.quantities[i];
    }
    // Requirement: combined logical condition and nested if.
    if (cart.member && result.subtotal >= MEMBER_THRESHOLD) {
        if (result.subtotal >= GOLD_THRESHOLD) {
            result.discountPercent = GOLD_PERCENT;
            result.status = "Gold member discount: 10% applied";
        } else {
            result.discountPercent = MEMBER_PERCENT;
            result.status = "Member discount: 5% applied";
        }
    } else {
        if (result.items == 0) {
            result.status = "Add products to start a sale";
        } else if (!cart.member) {
            result.status = "Standard pricing - no member discount";
        } else {
            result.status = "Spend EGP " + money(MEMBER_THRESHOLD - result.subtotal)
                + " more to unlock 5% off";
        }
    }
    // Round the percentage discount to the nearest piastre (half up).
    result.discount = (result.subtotal * result.discountPercent + 50) / 100;
    result.total = result.subtotal - result.discount;
    return result;
}
bool checkout(Store& store, Cart& cart, Money paid,
              Sale& completedSale, std::string& error) {
    error.clear();
    const Totals totals = calculate(store, cart);
    if (totals.items == 0) {
        error = "The cart is empty. Add a product first.";
        return false;
    }
    // Revalidate all quantities before changing any inventory or sales data.
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        if (cart.quantities[i] < 0 || cart.quantities[i] > store.products[i].stock) {
            error = "Stock changed. Check the cart quantities.";
            return false;
        }
    }
    if (paid < totals.total) {
        error = "Cash received must cover the total amount.";
        return false;
    }
    if (paid > 100000000) {
        error = "Cash received cannot exceed EGP 1,000,000.00.";
        return false;
    }
    Sale sale;
    sale.number = static_cast<int>(store.sales.size()) + 1;
    sale.totals = totals;
    sale.paid = paid;
    sale.change = paid - totals.total;
    std::ostringstream receipt;
    receipt << "FRESHCART | MINI SUPERMARKET\n"
            << "Receipt #" << std::setfill('0') << std::setw(4) << sale.number
            << "\nAll amounts in EGP\n"
            << "----------------------------------------\n";
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        const int quantity = cart.quantities[i];
        if (quantity > 0) {
            receipt << store.products[i].name << '\n'
                    << "  " << quantity << " x " << money(store.products[i].price)
                    << " = " << money(store.products[i].price * quantity) << '\n';
        }
    }
    receipt << "----------------------------------------\n"
            << "Subtotal:        " << money(totals.subtotal) << '\n'
            << "Discount (" << totals.discountPercent << "%):  " << money(totals.discount) << '\n'
            << "TOTAL:           " << money(totals.total) << '\n'
            << "Cash received:   " << money(paid) << '\n'
            << "Change:          " << money(sale.change) << '\n'
            << totals.status << "\nThank you. Have a fresh day!\n";
    sale.receipt = receipt.str();
    store.sales.push_back(sale);
    for (int i = 0; i < PRODUCT_COUNT; ++i)
        store.products[i].stock -= cart.quantities[i];
    completedSale = sale;
    cart = Cart{};
    return true;
}
Money totalRevenue(const Store& store) {
    Money revenue = 0;
    for (const auto& sale : store.sales) revenue += sale.totals.total;
    return revenue;
}
}
