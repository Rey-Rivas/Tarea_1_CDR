#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

class JuegoCliente {
public:
    JuegoCliente(const char* server_ip, int port);
    void start();
private:
    int sock = 0;
    struct sockaddr_in serv_addr;
    void mostrarTablero(char board[6][7]);
};

JuegoCliente::JuegoCliente(const char* server_ip, int port) {
    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convertir la dirección IP a binario
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void JuegoCliente::start() {
    // Inicializar el tablero
    char board[6][7] = {};
    // Llenar el tablero con '-'
    memset(board, '-', sizeof(board));

    // Recibir si el cliente inicia
    bool client_turn;
    recv(sock, &client_turn, sizeof(client_turn), 0);

    // Juego
    while (true) {
        // Turno del cliente
        if (client_turn) {
            int column;
            std::cout << "Turno Jugador. Ingresa una columna (1-7): ";
            std::cin >> column;
            column -= 1; // Ajuste para que sea de 0 a 6
            send(sock, &column, sizeof(column), 0);
        }


        recv(sock, board, sizeof(board), 0);
        mostrarTablero(board);

        // Verificar si hay un ganador
        char status;
        recv(sock, &status, 1, 0);
        if (status == 'G') {
            std::cout << (client_turn ? "¡Gana el jugador!" : "¡Gana el servidor!") << std::endl;
            break;
        } else if (status == 'D') {
            std::cout << "Empate." << std::endl;
            break;
        }

        client_turn = !client_turn;
    }

    close(sock);
}

void JuegoCliente::mostrarTablero(char board[6][7]) {
    //Muestra el tablero en la consola
    std::cout << "TABLERO" << std::endl;
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 7; ++j) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-------------" << std::endl;
    std::cout << "1 2 3 4 5 6 7" << std::endl;
}

int main(int argc, char const *argv[]) {
    // Verificar argumentos
    if (argc != 3) {
        std::cerr << "Usage: ./cliente <server_ip> <port>" << std::endl;
        return -1;
    }

    // Crear el cliente
    JuegoCliente client(argv[1], atoi(argv[2]));
    client.start();
    return 0;
}