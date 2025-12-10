#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 100
#define MAX_CART 100
#define NAME_LEN 50
#define GST_RATE 0.05

typedef struct {
    int id;
    char name[NAME_LEN];
    double price;
    int qty; 
} Item;

typedef struct {
    int id;
    char name[NAME_LEN];
    double price;
    int qty; 
    double total;
} CartItem;

Item inventory[MAX_ITEMS];
int inventory_count = 0;

CartItem cart[MAX_CART];
int cart_count = 0;

void seed_inventory();
void show_inventory();
int find_item_index_by_id(int id);
void add_to_cart();
void view_cart();
void generate_bill();
void save_bill_to_file(const char *bill_text);
void clear_cart();
void press_enter_to_continue();

int main() {
    int choice;
    seed_inventory();

    while (1) {
        printf("\n====== Grocery Billing System ======\n");
        printf("1. Show Inventory\n");
        printf("2. Add Item to Cart\n");
        printf("3. View Cart\n");
        printf("4. Generate Bill & Save\n");
        printf("5. Clear Cart\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n');
            printf("Invalid input. Try again.\n");
            continue;
        }

        switch (choice) {
            case 1: show_inventory(); break;
            case 2: add_to_cart(); break;
            case 3: view_cart(); break;
            case 4: generate_bill(); break;
            case 5: clear_cart(); break;
            case 0: printf("Exiting. Goodbye!\n"); exit(0);
            default: printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

void seed_inventory() {
    inventory_count = 8;
    inventory[0] = (Item){1, "Rice (5kg)", 240.00, 20};
    inventory[1] = (Item){2, "Wheat Flour (1kg)", 45.00, 50};
    inventory[2] = (Item){3, "Sugar (1kg)", 38.50, 30};
    inventory[3] = (Item){4, "Salt (1kg)", 20.00, 40};
    inventory[4] = (Item){5, "Milk (1L)", 50.00, 100};
    inventory[5] = (Item){6, "Cooking Oil (1L)", 160.00, 25};
    inventory[6] = (Item){7, "Eggs (12pcs)", 90.00, 60};
    inventory[7] = (Item){8, "Tea Powder (250g)", 120.00, 15};
}

void show_inventory() {
    printf("\n--- Inventory ---\n");
    printf("%-5s %-25s %-10s %-8s\n", "ID", "Name", "Price(Rs)", "Stock");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < inventory_count; ++i) {
        printf("%-5d %-25s %-10.2f %-8d\n",
            inventory[i].id,
            inventory[i].name,
            inventory[i].price,
            inventory[i].qty);
    }
}

int find_item_index_by_id(int id) {
    for (int i = 0; i < inventory_count; ++i) {
        if (inventory[i].id == id) return i;
    }
    return -1;
}

void add_to_cart() {
    int id, q;
    show_inventory();
    printf("\nEnter Item ID to add to cart (0 to cancel): ");
    if (scanf("%d", &id) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); return; }
    if (id == 0) return;
    int idx = find_item_index_by_id(id);
    if (idx == -1) { printf("Item ID not found.\n"); return; }

    printf("Enter quantity: ");
    if (scanf("%d", &q) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); return; }
    if (q <= 0) { printf("Quantity must be positive.\n"); return; }
    if (q > inventory[idx].qty) {
        printf("Not enough stock. Available: %d\n", inventory[idx].qty);
        return;
    }

    for (int i = 0; i < cart_count; ++i) {
        if (cart[i].id == id) {
        
            cart[i].qty += q;
            cart[i].total = cart[i].qty * cart[i].price;
            inventory[idx].qty -= q;
            printf("Updated %s in cart. New qty: %d\n", cart[i].name, cart[i].qty);
            return;
        }
    }

    if (cart_count >= MAX_CART) {
        printf("Cart is full!\n");
        return;
    }
    cart[cart_count].id = inventory[idx].id;
    strncpy(cart[cart_count].name, inventory[idx].name, NAME_LEN-1);
    cart[cart_count].name[NAME_LEN-1] = '\0';
    cart[cart_count].price = inventory[idx].price;
    cart[cart_count].qty = q;
    cart[cart_count].total = cart[cart_count].price * q;
    cart_count++;
    inventory[idx].qty -= q;
    printf("Added %s x%d to cart.\n", inventory[idx].name, q);
}

