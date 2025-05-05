#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN() system("cls")
#else
#define CLEAR_SCREEN() system("clear")
#endif

#define MAX_PRODUCTS 100
#define FILENAME "pharmacy_products.txt"

#define RESET "\033[0m"
#define VERDE "\033[0;32m"
#define ROSU "\033[0;31m"
#define CYAN "\033[0;36m"
#define ROZ "\033[1;35m"

typedef struct {
    char name[50];
    int quantity;
    int reserved;
    float price;
} Product;

Product products[MAX_PRODUCTS];
int product_count = 0;

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

void load_products() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf(ROSU "Nu există un fișier de date. Se va crea unul nou.\n" RESET);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Product p;
        char name[100];
        int quantity, reserved;
        float price;

        int i = 0;
        while (line[i] && (line[i] < '0' || line[i] > '9')) i++;
        if (i == 0 || line[i] == '\0') continue;

        strncpy(name, line, i - 1);
        name[i - 1] = '\0';
        trim_newline(name);
        strcpy(p.name, name);

        sscanf(line + i, "%d %d %f", &quantity, &reserved, &price);
        p.quantity = quantity;
        p.reserved = reserved;
        p.price = price;

        products[product_count++] = p;
    }

    fclose(file);
}

void save_products() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf(ROSU "Eroare la deschiderea fișierului pentru scriere.\n" RESET);
        return;
    }

    for (int i = 0; i < product_count; i++) {
        fprintf(file, "%s %d %d %.2f\n",
                products[i].name,
                products[i].quantity,
                products[i].reserved,
                products[i].price);
    }

    fclose(file);
}

void show_main_menu() {
    CLEAR_SCREEN();
    printf(ROZ "===== Meniu Principal =====\n" RESET);
    printf("1. Vizualizare produse disponibile\n");
    printf("2. Adăugare produs\n");
    printf("3. Ștergere produs\n");
    printf("4. Căutare produs\n");
    printf("5. Rezervare produs\n");
    printf("6. Afișare produse rezervate\n");
    printf("7. Ieșire\n");
}

void wait_for_return() {
    int choice;
    do {
        printf("\n0. Înapoi la meniul principal\n");
        printf("Alegeți o opțiune: ");
        scanf("%d", &choice);
        if (choice != 0) {
            printf(ROSU "Opțiune invalidă. Încercați din nou.\n" RESET);
        }
    } while (choice != 0);
}

void show_available_products() {
    CLEAR_SCREEN();
    printf(ROZ "=== Produse Disponibile ===\n" RESET);
    for (int i = 0; i < product_count; i++) {
        if (products[i].quantity > 0) {
            printf("Produs: " CYAN "%s" RESET ", Cantitate: " VERDE "%d" RESET ", Rezervate: " ROSU "%d" RESET ", Preț: %.2f RON\n",
                   products[i].name, products[i].quantity, products[i].reserved, products[i].price);
        }
    }
    wait_for_return();
}

void add_product() {
    CLEAR_SCREEN();
    printf(ROZ "=== Adăugare Produs ===\n" RESET);

    if (product_count >= MAX_PRODUCTS) {
        printf(ROSU "Nu se pot adăuga mai multe produse. Limita maximă a fost atinsă.\n" RESET);
        wait_for_return();
        return;
    }

    Product p;
    getchar();
    printf("Introduceți numele produsului: ");
    fgets(p.name, sizeof(p.name), stdin);
    trim_newline(p.name);
    printf("Introduceți cantitatea de adaugat: ");
    scanf("%d", &p.quantity);
    printf("Introduceți prețul produsului in RON (EX:00.00): ");
    scanf("%f", &p.price);
    p.reserved = 0;

    products[product_count++] = p;
    printf(VERDE "Produsul a fost adăugat cu succes.\n" RESET);
    save_products();
    wait_for_return();
}

