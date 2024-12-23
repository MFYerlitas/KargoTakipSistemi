#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct CargoNode {
    int cargoID;
    char date[20];
    char status[20];
    int deliveryTime;
    char destination[50];
    char route[500];
    struct CargoNode* next;
} CargoNode;

typedef struct CustomerNode {
    int customerID;
    char name[50];
    char password[20];
    int age;
    char location[50];
    CargoNode* cargoHistory;
    struct CustomerNode* next;
} CustomerNode;

typedef struct Node {
    char name[50];   
    struct Node* next; 
    struct Node* child; 
} Node;

Node* createNode(const char* name) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->next = NULL;
    newNode->child = NULL;
    return newNode;
}

void addChild(Node* parent, Node* child) {
    child->next = parent->child;
    parent->child = child;
}

Node* buildTree() {
    Node* istanbul = createNode("Istanbul");

    Node* tekirdag = createNode("Tekirdag");
    Node* kocaeli = createNode("Kocaeli");

    Node* edirne = createNode("Edirne");
    Node* canakkale = createNode("Canakkale");
    Node* kirklareli = createNode("Kirklareli");

    Node* sakarya = createNode("Sakarya");
    Node* yalova = createNode("Yalova");
    Node* bursa = createNode("Bursa");

    Node* balikesir = createNode("Balikesir");
    Node* bilecik = createNode("Bilecik");

    addChild(istanbul, tekirdag);
    addChild(istanbul, kocaeli);

    addChild(tekirdag, edirne);
    addChild(tekirdag, canakkale);
    addChild(tekirdag, kirklareli);

    addChild(kocaeli, sakarya);
    addChild(kocaeli, yalova);
    addChild(kocaeli, bursa);

    addChild(bursa, balikesir);
    addChild(bursa, bilecik);

    return istanbul;
}

void printTree(Node* root, int level) {
    if (root == NULL) return;
    int i;
    for ( i = 0; i < level; i++) {
        printf("  ");
    }
    printf("%s\n", root->name);
    
    if (root->child != NULL) {
        printTree(root->child, level + 1);
    }
    
    if (root->next != NULL) {
        printTree(root->next, level);
    }
}

int findPath(Node* root, const char* city, Node* path[], int level) {
    if (root == NULL) return 0;

    path[level] = root;

    if (strcmp(root->name, city) == 0) {
        return level + 1;
    }

    int childPath = findPath(root->child, city, path, level + 1);
    if (childPath > 0) {
        return childPath;
    }

    int siblingPath = findPath(root->next, city, path, level);
    if (siblingPath > 0) {
        return siblingPath;
    }

    return 0;
}

int findDistance(Node* root, const char* city1, const char* city2) {
    Node* path1[100];
    Node* path2[100];

    int len1 = findPath(root, city1, path1, 0);
    int len2 = findPath(root, city2, path2, 0);

    if (len1 == 0 || len2 == 0) {
        return -1; 
    }

    int i = 0;
    while (i < len1 && i < len2 && path1[i] == path2[i]) {
        i++;
    }

    return (len1 - i) + (len2 - i);
}

void freeTree(Node* root) {
    if (root == NULL) return;

    freeTree(root->child);
    freeTree(root->next);
    free(root);
}

CustomerNode* customerList = NULL;

CargoNode* createCargoNode(int cargoID, char* date, char* status, int deliveryTime, char* destination) {
    CargoNode* newNode = (CargoNode*)malloc(sizeof(CargoNode));
    newNode->cargoID = cargoID;
    strcpy(newNode->date, date);
    strcpy(newNode->status, status);
    newNode->deliveryTime = deliveryTime;
    strcpy(newNode->destination, destination);
    newNode->next = NULL;
    return newNode;
}

void insertCargoNode(CargoNode** head, CargoNode* newNode) {
    if (*head == NULL || strcmp((*head)->date, newNode->date) > 0) {
        newNode->next = *head;
        *head = newNode;
        return;
    }

    CargoNode* current = *head;
    while (current->next != NULL && strcmp(current->next->date, newNode->date) <= 0) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
}

CustomerNode* createCustomerNode(int customerID, char* name, char* password, int age, char* location) {
    CustomerNode* newNode = (CustomerNode*)malloc(sizeof(CustomerNode));
    newNode->customerID = customerID;
    strcpy(newNode->name, name);
    strcpy(newNode->password, password);
    newNode->age = age;
    strcpy(newNode->location, location);
    newNode->cargoHistory = NULL;
    newNode->next = NULL;
    return newNode;
}

void insertCustomer(int customerID, char* name, char* password, int age, char* location) {
    CustomerNode* newCustomer = createCustomerNode(customerID, name, password, age, location);
    newCustomer->next = customerList;
    customerList = newCustomer;
    printf("Kullanici %s basariyla eklendi!\n", name);
}

bool authenticateCustomer(char* name, char* password, CustomerNode** authenticatedCustomer) {
    CustomerNode* current = customerList;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0 && strcmp(current->password, password) == 0) {
            *authenticatedCustomer = current;
            return true;
        }
        current = current->next;
    }
    return false;
}

