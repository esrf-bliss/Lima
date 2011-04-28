/*!
 * \file     
 * \brief    A yat::BitsStream example.
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */


#include <iostream>
#include <yat/network/ClientSocket.h>
#include "GalilBitsRecords.h"

#define UPDATE_DR_CMD "QRIABCDEFGH\r"

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------
int main (int argc, char* argv[])
{
  try
  {
    //- yat internal cooking
    yat::Socket::init();

    //- instanciating the Socket (default protocol is yat::Socket::TCP_PROTOCOL)
    std::cout << "Instanciating the Socket" << std::endl;
    yat::ClientSocket sock;

    //- set some socket option
    std::cout << "Setting sock options" << std::endl;
    sock.set_option(yat::Socket::SOCK_OPT_KEEP_ALIVE, 1);
    sock.set_option(yat::Socket::SOCK_OPT_NO_DELAY, 1);
    sock.set_option(yat::Socket::SOCK_OPT_OTIMEOUT, 1000);
    sock.set_option(yat::Socket::SOCK_OPT_ITIMEOUT, 1000);

    int ka = sock.get_option(yat::Socket::SOCK_OPT_KEEP_ALIVE);
    if (! ka)
    {
      std::cout << "unexpected Socket::SOCK_OPT_KEEP_ALIVE opt value" << std::endl;
      return -1;
    }

    int nd = sock.get_option(yat::Socket::SOCK_OPT_NO_DELAY);
    if (! nd)
    {
      std::cout << "unexpected Socket::SOCK_OPT_NO_DELAY opt value" << std::endl;
      return -1;
    }

    int otmo = sock.get_option(yat::Socket::SOCK_OPT_OTIMEOUT);
    if (otmo != 1000)
    {
      std::cout << "unexpected Socket::SOCK_OPT_OTIMEOUT opt value" << std::endl;
      return -1;
    }

    int itmo = sock.get_option(yat::Socket::SOCK_OPT_ITIMEOUT);
    if (itmo != 1000)
    {
      std::cout << "unexpected Socket::SOCK_OPT_ITIMEOUT opt value" << std::endl;
      return -1;
    }

    //- network address
    std::cout << "Instanciating network address" << std::endl;
    yat::Address addr("192.168.9.100", 5000);

    //- connect to addr
    std::cout << "Connecting to peer..." << std::endl;
    sock.connect(addr);

    do
    {
      //- read some data
      //- std::cout << "Sending command to peer" << std::endl;
      sock.send(UPDATE_DR_CMD);

      //- retrieve data from peer
      //- std::cout << "Retrieving data from peer" << std::endl;
      yat::Socket::Data data(253);
      size_t nb = sock.receive(data); 
      std::cout << "Got " << nb << " bytes from peer" << std::endl;

      //- extract data into a QRBlock
      yat::BitsStream bs(reinterpret_cast<unsigned char *>(data.base()), 
											   data.size(), 
											   yat::Endianness::BO_LITTLE_ENDIAN);
      QRBlock qr_block;
		  bs >> qr_block;
      qr_block.dump();
    } 
    while (0);

    //- disconnect from peer
    std::cout << "Disconnecting from peer..." << std::endl;
    sock.disconnect();

    //- yat internal cooking
    yat::Socket::terminate();

  }
  catch (const yat::SocketException & se)
  {
    std::cout << "*** yat::SocketException caught ***" << std::endl;

    for (size_t err = 0; err  < se.errors.size(); err++)
    {
      std::cout << "Err-" << err << "::reason..." << se.errors[err].reason << std::endl;
      std::cout << "Err-" << err << "::desc....." << se.errors[err].desc << std::endl;
      std::cout << "Err-" << err << "::origin..." << se.errors[err].origin << std::endl;
      std::cout << "Err-" << err << "::code....." << se.errors[err].code << std::endl;
    }
  } 
  catch (...)
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

	return 0;  
}
