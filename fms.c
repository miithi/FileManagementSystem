// file_management_system.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILES 100
#define MAX_NAME 50
#define MAX_USERS 10
#define MAX_CONTENT 1024

typedef struct {
    char username[MAX_NAME];
    int user_id;
} User;

typedef struct {
    int user_id;
    char permissions[4]; // e.g., "rwx"
} AccessEntry;

typedef struct {
    char filename[MAX_NAME];
    char content[MAX_CONTENT];
    int owner_id;
    char owner_permissions[4];
    int is_open;
    size_t size;
    time_t created_at;
    time_t modified_at;
    int is_deleted;
    AccessEntry acl[MAX_USERS];
    int acl_count;
} File;

File file_system[MAX_FILES];
int file_count = 0;

User users[MAX_USERS] = {
    {"admin", 0},
    {"alice", 1},
    {"bob", 2}
};
int user_count = 3;

User* current_user = &users[0];

File* find_file(const char* name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_system[i].filename, name) == 0 && !file_system[i].is_deleted) return &file_system[i];
    }
    return NULL;
}

void create_user(const char* username) {
    if (user_count >= MAX_USERS) {
        printf("Max user limit reached.\n");
        return;
    }
    strcpy(users[user_count].username, username);
    users[user_count].user_id = user_count;
    printf("User '%s' created with user_id %d\n", username, user_count);
    user_count++;
}

void create_file(const char* name, const char* permissions) {
    if (file_count >= MAX_FILES) {
        printf("File system is full.\n");
        return;
    }
    if (find_file(name)) {
        printf("File already exists.\n");
        return;
    }
    File new_file;
    strcpy(new_file.filename, name);
    strcpy(new_file.owner_permissions, permissions);
    new_file.owner_id = current_user->user_id;
    new_file.is_open = 0;
    new_file.size = 0;
    new_file.created_at = time(NULL);
    new_file.modified_at = time(NULL);
    new_file.is_deleted = 0;
    new_file.acl_count = 0;
    strcpy(new_file.content, "");

    file_system[file_count++] = new_file;
    printf("File '%s' created by %s\n", name, current_user->username);
}

int has_permission(File* file, char perm) {
    if (file->owner_id == current_user->user_id) {
        return strchr(file->owner_permissions, perm) != NULL;
    }
    for (int i = 0; i < file->acl_count; i++) {
        if (file->acl[i].user_id == current_user->user_id) {
            return strchr(file->acl[i].permissions, perm) != NULL;
        }
    }
    return 0;
}

void open_file(const char* name) {
    File* file = find_file(name);
    if (!file) {
        printf("File not found.\n");
        return;
    }
    if (!has_permission(file, 'r')) {
        printf("Permission denied to open file.\n");
        return;
    }
    file->is_open = 1;
    printf("File '%s' opened.\n", name);
}

void close_file(const char* name) {
    File* file = find_file(name);
    if (!file || !file->is_open) {
        printf("File is not open.\n");
        return;
    }
    file->is_open = 0;
    printf("File '%s' closed.\n", name);
}

void write_file(const char* name, const char* new_content) {
    File* file = find_file(name);
    if (!file || !has_permission(file, 'w') || !file->is_open) {
        printf("Cannot write to file.\n");
        return;
    }
    strncpy(file->content, new_content, MAX_CONTENT);
    file->size = strlen(file->content);
    file->modified_at = time(NULL);
    printf("Content written to '%s'.\n", name);
}

void read_file(const char* name) {
    File* file = find_file(name);
    if (!file || !file->is_open) {
        printf("Cannot read file.\n");
        return;
    }
    printf("Content of '%s':\n%s\n", name, file->content);
}

void delete_file(const char* name) {
    File* file = find_file(name);
    if (!file || file->owner_id != current_user->user_id) {
        printf("File not found or permission denied.\n");
        return;
    }
    file->is_deleted = 1;
    printf("File '%s' moved to trash. Use 'restore' or 'delete_permanent'.\n", name);
}

void delete_permanently(const char* name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_system[i].filename, name) == 0 && file_system[i].owner_id == current_user->user_id) {
            for (int j = i; j < file_count - 1; j++) {
                file_system[j] = file_system[j + 1];
            }
            file_count--;
            printf("File '%s' permanently deleted.\n", name);
            return;
        }
    }
    printf("File not found or permission denied.\n");
}

void restore_file(const char* name) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(file_system[i].filename, name) == 0 && file_system[i].is_deleted && file_system[i].owner_id == current_user->user_id) {
            file_system[i].is_deleted = 0;
            printf("File '%s' restored.\n", name);
            return;
        }
    }
    printf("File not found or not in trash.\n");
}