void delete_product() {
    CLEAR_SCREEN();
    printf(ROZ "=== Ștergere Produs ===\n" RESET);

    char name[50];
    getchar();
    printf("Introduceți numele produsului de șters: ");
    fgets(name, sizeof(name), stdin);
    trim_newline(name);

    int found = 0;
    for (int i = 0; i < product_count; i++) {
        if (strcmp(products[i].name, name) == 0) {
            if (products[i].reserved > 0) {
                products[i].quantity = 0;
                printf(ROSU "Produsul are rezervări active. A fost scos doar din stoc.\n" RESET);
            } else {
                for (int j = i; j < product_count - 1; j++) {
                    products[j] = products[j + 1];
                }
                product_count--;
                printf(VERDE "Produsul a fost șters complet.\n" RESET);
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf(ROSU "Produsul nu a fost găsit.\n" RESET);
    }

    save_products();
    wait_for_return();
}

void search_product() {
    CLEAR_SCREEN();
    printf(ROZ "=== Căutare Produs ===\n" RESET);

    char name[50];
    getchar();
    printf("Introduceți numele produsului: ");
    fgets(name, sizeof(name), stdin);
    trim_newline(name);

    int found = 0;
    for (int i = 0; i < product_count; i++) {
        if (strcmp(products[i].name, name) == 0) {
            printf("Produs: " CYAN "%s" RESET ", Cantitate: " VERDE "%d" RESET ", Rezervate: " ROSU "%d" RESET ", Preț: %.2f RON\n",
                   products[i].name, products[i].quantity, products[i].reserved, products[i].price);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf(ROSU "Produsul nu a fost găsit.\n" RESET);
    }

    wait_for_return();
}

void reserve_product() {
    CLEAR_SCREEN();
    printf(ROZ "=== Rezervare Produs ===\n" RESET);

    for (int i = 0; i < product_count; i++) {
        if (products[i].quantity > 0) {
            printf("Produs: " CYAN "%s" RESET ", Cantitate: " VERDE "%d" RESET ", Preț: %.2f RON\n",
                   products[i].name, products[i].quantity, products[i].price);
        }
    }

    char name[50];
    int quantity;
    getchar();
    printf("Introduceți numele produsului: ");
    fgets(name, sizeof(name), stdin);
    trim_newline(name);
    printf("Introduceți cantitatea de rezervat: ");
    scanf("%d", &quantity);

    int found = 0;
    for (int i = 0; i < product_count; i++) {
        if (strcmp(products[i].name, name) == 0) {
            if (products[i].quantity >= quantity) {
                products[i].quantity -= quantity;
                products[i].reserved += quantity;
                printf(VERDE "Rezervarea a fost realizată cu succes.\n" RESET);
            } else {
                printf(ROSU "Stoc insuficient pentru această rezervare.\n" RESET);
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf(ROSU "Produsul nu a fost găsit.\n" RESET);
    }

    save_products();
    wait_for_return();
}

void show_reserved_products() {
    CLEAR_SCREEN();
    printf(ROZ "=== Produse Rezervate ===\n" RESET);

    int any_reserved = 0;
    for (int i = 0; i < product_count; i++) {
        if (products[i].reserved > 0) {
            printf("Produs: " CYAN "%s" RESET ", Cantitate rezervată: " ROSU "%d" RESET ", Preț: %.2f RON\n",
                   products[i].name, products[i].reserved, products[i].price);
            any_reserved = 1;
        }
    }

    if (!any_reserved) {
        printf(ROSU "Nu există produse rezervate.\n" RESET);
    }

    wait_for_return();
}

int main() {
#ifdef _WIN32
    system("chcp 65001");
#endif

    load_products();

    int choice;
    do {
        show_main_menu();
        printf("Alegeți o opțiune: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: show_available_products(); break;
            case 2: add_product(); break;
            case 3: delete_product(); break;
            case 4: search_product(); break;
            case 5: reserve_product(); break;
            case 6: show_reserved_products(); break;
            case 7:
                printf(VERDE "Ieșire din meniu\n" RESET);
                break;
            default:
                printf(ROSU "Opțiune invalidă. Încercați din nou.\n" RESET);
                getchar(); getchar();
        }
    } while (choice != 7);

    return 0;
}
