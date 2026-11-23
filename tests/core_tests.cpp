#include "store.h"
#include <cstdlib>
#include <iostream>
using namespace freshcart;
int checks = 0;
void check(bool result, const char* message) {
    ++checks;
    if (!result) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
Totals subtotalCase(Money price, bool member) {
    Store store = createStore(); store.products[0].price = price;
    Cart cart; cart.quantities[0] = 1; cart.member = member;
    return calculate(store, cart);
}
int main() {
    Store store = createStore(); Cart cart; Sale sale; std::string error;
    check(calculate(store, cart).items == 0, "empty cart");
    check(!checkout(store, cart, 0, sale, error), "empty checkout rejected");
    check(!setQuantity(store, cart, -1, 1, error), "negative index rejected");
    check(!setQuantity(store, cart, 12, 1, error), "invalid index rejected");
    check(!setQuantity(store, cart, 0, -1, error), "negative quantity rejected");
    check(!setQuantity(store, cart, 0, 31, error), "stock overrun rejected");
    check(setQuantity(store, cart, 0, 30, error), "exact stock accepted");
    check(setQuantity(store, cart, 0, 0, error), "remove quantity");
    check(calculate(store, cart).items == 0, "remove empties cart");
    check(subtotalCase(49999, true).discountPercent == 0, "below member threshold");
    check(subtotalCase(50000, true).discount == 2500, "exact 500 threshold");
    check(subtotalCase(99999, true).discountPercent == 5, "below gold threshold");
    check(subtotalCase(100000, true).discount == 10000, "exact 1000 threshold");
    check(subtotalCase(150000, false).discount == 0, "non-member cannot discount");
    check(subtotalCase(50010, true).discount == 2501, "half-piastre rounds up");
    check(money(5) == "0.05" && money(50000) == "500.00", "money formatting");
    setQuantity(store, cart, 0, 2, error); setQuantity(store, cart, 10, 6, error); cart.member = true;
    auto totals = calculate(store, cart);
    check(totals.subtotal == 65400 && totals.discount == 3270 && totals.total == 62130, "mixed cart totals");
    check(!checkout(store, cart, 62129, sale, error), "one piastre short rejected");
    check(store.sales.empty() && store.products[0].stock == 30 && cart.quantities[0] == 2, "failed payment is atomic");
    check(!checkout(store, cart, 100000001, sale, error), "excessive cash rejected");
    check(checkout(store, cart, 70000, sale, error), "successful member sale");
    check(sale.change == 7870 && sale.number == 1 && sale.totals.items == 8, "change and receipt number");
    check(store.products[0].stock == 28 && store.products[10].stock == 22, "stock committed");
    check(!cart.member && calculate(store, cart).items == 0, "cart and membership reset");
    check(sale.receipt.find("621.30") != std::string::npos && sale.receipt.find("5%") != std::string::npos, "receipt content");
    setQuantity(store, cart, 6, 2, error);
    check(checkout(store, cart, 7600, sale, error), "exact cash second sale");
    check(sale.change == 0 && sale.number == 2 && totalRevenue(store) == 69730, "session revenue is net sales");
    cart.quantities[1] = 25;
    check(!checkout(store, cart, 100000, sale, error), "stale stock rejected at checkout");
    check(store.sales.size() == 2 && store.products[1].stock == 24, "stock rejection is atomic");
    Store soldOut = createStore(); Cart all;
    setQuantity(soldOut, all, 2, 20, error);
    check(checkout(soldOut, all, 170000, sale, error) && soldOut.products[2].stock == 0, "sell exact stock");
    check(!setQuantity(soldOut, all, 2, 1, error), "sold-out purchase rejected");
    std::cout << checks << " billing and inventory checks passed.\n";
}
