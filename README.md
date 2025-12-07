# FMSim – File Management System in C

## Overview

FMSim is a command-line-based file management system written in C that simulates essential and advanced file system operations. It supports multiple users, file-level permissions, soft and hard deletion, and secure access through an access control list (ACL). The project was developed to gain a deeper understanding of how file systems operate in real operating systems.

## Features

- Create, open, read, write, and close files
- Multi-user login support
- Access Control List (ACL) for per-user file permissions (`r`, `w`, `x`)
- File metadata: owner, size, creation/modification timestamps
- Soft delete with file restoration capability
- Permanent file deletion
- Dynamic user creation
- File search by keyword
- Command-line interface with real-time input parsing

## Concepts Simulated

- Ownership and permission control
- Secure multi-user access
- File state management (creation, deletion, restoration)
- Metadata tracking for each file

## Example Usage

```
> create_user charlie
User 'charlie' created with user_id 3

> login 1
Logged in as alice

> create notes.txt rw
File 'notes.txt' created by alice

> grant notes.txt 3 rw
Granted rw access to user 3 on 'notes.txt'

> login 3
Logged in as charlie

> open notes.txt
File 'notes.txt' opened.

> write notes.txt Hello from Charlie
Content written to 'notes.txt'.

> delete notes.txt
File 'notes.txt' moved to trash.

> restore notes.txt
File 'notes.txt' restored.

> delete_permanent notes.txt
File 'notes.txt' permanently deleted.
```

## Compilation and Running

### To compile:
```
gcc file_management_system.c -o fmsim
```

### To run:
```
./fmsim
```

## Command Reference

| Command                          | Description                                      |
|----------------------------------|--------------------------------------------------|
| `create_user <username>`        | Create a new user                                |
| `login <user_id>`               | Log in as a specific user                        |
| `create <filename> <perm>`      | Create a file with permissions (`r`, `w`, `x`)   |
| `open <filename>`               | Open a file                                      |
| `read <filename>`               | Read content from an open file                   |
| `write <filename> <content>`    | Write content to an open file                    |
| `close <filename>`              | Close an open file                               |
| `grant <filename> <uid> <perm>` | Grant permissions to another user                |
| `chmod <filename> <perm>`       | Change owner's permissions                       |
| `delete <filename>`             | Soft delete a file                               |
| `restore <filename>`            | Restore a soft-deleted file                      |
| `delete_permanent <filename>`   | Permanently delete a file                        |
| `search <keyword>`              | Search for files by name or content              |
| `help`                          | Display all available commands                   |
| `exit`                          | Exit the program                                 |

## Group Members

- Mithi Pandey – Core implementation, ACL system, command interface  
- [Group Member 2 Name] – Research and report writing, testing, documentation  
- [Group Member 3 Name] – Final presentation, slides, optional feature extensions

## Possible Future Extensions

- Directory and folder navigation (`mkdir`, `cd`, `ls`)
- File versioning and rollback
- Save/load system state to and from disk
- User authentication with passwords

## License

This project is for academic and educational purposes and is released under the MIT License.
