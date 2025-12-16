// Pavel-Kapitonov (2025)
// A simple example of calculating body BMI (UDP_SOCKET)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable: 4996)

using namespace std;
#define SERVER_PORT 666    // порт сервера

struct Patient {
    char surname[50];
    char name[50];
    int height;
    float weight;
};

int main() {
    setlocale(LC_ALL, "Ru");
    char buff[1024];
    cout << "UDP DEMO ECHO-SERVER\n" << "\n";

    if (WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        cout << "WSAStartup error: " << WSAGetLastError();
        return -1;
    }

    SOCKET Lsock;
    Lsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (Lsock == INVALID_SOCKET) {
        cout << "SOCKET() ERROR: " << WSAGetLastError();
        WSACleanup();
        return -1;
    }

    sockaddr_in Laddr;
    Laddr.sin_family = AF_INET;
    Laddr.sin_addr.s_addr = 0;   // или 0 (любой IP адрес)
    Laddr.sin_port = htons(SERVER_PORT);
    if (bind(Lsock, (sockaddr*)&Laddr, sizeof(Laddr)))
    {
        cout << "BIND ERROR:" << WSAGetLastError();
        closesocket(Lsock);
        WSACleanup();
        return -1;
    }

    while (1) {
        sockaddr_in Caddr;
        int Caddr_size = sizeof(Caddr);

        Patient inCominginfo;

        int bsize = recvfrom(Lsock, (char*)&inCominginfo, sizeof(inCominginfo), 0, (sockaddr*)&Caddr, &Caddr_size);

        if (bsize == SOCKET_ERROR)
            cout << "RECVFROM() ERROR:" << WSAGetLastError();
        if (bsize < sizeof(Patient)) {
            cout << "Получен неполный пакет" << endl;
            continue;
        }

        double heightM = inCominginfo.height / 100.0;
        double imt = inCominginfo.weight / (heightM * heightM);

        hostent* hst;
        hst = gethostbyaddr((char*)&Caddr.sin_addr, 4, AF_INET); // пытаемся найти имя хоста клиента (ip adress)
        cout << "NEW DATAGRAM!\n" << ((hst) ? hst->h_name : "Unknown host") << "/n" <<
            inet_ntoa(Caddr.sin_addr) << "/n" << ntohs(Caddr.sin_port) << '\n' << '\n';

        cout << "Данные: " << "Имя: " << inCominginfo.surname << " Рост: " << inCominginfo.height << " Вес: " << inCominginfo.weight << " ИМТ: " << imt << endl;

        string result;
        if (imt < 18.5) { result = "Недостаточная масса тела"; }
        else if (imt >= 18.5 && imt <= 24.9) { result = "Нормальная масса тела"; }
        else { result = "Избыточная масса тела"; }

        sendto(Lsock, result.c_str(), result.size(), 0, (sockaddr*)&Caddr, sizeof(Caddr));
    }
    return 0;
}