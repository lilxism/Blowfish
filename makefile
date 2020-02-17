socket.o: chual_chuaz_qianx_socket.cpp chual_chuaz_qianx_socket.h 
	g++ -c chual_chuaz_qianx_socket.cpp -std=c++11 -o socket.o
	
main.o: chual_chuaz_qianx_main.cpp chual_chuaz_qianx_socket.h chual_chuaz_qianx_blow.h
	g++ -c chual_chuaz_qianx_main.cpp -std=c++11 -o main.o

Blowfish.o: chual_chuaz_qianx_Blowfish.cpp chual_chuaz_qianx_Blowfish.h
	g++ -c chual_chuaz_qianx_Blowfish.cpp -std=c++11 -o Blowfish.o
	
blow.o: chual_chuaz_qianx_blow.cpp chual_chuaz_qianx_blow.h chual_chuaz_qianx_Blowfish.h
	g++ -c chual_chuaz_qianx_blow.cpp -std=c++11 -o blow.o
	
main: main.o socket.o blow.o Blowfish.o
	g++ -o main main.o socket.o blow.o Blowfish.o -std=c++11

clean: 
	$(RM) main *.o *~
