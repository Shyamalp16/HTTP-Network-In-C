#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define DS_SS_IMPLEMENTATION
#include "ds.h"
#define MAX_LEN 1024

int main(){
    int cfd, sfd, result;
    socklen_t client_addr_size;
    struct sockaddr_in addr, client_addr;

// Open socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1){
        perror("socket");
        return -1;
    }


// Bind socket with socketaddress
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    result = bind(sfd, (struct sockaddr *) &addr, sizeof(addr));
    if(result == -1){
        perror("binding error");
        return -1;
    }

// Listen to the connection now
    result = listen(sfd, 10);
    if(result == -1){
        perror("Error listening to the connection");
        return -1;
    }

    while(1){
        // Infinite loop to take requests
        client_addr_size = sizeof(client_addr);
        cfd = accept(sfd, (struct sockaddr *) &client_addr, &client_addr_size);
        if(cfd == -1){
            perror("Error Accepting from the client");
            return -1;
        }

        // Actual networking stuff
        // Read client message
        char buffer[MAX_LEN] = {0};
        int result = read(cfd, buffer, MAX_LEN);

        if(result == -1){
            perror("Error reading text");
            continue;
        }

        unsigned int buffer_len = result;
        printf("Client (%u): '%s'\n", buffer_len, buffer);

        // do stuff
        ds_string_slice request, token;
        ds_string_slice_init(&request, buffer, buffer_len);
        
        ds_string_slice_tokenize(&request, ' ', &token);
        ds_string_slice_tokenize(&request, ' ', &token);

        char *path = NULL;
        ds_string_slice_to_owned(&token, &path);
        printf("Path Requested: %s\n", path);

        // respond to client
        write(cfd, "Hello\n", 6);
    }

    result = close(sfd);
    if(result == -1){
        perror("Error Closing Connection");
        return -1;
    }
    return 0;
}