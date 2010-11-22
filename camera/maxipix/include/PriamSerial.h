#ifndef _PRIAM_SERIAL_H
#define _PRIAM_SERIAL_H

#include "EspiaSerialLine.h"
#include "ThreadUtils.h"
#include <string>

namespace lima {
namespace Maxipix {


  class PriamSerial {
    
    DEB_CLASS_NAMESPC(DebModCameraCom, "PriamSerial", "Maxipix");

  public:

    struct PriamCodeType {
	short	code;
	std::string  name;
	short 	writeCode;
	long	writeSize;
 	short  	readCode;
	long	readSize;
    };

    enum PriamSerialError {
	SERIAL_END=(short)0xff,
	SERIAL_ERR=(short)0xfe,
	SERIAL_BAD=(short)0xfd
    };
	
    enum PriamRegister {
	PR_MCR1,
	PR_MCR2,
	PR_BID,
	PR_MSR,
	PR_ET1,
	PR_ET2,
	PR_ROM,
	PR_AOP,
	PR_LUTAD1,
	PR_LUTAD2,
	PR_TP,
	PR_INB1,
	PR_INB2,
	PR_IT1,
	PR_IT2,
	PR_OSC,
	PR_TAS,
	PR_TAP,
	PR_ST1,
	PR_ST2,
	PR_TIP,
	PR_CFG,
	PR_MPINIT,
	PR_MPSEL,
	PR_OWT1,
	PR_OWT2,
	PR_OWTR,
	PR_OWCR,
	PR_SPISIZ,
	PR_SPICTZ,
	PR_SPITZ,
	PR_NB
    };
    static const PriamCodeType PriamRegCode[];

    enum PriamSerTx {
	PSER_MATRIX,
	PSER_FSR,
	PSER_NB
    };
    static const PriamCodeType PriamSerTxCode[];

    enum PriamLut {
	PLUT_CC,
	PLUT_FF1,
	PLUT_FF2,
	PLUT_FF3,
	PLUT_FF4,
	PLUT_FF5,
	PLUT_NB
    };
    static const PriamCodeType PriamLutCode[];


    PriamSerial(Espia::SerialLine &espia_serial_line);
    ~PriamSerial();

    void writeRegister(PriamRegister reg,const std::string& buffer);
    void readRegister(PriamRegister reg,std::string& buffer, long size=0) const;

    void writeFsr(const std::string& fsr,std::string& bid);

    void writeMatrix(const std::string& input);
    void readMatrix(std::string& output) const;

    void writeLut(PriamLut lut,const std::string& buffer);
    void readLut(PriamLut lut,std::string& buffer,long size) const;

  private:
    void _readAnswer(short code,long size,std::string& buf) const;
    void _writeCommand(short code,const std::string& buf) const;

    mutable Espia::SerialLine& m_espia_serial;
    mutable Mutex m_mutex;
  };

}
}
#endif // _PRIAM_SERIAL_H
