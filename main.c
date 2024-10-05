#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define DS_SB_IMPLEMENTATION
#define DS_SS_IMPLEMENTATION
#define DS_IO_IMPLEMENTATION

#include "ds.h"
#define MAX_LEN 1024

int main(){
    int cfd, sfd, result;
    socklen_t client_addr_size;
    struct sockaddr_in addr, client_addr;

// Open socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1){
        DS_PANIC("Error Initializing Socket");
        return -1;
    }


// Bind socket with socketaddress
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    result = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
    if(result == -1){
        DS_PANIC("Error Binding to Socket");
        return -1;
    }

// Listen to the connection now
    result = listen(sfd, 10);
    if(result == -1){
        DS_PANIC("Error listening to the connection");
        return -1;
    }

    while(1){
        // Infinite loop to take requests
        client_addr_size = sizeof(client_addr);
        cfd = accept(sfd, (struct sockaddr *) &client_addr, &client_addr_size);
        if(cfd == -1){
            DS_PANIC("Error Accepting from client");
            return -1;
        }

        // Actual networking stuff
        // Read client message
        char buffer[MAX_LEN] = {0};
        int result = read(cfd, buffer, MAX_LEN);

        if(result == -1){
            DS_PANIC("Error Reading text");
            continue;
        }

        unsigned int buffer_len = result; 

        // do stuff
        ds_string_slice request, token;
        ds_string_slice_init(&request, buffer, buffer_len);
        
        // Split method
        ds_string_slice_tokenize(&request, ' ', &token);
        char *verb = NULL;
        ds_string_slice_to_owned(&token, &verb);
        if(strcmp(verb, "GET") != 0){
            DS_LOG_ERROR("Invalid Request Method");
            // TODO: RESPOND WITH 400 (BAD REQUEST)
            continue;
        }
        printf("Request Method: %s\n", verb);

        // Split path
        ds_string_slice_tokenize(&request, ' ', &token);
        char *path = NULL;
        ds_string_slice_to_owned(&token, &path);
        printf("Path Requested: %s\n", path);

        // Use a string builder and store the resulting string in response ptr and write it to the client via HTTP server.
        ds_string_builder response_builder;
        ds_string_builder_init(&response_builder);
        ds_string_builder_append(&response_builder, "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\n\n%s", 6, "Hello\n");
        char *response = NULL;
        ds_string_builder_build(&response_builder, &response);
        int response_len = strlen(response);
        

        // respond to client
        printf("%s", response);
        write(cfd, response, response_len);
    }

    result = close(sfd);
    if(result == -1){
        DS_PANIC("Error Closing The connection");
        return -1;
    }
    return 0;
}