#include "usbtransfer.h"

usbtransfer::usbtransfer()
{
    isConnected = false;
}

bool usbtransfer::usb_init_com()
{
    struct hid_device_info *devs;

    if(hid_init()) return false;

    devs = hid_enumerate(PIC_VID,PIC_PID);

    if (devs != nullptr && isConnected == false)
    {
        isConnected = true;
        cout<<"USB CONECTADO"<<endl;
        wstring ws = devs->manufacturer_string;
        string str(ws.begin(),ws.end());
        cout<<"- "<<str<<endl;
        ws = devs->product_string;
        string str2(ws.begin(),ws.end());
        cout<<"- "<<str2<<endl;
    }
    handle = hid_open(PIC_VID,PIC_PID,nullptr);
    if(!handle)
    {
        //cout<<"No se pudo abrir el dispositivo"<<endl;
        return false;
    }
    return true;

}

bool usbtransfer::tx_data(uint8_t *tx,uint8_t size)
{
    uint8_t buff[65]={0};

    memset(buff,0,65);

    for(uint8_t i=0;i<size;++i)
    {
        buff[i+1]=tx[i];
    }
    buff[0] = 0;

    int i = hid_write(handle,buff,65);

    if(i<0)
    {
        cout<<"Error de transmicion"<<endl;
        return false;
    }

    return true;
}

bool usbtransfer::rx_data(uint8_t *rx, uint8_t size, int time)
{
    for(int i=0;i<size;++i)rx[i]=0;
    memset(rx,0,size);  // limpiando arreglo
    int i = hid_read_timeout(handle,rx,size,time);
    if(i<0) // error
    {
        cout<<"Error de recepción"<<endl;
        return false;
    }
    return true;
}

bool usbtransfer::init_MODE(uint8_t c0, uint8_t c1)
{
    uint8_t tx[64]={0};
    uint8_t rx[64]={0};

    tx[CODE_0] = c0;    //  preparando paquete
    tx[CODE_1] = c1;    //  preparando paquete

    if(!tx_data(tx)) return false;
    rx_data(rx,64);
    return (rx[CODE_0] == c0 && rx[CODE_1] == c1);
}

bool usbtransfer::erase_flash()
{
    uint8_t tx[64]={0};
    uint8_t rx[64]={0};

    tx[CODE_0] = ERASE_MODE;    // preparando paquete petición de modo borrado
    tx[CODE_1] = INIT;          // preparando paquete petición de modo borrado

    if(!tx_data(tx)) return false;
    rx_data(rx,64);
    if(rx[CODE_0] != ERASE_MODE || rx[CODE_1] != END) return false;
    return true;
}

bool usbtransfer::write_PACK(shared_ptr<code_packs> C_p32)
{
    uint8_t tx[64]={0};
    uint8_t rx[64]={0};

    tx[CODE_0] = WRITE_MODE;
    tx[CODE_1] = TASK;

    cout<<"Escritura: [";
    for(auto i:(C_p32->list_32))
    {
        tx[ADDR_H] = ((i->addr>>8) & 0x00FF);       // cargando parte alta de dirección de paquete
        tx[ADDR_L] = ((i->addr) & 0x00FF);          // cargando parte baja de dirección de paquete

        for(uint8_t j = 0; j<32; ++j)   // cargando paquete de datos de instrucción de salto APP
        {
            tx[j+DATA_PACK] = i->data[j];
        }
        while(1)
        {
            if(!tx_data(tx)) return false;      // envío de paquete de escritura
                                                // espera de escritura de 10ms
            rx_data(rx,64,100);                  // respuesta
            if(rx[CODE_1] != TASK)              // la respuesta es válida?
            {
                //tx[CODE_0] = EXCEPTION;
                //tx_data(tx);
                cout << "x";
                continue;
            }
            cout<<"-";
            break;
        }

    }
    //-----------------------------------salto de aplicación-----------------------------------------

    tx[ADDR_H] = ((C_p32->steep_app.addr>>8) & 0x00FF);       // cargando parte alta de dirección de instrucción de salto APP
    tx[ADDR_L] = ((C_p32->steep_app.addr) & 0x00FF);          // cargando parte baja de dirección de instrucción de salto APP


    for(uint8_t j = 0; j<32; ++j)   // cargando paquete de datos de instrucción de salto APP
    {
        tx[j+DATA_PACK] = C_p32->steep_app.data[j];
    }

    while(1)
    {
        if(!tx_data(tx)) return false;      // envío de paquete de escritura
                                            // espera de escritura de 10ms
        rx_data(rx,64,100);                  // respuesta
        if(rx[CODE_1] != TASK)              // la respuesta es válida?
        {
            //tx[CODE_0] = EXCEPTION;
            //tx_data(tx);
            cout << "x";
            continue;
        }
        cout<<"-";
        break;
    }
    //------------------------------------------------------------------------------------------------
    // falta escritura de registros de configuración
    //------------------------------------------------------------------------------------------------

    Sleep(100);
    if(!init_MODE(WRITE_MODE,END))
    {
        return false;
     }
    cout<<"]."<<endl;

    return true;
}

usbtransfer::~usbtransfer()
{
    hid_close(handle);
    hid_exit();
}
