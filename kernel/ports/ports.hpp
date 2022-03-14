#pragma once

namespace ports {

unsigned char byteIn(unsigned short port);
void byteOut(unsigned short port, unsigned char data);
unsigned short wordIn(unsigned short port);
void wordOut(unsigned short port, unsigned short data);

}