void addCargoHistory(char* customerName, int customerID, char* date, char* status, int deliveryTime, char* destination) {
    CustomerNode* current = customerList;
    while (current != NULL && current->customerID != customerID) {
        current = current->next;
    }

    if (current == NULL) {
        printf("%d ID li bir kullanici bulunamadi\n", customerID);
        return;
    }

    int cargoID = 10000 + rand() % 90000;
    CargoNode* newCargo = createCargoNode(cargoID, date, status, deliveryTime, destination);

    // Rota hesaplama kismi
    Node* istanbul = buildTree();
    Node* path[100];
    int pathLength = findPath(istanbul, current->location, path, 0);

    strcpy(newCargo->route, path[0]->name); // Baslangic secme
    int i;
    for ( i = 1; i < pathLength; i++) {
        strcat(newCargo->route, "-");
        strcat(newCargo->route, path[i]->name);
    }

    insertCargoNode(&current->cargoHistory, newCargo);
    printf("%s kullanicisina ait kargo eklendi\n", customerName);
}

void displayCargoHistory(int customerID) {
    CustomerNode* current = customerList;
    while (current != NULL && current->customerID != customerID) {
        current = current->next;
    }

    if (current == NULL) {
        printf("%d ID li bir kullanici bulunamadi\n", customerID);
        return;
    }

    printf("%s isimli kullanicinin kargo gecmisi (ID: %d):\n", current->name, current->customerID);
    CargoNode* cargo = current->cargoHistory;
    while (cargo != NULL) {
        printf("  Kargo ID: %d, Tarih: %s, Teslimat Suresi: %d gun, Varis noktasi: %s\n",
               cargo->cargoID, cargo->date, cargo->deliveryTime, cargo->destination);
        printf("  Rota: %s\n", cargo->route);  // Rota bilgisini ekliyoruz
        cargo = cargo->next;
    }
}

void displayCustomerInfo(CustomerNode* customer) {
    printf("\n****KULLANICI BILGILERI****\n");
    printf("-->Isim: %s\n", customer->name);
    printf("-->Yas: %d\n", customer->age);
    printf("-->Konum: %s\n", customer->location);
    printf("-->Kullanici ID: %d\n", customer->customerID);
    printf("-->Sifre: %s\n", customer->password);
}

void menu(CustomerNode* customer) {
    int choice, deliveryTime;
    char date[20], status[20];
    char destination[50];
    Node* istanbul = buildTree();
    int distance = findDistance(istanbul, "Istanbul", customer->location);
    deliveryTime = distance;

    do {
        printf("\n--- Kargo Takip Sistemi ---\n");
        printf("1. Bir Kargo Ekleyiniz\n");
        printf("2. Kargo Gecmisini Goster\n");
        printf("3. Kullanici Bilgilerini Goster\n");
        printf("4. Cikis\n");
        printf("Seciminizi giriniz: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Tarih giriniz (YYYY-AA-GG)): ");
                scanf("%s", date);
                strcpy(destination, customer->location);
                addCargoHistory(customer->name,customer->customerID, date, status, deliveryTime, destination);
                break;

            case 2:
                displayCargoHistory(customer->customerID);
                break;

            case 3:
                displayCustomerInfo(customer);
                break;

            case 4:
                printf("Cikis yapiliyor.\n");
                break;

            default:
                printf("Gecersiz secim. Lutfen tekrar deneyin.\n");
        }
    } while (choice != 4);
}


int main() {
    char inputName[50], inputPassword[20];
    int age;
    char location[50];
    CustomerNode* authenticatedCustomer = NULL;

    srand(time(NULL));

    printf("\n{{{{{{ MUKA KARGOYA HOSGELDIIZ }}}}}} \n");
    printf("!Hatirlatma:Kurumumuz sadece Marmara bolgesi icin gecerlidir.\n");
    

    while (1) {
        int choice;
        printf("\n1. Uye Ol\n2. Giris\n3. Cikis\nSeciminizi giriniz: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int customerID = 1000 + rand() % 9000;
            printf("Isim: ");
            scanf("%s", inputName);
            printf("Sifre: ");
            scanf("%s", inputPassword);
            printf("Yas: ");
            scanf("%d", &age);
            printf("Konumunuzu giriniz (Lutfen ilk harfi buyuk giriniz): ");
            scanf("%s", location);
            insertCustomer(customerID, inputName, inputPassword, age, location);
        } else if (choice == 2) {
            printf("Isim: ");
            scanf("%s", inputName);
            printf("Sifre: ");
            scanf("%s", inputPassword);

            if (authenticateCustomer(inputName, inputPassword, &authenticatedCustomer)) {
                printf("Giris Basarili!\n");
                menu(authenticatedCustomer);
                break;
            } else {
                printf("Gecersiz bilgiler. Tekrar deneyin.\n");
            }
        } else if(choice==3){
            printf("Sistemden cikiliyor.\n");
            break;
        } else{
            printf("Gecersiz bir numara sectiniz. Lutfen 1, 2 ya da 3'u seciniz.\n");
        }
        
    }

    return 0;
}
