#include "mainwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSaveFile>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <cmath>
using namespace freshcart;

namespace {
QString q(const std::string& value) { return QString::fromStdString(value); }
QString egp(Money amount) { return "EGP " + q(money(amount)); }
QLabel* label(const QString& text, const char* name = "") {
    auto* item = new QLabel(text);
    item->setObjectName(name);
    return item;
}
void clearLayout(QLayout* layout) {
    while (auto* item = layout->takeAt(0)) {
        if (auto* widget = item->widget()) { widget->hide(); widget->deleteLater(); }
        if (auto* child = item->layout()) clearLayout(child);
        delete item;
    }
}
QPixmap productArt(const Product& product) {
    QPixmap pix(88, 88);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    const QColor green("#22735c"), orange("#f1a654");
    const QString category = q(product.category);
    const QColor background = category == "Dairy" ? QColor("#e9f1fc") :
        category == "Bakery" ? QColor("#fff1e2") : category == "Produce" ? QColor("#edf5e7") :
        category == "Drinks" ? QColor("#fff0e8") : QColor("#f0eefa");
    p.setPen(Qt::NoPen);
    p.setBrush(background);
    p.drawRoundedRect(QRectF(0, 0, 88, 88), 18, 18);
    // Small original vector illustrations, drawn with C++ QPainter.
    p.setBrush(QColor(0, 0, 0, 16));
    p.drawEllipse(QRectF(19, 68, 51, 8));
    if (category == "Dairy" || category == "Drinks") {
        if (product.id == 102) {
            p.setBrush(QColor("#ffffff")); p.drawRoundedRect(25, 34, 38, 34, 8, 8);
            p.setBrush(green); p.drawRoundedRect(22, 29, 44, 9, 3, 3);
            p.drawRoundedRect(28, 47, 32, 12, 3, 3);
        } else if (product.id == 103) {
            p.setBrush(QColor("#efbe5a")); p.drawPolygon(QPolygonF{QPointF(20,62),QPointF(68,62),QPointF(68,32)});
            p.setBrush(QColor("#d39b35")); p.drawEllipse(47, 48, 6, 6); p.drawEllipse(60, 48, 5, 5);
        } else {
            const bool water = product.id == 110;
            p.setBrush(water ? QColor("#c2e3f2") : QColor("#ffffff"));
            p.drawRoundedRect(29, 26, 31, 43, water ? 10 : 3, water ? 10 : 3);
            p.setBrush(product.id == 109 ? orange : green);
            p.drawRoundedRect(33, 17, 23, 11, 3, 3);
            p.drawRect(29, 43, 31, 16);
            p.setPen(Qt::white); p.setFont(QFont("Arial", 7, QFont::Bold));
            p.drawText(QRect(29, 43, 31, 16), Qt::AlignCenter, water ? "H2O" : product.id == 109 ? "JUICE" : "MILK");
        }
    } else if (category == "Produce") {
        p.setBrush(product.id == 108 ? QColor("#e57555") : product.id == 106 ? QColor("#d96b5d") : QColor("#edc85f"));
        p.drawEllipse(19, 34, 30, 31); p.drawEllipse(42, 31, 29, 32);
        p.setPen(QPen(green, 3)); p.drawLine(37, 36, 41, 24); p.drawLine(56, 32, 58, 20);
        p.setPen(Qt::NoPen); p.setBrush(green); p.drawEllipse(42, 20, 14, 7);
    } else if (category == "Bakery") {
        p.setBrush(QColor("#c78a46")); p.drawRoundedRect(15, 35, 59, 31, 15, 15);
        p.setBrush(QColor("#e7b77a")); p.drawRoundedRect(17, 30, 55, 29, 15, 15);
        p.setPen(QPen(QColor("#fff0d8"), 3));
        for (int x = 30; x <= 55; x += 12) p.drawLine(x, 36, x - 5, 48);
    } else {
        p.setBrush(QColor("#e3d4b8")); p.drawRoundedRect(24, 21, 41, 49, 5, 5);
        p.setBrush(product.id == 111 ? green : QColor("#9a715c")); p.drawRoundedRect(24, 34, 41, 23, 2, 2);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(24, 34, 41, 23), Qt::AlignCenter, product.id == 111 ? "RICE" : "PASTA");
    }
    return pix;
}
const char* theme = R"(
QMainWindow, QDialog { background: #f5f7fa; color: #1c302a; }
QWidget { color: #1c302a; font-size: 13px; }
QLabel { background: transparent; }
QFrame#sidebar { background: white; border-right: 1px solid #e4e9e6; }
QLabel#brand { font-size: 25px; font-weight: 800; color: #24745b; }
QLabel#eyebrow { color: #87938e; font-size: 10px; font-weight: 700; }
QLabel#title { font-size: 28px; font-weight: 700; }
QLabel#sectionTitle { font-size: 19px; font-weight: 700; }
QLabel#muted { color: #7c8982; font-size: 12px; }
QLabel#body { color: #66786e; }
QLabel#price { font-size: 17px; font-weight: 700; }
QLabel#productName { font-weight: 700; font-size: 14px; }
QLabel#metric { font-size: 27px; font-weight: 700; }
QLabel#grandTotal { font-size: 26px; font-weight: 800; }
QFrame#whitePanel, QFrame#productCard, QFrame#metricCard {
    background: white; border: 1px solid #e4e9e6; border-radius: 14px;
}
QFrame#banner { background: #e6f2eb; border: none; border-radius: 14px; }
QLabel#bannerTitle { font-size: 21px; font-weight: 700; color: #235e4a; }
QLabel#bannerBadge { background: #ffffff; color: #24745b; padding: 9px 13px; border-radius: 12px; font-weight: 700; }
QPushButton { background: white; border: 1px solid #dce5df; border-radius: 8px; padding: 10px 13px; font-weight: 600; }
QPushButton:hover { background: #edf5f0; border-color: #90b6a4; }
QPushButton:pressed { background: #dceee4; }
QPushButton:disabled { background: #edf0ed; color: #9ca7a0; border-color: #edf0ed; }
QPushButton#primary { background: #24745b; color: white; border: none; padding: 14px; }
QPushButton#primary:hover { background: #195e48; }
QPushButton#primary:disabled { background: #dce5df; color: #8d9c93; }
QPushButton#nav { text-align: left; border: none; color: #718077; padding: 13px 15px; }
QPushButton#nav:checked { background: #e7f2ec; color: #24745b; }
QPushButton#add { background: #edf6f0; border: none; color: #24745b; padding: 7px 15px; }
QPushButton#step { padding: 0; border-radius: 6px; }
QPushButton#link { border: none; color: #a16b5c; background: transparent; padding: 4px; }
QLineEdit, QComboBox, QDoubleSpinBox {
    background: white; border: 1px solid #dce5df; padding: 10px 12px; border-radius: 8px;
}
QLineEdit:focus, QDoubleSpinBox:focus { border: 1px solid #24745b; }
QComboBox::drop-down { border: none; width: 26px; }
QCheckBox { spacing: 9px; font-weight: 600; }
QCheckBox::indicator { width: 18px; height: 18px; }
QLabel#discountStatus { background: #edf6f0; color: #337359; border-radius: 8px; padding: 10px; font-size: 12px; }
QLabel#notification { color: #49725e; font-size: 12px; }
QListWidget, QTextEdit { background: white; border: 1px solid #e4e9e6; border-radius: 10px; padding: 12px; }
QListWidget::item { padding: 16px 8px; border-radius: 7px; }
QListWidget::item:selected { background: #e7f2ec; color: #24745b; }
QScrollArea { background: transparent; border: none; }
QScrollArea > QWidget > QWidget { background: transparent; }
QScrollBar:vertical { width: 7px; background: transparent; margin: 0; }
QScrollBar::handle:vertical { background: #cedbd3; border-radius: 3px; min-height: 26px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }
)";
}
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("FreshCart | Mini Supermarket");
    resize(1440, 920);
    setMinimumSize(1240, 800);
    setStyleSheet(theme);
    auto* root = new QWidget;
    auto* shell = new QHBoxLayout(root);
    shell->setContentsMargins(0, 0, 0, 0); shell->setSpacing(0);
    auto* sidebar = new QFrame; sidebar->setObjectName("sidebar"); sidebar->setFixedWidth(204);
    auto* nav = new QVBoxLayout(sidebar); nav->setContentsMargins(18, 30, 18, 24); nav->setSpacing(10);
    nav->addWidget(label("FreshCart", "brand"));
    nav->addWidget(label("YOUR EVERYDAY MARKET", "eyebrow"));
    nav->addSpacing(40);
    nav->addWidget(label("WORKSPACE", "eyebrow"));
    posNav_ = new QPushButton("Point of sale"); posNav_->setObjectName("nav"); posNav_->setCheckable(true);
    salesNav_ = new QPushButton("Sales && receipts"); salesNav_->setObjectName("nav"); salesNav_->setCheckable(true);
    nav->addWidget(posNav_); nav->addWidget(salesNav_); nav->addStretch();
    auto* note = label("Made for the everyday.\n\nSample prices in EGP.\nSales and stock changes\nlast for this session.", "muted");
    note->setWordWrap(true); nav->addWidget(note);
    nav->addSpacing(12);
    auto* exit = new QPushButton("Exit application"); exit->setObjectName("exitButton"); nav->addWidget(exit);
    shell->addWidget(sidebar);
    auto* content = new QWidget; auto* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(28, 25, 28, 18); contentLayout->setSpacing(16);
    auto* header = new QHBoxLayout;
    auto* headerText = new QVBoxLayout; headerText->setSpacing(4);
    headerText->addWidget(label("THE NEIGHBORHOOD MARKET", "eyebrow"));
    headerText->addWidget(label("Good things, fresh daily.", "title"));
    header->addLayout(headerText); header->addStretch();
    header->addWidget(label(QDate::currentDate().toString("ddd, dd MMM yyyy") + "\nCashier workspace  /  EGP", "muted"));
    contentLayout->addLayout(header);
    pages_ = new QStackedWidget; pages_->setObjectName("pages");
    pages_->addWidget(makePos()); pages_->addWidget(makeSales()); contentLayout->addWidget(pages_, 1);
    notification_ = label("Welcome. Select a product to start your first sale.", "notification");
    contentLayout->addWidget(notification_); shell->addWidget(content, 1); setCentralWidget(root);
    connect(posNav_, &QPushButton::clicked, this, [this] { showPage(0); });
    connect(salesNav_, &QPushButton::clicked, this, [this] { showPage(1); });
    connect(exit, &QPushButton::clicked, this, &QWidget::close);
    showPage(0); refreshCatalog(); refreshCart(); refreshSales();
}
QWidget* MainWindow::makePos() {
    auto* page = new QWidget; auto* layout = new QHBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0); layout->setSpacing(22);
    auto* left = new QVBoxLayout; left->setSpacing(16);
    auto* banner = new QFrame; banner->setObjectName("banner");
    auto* bannerLayout = new QHBoxLayout(banner); bannerLayout->setContentsMargins(22, 20, 22, 20);
    auto* bannerText = new QVBoxLayout;
    bannerText->addWidget(label("A little fresher. A little better.", "bannerTitle"));
    bannerText->addWidget(label("Everyday essentials, one simple checkout.", "body"));
    bannerLayout->addLayout(bannerText); bannerLayout->addStretch(); bannerLayout->addWidget(label("FRESH FINDS", "bannerBadge"));
    left->addWidget(banner);
    auto* catalogHeader = new QHBoxLayout;
    catalogHeader->addWidget(label("Product catalog", "sectionTitle")); catalogHeader->addStretch();
    catalogHeader->addWidget(label("12 everyday essentials", "muted")); left->addLayout(catalogHeader);
    auto* filters = new QHBoxLayout;
    search_ = new QLineEdit; search_->setObjectName("productSearch"); search_->setPlaceholderText("Search products or product ID...");
    search_->setClearButtonEnabled(true);
    category_ = new QComboBox; category_->setObjectName("categoryFilter");
    category_->addItems({"All categories", "Dairy", "Bakery", "Produce", "Drinks", "Pantry"});
    category_->setMinimumWidth(145); filters->addWidget(search_, 1); filters->addWidget(category_); left->addLayout(filters);
    auto* scroll = new QScrollArea; scroll->setWidgetResizable(true);
    auto* grid = new QWidget; catalog_ = new QGridLayout(grid); catalog_->setContentsMargins(0, 0, 7, 0);
    catalog_->setSpacing(12); catalog_->setAlignment(Qt::AlignTop);
    scroll->setWidget(grid); left->addWidget(scroll, 1); layout->addLayout(left, 1);
    connect(search_, &QLineEdit::textChanged, this, [this] { refreshCatalog(); });
    connect(category_, &QComboBox::currentTextChanged, this, [this] { refreshCatalog(); });

    auto* panel = new QFrame; panel->setObjectName("whitePanel"); panel->setFixedWidth(350);
    auto* cartLayout = new QVBoxLayout(panel); cartLayout->setContentsMargins(20, 23, 20, 20); cartLayout->setSpacing(14);
    auto* cartHeader = new QHBoxLayout;
    cartHeader->addWidget(label("Current order", "sectionTitle")); cartHeader->addStretch();
    auto* clear = new QPushButton("Clear"); clear->setObjectName("clearCart"); cartHeader->addWidget(clear);
    cartLayout->addLayout(cartHeader); cartCount_ = label("", "muted"); cartLayout->addWidget(cartCount_);
    auto* cartScroll = new QScrollArea; cartScroll->setWidgetResizable(true);
    auto* cartContent = new QWidget; cartRows_ = new QVBoxLayout(cartContent);
    cartRows_->setContentsMargins(0, 0, 5, 0); cartRows_->setSpacing(15); cartRows_->setAlignment(Qt::AlignTop);
    cartScroll->setWidget(cartContent); cartLayout->addWidget(cartScroll, 1);
    member_ = new QCheckBox("Registered member"); member_->setObjectName("memberToggle"); cartLayout->addWidget(member_);
    auto* discountHelp = label("Members: 5% off from EGP 500;\n10% off from EGP 1,000.", "muted"); cartLayout->addWidget(discountHelp);
    discountStatus_ = label("", "discountStatus"); discountStatus_->setWordWrap(true); cartLayout->addWidget(discountStatus_);
    auto* totals = new QGridLayout; totals->setVerticalSpacing(10);
    subtotal_ = label(""); subtotal_->setObjectName("subtotalValue"); subtotal_->setAlignment(Qt::AlignRight);
    discount_ = label(""); discount_->setObjectName("discountValue"); discount_->setAlignment(Qt::AlignRight);
    total_ = label("", "grandTotal"); total_->setAlignment(Qt::AlignRight);
    totals->addWidget(label("Subtotal", "muted"), 0, 0); totals->addWidget(subtotal_, 0, 1);
    totals->addWidget(label("Member savings", "muted"), 1, 0); totals->addWidget(discount_, 1, 1);
    totals->addWidget(label("Total", "sectionTitle"), 2, 0); totals->addWidget(total_, 2, 1);
    cartLayout->addLayout(totals);
    pay_ = new QPushButton; pay_->setObjectName("primary"); pay_->setProperty("role", "checkout"); cartLayout->addWidget(pay_);
    auto* tenderNote = label("Cash payment  /  Stock checked at checkout", "muted"); tenderNote->setAlignment(Qt::AlignCenter);
    cartLayout->addWidget(tenderNote); layout->addWidget(panel);
    connect(clear, &QPushButton::clicked, this, [this] { clearCart(); });
    connect(member_, &QCheckBox::toggled, this, [this](bool checked) { cart_.member = checked; refreshCart(); });
    connect(pay_, &QPushButton::clicked, this, [this] { takePayment(); });
    return page;
}
QWidget* MainWindow::makeSales() {
    auto* page = new QWidget; auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0); layout->setSpacing(20);
    layout->addWidget(label("Sales & receipts", "sectionTitle"));
    layout->addWidget(label("Your completed transactions for this session. Export receipts before closing.", "muted"));
    auto* metrics = new QHBoxLayout; metrics->setSpacing(16);
    const QStringList titles{"NET REVENUE", "COMPLETED SALES", "ITEMS SOLD"};
    QLabel** values[] = {&revenue_, &transactions_, &units_};
    for (int i = 0; i < 3; ++i) {
        auto* card = new QFrame; card->setObjectName("metricCard"); auto* col = new QVBoxLayout(card);
        col->setContentsMargins(22, 20, 22, 20); col->addWidget(label(titles[i], "eyebrow"));
        *values[i] = label("0", "metric"); col->addWidget(*values[i]); metrics->addWidget(card, 1);
    }
    layout->addLayout(metrics);
    auto* body = new QHBoxLayout; body->setSpacing(20);
    receiptList_ = new QListWidget; receiptList_->setObjectName("receiptList"); receiptList_->setMinimumWidth(260);
    body->addWidget(receiptList_, 1);
    auto* receiptCol = new QVBoxLayout;
    auto* receiptHeader = new QHBoxLayout; receiptHeader->addWidget(label("Receipt preview", "sectionTitle")); receiptHeader->addStretch();
    save_ = new QPushButton("Export receipt (.txt)"); save_->setObjectName("exportReceipt"); receiptHeader->addWidget(save_);
    receiptCol->addLayout(receiptHeader);
    receiptView_ = new QTextEdit; receiptView_->setObjectName("receiptPreview"); receiptView_->setReadOnly(true);
    receiptView_->setFont(QFont("Consolas", 11)); receiptCol->addWidget(receiptView_, 1); body->addLayout(receiptCol, 2);
    layout->addLayout(body, 1);
    connect(receiptList_, &QListWidget::currentRowChanged, this, [this](int row) {
        const bool valid = row >= 0 && row < static_cast<int>(store_.sales.size());
        save_->setEnabled(valid);
        receiptView_->setPlainText(valid ? q(store_.sales[row].receipt) : "No completed sales yet.\nStart a sale in Point of sale.");
    });
    connect(save_, &QPushButton::clicked, this, [this] { saveReceipt(); });
    return page;
}
void MainWindow::showPage(int index) {
    pages_->setCurrentIndex(index); posNav_->setChecked(index == 0); salesNav_->setChecked(index == 1);
}
void MainWindow::refreshCatalog() {
    clearLayout(catalog_);
    int visible = 0;
    const QString filter = search_->text().trimmed();
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        const auto& p = store_.products[i];
        if (category_->currentIndex() != 0 && q(p.category) != category_->currentText()) continue;
        if (!filter.isEmpty() && !q(p.name).contains(filter, Qt::CaseInsensitive)
            && !QString::number(p.id).contains(filter)) continue;
        auto* card = new QFrame; card->setObjectName("productCard"); card->setMinimumHeight(157);
        auto* cardLayout = new QVBoxLayout(card); cardLayout->setContentsMargins(15, 14, 15, 13); cardLayout->setSpacing(8);
        auto* top = new QHBoxLayout; top->setSpacing(8); auto* nameCol = new QVBoxLayout; nameCol->setSpacing(4);
        auto* name = label(q(p.name), "productName"); name->setWordWrap(true);
        nameCol->addWidget(label(q(p.category).toUpper(), "eyebrow")); nameCol->addWidget(name);
        nameCol->addWidget(label(q(p.unit) + "  /  " + QString::number(p.stock - cart_.quantities[i]) + " available", "muted"));
        nameCol->addStretch(); top->addLayout(nameCol, 1);
        auto* art = label(""); art->setPixmap(productArt(p).scaled(66, 66, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        art->setFixedSize(66, 66); top->addWidget(art); cardLayout->addLayout(top);
        auto* bottom = new QHBoxLayout; bottom->addWidget(label(egp(p.price), "price")); bottom->addStretch();
        auto* add = new QPushButton(p.stock > cart_.quantities[i] ? "+  Add" : "In cart / sold out");
        add->setObjectName("add"); add->setProperty("productIndex", i);
        add->setEnabled(p.stock > cart_.quantities[i]); bottom->addWidget(add); cardLayout->addLayout(bottom);
        connect(add, &QPushButton::clicked, this, [this, i] { changeQuantity(i, cart_.quantities[i] + 1); });
        catalog_->addWidget(card, visible / 2, visible % 2); ++visible;
    }
    catalog_->setColumnStretch(0, 1); catalog_->setColumnStretch(1, 1);
    if (visible == 0) {
        auto* none = label("No matching products. Try another search or category.", "muted");
        none->setWordWrap(true); none->setMinimumHeight(120); catalog_->addWidget(none, 0, 0, 1, 2);
    }
}
void MainWindow::refreshCart() {
    clearLayout(cartRows_);
    const auto totals = calculate(store_, cart_);
    cartCount_->setText(QString::number(totals.items) + (totals.items == 1 ? " item in this order" : " items in this order"));
    if (totals.items == 0) {
        auto* empty = label("Your cart is feeling light.\n\nAdd something fresh from\nthe catalog to get started.", "muted");
        empty->setAlignment(Qt::AlignCenter); empty->setMinimumHeight(140); cartRows_->addWidget(empty);
    }
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        if (cart_.quantities[i] == 0) continue;
        const auto& p = store_.products[i];
        auto* row = new QWidget; auto* col = new QVBoxLayout(row); col->setContentsMargins(0, 0, 0, 0); col->setSpacing(7);
        auto* top = new QHBoxLayout; top->addWidget(label(q(p.name), "productName")); top->addStretch();
        auto* remove = new QPushButton("Remove"); remove->setObjectName("link"); remove->setProperty("productIndex", i);
        top->addWidget(remove); col->addLayout(top);
        auto* bottom = new QHBoxLayout; auto* minus = new QPushButton("-"); minus->setObjectName("step"); minus->setFixedSize(27,27);
        minus->setProperty("productIndex", i); minus->setProperty("delta", -1);
        auto* plus = new QPushButton("+"); plus->setObjectName("step"); plus->setFixedSize(27,27);
        plus->setProperty("productIndex", i); plus->setProperty("delta", 1); plus->setEnabled(cart_.quantities[i] < p.stock);
        auto* count = label(QString::number(cart_.quantities[i])); count->setAlignment(Qt::AlignCenter); count->setMinimumWidth(25);
        bottom->addWidget(minus); bottom->addWidget(count); bottom->addWidget(plus); bottom->addStretch();
        bottom->addWidget(label(egp(p.price * cart_.quantities[i]), "productName")); col->addLayout(bottom); cartRows_->addWidget(row);
        connect(minus, &QPushButton::clicked, this, [this, i] { changeQuantity(i, cart_.quantities[i] - 1); });
        connect(plus, &QPushButton::clicked, this, [this, i] { changeQuantity(i, cart_.quantities[i] + 1); });
        connect(remove, &QPushButton::clicked, this, [this, i] { changeQuantity(i, 0); });
    }
    subtotal_->setText(egp(totals.subtotal)); discount_->setText("- " + egp(totals.discount)); total_->setText(egp(totals.total));
    discountStatus_->setText(q(totals.status)); pay_->setText("Charge " + egp(totals.total) + "   >"); pay_->setEnabled(totals.items > 0);
    const QSignalBlocker blocked(member_); member_->setChecked(cart_.member);
}
void MainWindow::changeQuantity(int index, int quantity) {
    std::string error;
    if (!setQuantity(store_, cart_, index, quantity, error)) { notify(q(error), true); return; }
    // Old card and cart widgets use deleteLater so a clicked sender remains alive
    // until Qt has finished handling its current event.
    refreshCatalog(); refreshCart();
    notify("Order updated. Stock is reserved in this cart until checkout.");
}
void MainWindow::clearCart() {
    if (calculate(store_, cart_).items == 0) return;
    if (QMessageBox::question(this, "Clear current order?", "Remove all items from this order?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) return;
    cart_ = Cart{}; refreshCart(); refreshCatalog(); notify("Order cleared. Ready for a fresh start.");
}
void MainWindow::takePayment() {
    const auto totals = calculate(store_, cart_);
    if (totals.items == 0) { notify("Add a product before taking payment.", true); return; }
    QDialog dialog(this); dialog.setObjectName("paymentDialog"); dialog.setWindowTitle("Complete cash payment"); dialog.setMinimumWidth(440);
    auto* layout = new QVBoxLayout(&dialog); layout->setContentsMargins(28, 28, 28, 28); layout->setSpacing(16);
    layout->addWidget(label("One last step.", "title")); layout->addWidget(label("Collect cash and confirm the sale.", "muted"));
    layout->addWidget(label("AMOUNT DUE", "eyebrow")); layout->addWidget(label(egp(totals.total), "grandTotal"));
    layout->addWidget(label("Cash received (EGP)", "productName"));
    auto* cash = new QDoubleSpinBox; cash->setObjectName("cashInput"); cash->setRange(0, 1000000); cash->setDecimals(2);
    cash->setGroupSeparatorShown(false); cash->setValue(totals.total / 100.0); cash->setSingleStep(10); layout->addWidget(cash);
    auto* change = label("Change: EGP 0.00", "sectionTitle"); layout->addWidget(change);
    auto* error = label("", "paymentError"); error->setWordWrap(true); error->setStyleSheet("color: #b45c48;"); layout->addWidget(error);
    auto* controls = new QHBoxLayout; auto* cancel = new QPushButton("Cancel"); cancel->setObjectName("cancelPayment");
    auto* confirm = new QPushButton("Complete sale"); confirm->setObjectName("confirmPayment");
    confirm->setStyleSheet("background: #24745b; color: white; padding: 13px;"); controls->addWidget(cancel); controls->addWidget(confirm, 1); layout->addLayout(controls);
    connect(cash, qOverload<double>(&QDoubleSpinBox::valueChanged), &dialog, [=](double value) {
        const Money difference = static_cast<Money>(std::llround(value * 100)) - totals.total;
        change->setText(difference >= 0 ? "Change: " + egp(difference) : "Still due: " + egp(-difference));
        error->clear();
    });
    connect(cancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(confirm, &QPushButton::clicked, &dialog, [&] {
        cash->interpretText();
        if (static_cast<Money>(std::llround(cash->value() * 100)) < totals.total) {
            error->setText("Cash is below the total. Enter enough cash to continue."); return;
        }
        dialog.accept();
    });
    if (dialog.exec() != QDialog::Accepted) return;
    Sale sale; std::string checkoutError;
    if (!checkout(store_, cart_, static_cast<Money>(std::llround(cash->value() * 100)), sale, checkoutError)) {
        notify(q(checkoutError), true); return;
    }
    refreshCart(); refreshCatalog(); refreshSales(); showPage(1);
    notify("Sale #" + QString::number(sale.number).rightJustified(4, '0') + " completed. Change: " + egp(sale.change) + ". Receipt ready to export.");
}
void MainWindow::refreshSales() {
    revenue_->setText(egp(totalRevenue(store_))); transactions_->setText(QString::number(store_.sales.size()));
    int items = 0; for (const auto& sale : store_.sales) items += sale.totals.items;
    units_->setText(QString::number(items));
    receiptList_->clear();
    for (const auto& sale : store_.sales) receiptList_->addItem("Receipt #" + QString::number(sale.number).rightJustified(4, '0')
        + "\n" + egp(sale.totals.total) + "   /   " + QString::number(sale.totals.items) + " items");
    if (!store_.sales.empty()) receiptList_->setCurrentRow(static_cast<int>(store_.sales.size()) - 1);
    else { receiptView_->setPlainText("No completed sales yet.\nStart a sale in Point of sale."); save_->setEnabled(false); }
}
void MainWindow::saveReceipt() {
    const int index = receiptList_->currentRow();
    if (index < 0 || index >= static_cast<int>(store_.sales.size())) return;
    const auto& sale = store_.sales[index];
    const QString path = QFileDialog::getSaveFileName(this, "Export receipt",
        "FreshCart_receipt_" + QString::number(sale.number).rightJustified(4, '0') + ".txt", "Text files (*.txt)");
    if (path.isEmpty()) return;
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) { notify("Could not open the receipt file. Choose a writable folder.", true); return; }
    const QByteArray data = q(sale.receipt).toUtf8();
    if (file.write(data) != data.size() || !file.commit()) { notify("Receipt could not be saved. Please try another folder.", true); return; }
    notify("Receipt exported successfully.");
}
void MainWindow::notify(const QString& text, bool error) {
    notification_->setText(text); notification_->setStyleSheet(error ? "color: #b45c48;" : "color: #49725e;");
}
void MainWindow::closeEvent(QCloseEvent* event) {
    if (calculate(store_, cart_).items > 0 || !store_.sales.empty()) {
        const auto answer = QMessageBox::question(this, "End this session?",
            "Sales and stock changes are kept only for this session. Export any receipts you need.\n\nClose FreshCart?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer != QMessageBox::Yes) { event->ignore(); return; }
    }
    event->accept();
}
