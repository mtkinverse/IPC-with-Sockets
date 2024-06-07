#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "processes.cpp" 
#include <fcntl.h>
#include <errno.h>

#include <X11/Xlib.h>

#define buffSize 200

class server{

	int sockfd,*clientSockFd,numClients;
	struct sockaddr_in addr;
		
	public:
	
	server(){
		sockfd = numClients = 0;
		clientSockFd = nullptr;
	}
	server(const char* ipAdd,int port,int numClients){		
		this->numClients = numClients;
		if(this->init(ipAdd,port,numClients)) std :: cout << "Server established successfully !\n";
		else{
			perror("Something went wrong !\n");
			this->closeSockets();
			exit(1);
		}
	}
	
	bool init(const char* ipAdd,int port,int numClients){
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }
    
    // Configure server address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ipAdd);

    int bind_result = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    // Check for errors
    if (bind_result == -1) {
        perror("bind");
        return false;
    }
    

    int listen_result = listen(sockfd, numClients); // numClients is the maximum length of the queue of pending connections
    // Check for errors
    if (listen_result == -1) {
        perror("listen");
        return false;
    }
    
    clientSockFd = new int[numClients]{0};

	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
	    perror("fcntl F_GETFL");
	    return false;
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
	    perror("fcntl F_SETFL O_NONBLOCK");
	    return false;
	}  
	
    
    	return true;
    }

    bool inBounds(int val) const{
    	if(val < 0 || val >= this->numClients) return false;
    	else return true;
    }

    void sendMsg(int i,char* buffer){
    	ssize_t bytesSend = send(clientSockFd[i], (void*)buffer, strlen(buffer), 0);
    	if(bytesSend == -1){
    		perror("Error while sending the data\n");
    		this->closeSockets();
    		exit(1);
    	}

    }
    
    void recvMsg(int i,char* buffer){
    	ssize_t bytesRecv = recv(clientSockFd[i], buffer,buffSize-1,0);

    	if(bytesRecv == -1){
    		strcpy(buffer,"Client has not send the message !");
    	}
    	else if(bytesRecv == 0){
    		strcpy(buffer,"Client has been exited !");
    	}
    	else buffer[bytesRecv] = '\0';

    }

    int& getClientSockFd(int i){
    	if(inBounds(i)) return clientSockFd[i];
    	else{
    		perror("Index out of range !");
    		this->closeSockets();
    		exit(0);
    	}
    }
    
    int getSockFd(){
    	return this->sockfd;
    }
    
    void setClientSockFd(int i,int val){
    	if(inBounds(i)) this->clientSockFd[i] = val;
    	else{
    		perror("Index not in range !");
    		this->closeSockets();
    		exit(1);
    	}
    }

    void setSockFd(int val){ this->sockfd = val; }
    
    void handleClient(int i,char* str,bool toBeSend){
    	
    	if(!inBounds(i)){
    		perror("Index not in range !");
    		this->closeSockets();
    		exit(1);
    	}
    	
    	if(toBeSend){
	    	this->sendMsg(i,str);
    	}else{
    		this->recvMsg(i,str);
    	}
    }
    
    void closeSockets(){
    	close(sockfd);
    	if(clientSockFd){
    		for(int i=0;i<numClients;i++) close(clientSockFd[i]);
    	}
    	
    	delete[] clientSockFd;
    }

    ~server(){
    	closeSockets();
    }
};

class wrapper{
	public:
	server *obj;
	windowWrapper* w;
	int clientNo;
	wrapper(){
		obj = nullptr;
		w = nullptr;
		clientNo = 0;
	}
	wrapper(server* obj,windowWrapper* w,int num){
		this->obj = obj;
		this->w = w;
		this->clientNo = num;
	}
};

void* show(void*);

void* acceptConnections(void* arg){
    	
    	wrapper* w = static_cast <wrapper*>(arg);
    	int i = w->clientNo;
    	class server* s = w->obj;
    	if(s->inBounds(i)){
    		int tempSocket = -1;
    		
    		do{
    			tempSocket = accept(s->getSockFd(), NULL, NULL);
    		}while(tempSocket == -1);

		int flags = fcntl(tempSocket, F_GETFL, 0);
		if (flags == -1) {
		    perror("fcntl F_GETFL");
		    exit(1);
		}

		if (fcntl(tempSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
		    perror("fcntl F_SETFL O_NONBLOCK");
		    exit(1);
		}
    		
    		s->setClientSockFd(i,tempSocket);
    		
    		void* ptr = show(arg);
    	}
    	else{
    		perror("Given index not in range !");
        	std :: cout << "value of i : " << i << std :: endl;
    		exit(1);
    	}
    	
    	return nullptr;
    	
    }
    
    void* show(void* args){
    	wrapper* wrap = static_cast<wrapper*>(args);
    	char val = '0' + wrap->clientNo;
    	std :: string name = "Server Process ";
    	name += val; 
    	
    	wrap->w = new windowWrapper(name);
    	
    	wrap->w->render(wrap);
    
    return nullptr;
    
    }
    
    void Button :: update(wrapper* wrap,bool toBeSend) {
   	sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(wrap->w->getWindow()));
        bool isHovered = shape.getGlobalBounds().contains(mousePos);
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

        if (isHovered && isMousePressed && !isClicked) {
            
            shape.setFillColor(clickColor);

    	    if(!toBeSend){
            	std :: string temp = "Receiving Message ....";

            	wrap->w->setText(temp);
            	wrap->w->innerRender();
            	sleep(1);
            	
            	char str[buffSize];
            	wrap->obj->handleClient(wrap->clientNo,str,toBeSend);
            	wrap->w->setText(str);
            }
            else
            wrap->obj->handleClient(wrap->clientNo,wrap->w->getText(),toBeSend);
        
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
    
        
    void windowWrapper ::  render(class wrapper* w){
		
		while (window.isOpen())
    		{
    		
	        button.update(w,true);
        	recvButton.update(w,false);
        	
        	this->innerRender();
        
        	}
	
	}


int main() {

	std :: cout << "Enter the number of clients : ";
	int n;
	std :: cin >> n;
	server s("127.0.0.1",8000,n);
	
	// This is the library function used by SFML for multithreading purpose ; therefore, calling it
	XInitThreads();
	
	pthread_t* tid = new pthread_t[n];	
	wrapper* wrap = new wrapper[n];
	windowWrapper** arrW = new windowWrapper*[n];	
	
	for(int i=0;i<n;i++){
		wrap[i] = wrapper(&s,arrW[i],i);
		pthread_create(tid + i,NULL,acceptConnections,static_cast<void*>(wrap + i));
	}
	  
	for(int i=0;i<n;i++)
		pthread_join(tid[i],NULL);
		
	delete[] tid;
	delete[] wrap;

	for(int i=0;i<n;i++) delete arrW[i];
	delete[] arrW;

    return 0;

}







