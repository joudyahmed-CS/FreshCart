# Validation evidence

Environment: Debian Linux x86-64, GCC 14.2, C++17, Qt 6.8.2, CMake 3.31.6.

## Results

- Both `FreshCart` (Qt GUI) and `FreshCartConsole` compiled successfully.
- Core suite: **31 checks passed**, including exact discount thresholds, currency rounding, negative/over-stock input, empty carts, sold-out products, insufficient cash, inventory commit and rollback, receipt totals and cumulative net revenue.
- GUI integration: actual Qt widgets were driven using button clicks and field changes on the offscreen platform. Search, product ID, category filtering, add to cart, membership, cancel payment, insufficient payment, three completed sales and receipt-history selection passed.
- Console integration: three fresh-process runs passed, plus clean EOF handling. Full input and actual stdout are in `test-runs/`.
- Screenshots in `screenshots/` are captures of the running C++ Qt application, not design mockups. The dates use the application host's local date.

## Completed transaction examples

| Run | Products | Member | Subtotal | Discount | Due | Cash | Change |
|---|---|---|---:|---:|---:|---:|---:|
| 1 | Whole Milk x2, Basmati Rice x6 | Yes | 654.00 | 32.70 (5%) | 621.30 | 700.00 | 78.70 |
| 2 | Fresh Bananas x2 | No | 76.00 | 0.00 | 76.00 | 76.00 | 0.00 |
| 3 | Basmati Rice x11 | Yes | 1045.00 | 104.50 (10%) | 940.50 | 940.50 | 0.00 |

All amounts are EGP. The GUI examples are consecutive sales in one session, so its revenue cards are cumulative: 621.30, 697.30, then 1637.80. Console runs start fresh; the gold console case deliberately pays 1000.00 to verify change of 59.50.

## Reproduce

```sh
cmake -S . -B build -DFRESHCART_GUI_TESTS=ON -DCMAKE_PREFIX_PATH="YOUR_QT_KIT_PATH"
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
python tests/console_integration.py build/Release/FreshCartConsole.exe
```

For single-config Linux builds, use `build/FreshCartConsole` instead of the Windows Release path.

## Windows verification

The team subsequently built both applications and the core test suite with Visual Studio 2026 MSVC tools on Windows 10. The initial CTest suite passed. A later VS Code Debug build ended with exit code 0, and the deployed GUI launched successfully with Qt 6.12.1. See WINDOWS_VERIFICATION.md for the scope and evidence.

The three transaction screenshots above remain the original automated Linux Qt runs. Interactive receipt-file export and the course demonstration should be checked on the team computer. The GUI automation verifies receipt content without driving a system save dialog.
