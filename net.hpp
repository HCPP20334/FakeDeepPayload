#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <thread>
#include <format>
#include <atomic>
#include "io.hpp"
#include <mutex>
#include "crypto.hpp"
#pragma comment(lib,"ws2_32.lib")

struct net {
    const char* target_ip = "142.251.1.102";
    int target_port = 443;                   
    size_t PACKET_SIZE = 64000;
    SOCKET sock = INVALID_SOCKET;          
    sockaddr_in addr;
    int64_t max_iteration = 10;
    bool bMultiThread = false;
    int64_t i64MaxThreads = std::jthread::hardware_concurrency();

    // Конструктор для инициализации WinSock
    net() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            print("WSAStartup failed\n");
        }
    }

    void sendDeep() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            print("Socket creation failed\n");
            return;
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(target_port);
        addr.sin_addr.s_addr = inet_addr(target_ip);

        print("Connecting to{}:{}...\n", target_ip, target_port);

        if (bMultiThread) {
           
            int64_t  _chunk = (max_iteration / i64MaxThreads);
            static std::atomic<int> sentBytes = 0,array = 0;
            print("Enable MultiThreading :: cpu_threads: {}\n", i64MaxThreads);
            std::vector<std::jthread> cpu_thread;
            cpu_thread.reserve(i64MaxThreads);
            std::string rand_sha256_str = std::format("{}", cryptEngine->seed_rand_entropy());
            for (int cthread = 0; cthread < i64MaxThreads; cthread++) {
                cpu_thread.emplace_back([=] {
                    int local_sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (connect(local_sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                        std::vector<const char*> buf(PACKET_SIZE, rand_sha256_str.data());
                        for (int64_t count = 0; count < _chunk; count++) {
                            
                            sentBytes = send(local_sock, buf[rand() % buf.size()], (int)buf.size(), 0);
                            print("[{}:{}] send :{}\n",
                                target_ip, target_port, sentBytes.load());
                        }
                        closesocket(local_sock);
                    }
                    });
                print("[{}:{}]  cpu_thread{}::send() raw_bytes:{}\n",
                    target_ip, target_port, cthread, sentBytes.load());
            }
            
        }
        else {
            if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                std::vector<char> buf(PACKET_SIZE, 'X');
                for (int64_t count = 0; count < max_iteration; count++) {

                    int sentBytes = send(sock, buf.data(), buf.size(), 0);

                    if (sentBytes != SOCKET_ERROR) {
                        print("[{}:{}] Request sent! Bytes: {}\n", target_ip, target_port, sentBytes);
                    }
                }

                closesocket(sock);
            }
            else {
                int err = WSAGetLastError();
                print("[{}:{}] Connection Error! Code: {}\n", target_ip, target_port, err);
            }
        }
    }
    ~net() {
        WSACleanup();
    }
};