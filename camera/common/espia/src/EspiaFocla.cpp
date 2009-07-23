/*******************************************************************
 * @file EspiaFocla.cpp
 * @brief Focla::Dev and Focla::SerialLine classes implementation
 *
 * @author A.Kirov
 * @date 25/06/2009
 *******************************************************************/

#include <utility>
#include <sstream>
#include "EspiaFocla.h"
#include "EspiaSerialLine.h"

using namespace std;
using namespace lima;
using namespace Espia::Focla;


#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)


typedef pair<ParamNb,string> Nb2NamePair;

static const Nb2NamePair ParamNb2NameList[] =
{
	Nb2NamePair(TRIG_MODE, "TRIG_MODE"),
	Nb2NamePair(TRIG, "TRIG"),
	Nb2NamePair(CAM_SEL, "CAM_SEL"),
	Nb2NamePair(TEST_IMAGE, "TEST_IMAGE"),
	Nb2NamePair(CL1_PIX_PACK, "CL1_PIX_PACK"),
	Nb2NamePair(CL2_PIX_PACK, "CL1_PIX_PACK"),
	Nb2NamePair(CL1_CC1, "CL1_CC1"),
	Nb2NamePair(CL1_CC2, "CL1_CC2"),
	Nb2NamePair(CL1_CC3, "CL1_CC3"),
	Nb2NamePair(CL1_CC4, "CL1_CC4"),
	Nb2NamePair(CL2_CC1, "CL2_CC1"),
	Nb2NamePair(CL2_CC2, "CL2_CC2"),
	Nb2NamePair(CL2_CC3, "CL2_CC3"),
	Nb2NamePair(CL2_CC4, "CL2_CC4"),
	Nb2NamePair(CL1_IN1, "CL1_IN1"),
	Nb2NamePair(CL1_IN2, "CL1_IN2"),
	Nb2NamePair(CL1_IN3, "CL1_IN3"),
	Nb2NamePair(CL2_IN1, "CL2_IN1"),
	Nb2NamePair(CL2_IN2, "CL2_IN2"),
	Nb2NamePair(CL2_IN3, "CL2_IN3"),
	Nb2NamePair(CL1_OUT1, "CL1_OUT1"),
	Nb2NamePair(CL1_OUT2, "CL1_OUT2"),
	Nb2NamePair(CL1_OUT3, "CL1_OUT3"),
	Nb2NamePair(CL2_OUT1, "CL2_OUT1"),
	Nb2NamePair(CL2_OUT2, "CL2_OUT2"),
	Nb2NamePair(CL2_OUT3, "CL2_OUT3"),
};

const map<ParamNb, string> Espia::Focla::ParamNb2NameMap = 
	map<ParamNb, string>( ParamNb2NameList, ParamNb2NameList +
		(sizeof(ParamNb2NameList)/sizeof(ParamNb2NameList[0])) );


map<string, int> Espia::Focla::ParamName2IdxMap;

/*******************************************************************
 * @brief Initialize ParamName2IdxMap (once)
 *******************************************************************/
void Espia::Focla::initParamName2IdxMap()
{
	static bool is_init_ParamName2IdxMap=false;

	if( !is_init_ParamName2IdxMap ) {
		int i, nr_param;
		struct espia_param *fparam;

		nr_param = -1;
		CHECK_CALL( focla_get_param_data(&nr_param, NULL) );

		for (i = 0; i < nr_param; i++) {
			CHECK_CALL( focla_get_param_data(&i, &fparam) );
			ParamName2IdxMap[string(fparam->name)] = i;
		}

		// Check if ParamNb2NameList is consistent with ParamName2IdxMap?

		is_init_ParamName2IdxMap = true;
	}
}


static const pair<string,int> PixelPackList[] =
{
	pair<string,int>("Default", 0),
	pair<string,int>("Aviex",   2),
	pair<string,int>("Dalsa",   3),
	pair<string,int>("Sarnoff", 6),
};

const map<string, int> Espia::Focla::PixelPack = 
	map<string, int>( PixelPackList, PixelPackList +
		(sizeof(PixelPackList)/sizeof(PixelPackList[0])) );


/* CL CC Levels: Low, High, Ext */
static const int CCLevelArray[] = {2, 3, 0};

