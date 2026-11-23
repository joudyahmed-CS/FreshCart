#pragma once
#include "store.h"
#include <QMainWindow>
class QCheckBox;
class QCloseEvent;
class QComboBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QStackedWidget;
class QTextEdit;
class QVBoxLayout;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    freshcart::Store store_ = freshcart::createStore();
    freshcart::Cart cart_;
    QStackedWidget* pages_ = nullptr;
    QPushButton* posNav_ = nullptr;
    QPushButton* salesNav_ = nullptr;
    QLineEdit* search_ = nullptr;
    QComboBox* category_ = nullptr;
    QGridLayout* catalog_ = nullptr;
    QVBoxLayout* cartRows_ = nullptr;
    QCheckBox* member_ = nullptr;
    QLabel* cartCount_ = nullptr;
    QLabel* subtotal_ = nullptr;
    QLabel* discount_ = nullptr;
    QLabel* total_ = nullptr;
    QLabel* discountStatus_ = nullptr;
    QLabel* notification_ = nullptr;
    QPushButton* pay_ = nullptr;
    QLabel* revenue_ = nullptr;
    QLabel* transactions_ = nullptr;
    QLabel* units_ = nullptr;
    QListWidget* receiptList_ = nullptr;
    QTextEdit* receiptView_ = nullptr;
    QPushButton* save_ = nullptr;
    QWidget* makePos();
    QWidget* makeSales();
    void showPage(int index);
    void refreshCatalog();
    void refreshCart();
    void refreshSales();
    void changeQuantity(int index, int quantity);
    void clearCart();
    void takePayment();
    void saveReceipt();
    void notify(const QString& text, bool error = false);
};
