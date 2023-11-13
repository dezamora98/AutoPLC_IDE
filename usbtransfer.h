#ifndef USBTRANSFER_H
#define USBTRANSFER_H

#include <QByteArray>
#include <iostream>
#include <memory>
#include <chrono>
#include <QThread>

#include <windows.h>

#include "hidapi.h"
#include "hexcode.h"


#define PIC_PID 0x0010
#define PIC_VID 0x04D8

//----------CODE_0----------
#define APP_MODE        0x01
#define BOOT_MODE       0x02

#define WRITE_MODE      0x03
#define READ_MODE       0x04

#define MONITOR_MODE    0x05
#define CONTROL_MODE    0x06
#define ERASE_MODE      0x07

#define EXCEPTION       0x08
//--------------------------

//----------CODE_1----------
#define INIT            0x01
#define END             0x02
#define TASK            0x03
#define ERROR_ADDR      0xFF
#define STEEP_APP       0x10
//--------------------------

//-----------PACK-----------
#define CODE_0          0x01
#define CODE_1          0x02
#define ADDR_L          0x03
#define ADDR_H          0x04
#define DATA_PACK       0x05
//--------------------------

using namespace std;
using namespace chrono;


class usbtransfer
{
public:
    usbtransfer();
    bool usb_init_com();
    bool tx_data(uint8_t *tx, uint8_t size=64);
    bool rx_data(uint8_t *rx, uint8_t size=64, int time=20000);


    bool init_MODE(uint8_t c0, uint8_t c1);
    bool erase_flash();
    bool write_PACK(shared_ptr<code_packs> C_p32);

     ~usbtransfer();
    //bool tx_hexcode(hexcode_list &code);

private:

    bool isConnected;
    hid_device *handle;
};



#endif // USBTRANSFER_H