const vector<int> Espia::Focla::CCLevel = 
	vector<int>( CCLevelArray, CCLevelArray +
		(sizeof(CCLevelArray)/sizeof(CCLevelArray[0])) );


#if 0
// Static initialization (if we include the enum from focla_lib)
static const pair<string, SignalIdx> SigName2IdxList[] = {
	pair<string, SignalIdx>("CC1", FOCLA_SIG_CC1),
	pair<string, SignalIdx>("CC2", FOCLA_SIG_CC2),
	pair<string, SignalIdx>("CC3", FOCLA_SIG_CC3),
	pair<string, SignalIdx>("CC4", FOCLA_SIG_CC4),
	pair<string, SignalIdx>("OUT1", FOCLA_SIG_OUT1),
	pair<string, SignalIdx>("OUT2", FOCLA_SIG_OUT2),
	pair<string, SignalIdx>("OUT3", FOCLA_SIG_OUT3),
	pair<string, SignalIdx>("TRIG", FOCLA_SIG_TRIG),
};

const map<string, SignalIdx> Espia::Focla::SigName2IdxMap( SigName2IdxList,
    SigName2IdxList + (sizeof(SigName2IdxList) / sizeof(SigName2IdxList[0])) );

#else

// Dynamic initialization (if we don't include the enum from focla_lib)
map<string, int> Espia::Focla::SigName2IdxMap;

/*******************************************************************
 * @brief Initialize the SigName2IdxMap (once)
 *******************************************************************/
void Espia::Focla::initSigName2IdxMap()
{
	static bool is_init_SigName2IdxMap=false;

	if( !is_init_SigName2IdxMap ) {
		struct focla_signal *fsig;
		int sig, nr_sig;

		nr_sig = -1;
		CHECK_CALL( focla_get_sig_data(&nr_sig, NULL) );

		for( sig=0; sig<nr_sig; sig++ ) {
			CHECK_CALL( focla_get_sig_data(&sig, &fsig) );
			SigName2IdxMap[string(fsig->name)] = sig;
		}

		is_init_SigName2IdxMap = true;
	}
}
#endif /* 0/1 */


/*******************************************************************
 * @brief Espia::Focla::Dev class constructor
 *
 * @param[in] espia_dev  reference to Espia::Dev object
 *******************************************************************/
Dev::Dev( Espia::Dev &espia_dev ) :
	m_edev(espia_dev)
{
	initParamName2IdxMap();
	initSigName2IdxMap(); // If we don't have static initialization

	m_focla = FOCLA_DEV_INVAL;
	m_no_cache = false;
	m_cl_dev[0] = m_cl_dev[1] = NULL;  // XXX
	m_dev_open = false;

	Espia::SerialLine espia_ser_line(espia_dev);
	espia_ser_line.flush();

	open();
	setTestImage(0);
	getSelectedCamera( m_curr_cam );
}


Dev::~Dev()
{
	close();
}


void Dev::open()
{
	if( ! m_dev_open ) {
		CHECK_CALL( focla_open( m_edev, &m_focla ) );
		m_dev_open = true;
	}
}


void Dev::close()
{
	if( m_dev_open ) {
		CHECK_CALL( focla_close( m_focla ) );
		m_dev_open = false;
//		m_focla = FOCLA_DEV_INVAL; ?
	}
}


/*******************************************************************
 * @fn      Focla::Dev::getEspiaDev()
 * @return  Espia::Dev reference
 *******************************************************************/
Espia::Dev &Dev::getEspiaDev()
{
	return m_edev;
}


/*******************************************************************
 * @fn      Focla::Dev::getFocla()
 * @return  focla_t handler
 *******************************************************************/
focla_t Dev::getFocla()
{
	return m_focla;
}


int Dev::pIdxFromName( const string pname )
{
	map<string, int>::iterator pnm = ParamName2IdxMap.find(pname);
	if( pnm == ParamName2IdxMap.end() )
		throw LIMA_HW_EXC(InvalidValue, "No such Focla parameter name");
	return pnm->second;
}


