#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define votersCount 10
#define port 8080
#define csize 1024

typedef struct {
  char vname[100];
  char vcnic[20];
  int voteFlag;
} Voter;

Voter voters[votersCount];
int vCount = 0;

int load_voter_list(const char *infilename) {
  FILE *infile = fopen(infilename, "r");
  if (infile == NULL) {
    printf("File is empty");
    return 0;
  }

  while (fscanf(infile, "%[^/]/%s\n", voters[vCount].vname,
                voters[vCount].vcnic) != EOF) {
    vCount++;
    if (vCount >= votersCount)
      break;
  }
  printf("Done with file reading - Voter List\n");
  fclose(infile);
  return 1;
}

char *load_candidates_list() {
  static char candidates[1024];
  FILE *infile = fopen("Candidates_List.txt", "r");
  if (infile == NULL) {
    printf("File is empty");
    return NULL;
  }

  char line[256];
  while (fgets(line, sizeof(line), infile)) {
    strcat(candidates, line);
  }

  printf("Done with file reading - Candidate List\n");
  fclose(infile);
  return candidates;
}

void cast_vote(const char *symbol, const char *name, const char *cnic) {
  FILE *infile = fopen("ECP.txt", "a");
  if (infile != NULL) {
    fprintf(infile, "%s has given vote for %s, CNIC: %s\n", name, symbol, cnic);
    fclose(infile);
  }
}

int authenticate_voter(const char *name, const char *cnic) {
  for (int i = 0; i < vCount; i++) {
    if (strcmp(voters[i].vname, name) == 0 &&
        strcmp(voters[i].vcnic, cnic) == 0) {
      return i;
    }
  }
  return -1;
}

void *chandler(void *socket_desc) {
  int sock = *(int *)socket_desc;
  char client_message[csize], name[80], cnic[20], symbol[10];

  if (recv(sock, client_message, csize, 0) > 0) {
    sscanf(client_message, "%[^/]/%s", name, cnic);
    int voterIndex = authenticate_voter(name, cnic);
    if (voterIndex != -1) {
      if (voters[voterIndex].voteFlag) {
        char msg[] = "You have already casted your vote.\n";
        send(sock, msg, strlen(msg), 0);
      } else {
        char *candidates = load_candidates_list();
        if (candidates != NULL) {
          send(sock, candidates, strlen(candidates), 0);

          if (recv(sock, symbol, sizeof(symbol), 0) > 0) {
            cast_vote(symbol, voters[voterIndex].vname,
                      voters[voterIndex].vcnic);
            voters[voterIndex].voteFlag = 1;
            char msg[] =
                "You have succesfully casted your vote. Lets see who wins.\n";
            send(sock, msg, strlen(msg), 0);
          }
        }
      }
    } else {
      char msg[] = "Authentication failure has incurred.\n";
      send(sock, msg, strlen(msg), 0);
    }
  }

  close(sock);
  free(socket_desc);
  return 0;
}

int main() {
  int server_socket, client_socket, c, *new_sock;
  struct sockaddr_in server, client;

  if (!load_voter_list("Voters_List.txt")) {
    printf("Unable to render voters list from file.\n");
    return 1;
  }

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    printf("Unable to create socket\n");
    return 1;
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("Socket binding failure Incurred\n");
    return 1;
  }

  listen(server_socket, 3);

  printf("Polling Booth waiting for the voters to caste vote\n");
  c = sizeof(struct sockaddr_in);
  while ((client_socket = accept(server_socket, (struct sockaddr *)&client,
                                 (socklen_t *)&c))) {
    printf("Connection established successfully\n");

    pthread_t client_thread;
    new_sock = malloc(1);
    *new_sock = client_socket;

    if (pthread_create(&client_thread, NULL, chandler, (void *)new_sock) < 0) {
      printf("pthread creation failure incurred\n");
      return 1;
    }

    printf("Voter has been assigned to polling booth\n");
  }

  if (client_socket < 0) {
    printf("Acceptance failure has been detected - by client side\n");
    return 1;
  }

  return 0;
}
