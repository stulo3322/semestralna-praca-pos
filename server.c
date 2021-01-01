
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int hrajHru(int sock, char* buffer) {
    int n = 0;
    int sockfd = sock;
    int hrac = 0;
    int vitaz = 0;
    int riadok = 0;
    int stlpec = 0;
    int tah = 0;
    char hraciaPlocha[3][3] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'}};

    for (int i = 0; i < 9 && vitaz == 0; i++) {

        printf("\n\n");
        printf(" %c | %c | %c\n", hraciaPlocha[0][0], hraciaPlocha[0][1], hraciaPlocha[0][2]);
        printf("---+---+---\n");
        printf(" %c | %c | %c\n", hraciaPlocha[1][0], hraciaPlocha[1][1], hraciaPlocha[1][2]);
        printf("---+---+---\n");
        printf(" %c | %c | %c\n", hraciaPlocha[2][0], hraciaPlocha[2][1], hraciaPlocha[2][2]);

        hrac = i%2 + 1;

        do {
            if(hrac == 1) {
                printf("Zadaj cislo stvorca: \n");
                scanf("%d", &tah);
                send(sockfd,&tah,sizeof(tah),0);

            } else {
                printf("Cakaj kym protihrac vyberie policko.\n");
                n = recv(sockfd,&tah,200,0);
                printf("Protihrac vybral policko %d\n",tah);
            }

            riadok = --tah/3;
            stlpec = tah%3;
        } while(tah<0 || tah > 9 || hraciaPlocha[riadok][stlpec]>'9');

        hraciaPlocha[riadok][stlpec] = (hrac == 1) ? 'X' : 'O';

        if((hraciaPlocha[0][0] == hraciaPlocha[1][1] && hraciaPlocha[0][0] == hraciaPlocha[2][2]) ||
           (hraciaPlocha[0][2] == hraciaPlocha[1][1] && hraciaPlocha[0][2] == hraciaPlocha[2][0]))
            vitaz = hrac;
        else
            for(int j = 0; j <= 2; j++)
                if((hraciaPlocha[j][0] == hraciaPlocha[j][1] && hraciaPlocha[j][0] == hraciaPlocha[j][2])||
                   (hraciaPlocha[0][j] == hraciaPlocha[1][j] && hraciaPlocha[0][j] == hraciaPlocha[2][j]))
                    vitaz = hrac;
    }

    printf("\n\n");
    printf(" %c | %c | %c\n", hraciaPlocha[0][0], hraciaPlocha[0][1], hraciaPlocha[0][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", hraciaPlocha[1][0], hraciaPlocha[1][1], hraciaPlocha[1][2]);
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", hraciaPlocha[2][0], hraciaPlocha[2][1], hraciaPlocha[2][2]);

    if(vitaz == 0) {
        printf("Remiza\n");
    } else if (vitaz == 1) {
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
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }

    do {
        hrajHru(newsockfd,&buffer);
        /* bzero(buffer,256);
         n = read(newsockfd, buffer, 255);
         if (n < 0)
         {
             perror("Error reading from socket");
             return 4;
         }
         if(buffer[0] != 'x') {
             printf("Here is the message: %s\n", buffer);

             const char* msg = "I got your message";
             n = write(newsockfd, msg, strlen(msg)+1);
             if (n < 0)
             {
                 perror("Error writing to socket");
                 return 5;
             }
        } else {
            printf("Server is closing ");

            const char* msge = "Dovidenia";
            n = write(newsockfd, msge, strlen(msge)+1);
            if (n < 0)
            {
                perror("Error writing to socket");
                return 5;
            }
        }*/

    } while(buffer != 'x');



    close(newsockfd);
    close(sockfd);

    return 0;
}