int Dev::pIdxFromNb( const ParamNb pnum )
{
#if 1
	map<ParamNb,string>::const_iterator pnb = ParamNb2NameMap.find(pnum);
	if( pnb == ParamNb2NameMap.end() )
		throw LIMA_HW_EXC(InvalidValue, "No such Focla parameter num");
	return pIdxFromName( pnb->second );
#else  // pnum==pidx, but we can't be sure if we don't include from focla_lib.h
	return pnum;
#endif /* 1/0 */
}


void Dev::checkMeta() throw(Exception)
{
	if( m_edev.isMeta() ) {
		throw LIMA_HW_EXC(NotSupported, "Operation not permitted on "
		                                "meta-devices!");
	}
}


/*******************************************************************
 * @fn          Focla::Dev::getParam(const string pname, int &value)
 * @brief       Gets param value from his name
 *
 * @param[in]   pname  string parameter name
 * @param[out]  value  int parameter value
 *******************************************************************/
void Dev::getParam( const string pname, int &value )
{
	checkMeta();
	CHECK_CALL( focla_get_param(m_focla, pIdxFromName(pname), &value) );
}


/*******************************************************************
 * @fn          Focla::Dev::getParam(const ParamNb pnum, int &value)
 * @brief       Gets param value from his enum in ParamNb
 *
 * @param[in]   pnum   ParamNb parameter number
 * @param[out]  value  int parameter value
 *******************************************************************/
void Dev::getParam( const ParamNb pnum, int &value )
{
	checkMeta();
	CHECK_CALL( focla_get_param(m_focla, pIdxFromNb(pnum), &value) );
}


/*******************************************************************
 * @fn         Focla::Dev::setParam(const string pname, int value)
 * @brief      Sets param value based on param name
 *
 * @param[in]  pname  string parameter name
 * @param[in]  value  int parameter new value
 *******************************************************************/
void Dev::setParam( const string pname, int value )
{
	checkMeta();
	CHECK_CALL( focla_set_param(m_focla, pIdxFromName(pname), value) );
}


/*******************************************************************
 * @fn         Focla::Dev::setParam(const ParamNb pnum, int value)
 * @brief      Sets param value based on param enum from ParamNb
 *
 * @param[in]  pnum   aramNb parameter number
 * @param[in]  value  int parameter new value
 *******************************************************************/
void Dev::setParam( const ParamNb pnum, int value )
{
	checkMeta();
	CHECK_CALL( focla_set_param(m_focla, pIdxFromNb(pnum), value) );
}


/*******************************************************************
 * @fn         Focla::Dev::disableCache()
 * @brief      Disables camera number cache
 *
 * Sets m_no_cache variable to true
 *******************************************************************/
void Dev::disableCache()
{
	m_no_cache = true;
}


/*******************************************************************
 * @fn         Focla::Dev::enableCache()
 * @brief      Enables camera number cache
 *
 * Sets m_no_cache variable to false and calls getSelectedCamera()
 *******************************************************************/
void Dev::enableCache()
{
	m_no_cache = false;
	getSelectedCamera( m_curr_cam );
}


/*******************************************************************
 * @fn          Focla::Dev::getSelectedCamera(int &curr_cam)
 * @brief       Gets Selected camera number
 *
 * @param[out]  curr_cam  int reference to selected camera number
 *******************************************************************/
void Dev::getSelectedCamera( int &curr_cam )
{
	getParam( "CAM_SEL", curr_cam );
}


/*******************************************************************
 * @fn          Focla::Dev::selectCamera(int cam_nb)
 * @brief       Selects a camera number
 *
 * @param[in]   cam_nb  int number of the camera to select
 *******************************************************************/
void Dev::selectCamera( int cam_nb )
{
	if( m_no_cache || (cam_nb != m_curr_cam) ) {
		setParam( "CAM_SEL", cam_nb );
		m_curr_cam = cam_nb;
	}
}


string Dev::pixelPackParName( int cam_nb )
{
	ostringstream s;
	s << "CL" << (cam_nb+1) << "_PIX_PACK";
	return s.str();
}


/*******************************************************************
 * @fn          Focla::Dev::getPixelPack(int cam_nb, string &pix_pack_str)
 * @brief       Gets PixelPack param as a string for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[out]  pix_pack_str  string PixelPack
 *
 * @see         PixelPackList, EspiaAcqLib.py
 *******************************************************************/
