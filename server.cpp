#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> //read write functions
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>


int main(int argc, char *argv[])
{

   int connectlist[5]; /*connected sockets array*/;
   fd_set socks; //socket fd select()-i hamar;
   int highsock;
 
   int readsocks; //read-i patrast socketneri tivy
   int socket_desc, client_sock, c;

   struct sockaddr_in server, client;
   socket_desc = socket(AF_INET, SOCK_STREAM, 0);
   if(socket_desc == -1)
   {
      printf("creating socket failed!\n");
   }
  

   memset((char*)&server, 0, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr =  inet_addr("0.0.0.0"); 
   server.sin_port = htons(2000);
   
   if(bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
   {
       perror("bind failed error");
   }
   
   c = sizeof(struct sockaddr_in); 
   
   listen(socket_desc, 5);
   
   highsock = socket_desc;
   memset((char*)&connectlist, 0, sizeof(connectlist));

   while(1)
   {
       FD_ZERO(&socks); //socks-y NULL a sarqum
       FD_SET(socket_desc, &socks); //avelacnum a fd sockety  fd_set - um, select()-ov ashxatelu hamar
       for(int i = 0; i < 5; i++)
       {
          if(connectlist[i]!=0)
          {
             FD_SET(connectlist[i], &socks);
             if(connectlist[i] > highsock)
                 highsock = connectlist[i];
          }
       } //bolor fd-nern a avelacnum fd_set-in
       

      //mec hamarov file descriptory + 1
       readsocks = select(highsock+1, &socks, (fd_set*)0, (fd_set*)0, NULL);
       if(readsocks < 0)
       {
          perror("select exception");
          exit(EXIT_FAILURE);
       }

       if(readsocks == 0)//kpacneri tivy 0a
       {
          printf("nothing to read\n");
       }
       else
       {
          //connection
          if(FD_ISSET(socket_desc, &socks)) //connection accepted //fd-n(socket_desc) set arel a te che, socks-i andam a te che
          {
              client_sock = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
              if(client_sock < 0)
              {
                  perror("accept failed");
                  break;
              }
              
              
              for(int i = 0; i < 5 && client_sock != -1; i++)
              {
                 if(connectlist[i] == 0)
                 {

                     connectlist[i]=client_sock;
                     char temp[] = "server: i initialize the talk!\n";
                     write(client_sock, temp, sizeof(temp));
                     client_sock = -1;
                 }
              }
              if(client_sock != -1) 
              {
                 printf("no room available for client connection\n");
              }
          }

          //data
          for(int i = 0; i < 5; i++)
          {
             if(FD_ISSET(connectlist[i], &socks))
             { 
               char buffer[256];
               int numch = 0;      
               if((numch = recv(connectlist[i], buffer, 256, 0))<0)
               {
                  printf("connection lost: socket id= %d, %d\n", connectlist[i], i);
                  close(connectlist[i]);
                  connectlist[i]=0;
               }
               else
               {
                  buffer[numch]='\0';
                  printf("received: %s\n", buffer);
                  write(connectlist[i], buffer, strlen(buffer)); 
               }
             }
          }
        }
   }

}
