// Клиент сетевой службы daytime (RFC 867), протокол UDP, порт 13.
// Использование: ./daytime-client [адрес_сервера] [порт]
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    const char *server_ip = (argc > 1) ? argv[1] : "172.16.40.1";
    int port = (argc > 2) ? std::atoi(argv[2]) : 13;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // Таймаут ожидания ответа - 5 секунд
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    struct sockaddr_in serv;
    std::memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &serv.sin_addr) != 1) {
        std::cerr << "Неверный адрес сервера: " << server_ip << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Подключаюсь к серверу кафедры: " << server_ip << ":" << port << std::endl;

    // Служба daytime по UDP отвечает на любую датаграмму (в т.ч. пустую)
    const char *req = "\n";
    if (sendto(sock, req, std::strlen(req), 0,
               (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("sendto");
        close(sock);
        return 1;
    }
    std::cout << "Запрос отправлен, жду ответ..." << std::endl;

    char buf[1024];
    ssize_t n = recvfrom(sock, buf, sizeof(buf) - 1, 0, NULL, NULL);
    if (n < 0) {
        perror("recvfrom");
        close(sock);
        return 1;
    }
    buf[n] = '\0';

    std::cout << "\nОтвет от сервера кафедры:" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << buf << std::endl;

    close(sock);
    return 0;
}