void Dev::getPixelPack( int cam_nb, string &pix_pack_str )
{
	int pix_pack;
	string pname = pixelPackParName( cam_nb );
	getParam( pname, pix_pack );
	for( map<string,int>::const_iterator p=Espia::Focla::PixelPack.begin();
	                              p!=Espia::Focla::PixelPack.end(); ++p ) {
		if( pix_pack == p->second ) {
			pix_pack_str = p->first;
			return;
		}
	}
	ostringstream s;
	s << pix_pack;
	pix_pack_str = s.str();
}


/*******************************************************************
 * @fn         Focla::Dev::setPixelPack(int cam_nb, int pix_pack,\
 *                                      string pix_pack_str)
 * @brief      Sets PixelPack param for a camera
 *
 * @param[in]  cam_nb  int number of the camera
 * @param[in]  pix_pack  int PixelPack. Default is -1. In that case the next
 *             parameter is taken into account.
 * @param[in]  pix_pack_str  string PixelPack. Default is "Default"
 *
 * @see        PixelPackList, EspiaAcqLib.py
 *******************************************************************/
void Dev::setPixelPack( int cam_nb, int pix_pack, string pix_pack_str )
{
	string pname = pixelPackParName( cam_nb );
	if( -1 == pix_pack ) {
		map<string,int>::const_iterator p = 
		                    Espia::Focla::PixelPack.find(pix_pack_str);
		if( p != Espia::Focla::PixelPack.end() )
			pix_pack = p->second;
		else
			pix_pack = 0; // Or throw an exception???
	}
	setParam( pname, pix_pack );
}


/*******************************************************************
 * @fn          Focla::Dev::getTestImage(int &val)
 * @brief       Gets TEST_IMAGE param value
 *
 * @param[out]  val  int reference to TEST_IMAGE value
 *******************************************************************/
void Dev::getTestImage( int &val )
{
	getParam( "TEST_IMAGE", val );
}


/*******************************************************************
 * @fn          Focla::Dev::setTestImage(int val)
 * @brief       Sets TEST_IMAGE param value
 *
 * @param[in]   val  int TEST_IMAGE value
 *******************************************************************/
void Dev::setTestImage( int val )
{
	setParam( "TEST_IMAGE", val );
}


/*******************************************************************
 * @fn          Focla::Dev::getTriggerMode(int &val)
 * @brief       Gets TRIG_MODE param value
 *
 * @param[out]  val  int reference to TRIG_MODE value
 *******************************************************************/
void Dev::getTriggerMode( int &val )
{
	getParam( "TRIG_MODE", val );
}


/*******************************************************************
 * @fn          Focla::Dev::setTriggerMode(int val)
 * @brief       Sets TRIG_MODE param value
 *
 * @param[out]  val  int TRIG_MODE value
 *******************************************************************/
void Dev::setTriggerMode( int val )
{
	setParam( "TRIG_MODE", val );
}


/*******************************************************************
 * @fn          Focla::Dev::triggerImage()
 * @brief       Sets TRIG param to 1
 *******************************************************************/
void Dev::triggerImage()
{
	setParam( "TRIG", 1 );
}


string Dev::ccLevelParName( int cam_nb, int cc_nb )
{
	ostringstream s;
	s <<  "CL" << (cam_nb+1) << "_CC" << (cc_nb+1);
	return s.str();
}


/*******************************************************************
 * @fn          Focla::Dev::ccLevelGet(int cam_nb, int cc_nb,\
 *                                     unsigned int &cc_level)
 * @brief       Gets ccLevel for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[in]   cc_nb  int
 * @param[out]  cc_level  unsigned int ccLevel
 *
 * @see         CCLevel vector, EspiaAcqLib.py
 *******************************************************************/
void Dev::ccLevelGet( int cam_nb, int cc_nb, unsigned int &cc_level )
{
	int pval;
	string pname = ccLevelParName( cam_nb, cc_nb );
	getParam( pname, pval );
	for( unsigned int i=0; i<Espia::Focla::CCLevel.size(); i++ ) {
		if( Espia::Focla::CCLevel[i] == pval ) {
			cc_level = i;
			return;
		}
	}
	throw LIMA_HW_EXC(InvalidValue, "Interanl error"); // ???
}


