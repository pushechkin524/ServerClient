#define WIN32_LEAN_AND_MEAN // Определяет макрос для исключения редко используемых заголовков Windows из Windows.h, что уменьшает время компиляции и размер скомпилированного файла.

#include <iostream>  // Подключение заголовка для работы с вводом-выводом (например, std::cout).
#include <Windows.h> // Подключение основного заголовочного файла Windows API для работы с функциями Windows.
#include <WinSock2.h> // Подключение заголовка для работы с сетевыми функциями Windows Sockets 2 (WinSock2).
#include <WS2tcpip.h> // Подключение дополнительного заголовка для работы с функциями TCP/IP в Windows Sockets.

using namespace std; // Использование пространства имен std, чтобы можно было использовать объекты и функции стандартной библиотеки C++ без префикса std::.

int main()
{
    WSADATA wsaData; // Объявление структуры WSADATA, которая будет содержать информацию о реализации WinSock.
    ADDRINFO hints; // Объявление структуры ADDRINFO для хранения параметров, используемых функцией getaddrinfo.
    ADDRINFO* addrResult; // Объявление указателя на структуру ADDRINFO, которая будет хранить результат функции getaddrinfo.
    SOCKET ClientSocket = INVALID_SOCKET; // Инициализация переменной ClientSocket значением INVALID_SOCKET, что означает, что сокет пока не создан.
    SOCKET ListenSocket = INVALID_SOCKET; // Инициализация переменной ListenSocket значением INVALID_SOCKET, что означает, что сокет пока не создан.

    const char* sendBuffer2 = "раб мпт";
    const char* sendBuffer = "Hello from Client"; // Объявление и инициализация строки, которая будет отправлена клиенту.
    char recvBuffer[512]; // Объявление массива для хранения данных, полученных от клиента.

    // Функция WSAStartup возвращает 0 при успешной инициализации.
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed" << endl; // Вывод сообщения об ошибке, если инициализация не удалась.
        return 1; // Завершение программы с кодом ошибки.
    }

    ZeroMemory(&hints, sizeof(hints)); // Заполнение структуры hints нулями.
    hints.ai_family = AF_INET; // Установка семейства адресов на IPv4.
    hints.ai_socktype = SOCK_STREAM; // Установка типа сокета на потоковый (TCP).
    hints.ai_protocol = IPPROTO_TCP; // Установка протокола на TCP.
    hints.ai_flags = AI_PASSIVE; // Установка флага для использования адреса сервера.

    // Получение адресной информации для создания и связывания сокета.
    result = getaddrinfo(NULL, "788", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed" << endl; // Вывод сообщения об ошибке, если функция getaddrinfo не удалась.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Создание серверного сокета с параметрами, полученными из addrResult.
    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket creation failed" << endl; // Вывод сообщения об ошибке, если создание сокета не удалось.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Привязка сокета к адресу, указанному в addrResult.
    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "binding failed" << endl; // Вывод сообщения об ошибке, если привязка сокета не удалась.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        closesocket(ListenSocket); // Закрытие сокета.
        WSACleanup(); // Очистка ресурсов, использованных WinSock.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Перевод сокета в режим прослушивания, чтобы он мог принимать входящие соединения.
    result = listen(ListenSocket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        cout << "listening failed" << endl; // Вывод сообщения об ошибке, если сокет не удалось перевести в режим прослушивания.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        closesocket(ListenSocket); // Закрытие сокета.
        WSACleanup(); // Очистка ресурсов, использованных WinSock.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Ожидание подключения клиента.
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accepting failed" << endl; // Вывод сообщения об ошибке, если подключение клиента не удалось.
        closesocket(ListenSocket); // Закрытие сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Очистка ресурсов, использованных WinSock.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Цикл для получения и отправки данных клиенту.
    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера получения.
        result = recv(ClientSocket, recvBuffer, 512, 0); // Получение данных от клиента.
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl; // Вывод количества полученных байт.
            cout << "Received message: " << recvBuffer << endl; // Вывод полученного сообщения.

            // Отправка данных клиенту.
            result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "send failed" << endl; // Вывод сообщения об ошибке, если отправка данных не удалась.
                closesocket(ClientSocket); // Закрытие клиентского сокета.
                closesocket(ListenSocket); // Закрытие серверного сокета.
                freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
                WSACleanup(); // Очистка ресурсов, использованных WinSock.
                return 1; // Завершение программы с кодом ошибки.
            }
            
            result = send(ClientSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
            if (result == SOCKET_ERROR) {
                cout << "send failed" << endl; // Вывод сообщения об ошибке, если отправка данных не удалась.
                closesocket(ClientSocket); // Закрытие клиентского сокета.
                closesocket(ListenSocket); // Закрытие серверного сокета.
                freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
                WSACleanup(); // Очистка ресурсов, использованных WinSock.
                return 1; // Завершение программы с кодом ошибки.
            }
        }
        else if (result == 0) {
            cout << "Connection closed" << endl; // Вывод сообщения, если соединение было закрыто.
        }
        else {
            cout << "recv failed" << endl; // Вывод сообщения об ошибке, если получение данных не удалось.
        }
    } while (result > 0);

    // Завершение соединения для отправки данных.
    result = shutdown(ClientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "shutdown failed" << endl; // Вывод сообщения об ошибке, если завершение соединения не удалось.
        closesocket(ClientSocket); // Закрытие клиентского сокета.
        closesocket(ListenSocket); // Закрытие серверного сокета.
        freeaddrinfo(addrResult); // Освобождение памяти, выделенной для addrResult.
        WSACleanup(); // Очистка ресурсов, использованных WinSock.
        return 1; // Завершение программы с кодом ошибки.
    }

    // Закрытие клиентского сокета.
    closesocket(ClientSocket);
    // Закрытие серверного сокета.
    closesocket(ListenSocket);
    // Освобождение памяти, выделенной для addrResult.
    freeaddrinfo(addrResult);
    // Очистка ресурсов, использованных WinSock.
    WSACleanup();

    return 0; // Успешное завершение программы.
}
