#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    int tah = 0;

    int riadok = 0;
    int stlpec = 0;
    char hraciaPlocha[3][3] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'}};


    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }
    int flag = 0;

    int vitaz = 0;
    int spravne = 0;
    char policko = ' ';


    for (int i = 0; i < 9 && vitaz == 0; ++i) {

        printf("Cakaj kym protihrac vyberie policko\n");
            n = recv(sockfd, &tah, 200, 0);
        printf("Protihrac vybral policko %d\n", tah);

        riadok = --tah / 3;
        stlpec = tah % 3;
        hraciaPlocha[riadok][stlpec] = 'X';

        for (int o = 0; o < 3; o++) {
            printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
            if (o < 2) {
                printf("---+---+---\n");
            }

            printf("\n\n");

            printf("Zadaj cislo stvorca: ");

            do {
                scanf("%d", &tah);
                if (tah > 0 && tah < 10) {
                    riadok = --tah / 3;
                    stlpec = tah % 3;
                    policko = hraciaPlocha[riadok][stlpec];

                    if (policko != 'O' || policko != 'X') {
                        spravne = 1;
                    } else {
                        printf("Zadali ste uz obsadene policko. \n");
                    }
                } else {
                    printf("Zadali ste nespravne policko. \n");
                }

            } while (spravne != 1);


            send(sockfd, &tah, sizeof(tah), 0);
            hraciaPlocha[riadok][stlpec] = 'O';

            for (int o = 0; o < 3; o++) {
                printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                if (o < 2) {
                    printf("---+---+---\n");
                }
            }

            printf("\n\n");
        }

        close(sockfd);

        return 0;
    }
}