/*******************************************************************
 * @fn          Focla::Dev::ccLevelSet(int cam_nb, int cc_nb,\
 *                                     unsigned int cc_level)
 * @brief       Sets ccLevel for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[in]   cc_nb  int
 * @param[in]   cc_level  unsigned int ccLevel
 *
 * @see         CCLevel vector, EspiaAcqLib.py
 *******************************************************************/
void Dev::ccLevelSet( int cam_nb, int cc_nb, unsigned int cc_level )
{
	int pval;
	string pname = ccLevelParName( cam_nb, cc_nb );
	if( cc_level < Espia::Focla::CCLevel.size() ) {
		pval = Espia::Focla::CCLevel[cc_level];
		setParam( pname, pval );
	} else {
		throw LIMA_HW_EXC(InvalidValue, "Interanl error"); // ???
	}
}


string Dev::ccSignalName( int cam_nb, int cc_nb )
{
	ostringstream s;
	s << "CC" << (cc_nb + 1);
	return s.str();
}


int Dev::sigNbFromName( const string sname )
{
	map<string, int>::iterator snm = SigName2IdxMap.find(sname);
	if( snm == SigName2IdxMap.end() )
		throw LIMA_HW_EXC(InvalidValue, "No such Focla signal name");
	return snm->second;
}


/*******************************************************************
 * @fn          Focla::Dev::ccPulseStart( int cam_nb, int cc_nb, int polarity,\
 *                                   int width_us, int delay_us, int nb_pulse )
 *
 * @param[in]   cam_nb    int number of the camera
 * @param[in]   cc_nb     int
 * @param[in]   polarity  int
 * @param[in]   width_us  int
 * @param[in]   delay_us  int
 * @param[in]   nb_pulse  int
 *******************************************************************/
void Dev::ccPulseStart( int cam_nb, int cc_nb, int polarity, int width_us,
                        int delay_us, int nb_pulse )
{
	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	int width_arr[2] = {width_us, delay_us};
	int nb_stage = sizeof(width_arr) / sizeof(width_arr[0]);
	CHECK_CALL( focla_sig_pulse_start( m_focla, cam_nb, sig_nb, polarity, 
	                                   width_arr, nb_stage, nb_pulse ) );
}


/*******************************************************************
 * @fn          Focla::Dev::ccPulseStop(int cam_nb, int cc_nb)
 *
 * @param[in]   cam_nb    int number of the camera
 * @param[in]   cc_nb     int
 *******************************************************************/
void Dev::ccPulseStop( int cam_nb, int cc_nb )
{
	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	CHECK_CALL( focla_sig_pulse_stop( m_focla, cam_nb, sig_nb ) );
}


/*******************************************************************
 * @fn          Focla::Dev::ccPulseStatus( int cam_nb, int cc_nb,\
 *               int &pulse_active, int &curr_pulse, int &curr_stage)
 *
 * @param[in]   cam_nb       int number of the camera
 * @param[in]   cc_nb        int
 * @param[out]  pulse_active int reference
 * @param[out]  curr_pulse   int reference
 * @param[out]  curr_stage   int reference
 *******************************************************************/
void Dev::ccPulseStatus( int cam_nb, int cc_nb, int &pulse_active, 
                         int &curr_pulse, int &curr_stage )
{
	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	CHECK_CALL( focla_sig_pulse_status( m_focla, cam_nb, sig_nb, 
				   &pulse_active, &curr_pulse, &curr_stage ) );
}


void Dev::setCLDev( int cam_nb, void *cl_dev )
{
	m_cl_dev[cam_nb] = cl_dev; // XXX What about the del callback in Python?
}


void Dev::getCLDev( int cam_nb, void **cl_dev )
{
	*cl_dev = m_cl_dev[cam_nb];  // XXX
}


void Dev::delCLDev( void *cl_dev )  // XXX This function should be called back
{
	for( unsigned i=0; i<(sizeof(m_cl_dev)/sizeof(m_cl_dev[0])); i++ ) {
		if( m_cl_dev[i] == cl_dev )
			m_cl_dev[i] = NULL;
	}
}


/*******************************************************************
 * @brief Espia::Focla::SerialLine class constructor
 *
 * @param[in] focla_dev  refeernce to Focla::Dev object
 *******************************************************************/
SerialLine::SerialLine( Focla::Dev &focla_dev ) :
	HwSerialLine(),
	m_fdev( focla_dev )
{
}


