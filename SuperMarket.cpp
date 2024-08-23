#include <iostream>
#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

// Base class Product
class Product {
protected:
    int pno;
    string name;
    float price;
    float dis;

public:
    virtual void create_product() {
        cout << "=============================================\n";
        cout << "        Enter Product Details                \n";
        cout << "=============================================\n";
        cout << "Product No.: ";
        cin >> pno;
        cout << "Product Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Price: ";
        cin >> price;
        cout << "Discount (%): ";
        cin >> dis;
        cout << "=============================================\n";
    }

    virtual void show_product() const {
        cout << left << setw(15) << "Product No." << ": " << pno << endl;
        cout << left << setw(15) << "Name" << ": " << name << endl;
        cout << left << setw(15) << "Price" << ": " << fixed << setprecision(2) << price << endl;
        cout << left << setw(15) << "Discount" << ": " << dis << "%" << endl;
    }

    int retpno() const { return pno; }
    float retprice() const { return price; }
    string retname() const { return name; }
    float retdis() const { return dis; }

    virtual ~Product() {}
};

// ProductManager class to handle all products
class ProductManager {
private:
    vector<shared_ptr<Product>> products;
    const string filename = "Shop.dat";

public:
    ProductManager() {
        load_from_file();  // Load products when the manager is created
    }

    ~ProductManager() {
        save_to_file();  // Save products when the manager is destroyed
    }

    void add_product(shared_ptr<Product> product) {
        products.push_back(product);
    }

    void save_to_file() {
        ofstream outFile(filename, ios::binary);
        for (const auto& product : products) {
            outFile.write(reinterpret_cast<const char*>(&*product), sizeof(Product));
        }
        outFile.close();
    }

    void load_from_file() {
        ifstream inFile(filename, ios::binary);
        if (!inFile) {
            cout << "File not found: " << filename << "\n";
            return;
        }

        Product product;
        products.clear();  // Clear the current list
        while (inFile.read(reinterpret_cast<char*>(&product), sizeof(Product))) {
            products.push_back(make_shared<Product>(product));
        }
        inFile.close();
    }

    void display_all() const {
        cout << "\n\n";
        cout << "=============================================\n";
        cout << "          Displaying All Products            \n";
        cout << "=============================================\n";
        for (const auto& product : products) {
            product->show_product();
            cout << "---------------------------------------------\n";
        }
    }

    void display_sp(int n) const {
        bool found = false;
        cout << "\n";
        for (const auto& product : products) {
            if (product->retpno() == n) {
                cout << "=============================================\n";
                product->show_product();
                cout << "=============================================\n";
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "\nProduct not found\n";
        }
    }

    void modify_product(int n) {
        bool found = false;
        for (auto& product : products) {
            if (product->retpno() == n) {
                cout << "\nCurrent Product Details\n";
                product->show_product();
                cout << "Enter New Details\n";
                product->create_product();
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "\nProduct not found\n";
        }
    }

    void delete_product(int n) {
        auto it = remove_if(products.begin(), products.end(),
                            [n](shared_ptr<Product> product) { return product->retpno() == n; });
        if (it != products.end()) {
            products.erase(it, products.end());
            cout << "\nProduct deleted\n";
        } else {
            cout << "\nProduct not found\n";
        }
    }

    shared_ptr<Product> get_product(int n) const {
        for (const auto& product : products) {
            if (product->retpno() == n) {
                return product;
            }
        }
        return nullptr;
    }
};

// Order class to handle product orders
class Order {
private:
    vector<int> order_pnos;
    vector<int> order_quantities;
    float total;

public:
    Order() : total(0) {}

    void add_product_to_order(int pno, int quantity) {
        order_pnos.push_back(pno);
        order_quantities.push_back(quantity);
    }

    void display_order(ProductManager& manager) {
        cout << "\n\n";
        cout << "=============================================\n";
        cout << "                INVOICE                      \n";
        cout << "=============================================\n";
        cout << left << setw(10) << "Pr No." << setw(20) << "Pr Name" << setw(10) << "Quantity"
             << setw(10) << "Price" << setw(15) << "Amount" << setw(20) << "Amount after Discount\n";
        cout << "---------------------------------------------\n";
        total = 0;
        for (size_t i = 0; i < order_pnos.size(); i++) {
            shared_ptr<Product> product = manager.get_product(order_pnos[i]);
            if (product != nullptr) {
                float amt = product->retprice() * order_quantities[i];
                float damt = amt - (amt * product->retdis() / 100);
                total += damt;
                cout << left << setw(10) << order_pnos[i] << setw(20) << product->retname()
                     << setw(10) << order_quantities[i] << setw(10) << product->retprice()
                     << setw(15) << amt << setw(20) << damt << "\n";
            }
        }
        cout << "=============================================\n";
        cout << right << setw(50) << "TOTAL = " << total << "\n";
        cout << "=============================================\n";
    }
};

// Functions for different menus
void customer_menu(ProductManager& manager) {
    char ch;
    do {
        cout << "\n\n";
        cout << "=============================================\n";
        cout << "              CUSTOMER MENU                  \n";
        cout << "=============================================\n";
        cout << "1. Place Order\n";
        cout << "2. View Product Menu\n";
        cout << "3. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> ch;

        switch (ch) {
            case '1': {
                Order order;
                char choice;
                do {
                    int pno, qty;
                    cout << "Enter product number: ";
                    cin >> pno;
                    cout << "Enter quantity: ";
                    cin >> qty;
                    order.add_product_to_order(pno, qty);
                    cout << "Do you want to add more products? (y/n): ";
                    cin >> choice;
                } while (choice == 'y' || choice == 'Y');
                order.display_order(manager);
                break;
            }
            case '2': {
                manager.display_all();
                break;
            }
            case '3':
                return;
            default:
                cout << "Invalid choice, try again.\n";
                break;
        }
    } while (ch != '3');
}

void admin_menu(ProductManager& manager) {
    char ch;
    do {
        cout << "\n\n";
        cout << "=============================================\n";
        cout << "            ADMINISTRATOR MENU               \n";
        cout << "=============================================\n";
        cout << "1. Add Product\n";
        cout << "2. Display All Products\n";
        cout << "3. Modify Product\n";
        cout << "4. Delete Product\n";
        cout << "5. Back to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> ch;

        switch (ch) {
            case '1': {
                auto product = make_shared<Product>();
                product->create_product();
                manager.add_product(product);
                break;
            }
            case '2':
                manager.display_all();
                break;
            case '3': {
                int pno;
                cout << "Enter product number to modify: ";
                cin >> pno;
                manager.modify_product(pno);
                break;
            }
            case '4': {
                int pno;
                cout << "Enter product number to delete: ";
                cin >> pno;
                manager.delete_product(pno);
                break;
            }
            case '5':
                return;
            default:
                cout << "Invalid choice, try again.\n";
                break;
        }
    } while (ch != '5');
}

// Main function to run the program
int main() {
    ProductManager manager;
    char ch;

    do {
        cout << "\n\n";
        cout << "=============================================\n";
        cout << "                 MAIN MENU                   \n";
        cout << "=============================================\n";
        cout << "1. Customer\n";
        cout << "2. Administrator\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> ch;

        switch (ch) {
            case '1':
                customer_menu(manager);
                break;
            case '2':
                admin_menu(manager);
                break;
            case '3':
                cout << "Thanks for visiting Market!\n";
                break;
            default:
                cout << "Invalid choice, try again.\n";
                break;
        }
    } while (ch != '3');

    return 0;
}
