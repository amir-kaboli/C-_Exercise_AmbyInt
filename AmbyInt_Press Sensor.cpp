
#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <string>
#pragma comment (lib,"ws2_32.lib")

using namespace std;



			
	class Timer     //*******************************************Timer*********************************
			{
			public:
				void start()
				{

					m_StartTime = std::chrono::system_clock::now();
					m_bRunning = true;
				}

				void stop()
				{

					m_EndTime = std::chrono::system_clock::now();
					m_bRunning = false;
				}

				double elapsedMilliseconds()
				{
					std::chrono::time_point<std::chrono::system_clock> endTime;

					if (m_bRunning)
					{
						endTime = std::chrono::system_clock::now();
					}
					else
					{
						endTime = m_EndTime;
					}

					return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
				}

			private:
				std::chrono::time_point<std::chrono::system_clock> m_StartTime;
				std::chrono::time_point<std::chrono::system_clock> m_EndTime;
				bool                                               m_bRunning = false;


			};




int main()//************************************************TCP Server******************************
{
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOK = WSAStartup(ver, &wsData);
	char buf[8];
	
	if (wsOK != 0)
				{
					cerr << "can't initialize winsock!quitting" << endl;
					return 0;

				}
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
				if (listening == INVALID_SOCKET)
				{
					cerr << "can't create a socket! Quitting" << endl;
					return 0;

				}

	sockaddr_in hint;
				hint.sin_family = AF_INET;
				hint.sin_port = htons(54000);
				hint.sin_addr.S_un.S_addr = INADDR_ANY;
				bind(listening, (sockaddr*)&hint, sizeof(hint));
				listen(listening, SOMAXCONN);//tell winsock the socket is for listening
						 
	sockaddr_in client;//**********************wait for connection***************************
				int clientsize = sizeof(client);
				SOCKET clientsocket = accept(listening, (sockaddr*)&client, &clientsize);
				char host[NI_MAXHOST];
				char service[NI_MAXSERV];
					
				ZeroMemory(host, NI_MAXHOST);
				ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
				
					cout << host << "    connected on port:    " << service << endl;
				else
					{
						inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
						cout << host << "    connected on port" << ntohs(client.sin_port) << endl;
					}
	
				
				
				while (true)

				{
					ZeroMemory(buf, 8);
				
										//********************************************reading from file**********************************

					Timer timer;

					float Xline = 0;
					float sensor = 0;
					int Nline = 0;
					int i = 0;
					float elapsed = 0;
					char CR[4] = "\n\r";
					string line;
					ifstream inFile;

					inFile.open("c:\data.txt");   // please make sure data file is copied C:\ drive

					if (!inFile)
							{
								cout << "Unable to open file";
								exit(1); // terminate with error
							}
					timer.start();
					while (getline(inFile, line))

					{

						if (timer.elapsedMilliseconds() < 150)   //150 msec 
						{
							stringstream geek(line);   //converting string to integer
							geek >> Nline;
							Xline = Nline + Xline;
							++i;
						}
						else
						
							
							{

								Xline = Xline / i; //calculating the average of every 150 msec data reading from sensor and printing the result
								snprintf(buf, sizeof(buf), "%.2f ", Xline);  //converting sensor data to string befor sending
								send(clientsocket, buf, 8, 0);  //sending sensor data via TCP 
								send(clientsocket, CR, 4, 0);  //Sending carriage return command
							
														
								timer.stop();
								timer.start(); //reset timer
								Xline = 0;
								Nline = 0;
								i = 0;
							}
								
					}


					inFile.close();
					return 0;



					//close the socket
					closesocket(clientsocket);
				}
	//Cleanup the socket
	WSACleanup();



}