SerialLine::~SerialLine()
{
}


/*******************************************************************
 * @fn      Focla::SerialLine::getFoclaDev()
 * @return  Espia::Focla::Dev object reference
 *******************************************************************/
Espia::Focla::Dev &SerialLine::getFoclaDev()
{
	return m_fdev;
}


/*******************************************************************
 * @fn      Focla::SerialLine::getEspiaDev()
 * @return  Espia::Dev object reference
 *******************************************************************/
Espia::Dev &SerialLine::getEspiaDev()
{
	return m_fdev.getEspiaDev();
}


/*******************************************************************
 * @fn int Focla::SerialLine::getTimeoutUSec(double timeout_s, bool avail)
 * @brief Get Focla serial line timeout in us
 *
 * This is just an auxiliary function
 *******************************************************************/
int SerialLine::getTimeoutUSec( double timeout_s, bool avail )
{
	if( timeout_s == TimeoutDefault ) {
		if( avail ) {
			timeout_s = TimeoutNoBlock;
		} else {
			getTimeout( timeout_s );
		}
	}

	if( timeout_s > 0 )
		return (int)(timeout_s * 1000000);
	else
		return (int)timeout_s;
}


/*******************************************************************
 * @fn Focla::SerialLine::flush()
 * @brief Flush Focla serial line - read and discard.
 *******************************************************************/
void SerialLine::flush()
{
	CHECK_CALL( focla_ser_flush( m_fdev.getFocla() ) );
}


/*******************************************************************
 * @fn Focla::SerialLine::getNbAvailBytes(int &nb_avail)
 * @brief Get the number of bytes available for reading from Focla serial line
 *
 * @param[out]  nb_avail  int reference 
 *******************************************************************/
void SerialLine::getNbAvailBytes( int &nb_avail )
{
	unsigned long ret_len=0;
	CHECK_CALL( focla_ser_read( m_fdev.getFocla(), NULL, &ret_len, 0 ) );
	nb_avail = (int) ret_len;
}


/*******************************************************************
 * @fn Focla::SerialLine::read(string& buffer, int max_len, double timeout)
 * @brief Read max_len bytest from Focla serial line
 *******************************************************************/
void SerialLine::read( string& buffer, int max_len, double timeout )
{
	bool avail = (max_len == Available);
	if( avail )
		getNbAvailBytes( max_len );

	int timeout_us = getTimeoutUSec(timeout, avail);

	buffer.resize( max_len );
	char *ptr = max_len ? (char *) buffer.data() : NULL;

	unsigned long ret_len = max_len;
	CHECK_CALL( focla_ser_read( m_fdev.getFocla(), ptr, &ret_len, 
	                            timeout_us ) );
	buffer.resize( ret_len );
}


/*******************************************************************
 * @fn Focla::SerialLine::readLine(std::string& buffer, int max_len,\
 *                                 double timeout)
 * @brief Read from Focla serial line until line terminator
 *******************************************************************/
void SerialLine::readLine( std::string& buffer, int max_len, double timeout )
{
	string term;
	getLineTerm( term );

	int timeout_us = getTimeoutUSec( timeout );

	buffer.resize( max_len );
	char *ptr = max_len ? (char *) buffer.data() : NULL;

	unsigned long ret_len = max_len;
	CHECK_CALL( focla_ser_read_str( m_fdev.getFocla(), ptr, &ret_len,
	                                (char *)term.data(), term.size(), 
	                                timeout_us ) );
	buffer.resize( ret_len );
}


/*******************************************************************
 * @fn Focla::SerialLine::write(const std::string& buffer, bool no_wait)
 * @brief Write buffer to Focla serial line
 *******************************************************************/
void SerialLine::write( const std::string& buffer, bool no_wait )
{
	unsigned long nb_wr_bytes = buffer.size();

	char *ptr = nb_wr_bytes ? (char *) buffer.data() : NULL;

	int block_size; getBlockSize( block_size );

	double block_delay; getBlockDelay( block_delay );

	CHECK_CALL( focla_ser_write( m_fdev.getFocla(), ptr, &nb_wr_bytes, 
	                             block_size, getTimeoutUSec(block_delay),
	                             !no_wait ) );
}
