#include "hexcode.h"

hexcode::hexcode(addr_R addr_range, uint16_t addr_steep)
{
    m_addr_range = addr_range;
    m_addr_steep = addr_steep;
}

bool hexcode::open(QString addrf)
{
    m_dirfile.setFileName(addrf);
    if(!m_dirfile.exists()) return false;

    m_dirfile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!m_dirfile.isOpen()) return false;

    QTextStream ihex(&m_dirfile);
    QString line = ihex.readLine();
    while (!line.isNull())
    {
        if(line.startsWith(":"))   //valid line
        {
            process_line(line);
        }
        line = ihex.readLine();
    }
    m_dirfile.close();

    if(!process_pack32()) return false;

    return true;
}

bool hexcode::process_line(QString line)
{
    auto t_line = make_shared<intel_line>();

    t_line->addr = static_cast<uint16_t>(line.mid(3,4).toUInt(nullptr,16));                           //  dirección
    t_line->byte_count = static_cast<uint8_t>(line.mid(1,2).toUInt(nullptr,16));                      //  número de bytes
    t_line->code = static_cast<uint8_t>(line.mid(7,2).toUInt(nullptr,16));                            //  código
    t_line->valid = static_cast<uint8_t>(line.mid(9+2*t_line->byte_count,2).toUInt(nullptr,16));      //  código de suma C2

    QString t_data_str;

    for(uint8_t i=0;i<2*t_line->byte_count;i+=2)    // carga de datos de línea
    {
        t_data_str = line.mid(9+i,2);               // recortando 1 byte
        t_line->data.push_back(static_cast<char>(t_data_str.toUInt(nullptr,16)));   //  añadiendo byte
    }

    if(t_line->data.size()!=t_line->byte_count) return false;   // validación por tamaño

    //falta validación por sumatoria C2

#ifdef little_endian_format
    char t_byte;

    for(int i = 0; i < t_line->byte_count; i+=2)
    {
        t_byte = t_line->data[i];
        t_line->data[i] = t_line->data[i+1];
        t_line->data[i+1] = t_byte;
    }
#endif

    m_clist.push_back(t_line);  //  línea hex intel añadida a la lista
    return true;
}

