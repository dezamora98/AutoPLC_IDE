#ifndef HEXCODE_H
#define HEXCODE_H

#include <QTextStream>
#include <QDebug>
#include <QFile>

#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

typedef pair<uint16_t,uint16_t> addr_R;

struct intel_line
{
    uint8_t byte_count;
    uint16_t addr;
    uint8_t code;
    QByteArray data;
    uint8_t valid;
};

struct pack_32
{
    uint16_t addr;
    uint8_t data[32];
};

typedef QList<shared_ptr<pack_32>> list_pack32;
typedef QList<shared_ptr<intel_line>> line_list;

struct code_packs
{
    uint16_t addr_steep;
    pack_32 steep_app;
    uint8_t conf_bits[14];
    list_pack32 list_32;
};


class hexcode
{
public:
    hexcode(addr_R addr_range, uint16_t addr_steep);
    bool open(QString addrf);
    shared_ptr<code_packs> get_list_pack32(){return make_shared<code_packs>(m_list_pack);}
    ~hexcode(){}

private:

    addr_R m_addr_range;
    uint16_t m_addr_steep;

    QFile m_dirfile;
    line_list m_clist;
    code_packs m_list_pack;

    bool process_line(QString line);
    bool process_pack32();
};



#endif // HEXCODE_H
