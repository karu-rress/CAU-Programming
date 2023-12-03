/*
*
*   addrbook.c
*
*   경영경제대학 산업보안학과
*   20234748 나선우
*
*/


/*

주소록을 검색해서 delete_book() 함수로 주소록을 삭제하는 코드를 구현해보자.
이름, 전화번호, 주소로 검색해서 해당 주소록을 삭제할 수 있도록 코드를 완성해보자.
단, 주소록 삭제 후에도 다른 기능들이 정상 동작 하여야 한다. (head, tail, index 등 변화)

*/

#define _CRT_SECURE_NO_WARNINGS

#include <conio.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// CONSTANTS
#define MAX_NAME        20
#define MAX_PHONE       20
#define MAX_ADDRESS     100
const char *DATA_FILE = "addrbook.dat";

// single linked-list struct: book
typedef struct _book {
    unsigned long index;            // Index
    char name[MAX_NAME];            // Name
    char phone[MAX_PHONE];          // Phone no.
    char address[MAX_ADDRESS];      // Address
    struct _book* next;
} book;


book *head, *tail;
unsigned long num_books;            // count of elems
unsigned long last_index;           // last index

#pragma region UI functions
void menu_main(void);               // main menu
void menu_add(void);                // add menu
void menu_search(void);             // search menu
void menu_delete(void);              // delete menu
#pragma endregion

#pragma region Management functions
void init_book(void);                   // linked list의 초기화
void view_book(const book* data);       // 하나의 항목 출력
void list_book(void);                   // 주소록 전체 항목 출력
bool add_book(book data);               // 항목 추가
bool delete_book(int index);            // 항목 삭제
void clear_book(void);                  // linked list 해제
bool read_book(void);                   // 파일에서 데이터 읽기
bool write_book(void);                  // 파일에 데이터 저장
int search_name(const char* name);      // 이름 검색
int search_phone(const char* phone);    // 전화번호 검색
int search_address(const char* address);    // 주소 검색
#pragma endregion

inline void clear_input_buffer() { while (getchar() != '\n'); }
inline void clear_screen() { system("cls"); }
#define for_each_book(iter)             for (book *iter = head; iter != NULL; iter = iter->next)


int main(void) {
    init_book();        // initialize
    read_book();        // load data
    menu_main();        // run main menu
    write_book();       // save data
    clear_book();       // dealloc memory
    return 0;
}

#pragma region UI functions
/// <summary>
/// 메인메뉴 실행
/// </summary>
void menu_main() {
    int choice;
    for (;;) {
        do {
            clear_screen();
            puts("[주소록 - 메인메뉴]");
            puts("=====================");
            puts("1. 주소록 추가");
            puts("2. 주소록 보기");
            puts("3. 주소록 검색");
            puts("4. 주소록 삭제");
            puts("5. 주소록 저장");
            puts("6. 종료\n");
            printf("선택 >> ");
            scanf("%d", &choice);
            clear_input_buffer();
        } while (choice < 1 || choice > 6);

        switch (choice)               // 사용자의 입력에 따라 적절한 함수 호출
        {
        case 1: menu_add();     break;
        case 2: list_book();    break;
        case 3: menu_search();  break;
        case 4: menu_delete();  break;
        case 5: write_book();   break;
        default: return; // 1~6 외 값이 들어올 수 없음. 고로 default는 6.
        }
    }
}


/// <summary>
/// 새 항목 추가
/// </summary>
void menu_add() {
    book data = {.index = last_index + 1};

    do {
        puts("[새로운 데이터 입력]");

        printf("1. 이름: ");
        gets_s(data.name, sizeof data.name);
        printf("2. 전화: ");
        gets_s(data.phone, sizeof data.phone);
        printf("3. 주소: ");
        gets_s(data.address, sizeof data.address);
    } while (strlen(data.name) <= 0 || strlen(data.phone) <= 0 || strlen(data.address) <= 0);

    if (add_book(data)) {
        // increase last_index, num_books if added
        printf("[성공적으로 데이터를 추가하였습니다.]\n\n");
        last_index++;
        num_books++;
    }
    else
        printf("[데이터를 추가하는데 실패하였습니다.]\n\n");
}

