#include <iostream>
#include <mysql.h>
#include "database.h"
#include <string>

using namespace std;

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

const char *server   = "localhost";
const char *user     = "root";
const char *password = "1234";    
const char *database = "store";

//connection
bool connectDB() {

    conn = mysql_init(nullptr);
    if (!conn) {
        cerr << "mysql_init() failed\n";
        return false;
    }
    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        cerr << "mysql_real_connect() failed : " << mysql_error(conn) << '\n';
        mysql_close(conn);
        return false;
    }
    cout << "Connected to MySQL.\n";
    return true;
}

void closeDB() {
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
        cout << "MySQL connection closed.\n";
    }
}

//execute and print
void execute(const string &query) {

    if (mysql_query(conn, query.c_str())) {
        cerr << "Query failed : " << mysql_error(conn) << '\n';
        return;
    }

    res = mysql_store_result(conn);

    //non select 
    if (!res) {
        if (mysql_field_count(conn) == 0)
            cout << "Query succeeded.\n";
        else
            cerr << "mysql_store_result() failed : " << mysql_error(conn) << '\n';
        return;
    }

    //print header
    unsigned int cols = mysql_num_fields(res);
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    for (unsigned int i = 0; i < cols; ++i) cout << fields[i].name << '\t';
    cout << '\n';

    //print rows
    while ((row = mysql_fetch_row(res))) {
        for (unsigned int i = 0; i < cols; ++i)
            cout << (row[i] ? row[i] : "NULL") << '\t';
        cout << '\n';
    }
    mysql_free_result(res);
}

// TYPE 1
void ProductAvailabilityQuery() {
    cout << "\n-------- TYPE 1 --------\n"
            "** Which stores currently carry a certain product "
            "(by UPC, name, or brand), and how much inventory do they have? **\n"
            "Enter product identifier (UPC, name, or brand): ";
    string keyword;
    cin.ignore();                  
    getline(cin, keyword);

    //escape string
    string escaped(keyword.size() * 2 + 1, '\0');
    unsigned long len = mysql_real_escape_string(
        conn, &escaped[0], keyword.c_str(), keyword.length());
    escaped.resize(len);

    string query =
    "SELECT s.name AS store_name, p.product_name, i.quantity "
    "FROM Inventory i "
    "JOIN Store   s ON i.store_id  = s.store_id "
    "JOIN Product p ON i.product_id = p.product_id "
    "WHERE p.product_name LIKE CONCAT('%', '" + escaped + "', '%') "
    "   OR p.brand        LIKE CONCAT('%', '" + escaped + "', '%') "
    "   OR p.product_id   = '" + escaped + "';";

    execute(query);

}

// TYPE 2
void TopSellingItemsQuery() {
    cout << "\n-------- TYPE 2 --------\n"
            "** Which products have the highest sales volume in each store "
            "over the past month? **\n";

    string query =
    "SELECT s.name AS store_name, p.product_name, sp.qty "
    "FROM ( "
    "  SELECT t.store_id, sr.product_id, SUM(sr.quantity) AS qty "
    "  FROM Sale_Record sr "
    "  JOIN Transaction_Info t ON sr.transaction_id = t.transaction_id "
    "  WHERE t.transaction_date >= DATE_SUB(CURDATE(), INTERVAL 1 MONTH) "
    "  GROUP BY t.store_id, sr.product_id "
    ") sp "
    "JOIN ( "
    "  SELECT store_id, MAX(qty) AS max_qty "
    "  FROM ( "
    "    SELECT t.store_id, sr.product_id, SUM(sr.quantity) AS qty "
    "    FROM Sale_Record sr "
    "    JOIN Transaction_Info t ON sr.transaction_id = t.transaction_id "
    "    WHERE t.transaction_date >= DATE_SUB(CURDATE(), INTERVAL 1 MONTH) "
    "    GROUP BY t.store_id, sr.product_id "
    "  ) a "
    "  GROUP BY store_id "
    ") m ON sp.store_id = m.store_id AND sp.qty = m.max_qty "
    "JOIN Store   s ON sp.store_id  = s.store_id "
    "JOIN Product p ON sp.product_id = p.product_id;";

    execute(query);
}

// TYPE 3
void StorePerformanceQuery() {
    cout << "\n-------- TYPE 3 --------\n"
            "** Which store has generated the highest overall revenue "
            "this quarter? **\n";

    string query =
    "SELECT s.name AS store_name, SUM(t.total_amount) AS revenue "
    "FROM Transaction_Info t "
    "JOIN Store s ON t.store_id = s.store_id "
    "WHERE QUARTER(t.transaction_date) = QUARTER(CURDATE()) "
    "  AND YEAR(t.transaction_date)    = YEAR(CURDATE()) "
    "GROUP BY t.store_id "
    "ORDER BY revenue DESC "
    "LIMIT 1;";

    execute(query);

}

