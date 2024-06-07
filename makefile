server: processes.o socket.o
	g++ -o finale processes.o socket.o -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lX11

socket.o: processes.o
	g++ -c socket.cpp -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lX11

processes.o:
	g++ -c processes.cpp -lsfml-graphics -lsfml-window -lsfml-system

client: processes.o client.o
	g++ -o client processes.o client.o -lsfml-graphics -lsfml-window -lsfml-system -lpthread

client.o: processes.o
	g++ -c client1.cpp -o client.o 

getPortInfo:
	lsof -i :8000

clear:
	rm *.o
