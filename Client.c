#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define saddr "127.0.0.1"
#define port 8080
#define sizee 1024

int main(void) {
  int csocket;
  struct sockaddr_in server;
  char message[sizee], sreply[sizee], symbol[10];
  char name[80], cnic[20];

  csocket = socket(AF_INET, SOCK_STREAM, 0);
  if (csocket == -1) {
    printf("Unable to create socket\n");
    return -1;
  }

  server.sin_addr.s_addr = inet_addr(saddr);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (connect(csocket, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("Connection failure has incurred\n");
    return -1;
  }

  printf("Enter your name: ");
  scanf(" %[^\n]s", name);
  printf("Enter your CNIC: ");
  scanf(" %[^\n]s", cnic);

  sprintf(message, "%s/%s", name, cnic);

  if (send(csocket, message, strlen(message), 0) < 0) {
    printf("Unble to send the data properly - ECP take action please\n");
    return -1;
  }

  if (recv(csocket, sreply, sizeof(sreply), 0) < 0) {
    printf("Unable to receive response - Rigging at polls detected\n");
    return -1;
  }
  printf("ECP response:\n%s\n", sreply);

  if (!strstr(sreply, "Authentication failed")) {
    printf("Enter the poll symbol: ");
    scanf(" %[^\n]s", symbol);
    if (send(csocket, symbol, strlen(symbol), 0) < 0) {
      printf("Unble to send the data properly - ECP take action please\n");
      return -1;
    }
  }

  close(csocket);
  return 0;
}
