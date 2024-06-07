#include <iostream>
#include<string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define buffSize 200
#include "processes.cpp"  

class client{
	int sockfd;
	struct sockaddr_in addr;

	public:
	
 	client(){
	   sockfd = socket(AF_INET, SOCK_STREAM, 0);
	   if(sockfd == -1){
	   	perror("socket");
	   	exit(1);
	   }
   
	   addr.sin_family = AF_INET;
	   addr.sin_port = htons(8000);
	   addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	   int flags = fcntl(sockfd, F_GETFL, 0);
	   	if (flags == -1) {
		    perror("fcntl F_GETFL");
		    exit(1);
		}

		if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		    perror("fcntl F_SETFL O_NONBLOCK");
		    exit(1);
		}
	
	   // Connect to the server
	   while (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) ;
	
	   std::cout << "Connected to server" << std::endl;
	}
	
	void sendMsg(char* buff){
		if(send(sockfd,buff,strlen(buff),0) == -1){
			perror("send");
			close(sockfd);
			exit(1);
		}
	}
	
	void recvMsg(char* buff){
		ssize_t bytesRead = recv(sockfd,buff,buffSize-1,0);
		if(bytesRead == -1){
			strcpy(buff,"Message doesn't send by the server !");
		}
		else if(bytesRead == 0) strcpy(buff,"Server has been closed !");
 		else
		buff[bytesRead] = '\0';
		
	}
	
	~client(){
		close(sockfd);
	}
	
};

class wrapper{

	public:
	client *obj;
	windowWrapper* w;
	wrapper(){
		obj = nullptr;
	}
	wrapper(client* obj,windowWrapper* w){
		this->obj = obj;
		this->w = w;
	}

};


void Button :: update(wrapper* wrap,bool toBeSend) {
   	sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(wrap->w->getWindow()));
        bool isHovered = shape.getGlobalBounds().contains(mousePos);
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        if (isHovered && isMousePressed && !isClicked) {
            
            shape.setFillColor(clickColor);

	    if(toBeSend)
		wrap->obj->sendMsg(wrap->w->getText());
            else{
            
            	std :: string temp = "Receiving Message ....";
            	wrap->w->setText(temp);
            	wrap->w->innerRender();
            	sleep(1);
            	
            	char tempBuff[buffSize];
            	wrap->obj->recvMsg(tempBuff);
            	wrap->w->setText(tempBuff);
            }

            isClicked = true;
            
        } else if (isHovered && !isClicked) {
            shape.setFillColor(hoverColor);
        } else {
            shape.setFillColor(idleColor);
        }

        if (!isMousePressed) {
            isClicked = false;
        }
    }
  
    
    void windowWrapper ::  render(wrapper* w){

		while (window.isOpen())
    		{
        	
	        button.update(w,true);
	        recvButton.update(w,false);
        	this->innerRender();
    		
    		}
	
	}


int main(int count,char* argc[]){

	if(count < 2){
		perror("Please provide client number !");
		exit(1);
	}

	client c;
	int val = atoi(argc[1]);
	
	windowWrapper w(std :: string("Client ") + char('0'+val));
	
	wrapper wrap(&c,&w);
	
	w.render(&wrap);

   return 0;

}







