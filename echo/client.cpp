// Клиент сетевой службы echo (RFC 862), протокол TCP, порт 7.
// Использование: ./echo-client [адрес_сервера] [порт]
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    const char *server_ip = (argc > 1) ? argv[1] : "172.16.40.1";
    int port = (argc > 2) ? std::atoi(argv[2]) : 7;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv;
    std::memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &serv.sin_addr) != 1) {
        std::cerr << "Неверный адрес сервера: " << server_ip << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "Подключаюсь к серверу echo: " << server_ip << ":" << port << std::endl;
    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }
    std::cout << "Подключение успешно!" << std::endl;
    std::cout << "Введите текст для отправки (или 'exit' для выхода):" << std::endl;

    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line) || line == "exit")
            break;
        if (line.empty())
            continue;

        // Отправка всей строки
        size_t sent = 0;
        while (sent < line.size()) {
            ssize_t n = send(sock, line.data() + sent, line.size() - sent, 0);
            if (n < 0) {
                perror("send");
                close(sock);
                return 1;
            }
            sent += n;
        }
        std::cout << "Отправлено: " << line << " (" << line.size() << " байт)" << std::endl;

        // Приём ответа: ждём столько же байт, сколько отправили
        std::string reply;
        char buf[1024];
        while (reply.size() < line.size()) {
            ssize_t n = recv(sock, buf, sizeof(buf), 0);
            if (n < 0) {
                perror("recv");
                close(sock);
                return 1;
            }
            if (n == 0) {
                std::cout << "Сервер закрыл соединение" << std::endl;
                close(sock);
                return 0;
            }
            reply.append(buf, n);
        }
        std::cout << "Получено: " << reply << " (" << reply.size() << " байт)" << std::endl;
    }

    close(sock);
    std::cout << "Соединение закрыто" << std::endl;
    return 0;
}
