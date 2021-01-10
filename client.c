#include "client.h"

void vypisHry(int sockfd){
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&reset,sizeof reset);
    int znova = 0;

    do {
        znova = 0;
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
            if(i == 0) {
                recv(sockfd, &tah, 200, 0);
                if(tah == 5) {
                    printf("Nahodny vyber hraca : Zacinate.\n");
                    opacne = 1;
                }else{
                    printf("Nahodny vyber hraca : Zacina protihrac.\n");
                }

                printf("\n\n");
                for (int o = 0; o < 3; o++) {
                    printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                    if (o < 2) {
                        printf("---+---+---\n");
                    }
                }
                printf("\n\n");
            }




            if(opacne == 1) {
                hrac = i%2;
            } else {
                hrac = i%2 + 1;
            }

            do {
                spravne = 0;
                if(hrac == 1) {
                    printf("Cakajte kym protihrac vyberie policko\n");
                    recv(sockfd, &tah, 200, 0);
                    printf("Protihrac vybral policko %d\n", tah);

                    riadok = --tah / 3;
                    stlpec = tah % 3;
                    hraciaPlocha[riadok][stlpec] = opacne == 1 ? 'O' : 'X';

                    printf("\n\n");

                    for (int o = 0; o < 3; o++) {
                        printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                        if (o < 2) {
                            printf("---+---+---\n");
                        }
                    }

                    printf("\n\n");
                    spravne = 1;

                } else {
                    do {
                        printf("Zadajte cislo stvorca: ");
                        scanf("%d", &tah);
                        if (tah > 0 && tah < 10) {
                            riadok = --tah/3;
                            stlpec = tah%3;
                            tah++;
                            policko = hraciaPlocha[riadok][stlpec];

                            if (policko != 'O' && policko != 'X') {
                                spravne = 1;
                            } else {
                                printf("Zadali ste uz obsadene policko. \n");
                            }
                        } else {
                            printf("Zadali ste nespravne policko. \n");
                        }

                    } while (spravne != 1);

                    send(sockfd, &tah, sizeof(tah), 0);
                    hraciaPlocha[riadok][stlpec] = opacne == 1 ? 'X' : 'O';

                    printf("\n\n");

                    for (int o = 0; o < 3; o++) {
                        printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                        if (o < 2) {
                            printf("---+---+---\n");
                        }
                    }

                    printf("\n\n");
                }
            } while(spravne != 1);
            recv(sockfd, &vitaz, 200, 0);
        }

        if(vitaz == 0) {
            printf("Remiza\n");
        } else if (vitaz == 1) {
            printf("Prehrali ste\n");
        } else {
            printf("Vyhrali ste\n");
        }
        int ok = 0;
        int n = 5;
        printf("Chcete hrat znova? (1 - ano, 2 - nie)\n");
        do {
            scanf("%d", &znova);
            if (znova == 1 || znova == 2) {
                if (znova == 1) {
                    send(sockfd, &znova, sizeof(znova), 0);
                    printf("Cakanie na potvrdenie od protihraca\n");
                    n = recv(sockfd, &znova, 200, 0);
                    if (n == 0) {
                        znova = 2;
                        printf("Protihrac sa odpojil.\n");

                    } else if (n < 0) {
                        znova = 2;
                        printf("Chyba.\n");

                    } else {
                        if (znova == 1) {
                            printf("Protihrac chce hrat znova,\n");
                        } else if (znova == 2) {
                            printf("Protihrac nechce hrat znova.\n");
                        } else {
                            printf("Chyba.\n");
                        }
                    }
                } else {
                    printf("Koniec hry.\n");
                    send(sockfd, &znova, sizeof(znova), 0);
                }
                ok = 1;
            } else {
                printf("Zadali ste nespravnu hodnotu. Skuste znova.");
            }
        } while (ok!=1);
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
        return 7;
    }
    printf("Cakajte na pripojenie k serveru... \n");

    int i = 0;
    int znova = 0;
    int ok = 0;
    setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof timeout);
    recv(sockfd, &i, 200, 0);

    if(i == 0) {
        printf("Prave prebieha ina hra.\n");
        do {
            printf("Chcete pokracovat v cakani (+20 sekund)? {1 - ano, 2 - nie}\n");
            scanf("%d", &znova);
            if (znova == 1 || znova == 2) {
                if (znova == 1) {
                    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout2, sizeof timeout2);
                    printf("Cakanie na ukoncenie prebiehajucej hry\n");
                    recv(sockfd, &i, 200, 0);
                    if (i == 0) {
                        ok = 0;
                        printf("Cas uplynul.\n");
                    } else {
                        ok = 1;
                    }
                } else {
                    ok = 2;
                }

            } else {
                printf("Zadali ste nespravnu hodnotu. Skuste znova.");
            }
        } while (ok < 1);
    } else {
        ok = 1;
    }
    if(ok == 1) {
        i = 3;
        send(sockfd,&i,sizeof(i),0);
        if (i == 1) {
            printf("Pripojenie uspesne\n");

        }
        vypisHry(sockfd);
        close(sockfd);
        printf("Dovidenia, prajem prijemny zvysok dna.\n");
    } else {
        close(sockfd);
        printf("Dovidenia, prajem prijemny zvysok dna.\n");
    }

    return 0;
}