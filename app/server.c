#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>	// for concurrent connections to server

#define BUFF_SIZE 1024

// Function prototypes
void *process_request(void *socket_fd);

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	
	 int server_fd, client_addr_len;
	 int *client_fd;
	 struct sockaddr_in client_addr;
	
	 server_fd = socket(AF_INET, SOCK_STREAM, 0);
	 if (server_fd == -1) {
	 	printf("Socket creation failed: %s...\n", strerror(errno));
	 	return 1;
	 }
	
	 // Since the tester restarts your program quite often, setting SO_REUSEADDR
	 // ensures that we don't run into 'Address already in use' errors
	 int reuse = 1;
	 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
	 	printf("SO_REUSEADDR failed: %s \n", strerror(errno));
	 	return 1;
	 }
	//
	 struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
	 								 .sin_port = htons(4221),
	 								 .sin_addr = { htonl(INADDR_ANY) },
	 								};
	//
	 if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
	 	printf("Bind failed: %s \n", strerror(errno));
	 	return 1;
	 }
	//
	 int connection_backlog = 5;
	 if (listen(server_fd, connection_backlog) != 0) {
	 	printf("Listen failed: %s \n", strerror(errno));
	 	return 1;
	 }
	//
	 while (1)
	 {
		printf("Waiting for a client to connect...\n");
	 	client_addr_len = sizeof(client_addr);

		client_fd = malloc(sizeof(int));
		if (client_fd == NULL)
		{
			fprintf(stderr, "Error: Allocating memory for client_fd failed\n");
			return 1;
		}
		*client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
	 	printf("Client connected\n");

		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, process_request, (void*)client_fd) < 0)
		{
			fprintf(stderr, "Error: Could not create thread\n");
			free(client_fd);
			close(*client_fd);
			close(server_fd);
		}
		// detaching the thread
		pthread_detach(thread_id);
	 }
	 
	//
	 

     /*char buf[BUFF_SIZE];
	 // reading the message
	 int read_bytes = read(client_fd, buf, BUFF_SIZE);
	 printf("msg read - %s\n", buf);

	 char method[16], url[512], protocol[16];
	 sscanf(buf, "%s %s %s", method, url, protocol);
	 printf("URL %s", url);
	// We take a string literal "HTTP/1.1 200 OK\r\n\r\n"
	 char *reply = "HTTP/1.1 200 OK\r\n\r\n";
	 char *replay_bad = "HTTP/1.1 404 Not Found\r\n\r\n";
	 int bytes_sent;
	 char response[BUFF_SIZE] = {0};
	 if (strcmp(url, "/") == 0)
	 {
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
	 }
	 else if (strncmp(url, "/echo/", 6) == 0)
	 {
		char *echo = url + 6;
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %u\r\n\r\n%s", strlen(echo), echo);
	 }
	 else if (strncmp(url, "/user-agent", 11) == 0)
	 {

	 }
	 else
	 {
		bytes_sent = send(client_fd, replay_bad, strlen(replay_bad), 0);
	 }
	 bytes_sent = send(client_fd, response, strlen(response), 0);
	 // taking a char array to receive the GET request in
	 /*char buffer[BUFF_SIZE]; //= {0};
	 recv(client_fd, buffer, BUFF_SIZE, 0);
	 char *token = strtok(buffer, " ");
	 token = strtok(NULL, " ");
	 printf("%s", token);
	 char *reqpath = strdup(token);
	 printf("req path - %s\n", reqpath);
     char *main_path = strtok(reqpath, "/");
	 char *content = strtok(NULL, "");
     printf("main - %s", main_path);
	 printf("content - %s", content);
	 if (strcmp(reqpath, "/") == 0)
	 {
		bytes_sent = send(client_fd, reply, strlen(reply), 0);
	 }
	 if (strcmp(reqpath, "/echo") == 0)
	 {
		size_t length = strlen(content);
		char response[512];
		sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %u\r\n\r\n%s", length, content);
		printf("%s", response);
		bytes_sent = send(client_fd, response, strlen(response), 0);
	 }
	 if (strncmp(reqpath, "/user-agent", 11) == 0)
	 {
		char *s = reqpath + 11;
		printf("Status - %s", s);
	 }
	 else
	 {
		bytes_sent = send(client_fd, replay_bad, strlen(replay_bad), 0);
	 }*/
	 //close(client_fd);
	 close(server_fd);

	return 0;
}

void *process_request(void *socket_fd)
{
	int *client_fd = socket_fd;

	char buf[BUFF_SIZE];
	// reading the message
	int read_bytes = read(client_fd, buf, BUFF_SIZE);
	printf("msg read - %s\n", buf);

	// extracting method, url and protocol
	char method[16], url[512], protocol[16];
	sscanf(buf, "%s %s %s", method, url, protocol);

	int bytes_sent;
	char response[BUFF_SIZE] = {0};
	if (strcmp(method, "GET") == 0)
	{
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n200 OK");
	}
	else if (strcmp(url, "/") == 0)
	{
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
	}
	else if (strncmp(url, "/echo/", 6) == 0)
	{
		char *echo = url + 6;
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %u\r\n\r\n%s", strlen(echo), echo);
	}
	else if (strncmp(url, "/user-agent", 11) == 0)
	{
		char *user_agent = strstr(buf, "User-Agent:");
		if (user_agent != NULL)
		{
			user_agent += 12;
			char *eol = strstr(user_agent, "\r\n");
			*eol = '\0';
		}
		else
		{
			user_agent = "User-agent not found";
		}
		// sending user agent
		snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %u\r\n\r\n%s", strlen(user_agent), user_agent);
	}
	else
	{
		snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\n\r\n\r\n");
	}

	// sending the processed request
	bytes_sent = send(*client_fd, response, strlen(response), 0);

	close(*client_fd);
	return NULL;
}