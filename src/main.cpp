#include "AVSManager.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <windows.h>
#include <chrono>

std::atomic<bool> running_flag(true), exit_thread_flag(false);

void get_avs_data() {
    // connect
    int i = 0;
    std::cout << "Thread ID: " << std::this_thread::get_id() << " is running\r\n";
    while(!exit_thread_flag) {
        while(running_flag) {
            std::cout << "Data: " << i++ << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));  // 每秒输出一次数据
        }
    }
    std::cout << "Thread ID: " << std::this_thread::get_id() << " is exit\r\n";
}

void listen_user_input_flag() {
    auto __id = std::this_thread::get_id();
    std::cout << "Thread ID: " << __id << " is running\r\n"; 
    std::string input;
    while(!exit_thread_flag) {
        std::getline(std::cin, input);
        std::cout << "Get Str is " << input << std::endl;
        if (input == "exit") {
            exit_thread_flag = true;
            running_flag = false;
        }
        if (input == "stop") {
            running_flag = false;
        }
        if (input == "run") {
            running_flag = true;
        }
    }
    std::cout << "Thread ID: " << __id << " is exit\r\n"; 
}


int main(int argc, char** argv)
{
    char version_info[1000];
    int result = AVS_Init(0); // 使用导入的函数
    AVS_GetDLLVersion(version_info);
    std::cout << version_info << std::endl;

    std::thread thread_of_avs(get_avs_data);
    std::thread thread_of_listen(listen_user_input_flag);

    thread_of_listen.join();
    thread_of_avs.join();

    return 0;
}