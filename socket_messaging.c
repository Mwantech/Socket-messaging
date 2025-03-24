#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_BUFFER 1024
#define MAX_CLIENTS 10

// Global variables
int client_sockets[MAX_CLIENTS] = {0};
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to broadcast message to all clients except sender
void broadcast_message(char *message, int sender_socket) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_socket) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Thread function to handle individual client communication
void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[MAX_BUFFER] = {0};
    int valread;

    // Send welcome message
    char welcome_msg[MAX_BUFFER] = "Welcome to the chat server!\n";
    send(sock, welcome_msg, strlen(welcome_msg), 0);

    while ((valread = read(sock, buffer, MAX_BUFFER)) > 0) {
        buffer[valread] = '\0';  // Null-terminate the received message

        // Check for exit condition
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Client disconnected\n");
            break;
        }

        // Broadcast the message to other clients
        printf("Received: %s", buffer);
        broadcast_message(buffer, sock);
    }

    // Remove the socket from active clients
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == sock) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    close(sock);
    free(socket_desc);
    return NULL;
}

// Server application with multi-client support
void run_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configure socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept incoming connections
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Add new client to the client sockets array
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        // Create a new thread for the client
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            return;
        }

        // Detach the thread so it cleans up after itself
        pthread_detach(thread_id);
    }

    // Close server socket
    close(server_fd);
}

// Client application (unchanged from previous version)
void run_client() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_BUFFER] = {0};
    pthread_t recv_thread;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type your messages (type 'exit' to quit):\n");

    // Send messages to server
    while (1) {
        printf("You: ");
        fgets(buffer, MAX_BUFFER, stdin);

        // Send message
        send(sock, buffer, strlen(buffer), 0);

        // Check for exit condition
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting client...\n");
            break;
        }
    }

    // Close socket
    close(sock);
}

// Main function to choose server or client mode
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [server|client]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0) {
        run_client();
    } else {
        printf("Invalid mode. Use 'server' or 'client'\n");
        return 1;
    }

    return 0;
}