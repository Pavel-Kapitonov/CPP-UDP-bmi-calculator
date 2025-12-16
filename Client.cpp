// Pavel-Kapitonov (2025)
// A simple example of calculating body BMI (UDP_SOCKET)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;
#define SERVER_PORT 666
#define SERVERADDR "127.0.0.1" // IP-адрес сервера

struct Patient {
    char surname[50];
    char name[50];
    int height;
    float weight;
};

int main()
{
    setlocale(LC_ALL, "Ru");
    char buff[10 * 1014]; // 2^10
    cout << "UDP Demo Client\nType quit to quit \n";

    if (WSAStartup(0x202, (WSADATA*)&buff))
    {
        cout << "WSASTARTUP ERROR: " << WSAGetLastError() << "\n";
        return -1;
    }

    SOCKET my_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (my_sock == INVALID_SOCKET) {
        cout << "SOCKET() ERROR: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    hostent* hst;
    sockaddr_in Daddr; // структура для адреса и порта сервера
    Daddr.sin_family = AF_INET;
    Daddr.sin_port = htons(SERVER_PORT);

    if (inet_addr(SERVERADDR))
        Daddr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else
        if (hst = gethostbyname(SERVERADDR))
            memcpy(&Daddr.sin_addr.s_addr, hst->h_addr, 4);
        else { // если нулевой указатель
            cout << "Unknown Host: " << WSAGetLastError() << "\n";
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    while (1) {
        cout << "C => S:" << "\n" << "\n";

        Patient info;
        cout << "Введите фамилию: ";
        cin >> info.surname;
        cout << "Введите имя: ";
        cin >> info.name;
        cout << "Введите ваш рост (в см): ";
        cin >> info.height;
        cout << "Введите ваш вес: ";
        cin >> info.weight;
        // Передача сообщений на сервер
        sendto(my_sock, (char*)&info, sizeof(info), 0, (sockaddr*)&Daddr, sizeof(Daddr));

        // Прием сообщения с сервера
        sockaddr_in SRaddr;
        int SRaddr_size = sizeof(SRaddr);
        int n = recvfrom(my_sock, buff, sizeof(buff) - 1, 0, (sockaddr*)&SRaddr, &SRaddr_size);
        if (n == SOCKET_ERROR) {
            cout << "RECVFROM() ERROR:" << WSAGetLastError() << "\n";
            closesocket(my_sock);
            WSACleanup();  return -1;
        }
        buff[n] = '\0';
        cout << "S => C:" << buff << "\n";
    }

    closesocket(my_sock);
    WSACleanup();     return 0;
}

