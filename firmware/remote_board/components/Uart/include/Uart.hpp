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

    private:
        static constexpr uint16_t bufferSize = 1024;
        static constexpr uint8_t pinRx = 17;
        static constexpr uint8_t pinTx = 18;
        static constexpr uint8_t queueSize = 10;
        static constexpr uint8_t rxTicksToWait = 100;
        static constexpr uint8_t taskPriority = 12;
        static constexpr uint16_t taskStackDepth = 3072;
        
};

#endif