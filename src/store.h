#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace freshcart {
// Shared by the GUI and console. Monetary amounts are integer piastres.
using Money = std::int64_t;
constexpr int PRODUCT_COUNT = 12;
constexpr Money MEMBER_THRESHOLD = 50000;  // EGP 500.00
constexpr Money GOLD_THRESHOLD = 100000;   // EGP 1,000.00
constexpr int MEMBER_PERCENT = 5;
constexpr int GOLD_PERCENT = 10;

struct Product {
    int id;
    std::string name;
    std::string category;
    std::string unit;
    Money price;
    int stock;
};
struct Cart {
    std::array<int, PRODUCT_COUNT> quantities{};
    bool member = false;
};
struct Totals {
    Money subtotal = 0;
    Money discount = 0;
    Money total = 0;
    int items = 0;
    int discountPercent = 0;
    std::string status;
};
struct Sale {
    int number = 0;
    Totals totals;
    Money paid = 0;
    Money change = 0;
    std::string receipt;
};
struct Store {
    std::array<Product, PRODUCT_COUNT> products;
    std::vector<Sale> sales;
};
Store createStore();
std::string money(Money amount);
bool setQuantity(const Store& store, Cart& cart, int productIndex,
                 int quantity, std::string& error);
Totals calculate(const Store& store, const Cart& cart);
bool checkout(Store& store, Cart& cart, Money paid,
              Sale& completedSale, std::string& error);
Money totalRevenue(const Store& store);
}
