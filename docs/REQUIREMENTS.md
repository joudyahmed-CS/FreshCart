# Course requirement mapping

| Requirement | Implementation |
|---|---|
| Practical C++ application | Mini supermarket billing, stock and cash checkout |
| Welcome and numbered menu, at least four options including exit | `src/console.cpp`, six options |
| `cin`, `cout` | `getline(cin, ...)`, validation, product/menu output in console |
| Appropriate variables | int IDs/quantities, bool membership, strings, integer piastres; double only at the Qt input boundary |
| At least two arithmetic calculations | price times quantity, subtotal sum, percentage discount, paid minus total |
| At least one constant | PRODUCT_COUNT, MEMBER_THRESHOLD, GOLD_THRESHOLD and discount percentages in store.h |
| At least two if/else decisions | quantity validation, member eligibility, payment/empty-cart decisions |
| Combined logical condition | `cart.member && subtotal >= MEMBER_THRESHOLD`; range validation with `||` |
| Nested if | member discount band inside eligibility condition in `calculate` |
| switch/case/break/default | six-option main console menu |
| while/do-while menu | do-while around the console switch |
| for repeated processing | catalog, cart totals, checkout stock deduction |
| At least two invalid input types; retry | bad menu/text input, negative or excessive quantities, insufficient/invalid cash |
| Calculated results and status | subtotal/discount/total/change plus discount status and receipt |
| GUI optional | Actual Qt C++ desktop GUI included as an enhancement |
| Console mandatory | Separate `FreshCartConsole` target; no Qt dependency |
| Visual Studio development/demo | Windows MSVC build and GUI launch verified; demonstrate the console and concepts in the viva |
| Source and README on public GitHub | Prepared source tree; team must upload and insert actual URL |
| Short PDF with three test runs | Report with three actual Qt test screenshots and calculations; third team row remains blank |
| LinkedIn post and link | Draft in LINKEDIN_POST.md; team must publish and submit real link |
| Team of 3-5 and full names/IDs | Two supplied names and IDs entered; third row must be completed |

The GUI necessarily uses Qt classes. The course states that classes and external libraries are not required, while explicitly requiring a console application. The included console is the direct course-compatible entry point.
