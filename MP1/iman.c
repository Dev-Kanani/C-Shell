#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 80
#define BUFFER_SIZE 1024

void print_debug(const char *message)
{
    printf("[DEBUG] %s\n", message);
}

void iMan(const char *command_name)
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[BUFFER_SIZE], line[BUFFER_SIZE];
    ssize_t n;
    int in_tag = 0;      
    int name_flag = 0;   
    int header_flag = 1;

    print_debug("Starting iMan...");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return;
    }
    print_debug("Socket created successfully");

    server = gethostbyname("man.he.net");
    if (server == NULL)
    {
        fprintf(stderr, "No such host\n");
        close(sockfd);
        return;
    }
    print_debug("Host resolved successfully");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        return;
    }
    print_debug("Connected to the server successfully");

    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "Connection: close\r\n\r\n",
             command_name);

    print_debug("Sending HTTP request...");
    if (write(sockfd, request, strlen(request)) < 0)
    {
        perror("Error sending request");
        close(sockfd);
        return;
    }

    FILE *sock_stream = fdopen(sockfd, "r");
    if (sock_stream == NULL)
    {
        perror("Failed to open socket stream");
        close(sockfd);
        return;
    }

    while (fgets(line, sizeof(line), sock_stream) != NULL)
    {

        if (!name_flag)
        {
            if (strstr(line, command_name) != NULL)
            {
                name_flag = 1;
            }
            continue;
        }

        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] == '<')
            {
                in_tag = 1;
            }
            else if (line[i] == '>')
            {
                in_tag = 0;
                continue;
            }

            if (!in_tag)
            {
                putchar(line[i]);
            }
        }
    }

    fclose(sock_stream);
    close(sockfd);
    print_debug("Response reading complete");
}
