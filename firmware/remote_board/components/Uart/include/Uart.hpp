//
//  Uart.hpp
//  remote_board
//
//  Created by michaelobed on 31/03/2026.
//  
//  Copyright © 2026 Michael Obed.

#ifndef Uart_hpp
#define Uart_hpp

#include <cstdint>
#include "driver/uart.h"

class Uart
{
    public:
        Uart() { }

        static Uart& GetInstance()
        {
            static Uart s;
            return s;
        }

        QueueHandle_t Queue;
        static constexpr uart_port_t UartNum = UART_NUM_1;

        void Handle();
        bool Init();
        void SwitchToInput(char inputChar);

    private:
        static constexpr int bufferSize = 1024;
        static constexpr int pinRx = 17;
        static constexpr int pinTx = 18;
        static constexpr int queueSize = 10;
        static constexpr int rxTicksToWait = 100;
        static constexpr int taskPriority = 12;
        static constexpr int taskStackDepth = 3072;
        
};

#endif