void view_cart() {
    if (cart_count == 0) {
        printf("\nCart is empty.\n");
        return;
    }
    printf("\n--- Cart ---\n");
    printf("%-5s %-25s %-10s %-8s %-10s\n", "ID", "Name", "Price(Rs)", "Qty", "Total(Rs)");
    printf("-------------------------------------------------------------------\n");
    double subtotal = 0.0;
    for (int i = 0; i < cart_count; ++i) {
        printf("%-5d %-25s %-10.2f %-8d %-10.2f\n",
            cart[i].id, cart[i].name, cart[i].price, cart[i].qty, cart[i].total);
        subtotal += cart[i].total;
    }
    double gst = subtotal * GST_RATE;
    double final_total = subtotal + gst;
    printf("-------------------------------------------------------------------\n");
    printf("Subtotal: Rs %.2f\n", subtotal);
    printf("GST (5%%): Rs %.2f\n", gst);
    printf("Final Total: Rs %.2f\n", final_total);
}

void generate_bill() {
    if (cart_count == 0) {
        printf("Cart is empty. Add items before generating a bill.\n");
        return;
    }

    char bill_text[10000];
    char line[512];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char filename[128];
    strftime(filename, sizeof(filename), "bill_%Y%m%d_%H%M%S.txt", tm_info);

    double subtotal = 0.0;
    for (int i = 0; i < cart_count; ++i) subtotal += cart[i].total;
    double gst = subtotal * GST_RATE;
    double final_total = subtotal + gst;

    snprintf(bill_text, sizeof(bill_text),
        "================ Grocery Bill ================\n"
        "Bill Date: %s\n"
        "----------------------------------------------\n"
        "%-5s %-25s %-10s %-8s %-10s\n",
        asctime(tm_info), "ID", "Name", "Price", "Qty", "Total");

    for (int i = 0; i < cart_count; ++i) {
        snprintf(line, sizeof(line), "%-5d %-25s %-10.2f %-8d %-10.2f\n",
            cart[i].id, cart[i].name, cart[i].price, cart[i].qty, cart[i].total);
        strncat(bill_text, line, sizeof(bill_text) - strlen(bill_text) - 1);
    }

    snprintf(line, sizeof(line),
        "----------------------------------------------\n"
        "Subtotal: Rs %.2f\n"
        "GST (5%%): Rs %.2f\n"
        "Final Total: Rs %.2f\n"
        "==============================================\n",
        subtotal, gst, final_total);
    strncat(bill_text, line, sizeof(bill_text) - strlen(bill_text) - 1);

    printf("\n%s\n", bill_text);

    save_bill_to_file(bill_text);

    char record[256];
    strftime(line, sizeof(line), "%Y-%m-%d %H:%M:%S", tm_info);
    snprintf(record, sizeof(record), "%s | %s | Total: Rs %.2f\n", line, filename, final_total);
    FILE *rf = fopen("bills.txt", "a");
    if (rf) {
        fputs(record, rf);
        fclose(rf);
    } else {
        printf("Warning: could not open bills.txt to append record.\n");
    }

    clear_cart();
}

void save_bill_to_file(const char *bill_text) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char filename[128];
    strftime(filename, sizeof(filename), "bill_%Y%m%d_%H%M%S.txt", tm_info);

    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Failed to create bill file '%s'\n", filename);
        return;
    }
    fputs(bill_text, f);
    fclose(f);
    printf("Bill saved to file: %s\n", filename);
}

void clear_cart() {
    if (cart_count == 0) {
        printf("Cart already empty.\n");
        return;
    }

    for (int i = 0; i < cart_count; ++i) {
        int idx = find_item_index_by_id(cart[i].id);
        if (idx != -1) inventory[idx].qty += cart[i].qty;
    }
    cart_count = 0;
    printf("Cart cleared and items returned to inventory.\n");
}

void press_enter_to_continue() {
    printf("Press Enter to continue...");
    while (getchar() != '\n'); 
    getchar();
}
