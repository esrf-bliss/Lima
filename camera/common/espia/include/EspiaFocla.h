/*******************************************************************
 * @file EspiaFocla.h
 * @brief This file contains Focla Dev and SerialLine classes
 *
 * @author A.Kirov
 *
 * @date 25/06/2009
 *******************************************************************/

#ifndef ESPIAFOCLA_H
#define ESPIAFOCLA_H

#include <map>
#include <string>
#include <vector>
#include "EspiaDev.h"
#include "focla_lib.h"
#include "HwSerialLine.h"


namespace lima
{
namespace Espia
{
namespace Focla
{


enum ParamNb {  // Or move Focla param enum from focla_lib.cpp to focla_lib.h ?
	TRIG_MODE,
	TRIG,
	CAM_SEL,
	TEST_IMAGE,
	CL1_PIX_PACK,
	CL2_PIX_PACK,
	CL1_CC1,
	CL1_CC2,
	CL1_CC3,
	CL1_CC4,
	CL2_CC1,
	CL2_CC2,
	CL2_CC3,
	CL2_CC4,
	CL1_IN1,
	CL1_IN2,
	CL1_IN3,
	CL2_IN1,
	CL2_IN2,
	CL2_IN3,
	CL1_OUT1,
	CL1_OUT2,
	CL1_OUT3,
	CL2_OUT1,
	CL2_OUT2,
	CL2_OUT3,
};

// We don't need this if we include the enum from focla_lib.h
extern const std::map<ParamNb, std::string> ParamNb2NameMap;


extern std::map<std::string, int> ParamName2IdxMap;

void initParamName2IdxMap();


extern const std::map<std::string, int> PixelPack;


extern const std::vector<int> CCLevel;


#if 0
enum SignalIdx {  // Or move FOCLA_SIG enum from focla_lib.cpp to focla_lib.h ?
	FOCLA_SIG_CC1,
	FOCLA_SIG_CC2,
	FOCLA_SIG_CC3,
	FOCLA_SIG_CC4,
	FOCLA_SIG_OUT1,
	FOCLA_SIG_OUT2,
	FOCLA_SIG_OUT3,
	FOCLA_SIG_TRIG,
	FOCLA_NR_SIG,
};

extern std::map<std::string, SignalIdx> SigName2IdxMap;
#else
extern std::map<std::string, int> SigName2IdxMap;

void initSigName2IdxMap();
#endif /* 0/1 */


/*******************************************************************
 * @class lima::Espia:Focla::Dev
 * @brief Focla device class
 *
 *******************************************************************/
class Dev 
{

  public :

	Dev( Espia::Dev &espia_dev );
	~Dev();

	Espia::Dev &getEspiaDev();
	focla_t getFocla();

	void getParam( const std::string pname, int &value );
	void getParam( const ParamNb pnum, int &value );

	void setParam( const std::string pname, int value );
	void setParam( const ParamNb pnum, int value );

	void disableCache();
	void enableCache();

	void getSelectedCamera( int &curr_cam );
	void selectCamera( int cam_nb );

	void getPixelPack( int cam_nb, std::string &pix_pack_str );
	void setPixelPack( int cam_nb, int pix_pack=-1, 
	                                  std::string pix_pack_str="Default" );

	void getTestImage( int &val );
	void setTestImage( int val );

	void getTriggerMode( int &val );
	void setTriggerMode( int val );

	void triggerImage();

	void ccLevelGet( int cam_nb, int cc_nb, unsigned int &cc_level );
	void ccLevelSet( int cam_nb, int cc_nb, unsigned int cc_level );

	void ccPulseStart( int cam_nb, int cc_nb, int polarity, int width_us,
	                   int delay_us=0, int nb_pulse=1 );
	void ccPulseStop( int cam_nb, int cc_nb );
	void ccPulseStatus( int cam_nb, int cc_nb, int &pulse_active, 
	                    int &curr_pulse, int &curr_stage );

	// XXX Do we need these three below?
	void setCLDev( int cam_nb, void *cl_dev );
	void getCLDev( int cam_nb, void **cl_dev );
	void delCLDev( void *cl_dev );

  private :

	void open();
	void close();

	void checkMeta() throw(Exception);
	int pIdxFromName( const std::string pname );
	int pIdxFromNb( const ParamNb pnum );
	std::string pixelPackParName( int cam_nb );
	std::string ccLevelParName( int cam_nb, int cc_nb );
	std::string ccSignalName( int cam_nb, int cc_nb );
	int sigNbFromName( const std::string sname );

	focla_t     m_focla;
	Espia::Dev &m_edev;
	bool        m_no_cache;
	bool        m_dev_open;
	int         m_curr_cam;
	void       *m_cl_dev[2];  // XXX In Python: weakref to CLEspiaDevClass
};


/*******************************************************************
 * @class lima::Espia:Focla::SerialLine
 * @brief Focla serial line class
 *
 *******************************************************************/
class SerialLine : public HwSerialLine
{

  public :

	static const int Available = 0;

	SerialLine( Focla::Dev &focla_dev );
	virtual ~SerialLine();

	Focla::Dev &getFoclaDev();
	Espia::Dev &getEspiaDev();

	virtual void flush();

	virtual void getNbAvailBytes( int &nb_avail );

	virtual void read( std::string& buffer, int max_len=Available, 
	           double timeout=TimeoutDefault );

	virtual void readLine( std::string& buffer, int max_len, 
	               double timeout=TimeoutDefault );

	virtual void write( const std::string& buffer, bool no_wait=false );

  private :

	int getTimeoutUSec( double timeout_s=TimeoutDefault, bool avail=false );

	Focla::Dev &m_fdev;
};


} // namespace Focla
} // namespace Espia
} // namespace lima


#endif /* ESPIAFOCLA_H */
