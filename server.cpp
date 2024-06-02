#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <cstdlib>
#include <ctime>

class JuegoServidor {
public:
    JuegoServidor(int port);
    void start();

private:
    int server_fd;
    struct sockaddr_in address;
    std::vector<std::thread> client_threads;
    std::mutex mtx;

    void manejarCliente(int client_socket, const std::string& client_info);
    void mostrarTablero(char board[6][7]);
    bool chequearGanador(char board[6][7], char player);
};

JuegoServidor::JuegoServidor(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

void JuegoServidor::start() {
    std::cout << "Esperando conexiones ..." << std::endl;
    srand(time(0));

    while (true) {
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        std::string client_info = inet_ntoa(address.sin_addr) + std::string(":") + std::to_string(ntohs(address.sin_port));
        std::cout << "Juego nuevo[" << client_info << "]" << std::endl;

        client_threads.push_back(std::thread(&JuegoServidor::manejarCliente, this, client_socket, client_info));
    }
}

void JuegoServidor::manejarCliente(int client_socket, const std::string& client_info) {
    char board[6][7] = {};
    memset(board, '-', sizeof(board));

    bool client_turn = rand() % 2 == 0;
    send(client_socket, &client_turn, sizeof(client_turn), 0);
    std::cout << "Juego [" << client_info << "]: " << (client_turn ? "inicia juego el cliente." : "inicia juego el servidor.") << std::endl;

    while (true) {
        int column;
        if (client_turn) {
            recv(client_socket, &column, sizeof(column), 0);
            for (int i = 5; i >= 0; --i) {
                if (board[i][column] == '-') {
                    board[i][column] = 'C';
                    break;
                }
            }
            std::cout << "Juego [" << client_info << "]: cliente juega columna " << column + 1 << "." << std::endl;
        } else {
            column = rand() % 7;
            for (int i = 5; i >= 0; --i) {
                if (board[i][column] == '-') {
                    board[i][column] = 'S';
                    break;
                }
            }
            std::cout << "Juego [" << client_info << "]: servidor juega columna " << column + 1 << "." << std::endl;
        }

        send(client_socket, board, sizeof(board), 0);

        if (chequearGanador(board, client_turn ? 'C' : 'S')) {
            char status = 'G';
            send(client_socket, &status, 1, 0);
            std::cout << "Juego [" << client_info << "]: " << (client_turn ? "gana cliente." : "gana servidor.") << std::endl;
            break;
        } else {
            bool draw = true;
            for (int i = 0; i < 7; ++i) {
                if (board[0][i] == '-') {
                    draw = false;
                    break;
                }
            }
            if (draw) {
                char status = 'D';
                send(client_socket, &status, 1, 0);
                std::cout << "Juego [" << client_info << "]: empate." << std::endl;
                break;
            }
        }

        char status = 'C';
        send(client_socket, &status, 1, 0);
        client_turn = !client_turn;
    }

    close(client_socket);
    std::cout << "Juego [" << client_info << "]: fin del juego." << std::endl;
}

void JuegoServidor::mostrarTablero(char board[6][7]) {
    // Muestra el tablero en la consola
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

bool JuegoServidor::chequearGanador(char board[6][7], char player) {
    // Checkea ganador horizontal
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == player && board[i][j+1] == player && board[i][j+2] == player && board[i][j+3] == player) {
                return true;
            }
        }
    }
    // Check ganador vertical
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 7; ++j) {
            if (board[i][j] == player && board[i+1][j] == player && board[i+2][j] == player && board[i+3][j] == player) {
                return true;
            }
        }
    }
    // Check gana diagonal (abajo izquierda a arriba derecha)
    for (int i = 3; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == player && board[i-1][j+1] == player && board[i-2][j+2] == player && board[i-3][j+3] == player) {
                return true;
            }
        }
    }
    // Check ganador diagonal (arriba izquierda a abajo derecha)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == player && board[i+1][j+1] == player && board[i+2][j+2] == player && board[i+3][j+3] == player) {
                return true;
            }
        }
    }
    return false;
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./servidor <port>" << std::endl;
        return -1;
    }

    int port = atoi(argv[1]);
    JuegoServidor server(port);
    server.start();
    return 0;
}