// TYPE 4
void VendorStatisticsQuery() {
    cout << "\n-------- TYPE 4 --------\n"
            "** Which vendor supplies the most products across the chain, "
            "and how many total units have been sold? **\n";

    string query =
        "SELECT v.name AS vendor, "
        "       IFNULL(sup.total_supplied,0) AS total_supplied, "
        "       IFNULL(sal.total_sold,0)     AS total_sold "
        "FROM   Vendor v "
        "LEFT JOIN ( "
        "    SELECT p.vendor_id, SUM(od.quantity) AS total_supplied "
        "    FROM   Order_Detail od "
        "    JOIN   Product p ON od.product_id = p.product_id "
        "    GROUP BY p.vendor_id "
        ") sup ON v.vendor_id = sup.vendor_id "
        "LEFT JOIN ( "
        "    SELECT p.vendor_id, SUM(sr.quantity) AS total_sold "
        "    FROM   Sale_Record sr "
        "    JOIN   Product p ON sr.product_id = p.product_id "
        "    GROUP BY p.vendor_id "
        ") sal ON v.vendor_id = sal.vendor_id "
        "ORDER BY total_supplied DESC "
        "LIMIT 1;";

    execute(query);
}

// TYPE 5
void InventoryAlertsQuery() {
    cout << "\n-------- TYPE 5 --------\n"
            "** Which products in each store are below the reorder "
            "threshold and need restocking? **\n";

    string query =
    "SELECT s.name AS store_name, p.product_name, "
    "       i.quantity, i.reorder_threshold "
    "FROM Inventory i "
    "JOIN Store   s ON i.store_id  = s.store_id "
    "JOIN Product p ON i.product_id = p.product_id "
    "WHERE i.quantity < i.reorder_threshold "
    "ORDER BY s.store_id;";

    execute(query);
}

// TYPE 6
void CustomerPatternsQuery() {
    cout << "\n-------- TYPE 6 --------\n"
            "** List the top 3 items that loyalty program customers "
            "typically purchase with coffee. **\n"
            "Enter a product name: ";
    string baseProduct;
    cin.ignore();
    getline(cin, baseProduct);

    string escaped(baseProduct.size() * 2 + 1, '\0');
    unsigned long len = mysql_real_escape_string(
        conn, &escaped[0], baseProduct.c_str(), baseProduct.length());
    escaped.resize(len);

    string query =
    "SELECT p2.product_name, SUM(sr2.quantity) AS total_qty "
    "FROM Sale_Record sr2 "
    "JOIN Product p2 ON sr2.product_id = p2.product_id "
    "WHERE sr2.transaction_id IN ( "
    "  SELECT DISTINCT sr.transaction_id "
    "  FROM Sale_Record sr "
    "  JOIN Product p ON sr.product_id = p.product_id "
    "  JOIN Transaction_Info t ON sr.transaction_id = t.transaction_id "
    "  JOIN Customer c ON t.customer_id = c.customer_id "
    "  WHERE c.loyalty_grade = 'VIP' "
    "    AND p.product_name LIKE CONCAT('%', '" + escaped + "', '%') "
    ") "
    "AND p2.product_name NOT LIKE CONCAT('%', '" + escaped + "', '%') "
    "GROUP BY p2.product_id "
    "ORDER BY total_qty DESC "
    "LIMIT 3;";

    execute(query);
}

// TYPE 7
void FranchiseComparisonQuery() {
    cout << "\n-------- TYPE 7 --------\n"
            "** Among franchise-owned stores, which one offers the widest "
            "variety of products, and how does that compare to "
            "corporate-owned stores? **\n";

    string query =
    
    "SELECT sv.ownership_type, sv.name AS store_name, sv.variety "
    "FROM ( "
    "  SELECT s.store_id, s.name, s.ownership_type, "
    "         COUNT(DISTINCT i.product_id) AS variety "
    "  FROM Store s "
    "  JOIN Inventory i ON s.store_id = i.store_id "
    "  GROUP BY s.store_id "
    ") sv "
    "JOIN ( "
    "  SELECT ownership_type, MAX(variety) AS max_var "
    "  FROM ( "
    "    SELECT s.store_id, s.ownership_type, "
    "           COUNT(DISTINCT i.product_id) AS variety "
    "    FROM Store s "
    "    JOIN Inventory i ON s.store_id = i.store_id "
    "    GROUP BY s.store_id "
    "  ) v "
    "  GROUP BY ownership_type "
    ") mv ON sv.ownership_type = mv.ownership_type "
    "     AND sv.variety = mv.max_var;";

    execute(query);
}

// display
void displayMenu() {
    cout << "\n------------ SELECT QUERY TYPES ------------\n"
            " 1. TYPE 1 \n"
            " 2. TYPE 2 \n"
            " 3. TYPE 3 \n"
            " 4. TYPE 4 \n"
            " 5. TYPE 5 \n"
            " 6. TYPE 6 \n"
            " 7. TYPE 7 \n"
            " 0. QUIT\n"
            "Select: ";
}

int main() {
    if (!connectDB()) return 1;

    while (true) {
        displayMenu();
        int choice;
        if (!(cin >> choice)) break;         
        cin.ignore();                     

        switch (choice) {
            case 1: ProductAvailabilityQuery();  break;
            case 2: TopSellingItemsQuery();      break;
            case 3: StorePerformanceQuery();     break;
            case 4: VendorStatisticsQuery();     break;
            case 5: InventoryAlertsQuery();      break;
            case 6: CustomerPatternsQuery();     break;
            case 7: FranchiseComparisonQuery();  break;
            case 0:
                closeDB();
                cout << "Program terminated.\n";
                return 0;
            default:
                cout << "Invalid selection – try again.\n";
        }
    }
    closeDB();
    return 0;
}