bool hexcode::process_pack32()
{
    pack_32 t_p32;
    auto t_pack = make_shared<intel_line>();

    for(int i = 0; i < m_clist.size();++i)
    {
        t_pack = m_clist.at(i);               // paquere temporal

        if(t_pack->code == 4)
        {
            if(t_pack->data.at(1) == 0x30)                   // es la línea de offset bits de configuración?
            {
                t_pack = m_clist.at(++i);                                        //  incrementando a línea de bits de configuración
                for(uint8_t j=0;j<t_pack->byte_count;++j)
                {
                    m_list_pack.conf_bits[j] = static_cast<uint8_t>(t_pack->data.at(j));    // carga de bits de configuración
                }
            }
            else if(t_pack->data.at(1) == 0)                 // es la línea de offset 0x0000?
            {
                t_pack = m_clist.at(++i);                                                //  incrementando a línea de steep
                m_list_pack.steep_app.addr = m_addr_steep;
                for(uint8_t j=0;j<32;++j)                           // cargando paquete de steep app
                {
                    if(j<t_pack->byte_count)                   // si no son los últimos bytes  carga 0xFF
                    {
                        m_list_pack.steep_app.data[j] =
                                static_cast<uint8_t>(t_pack->data.at(j)); // cargando datos de "GOTO APP"
                    }
                    else
                    {
                        m_list_pack.steep_app.data[j] = 0xFF;
                    }

                }
            }
            else if(t_pack->data.at(1) == 0x20)              // si el offset es 0x2000 ignorar esa línea;
            {
                i++;
            }
            else
            {
                return false;               // si el offset no es ninguno de los anteriores el fichero hex no es válido para el AUTOUSB-KIT
            }

            continue;                       // ir a la próxima línea;
        }
        else if(t_pack->code == 0)
        {
            t_p32.addr = t_pack->addr & 0xFFE0;        // dirección reajustada
            uint8_t t_CF = t_pack->addr & 0x001F;      // factor corrector

            if(t_CF==0)                             //si el factor corrector es 0 el bloque se escribe directamente
            {
                for(uint8_t j=0;j<t_pack->byte_count;++j)   //escribiendo directamente la línea analizada en el paquete de 32 bytes
                {
                    t_p32.data[j] = static_cast<uint8_t>(t_pack->data[j]);
                }

                if( (i+1) != m_clist.size() && m_clist.at(i+1)->code == 0 )      // si la línea actual no es la final y su código es 0; ver si se puede completar el paquete.
                {
                    if( (m_clist.at(i+1)->addr - t_pack->byte_count) == t_pack->addr && (t_pack->byte_count + m_clist.at(i+1)->byte_count)<32)    // los bloques de datos son consecutivos y hay espacio para ambos bloques?
                    {
                        auto size_last_pack = t_pack->byte_count;          //  salvando espacio ocupado por el primer paquete
                        t_pack = m_clist.at(++i);                          //  actualizando iterador del ciclo completo
                        for(uint8_t j=0;j<t_pack->byte_count;++j)          //  cargando paquete consecutivo
                        {
                            if(j < t_pack->byte_count)                     //  verificar si es necezario rellenar el paquete con 0xFF
                            {
                                t_p32.data[j+size_last_pack] = static_cast<uint8_t>(t_pack->data[j]);  // cargando datos consecutivos
                            }
                            else
                            {
                                t_p32.data[j+size_last_pack] = 0xFF;
                            }

                        }
                    }
                    else    //cuando los bloques no son consecutivos o no hay espacio!
                    {
                        for(uint8_t j = t_pack->byte_count; j < 32; ++j)  //rellenando las direcciones sobrantes
                        {
                            t_p32.data[j]=0xFF;
                        }
                    }
                }
                else                                                // cuando (i) es el útimo bloque
                {
                    for(uint8_t j = t_pack->byte_count; j < 32; ++j)  //rellenando las direcciones sobrantes
                    {
                        t_p32.data[j]=0xFF;
                    }
                }

                m_list_pack.list_32.push_back( make_shared<pack_32>(t_p32));               // incorporando nuevo bloque;

            }
            else                                    // cuando el factor corrector no es 0;
            {
                for(uint8_t j = 0; j < 32; ++j)          //rellenando las direcciones hasta el factor corrector
                {
                    if(j<t_CF || j >= t_pack->byte_count+t_CF)
                    {
                        t_p32.data[j]=0xFF;    // dirección no usada 0xFF
                    }
                    else
                    {
                        t_p32.data[j] = static_cast<uint8_t>(t_pack->data[j-t_CF]); // cargando datos en direcciones válidas
                    }
                }
                m_list_pack.list_32.push_back( make_shared<pack_32>(t_p32));     // incorporando nuevo bloque;

                if( (32-t_CF) < t_pack->byte_count )        // faltaron datos de la línea?
                {
                    uint16_t new_addr = t_p32.addr + 32;                 // incrementando salvando dirección
                    t_p32.addr = new_addr;         // asignando dirección del próximo paquete para completar

                    for(uint8_t j = (32-t_CF); j < t_pack->byte_count;++j) // terminar de copiar datos en el nuevo paquete
                    {
                        t_p32.data[j-(32-t_CF)] = static_cast<uint8_t>(t_pack->data.at(j));
                    }

                    /* Al tener un bloque 32 bytes incompleto, que fue rellenado con uno menoro igual a 16 bytes
                     y no quedar completamente transferido, siempre se tendrá espacio para copiar un segundo
                     bloque consecutivo de tamaño menor menor o igual a 16 bytes, de existir este.*/

                    if(m_clist.at(i+1)->addr - t_pack->byte_count == t_pack->addr && m_clist.at(i+1)->code == 0)    // los bloques de datos son consecutivos y el código del proximo bloque es 0x00?
                    {
                        auto t_pack_next = m_clist.at(++i); // incrementando iterador general y salvando nueva línea

                        for(uint8_t j = 0; j < 32;++j) // cargando nuevos datos
                        {
                            if(j<t_pack_next->byte_count)   // cargar dato
                            {
                                t_p32.data[j + t_pack->byte_count - (32-t_CF)] = static_cast<uint8_t>(t_pack_next->data.at(j));
                            }
                            else  // cuado exeda el tamaño de la línea rellenar con 0xFF
                            {
                                t_p32.data[j + t_pack->byte_count - (32-t_CF)] = 0xFF;
                            }

                        }
                    }
                    else    //de no ser consecutivos los bloques; rellenar el espacio sobrante del paquete con 0xFF
                    {
                         for(uint8_t j =  t_pack->byte_count - (32-t_CF); j < 32; ++j)
                         {
                             t_p32.data[j] = 0xFF;
                         }
                    }
                    m_list_pack.list_32.push_back( make_shared<pack_32>(t_p32) );     // incorporando nuevo bloque
                }

            }
        }

    }
    return true;
}
