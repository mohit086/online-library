// utility unction declarations
int get_client(char *username);
int add_user(char *username);
void server_side_authenticate(int* sock, char* auth_request, char* auth_response, struct User* user);