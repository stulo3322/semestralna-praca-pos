
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t * mutex;
    pthread_cond_t * hraPrebieha;
    pthread_cond_t * mozeSaHrat;
    int hrac;
    int vitaz;
    int riadok;
    int stlpec;
    int tah;
    int sockfd;
} dataHra;

void* vlaknoHry(void*args) {


    dataHra * dat = (dataHra *)args;
    int n = 0;
    char hraciaPlocha[3][3] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'}};

    for (int i = 0; i < 9 && dat->vitaz == 0; i++) {
        printf("\n\n");
        for (int o = 0; o < 3; o++) {
                printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
                printf("---+---+---\n");
        }

        dat->hrac = i%2 + 1;

        do {
            if(dat->hrac == 1) {
                printf("Zadaj cislo stvorca: \n");
                scanf("%d", &dat->tah);
                send(dat->sockfd,&dat->tah,sizeof(dat->tah),0);

            } else {
                printf("Cakaj kym protihrac vyberie policko.\n");
                n = recv(dat->sockfd,&dat->tah,200,0);
                if(n<1) {
                    printf("Nepodarilo sa ziskat tah");
                }
                printf("Protihrac vybral policko %d\n",dat->tah);
            }

            dat->riadok = --dat->tah/3;
            dat->stlpec = dat->tah%3;
        } while(dat->tah<0 || dat->tah > 9 || hraciaPlocha[dat->riadok][dat->stlpec]>'9');

        hraciaPlocha[dat->riadok][dat->stlpec] = (dat->hrac == 1) ? 'X' : 'O';

        if((hraciaPlocha[0][0] == hraciaPlocha[1][1] && hraciaPlocha[0][0] == hraciaPlocha[2][2]) ||
           (hraciaPlocha[0][2] == hraciaPlocha[1][1] && hraciaPlocha[0][2] == hraciaPlocha[2][0]))
            dat->vitaz = dat->hrac;
        else
            for(int j = 0; j <= 2; j++)
                if((hraciaPlocha[j][0] == hraciaPlocha[j][1] && hraciaPlocha[j][0] == hraciaPlocha[j][2])||
                   (hraciaPlocha[0][j] == hraciaPlocha[1][j] && hraciaPlocha[0][j] == hraciaPlocha[2][j]))
                    dat->vitaz = dat->hrac;
    }

    printf("\n\n");
    for (int o = 0; o < 3; o++) {
        printf(" %c | %c | %c\n", hraciaPlocha[o][0], hraciaPlocha[o][1], hraciaPlocha[o][2]);
        printf("---+---+---\n");
    }

    if(dat->vitaz == 0) {
        printf("Remiza\n");
    } else if (dat->vitaz == 1) {
        printf("Vyhral si\n");
    } else {
        printf("Prehral si\n");
    }

}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer;

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);



    pthread_cond_t hraPrebieha;
    pthread_cond_t mozeSaHrat;
    pthread_mutex_t mutex;

    pthread_t id[5];
    int i = 0;
    while(1) {
        cli_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            return 3;
        }

        dataHra data = {&mutex,&hraPrebieha,&mozeSaHrat,0,
                        0,0,0,0,newsockfd};


        if (pthread_create(&id[i++],NULL,&vlaknoHry,&data) != 0) {
            perror("Nepodarilo sa vytvorit vlakno");
            return 1;
        }
        while(i < 5) {
            pthread_join(id[i++],NULL);
        }


    }

    close(newsockfd);
    close(sockfd);

    return 0;
}
