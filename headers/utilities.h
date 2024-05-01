// utility unction declarations
int get_client(char *username);
int add_user(char *username);
void server_side_authenticate(int* sock, char* auth_request, char* response, User* user);
void add_book(int id, char* title, char* author, int quantity, char* response);
void remove_book(int id, char* response);
void change_qty(int id, int quantity, char* response);
void remove_user(char* username, char* response);
void view_all_issues(char* response);
void view_avl_books(char* response);
void view_mybooks(char* username, char* response);
void issue_book(char* username, int id, char* response);
void return_book(char* username, int id, char* response);
void change_password(char* username, char* oldp, char* newp, char* response);
void add_admin(char* username, char* password, char* response);