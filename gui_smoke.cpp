#include "mainwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <cstdlib>
#include <iostream>

void require(bool value, const char* message) {
    if (!value) { std::cerr << "GUI FAIL: " << message << '\n'; std::exit(1); }
}
void settle() {
    QApplication::processEvents();
    QApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    QApplication::processEvents();
}
QPushButton* product(MainWindow& window, int index) {
    settle();
    for (auto* button : window.findChildren<QPushButton*>("add"))
        if (button->property("productIndex").toInt() == index) return button;
    return nullptr;
}
void add(MainWindow& window, int index, int count) {
    for (int i = 0; i < count; ++i) {
        auto* button = product(window, index); require(button && button->isEnabled(), "product available");
        button->click(); settle();
    }
}
QPushButton* checkoutButton(MainWindow& window) {
    for (auto* button : window.findChildren<QPushButton*>())
        if (button->property("role").toString() == "checkout") return button;
    return nullptr;
}
void capture(MainWindow& window, const QString& path) {
    settle(); require(window.grab().save(path), "screenshot saved");
}
int main(int argc, char** argv) {
    QApplication app(argc, argv); app.setStyle("Fusion"); app.setFont(QFont("DejaVu Sans", 10));
    const QString out = argc > 1 ? QString::fromLocal8Bit(argv[1]) : "screenshots";
    QDir().mkpath(out);
    MainWindow w; w.show(); settle();
    auto* pay = checkoutButton(w); require(pay && !pay->isEnabled(), "empty cart disables payment");
    auto* search = w.findChild<QLineEdit*>("productSearch");
    search->setText("no-product-xyz"); settle(); require(product(w, 0) == nullptr, "empty search result");
    search->setText("101"); settle(); require(product(w, 0) && !product(w, 1), "search by ID");
    search->clear(); auto* category = w.findChild<QComboBox*>("categoryFilter");
    category->setCurrentText("Dairy"); settle(); require(product(w, 0) && !product(w, 5), "category filter");
    category->setCurrentIndex(0);
    add(w, 0, 2); add(w, 10, 6);
    require(w.findChild<QLabel*>("subtotalValue")->text() == "EGP 654.00", "GUI subtotal");
    capture(w, out + "/01_standard_order.png");
    auto* member = w.findChild<QCheckBox*>("memberToggle"); member->setChecked(true); settle();
    require(w.findChild<QLabel*>("grandTotal")->text() == "EGP 621.30", "GUI member total");
    capture(w, out + "/02_member_discount.png");
    // Test cancelling without changing the order or stock.
    QTimer::singleShot(0, [&] {
        auto* dialog = w.findChild<QDialog*>("paymentDialog"); require(dialog, "cancel dialog opened");
        dialog->findChild<QPushButton*>("cancelPayment")->click();
    });
    pay->click(); settle(); require(w.findChild<QLabel*>("grandTotal")->text() == "EGP 621.30", "cancel preserves cart");
    QTimer::singleShot(0, [&] {
        auto* dialog = w.findChild<QDialog*>("paymentDialog"); require(dialog, "payment dialog opened");
        auto* cash = dialog->findChild<QDoubleSpinBox*>("cashInput"); auto* confirm = dialog->findChild<QPushButton*>("confirmPayment");
        cash->setValue(600); confirm->click();
        require(!dialog->findChild<QLabel*>("paymentError")->text().isEmpty(), "insufficient payment rejected");
        settle(); require(dialog->grab().save(out + "/03_insufficient_payment.png"), "validation screenshot");
        cash->setValue(700); confirm->click();
    });
    pay->click(); settle();
    auto* list = w.findChild<QListWidget*>("receiptList"); require(list->count() == 1, "sale recorded once");
    const QString receipt = w.findChild<QTextEdit*>("receiptPreview")->toPlainText();
    require(receipt.contains("621.30") && receipt.contains("78.70"), "receipt total and change");
    require(!member->isChecked() && !pay->isEnabled(), "cart reset after sale");
    capture(w, out + "/04_completed_receipt.png");
    w.findChild<QStackedWidget*>("pages")->setCurrentIndex(0);
    add(w, 6, 2);
    QTimer::singleShot(0, [&] {
        auto* dialog = w.findChild<QDialog*>("paymentDialog"); require(dialog, "second payment dialog");
        dialog->findChild<QPushButton*>("confirmPayment")->click();
    });
    pay->click(); settle(); require(list->count() == 2, "second sale recorded");
    require(w.findChild<QTextEdit*>("receiptPreview")->toPlainText().contains("76.00"), "second receipt");
    capture(w, out + "/05_standard_receipt.png");
    list->setCurrentRow(0); require(w.findChild<QTextEdit*>("receiptPreview")->toPlainText().contains("621.30"), "history selection");
    capture(w, out + "/05_sales_history.png");
    // Third independent order: gold threshold and successful exact-cash payment.
    w.findChild<QStackedWidget*>("pages")->setCurrentIndex(0);
    add(w, 10, 11); member->setChecked(true); settle();
    require(w.findChild<QLabel*>("grandTotal")->text() == "EGP 940.50", "gold total");
    capture(w, out + "/06_gold_order.png");
    QTimer::singleShot(0, [&] {
        auto* dialog = w.findChild<QDialog*>("paymentDialog"); require(dialog, "gold payment dialog");
        dialog->findChild<QPushButton*>("confirmPayment")->click();
    });
    pay->click(); settle(); require(list->count() == 3, "third sale recorded");
    require(w.findChild<QTextEdit*>("receiptPreview")->toPlainText().contains("10%"), "gold receipt");
    capture(w, out + "/07_gold_receipt.png");
    std::cout << "GUI workflow passed: filters, cart, discounts, cancel, insufficient cash, three sales, gold discount, receipt history.\n";
    // Hide rather than close: the production close confirmation is intentionally modal.
    w.hide(); return 0;
}
