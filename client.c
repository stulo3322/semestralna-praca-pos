#include "client.h"


void vypisHry(int sockfd){
    int znova = 0;

    do {
        int hrac = 0;
        int vitaz = 0;
        int spravne = 0;
        char policko = ' ';
        int opacne = 0;
        int tah = 0;
        int riadok = 0;
        int stlpec = 0;
        char hraciaPlocha[3][3] = {{'1', '2', '3'},
                                   {'4', '5', '6'},
                                   {'7', '8', '9'}};
        for (int i = 0; i < 9 && vitaz == 0; ++i) {
            printf("zaciatok for\n");

            if(i == 0) {
                recv(sockfd, &tah, 200, 0);
                printf("opacne %d\n",tah);
                if(tah == 5) {
                    opacne = 1;
                }
                for (int o = 0; o < 3; o++) {
                    printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                    if (o < 2) {
                        printf("---+---+---\n");
                    }
                }
            }

            if(opacne == 1) {
                hrac = i%2;
            } else {
                hrac = i%2 + 1;
            }

            do {
                spravne = 0;
                if(hrac == 1) {
                    printf("Cakaj kym protihrac vyberie policko\n");
                    recv(sockfd, &tah, 200, 0);
                    printf("Protihrac vybral policko %d\n", tah);

                    riadok = --tah / 3;
                    stlpec = tah % 3;
                    hraciaPlocha[riadok][stlpec] = opacne == 1 ? 'O' : 'X';

                    for (int o = 0; o < 3; o++) {
                        printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                        if (o < 2) {
                            printf("---+---+---\n");
                        }
                    }
                    printf("\n\n");
                    spravne = 1;

                } else {
                    printf("Zadaj cislo stvorca: ");

                    do {
                        scanf("%d", &tah);
                        printf("nacital som %d\n",tah);
                        if (tah > 0 && tah < 10) {
                            riadok = --tah/3;
                            stlpec = tah%3;
                            tah++;
                            policko = hraciaPlocha[riadok][stlpec];
                            printf("po vypoctoch je tah %d\n",tah);

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
                    printf("odoslal som %d\n",tah);
                    hraciaPlocha[riadok][stlpec] = opacne == 1 ? 'X' : 'O';

                    for (int o = 0; o < 3; o++) {
                        printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                        if (o < 2) {
                            printf("---+---+---\n");
                        }
                    }

                    printf("\n\n");
                }
            } while(spravne != 1);
            printf("Cakam na vitaza\n");
            recv(sockfd, &vitaz, 200, 0);
            printf("Prijal som vitaza %d\n",vitaz);
        }

        if(vitaz == 0) {
            printf("Remiza\n");
        } else if (vitaz == 1) {
            printf("Prehral si\n");
        } else {
            printf("Vyhral si\n");
        }

        printf("Chces hrat znova? (1 - ano, 2 - nie)\n");
        scanf("%d", &znova);
        if(znova == 1) {
            send(sockfd, &znova, sizeof(znova), 0);
            printf("Cakanie na potvrdenie od protihraca\n");
            if(recv(sockfd, &znova, 200, 0) == 0) {
                printf("Protihrac zamietol ponuku o hranie znova\n");
            }
        } else {
            send(sockfd, &znova, sizeof(znova), 0);
        }
    } while(znova == 1);
}


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

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

    vypisHry(sockfd);

    close(sockfd);
    return 0;
}