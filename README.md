Online Library Management System, done in C with operating systems concepts such as semaphores, threads and sockets, as part of the EG301P Operating Systems Lab course.

## Features

- Client-Server application with support for multiple users and administrators.
- Persistent storage with concurrency control.
- Admins can add/remove books and users.
- Users can view/issue/return books.

## Run Locally

- After cloning the repository, run the Makefile.
```bash
make clean
```
```bash
make all
```

- Navigate to /bin. Run setup to set the database, and then run the server or client.

```bash
./setup
```

```bash
./server
```

```bash
./client
```

- Use username _admin_, and any password to sign up for the first time as an admin. Then you can add new books or admins. 
- As a client, you can use any username and password except _admin_.
- You can run setup again to clear the database.