/// <summary>
/// 검색 메뉴
/// </summary>
void menu_search() {
    int choice;
    char buffer[MAX_ADDRESS];
    int result;

    for (;;) {
        do {
            clear_screen();
            puts("[어느 항목에서 찾으시겠습니까?]");
            puts("=====================");
            puts("1. 이름");
            puts("2. 전화");
            puts("3. 주소");
            puts("4. 종료\n");
            printf("[선택] ");
            scanf("%d", &choice);
            clear_input_buffer();
        } while (choice < 1 || choice > 4);
        switch (choice) {
        case 1:
            printf("찾을 이름이 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_name(buffer);
            break;
        case 2:
            printf("찾을 전화번호가 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_phone(buffer);
            break;
        case 3:
            printf("찾을 주소가 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_address(buffer);
            break;
        default:
            return;
        }

        if (result != 0)
            printf("[총 %d개의 항목을 찾았습니다.]\n\n", result);
        else
            printf("[%s]에 일치하는 항목을 찾지 못했습니다.\n\n", buffer);
    }
}

/// <summary>
/// 삭제 메뉴
/// </summary>
void menu_delete() {
    int choice;
    char buffer[MAX_ADDRESS];
    int result;

    for (;;) {
        do {
            clear_screen();
            puts("[어느 항목에서 찾아 삭제하시겠습니까?]");
            puts("1. 이름");
            puts("2. 전화");
            puts("3. 주소");
            puts("4. 종료\n");
            printf("[선택] ");
            scanf("%d", &choice);
            clear_input_buffer();
        } while (choice < 1 || choice > 4);
        switch (choice) {
        case 1:
            printf("찾을 이름이 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_name(buffer);
            break;
        case 2:
            printf("찾을 전화번호가 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_phone(buffer);
            break;
        case 3:
            printf("찾을 주소가 무엇입니까? ");
            gets_s(buffer, sizeof buffer);
            result = search_address(buffer);
            break;
        default:
            return;
        }

        if (result != 0)
            printf("[총 %d개의 항목을 찾았습니다.]\n\n", result);
        else {
            printf("[%s]에 일치하는 항목을 찾지 못했습니다.\n\n", buffer);
            continue;
        }

        puts("삭제할 항목의 코드를 입력하여 주십시오.");
        printf("코드 >> ");
        scanf("%d", &choice);
        clear_input_buffer();

        if (delete_book(choice) == false) {
            puts("삭제하는 데 오류가 발생했습니다.");
            continue;
        }
        else
            puts("성공적으로 삭제하였습니다.");
        
        puts("아무 키나 누르십시오.");
        _getch();
    }
}
#pragma endregion


#pragma region Management functions
/// <summary>
/// 초기화 함수
/// </summary>
void init_book() {
    num_books = 0;
    last_index = 0;
    clear_book();
}


/// <summary>
/// 하나의 항목 출력
/// </summary>
/// <param name="data">출력한 데이터의 포인터</param>
void view_book(const book *data) {
    printf("[항목 번호] %d번\n", data->index);
    printf("1. 이름: %s\n", data->name);
    printf("2. 전화: %s\n", data->phone);
    printf("3. 주소: %s\n", data->address);
    printf("[아무 키나 누르세요.] ");
    _getch();
    printf("\n\n");
}

/// <summary>
/// 모든 항목 출력
/// </summary>
void list_book() {
    book *t = head;

    // is empty?
    if (t == NULL) {
        printf("[데이터가 없습니다.]\n\n");
        return;
    }
    
    for (; t != NULL; t = t->next)
        view_book(t);
    printf("[총 %d개의 항목이 있습니다.]\n\n", num_books);
}

/// <summary>
/// 하나의 항목 추가
/// </summary>
/// <param name="data">저장할 데이터</param>
/// <returns>성공시 true, 실패시 false</returns>
bool add_book(book data) {
    book* t;
    if (data.index == 0)
        return false;

    if ((t = (book *)malloc(sizeof(book))) == NULL) {
        puts("[메모리가 부족하여 더 이상 데이터를 추가할 수 없습니다!]");
        return false;
    }

    t->index = data.index;
    strcpy(t->name, data.name);
    strcpy(t->phone, data.phone);
    strcpy(t->address, data.address);
    t->next = NULL;

    /* 현재 리스트가 비어있다면 */
    if (tail == NULL)
        head = tail = t;

    else {
        tail->next = t;
        tail = t;
    }
    return true;
}

/// <summary>
/// 하나의 항목 삭제
/// </summary>
/// <param name="index">삭제할 항목의 index (1~)</param>
/// <returns>성공시 true, 실패시 0</returns>
bool delete_book(int index) {
    book *t = head, *del;
    // 인덱스가 올바르지 않은 경우 또는 리스트가 비어 있는 경우
    if (index < 1 || index > num_books || t == NULL)
        return false;

    if (num_books == 1 && index == 1) {
        free(head);
        head = tail = NULL;
        num_books--;
        return true;
    }

    // head를 삭제한다면 말이 달라짐.
    if (index == 1) {
        del = head;
        head = head->next;
    }
    else {
        // 리스트가 ....AXB..일때, index 값은 X지만 t가 A를 가르키도록 함.
        for (int i = 1; i < index - 1; i++)
            t = t->next;

        // 현재 t는 A를 가리키고 있는 상태. A->next가 X가 아닌 B를 가리키도록 함.
        del = t->next;
        if (del == tail) // 다만 tail를 삭제한다면 tail 포인터는 재지정.
            tail = t;
        t->next = t->next->next; // NULL 체크는 필요하지 않음.
    }

    int idx = 1;
    for_each_book(p) {
        p->index = idx++; // 인덱스 재설정
    }

    // X 삭제 및 개수 감소
    free(del);
    num_books--;

    return true;
}

/// <summary>
/// 메모리 해제
/// </summary>
void clear_book(void) {
    book* t;

    while (head != NULL) {
        t = head;
        head = head->next;
        free(t);
    }
    head = tail = NULL;
}

/// <summary>
/// 파일에서 데이터를 읽어옴.
/// </summary>
/// <returns>성공시 true, 실패시 false</returns>
bool read_book() {
    book data;
    FILE* fp = fopen(DATA_FILE, "rb");

    if (fp == NULL) {
        puts("※ INFO: 저장된 데이터가 없거나, 데이터를 불러오는 데 실패했습니다.\n");
        return false;
    }

    while (!feof(fp)) {
        data.index = 0;
        if (fread(&data, sizeof(book), 1, fp)) {
            add_book(data);
            num_books++;
        }
    }

    fclose(fp);
    last_index = data.index;
    return true;
}

/// <summary>
/// 파일에 내용을 저장함.
/// </summary>
/// <returns>성공시 true, 실패시 false</returns>
bool write_book() {
    book* t;
    FILE* fp = fopen(DATA_FILE, "wb");

    if (fp == NULL) {
        puts("[데이터 파일에 쓰는데 실패했습니다.]");
        return false;
    }
    
    for_each_book(t)
        fwrite(t, sizeof(book), 1, fp);
    
    fclose(fp);
    puts("[성공적으로 데이터 파일에 저장했습니다.]\n");
    return true;
}


enum SEARCH_OPTIONS {
    NAME,
    PHONE,
    ADDRESS,
};

int search(enum SEARCH_OPTIONS option, const char *query) {
    book *t;
    int num = 0;

    for_each_book(t) {
        if (option == NAME && strstr(t->name, query) ||
            option == PHONE && strstr(t->phone, query) ||
            option == ADDRESS && strstr(t->address, query)) {
            view_book(t);
            num++;
        }
    }
    return num;
}


/// <summary>
/// 이름 검색
/// </summary>
/// <param name="name">찾을 이름</param>
/// <returns>찾은 항목의 수</returns>
int search_name(const char* name) {
    return search(NAME, name);
}

/// <summary>
/// 전화번호 검색
/// </summary>
/// <param name="phone">찾을 전화번호</param>
/// <returns>찾은 항목의 수</returns>
int search_phone(const char* phone) {
    return search(PHONE, phone);
}

/// <summary>
/// 주소 검색
/// </summary>
/// <param name="address">찾을 주소(문자열)</param>
/// <returns>찾은 항목의 수</returns>
int search_address(const char *address) {
    return search(ADDRESS, address);
}
#pragma endregion