void search_file(const char* keyword) {
    printf("Searching for files with keyword '%s':\n", keyword);
    for (int i = 0; i < file_count; i++) {
        if (!file_system[i].is_deleted && (strstr(file_system[i].filename, keyword) || strstr(file_system[i].content, keyword))) {
            printf("- %s (Owner: %d, Permissions: %s)\n", file_system[i].filename, file_system[i].owner_id, file_system[i].owner_permissions);
        }
    }
}

void login(int user_id) {
    if (user_id < 0 || user_id >= user_count) {
        printf("Invalid user ID.\n");
        return;
    }
    current_user = &users[user_id];
    printf("Logged in as %s\n", current_user->username);
}

int main() {
    char command[100];
    char arg1[100], arg2[100], arg3[1024];

    printf("Welcome to FMSim File System! Type 'help' for commands.\n");

    while (1) {
        printf("> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (strncmp(command, "login", 5) == 0) {
            int uid;
            sscanf(command, "login %d", &uid);
            login(uid);
        } else if (strncmp(command, "create_user", 11) == 0) {
            sscanf(command, "create_user %s", arg1);
            create_user(arg1);
        } else if (strncmp(command, "create", 6) == 0) {
            sscanf(command, "create %s %s", arg1, arg2);
            create_file(arg1, arg2);
        } else if (strncmp(command, "chmod", 5) == 0) {
            sscanf(command, "chmod %s %s", arg1, arg2);
            File* file = find_file(arg1);
            if (!file || file->owner_id != current_user->user_id) {
                printf("Cannot change permissions. Not owner or file not found.\n");
            } else {
                strcpy(file->owner_permissions, arg2);
                printf("Permissions for '%s' changed to '%s'\n", file->filename, file->owner_permissions);
            }
        } else if (strncmp(command, "grant", 5) == 0) {
            int target_id;
            sscanf(command, "grant %s %d %s", arg1, &target_id, arg2);
            File* file = find_file(arg1);
            if (!file || file->owner_id != current_user->user_id) {
                printf("Only the owner can grant access.\n");
            } else {
                int updated = 0;
                for (int i = 0; i < file->acl_count; i++) {
                    if (file->acl[i].user_id == target_id) {
                        strcpy(file->acl[i].permissions, arg2);
                        updated = 1;
                        break;
                    }
                }
                if (!updated && file->acl_count < MAX_USERS) {
                    file->acl[file->acl_count].user_id = target_id;
                    strcpy(file->acl[file->acl_count].permissions, arg2);
                    file->acl_count++;
                }
                printf("Granted %s access to user %d on '%s'\n", arg2, target_id, arg1);
            }
        } else if (strncmp(command, "open", 4) == 0) {
            sscanf(command, "open %s", arg1);
            open_file(arg1);
        } else if (strncmp(command, "write", 5) == 0) {
            sscanf(command, "write %s %[^]", arg1, arg3);
            write_file(arg1, arg3);
        } else if (strncmp(command, "read", 4) == 0) {
            sscanf(command, "read %s", arg1);
            read_file(arg1);
        } else if (strncmp(command, "close", 5) == 0) {
            sscanf(command, "close %s", arg1);
            close_file(arg1);
        } else if (strncmp(command, "delete_permanent", 17) == 0) {
            sscanf(command, "delete_permanent %s", arg1);
            delete_permanently(arg1);
        } else if (strncmp(command, "delete", 6) == 0) {
            sscanf(command, "delete %s", arg1);
            delete_file(arg1);
        } else if (strncmp(command, "restore", 7) == 0) {
            sscanf(command, "restore %s", arg1);
            restore_file(arg1);
        } else if (strncmp(command, "search", 6) == 0) {
            sscanf(command, "search %s", arg1);
            search_file(arg1);
        } else if (strcmp(command, "help") == 0) {
            printf("Commands:\n");
            printf(" login <user_id>\n create_user <username>\n create <filename> <permissions>\n open <filename>\n write <filename> <content>\n");
            printf(" read <filename>\n close <filename>\n delete <filename>\n delete_permanent <filename>\n restore <filename>\n search <keyword>\n grant <filename> <user_id> <permissions>\n chmod <filename> <permissions>\n exit\n");
        } else if (strcmp(command, "exit") == 0) {
            printf("Exiting FMSim.\n");
            break;
        } else {
            printf("Unknown command. Type 'help' for options.\n");
        }
    }

    return 0;
}
