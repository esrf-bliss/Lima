//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
/***************************************************************//**
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
#include "MiscUtils.h"

using namespace std;
using namespace lima;
using namespace Espia::Focla;


#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)


const string Espia::Focla::TRIG_MODE    = "TRIG_MODE";
const string Espia::Focla::TRIG         = "TRIG";
const string Espia::Focla::CAM_SEL      = "CAM_SEL";
const string Espia::Focla::TEST_IMAGE   = "TEST_IMAGE";
const string Espia::Focla::CL1_PIX_PACK = "CL1_PIX_PACK";
const string Espia::Focla::CL2_PIX_PACK = "CL2_PIX_PACK";
const string Espia::Focla::CL1_CC1      = "CL1_CC1";
const string Espia::Focla::CL1_CC2      = "CL1_CC2";
const string Espia::Focla::CL1_CC3      = "CL1_CC3";
const string Espia::Focla::CL1_CC4      = "CL1_CC4";
const string Espia::Focla::CL2_CC1      = "CL2_CC1";
const string Espia::Focla::CL2_CC2      = "CL2_CC2";
const string Espia::Focla::CL2_CC3      = "CL2_CC3";
const string Espia::Focla::CL2_CC4      = "CL2_CC4";
const string Espia::Focla::CL1_IN1      = "CL1_IN1";
const string Espia::Focla::CL1_IN2      = "CL1_IN2";
const string Espia::Focla::CL1_IN3      = "CL1_IN3";
const string Espia::Focla::CL2_IN1      = "CL2_IN1";
const string Espia::Focla::CL2_IN2      = "CL2_IN2";
const string Espia::Focla::CL2_IN3      = "CL2_IN3";
const string Espia::Focla::CL1_OUT1     = "CL1_OUT1";
const string Espia::Focla::CL1_OUT2     = "CL1_OUT2";
const string Espia::Focla::CL1_OUT3     = "CL1_OUT3";
const string Espia::Focla::CL2_OUT1     = "CL2_OUT1";
const string Espia::Focla::CL2_OUT2     = "CL2_OUT2";
const string Espia::Focla::CL2_OUT3     = "CL2_OUT3";

map<string, int> Espia::Focla::ParamName2IdxMap;


/***************************************************************//**
 * @brief Initialize ParamName2IdxMap (once)
 *******************************************************************/
void Espia::Focla::initParamName2IdxMap()
{
	DEB_GLOBAL_FUNCT();

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

const map<string, int> Espia::Focla::PixelPack(C_LIST_ITERS(PixelPackList));


/* CL CC Levels: Low, High, Ext */
static const int CCLevelArray[] = {2, 3, 0};

const vector<int> Espia::Focla::CCLevel = 
	vector<int>( CCLevelArray, CCLevelArray +
		(sizeof(CCLevelArray)/sizeof(CCLevelArray[0])) );


const string Espia::Focla::SIG_CC1  = "CC1";
const string Espia::Focla::SIG_CC2  = "CC2";
const string Espia::Focla::SIG_CC3  = "CC3";
const string Espia::Focla::SIG_CC4  = "CC4";
const string Espia::Focla::SIG_OUT1 = "OUT1";
const string Espia::Focla::SIG_OUT2 = "OUT2";
const string Espia::Focla::SIG_OUT3 = "OUT3";
const string Espia::Focla::SIG_TRIG = "TRIG";

map<string, int> Espia::Focla::SigName2IdxMap;

/***************************************************************//**
 * @brief Initialize the SigName2IdxMap (once)
 *******************************************************************/
void Espia::Focla::initSigName2IdxMap()
{
	DEB_GLOBAL_FUNCT();

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


/***************************************************************//**
 * @brief Espia::Focla::Dev class constructor
 *
 * @param[in] espia_dev  reference to Espia::Dev object
 *******************************************************************/
Espia::Focla::Dev::Dev( Espia::Dev &espia_dev ) :
	m_edev(espia_dev)
{
	DEB_CONSTRUCTOR();

	initParamName2IdxMap();
	initSigName2IdxMap();

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


Espia::Focla::Dev::~Dev()
{
	DEB_DESTRUCTOR();

	close();
}


void Espia::Focla::Dev::open()
{
	DEB_MEMBER_FUNCT();

	if( ! m_dev_open ) {
		CHECK_CALL( focla_open( m_edev, &m_focla ) );
		m_dev_open = true;
	}
}


void Espia::Focla::Dev::close()
{
	DEB_MEMBER_FUNCT();

	if( m_dev_open ) {
		CHECK_CALL( focla_close( m_focla ) );
		m_dev_open = false;
//		m_focla = FOCLA_DEV_INVAL; ?
	}
}


/***************************************************************//**
 * @return  Espia::Dev reference
 *******************************************************************/
Espia::Dev &Espia::Focla::Dev::getEspiaDev()
{
	DEB_MEMBER_FUNCT();
	return m_edev;
}


/***************************************************************//**
 * @return  focla_t handler
 *******************************************************************/
focla_t Espia::Focla::Dev::getFocla()
{
	DEB_MEMBER_FUNCT();
	return m_focla;
}


int Espia::Focla::Dev::pIdxFromName( const string pname )
{
	DEB_MEMBER_FUNCT();

	map<string, int>::iterator pnm = ParamName2IdxMap.find(pname);
	if( pnm == ParamName2IdxMap.end() )
		THROW_HW_ERROR(InvalidValue) << "No such Focla param. name: "
					     << DEB_VAR1(pname);
	return pnm->second;
}


void Espia::Focla::Dev::checkMeta() throw(Exception)
{
	DEB_MEMBER_FUNCT();

	if( m_edev.isMeta() )
		THROW_HW_ERROR(NotSupported) 
			<< "Operation not permitted on meta-devices!";
}


/***************************************************************//**
 * @brief       Gets param value from his name
 *
 * @param[in]   pname  string parameter name
 * @param[out]  value  int parameter value
 *******************************************************************/
void Espia::Focla::Dev::getParam( const string pname, int &value )
{
	DEB_MEMBER_FUNCT();

	checkMeta();
	CHECK_CALL( focla_get_param(m_focla, pIdxFromName(pname), &value) );
}


/***************************************************************//**
 * @brief      Sets param value based on param name
 *
 * @param[in]  pname  string parameter name
 * @param[in]  value  int parameter new value
 *******************************************************************/
void Espia::Focla::Dev::setParam( const string pname, int value )
{
	DEB_MEMBER_FUNCT();

	checkMeta();
	CHECK_CALL( focla_set_param(m_focla, pIdxFromName(pname), value) );
}


/***************************************************************//**
 * @brief      Disables camera number cache
 *
 * Sets m_no_cache variable to true
 *******************************************************************/
void Espia::Focla::Dev::disableCache()
{
	DEB_MEMBER_FUNCT();

	m_no_cache = true;
}


/***************************************************************//**
 * @brief      Enables camera number cache
 *
 * Sets m_no_cache variable to false and calls getSelectedCamera()
 *******************************************************************/
void Espia::Focla::Dev::enableCache()
{
	DEB_MEMBER_FUNCT();

	m_no_cache = false;
	getSelectedCamera( m_curr_cam );
}


/***************************************************************//**
 * @brief       Gets Selected camera number
 *
 * @param[out]  curr_cam  int reference to selected camera number
 *******************************************************************/
void Espia::Focla::Dev::getSelectedCamera( int &curr_cam )
{
	DEB_MEMBER_FUNCT();

	getParam( CAM_SEL, curr_cam );
}


/***************************************************************//**
 * @brief       Selects a camera number
 *
 * @param[in]   cam_nb  int number of the camera to select
 *******************************************************************/
void Espia::Focla::Dev::selectCamera( int cam_nb )
{
	DEB_MEMBER_FUNCT();

	if( m_no_cache || (cam_nb != m_curr_cam) ) {
		setParam( CAM_SEL, cam_nb );
		m_curr_cam = cam_nb;
	}
}


string Espia::Focla::Dev::pixelPackParName( int cam_nb )
{
	DEB_MEMBER_FUNCT();

	ostringstream s;
	s << "CL" << (cam_nb+1) << "_PIX_PACK";
	return s.str();
}


/***************************************************************//**
 * @brief       Gets PixelPack param as a string for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[out]  pix_pack_str  string PixelPack
 *
 * @see         PixelPackList, EspiaAcqLib.py
 *******************************************************************/
void Espia::Focla::Dev::getPixelPack( int cam_nb, string &pix_pack_str )
{
	DEB_MEMBER_FUNCT();

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


/***************************************************************//**
 * @brief      Sets PixelPack param for a camera
 *
 * @param[in]  cam_nb  int number of the camera
 * @param[in]  pix_pack  int PixelPack. Default is -1. In that case the next
 *             parameter is taken into account.
 * @param[in]  pix_pack_str  string PixelPack. Default is "Default"
 *
 * @see        PixelPackList, EspiaAcqLib.py
 *******************************************************************/
void Espia::Focla::Dev::setPixelPack( int cam_nb, int pix_pack, 
                                      string pix_pack_str )
{
	DEB_MEMBER_FUNCT();

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


/***************************************************************//**
 * @brief       Gets TEST_IMAGE param value
 *
 * @param[out]  val  int reference to TEST_IMAGE value
 *******************************************************************/
void Espia::Focla::Dev::getTestImage( int &val )
{
	DEB_MEMBER_FUNCT();

	getParam( TEST_IMAGE, val );
}


/***************************************************************//**
 * @brief       Sets TEST_IMAGE param value
 *
 * @param[in]   val  int TEST_IMAGE value
 *******************************************************************/
void Espia::Focla::Dev::setTestImage( int val )
{
	DEB_MEMBER_FUNCT();

	setParam( TEST_IMAGE, val );
}


/***************************************************************//**
 * @brief       Gets TRIG_MODE param value
 *
 * @param[out]  val  int reference to TRIG_MODE value
 *******************************************************************/
void Espia::Focla::Dev::getTriggerMode( int &val )
{
	DEB_MEMBER_FUNCT();

	getParam( TRIG_MODE, val );
}


/***************************************************************//**
 * @brief       Sets TRIG_MODE param value
 *
 * @param[out]  val  int TRIG_MODE value
 *******************************************************************/
void Espia::Focla::Dev::setTriggerMode( int val )
{
	DEB_MEMBER_FUNCT();

	setParam( TRIG_MODE, val );
}


/***************************************************************//**
 * @brief       Sets TRIG param to 1
 *******************************************************************/
void Espia::Focla::Dev::triggerImage()
{
	DEB_MEMBER_FUNCT();

	setParam( TRIG, 1 );
}


string Espia::Focla::Dev::ccLevelParName( int cam_nb, int cc_nb )
{
	DEB_MEMBER_FUNCT();

	ostringstream s;
	s <<  "CL" << (cam_nb+1) << "_CC" << (cc_nb+1);
	return s.str();
}


/***************************************************************//**
 * @brief       Gets ccLevel for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[in]   cc_nb  int
 * @param[out]  cc_level  unsigned int ccLevel
 *
 * @see         CCLevel vector, EspiaAcqLib.py
 *******************************************************************/
void Espia::Focla::Dev::ccLevelGet( int cam_nb, int cc_nb, 
                                    unsigned int &cc_level )
{
	DEB_MEMBER_FUNCT();

	int pval;
	string pname = ccLevelParName( cam_nb, cc_nb );
	getParam( pname, pval );
	for( unsigned int i=0; i<Espia::Focla::CCLevel.size(); i++ ) {
		if( Espia::Focla::CCLevel[i] == pval ) {
			cc_level = i;
			return;
		}
	}
	THROW_HW_ERROR(Error) << "Internal error"; // ???
}


/***************************************************************//**
 * @brief       Sets ccLevel for a camera
 *
 * @param[in]   cam_nb  int number of the camera
 * @param[in]   cc_nb  int
 * @param[in]   cc_level  unsigned int ccLevel
 *
 * @see         CCLevel vector, EspiaAcqLib.py
 *******************************************************************/
void Espia::Focla::Dev::ccLevelSet( int cam_nb, int cc_nb, 
                                    unsigned int cc_level )
{
	DEB_MEMBER_FUNCT();

	int pval;
	string pname = ccLevelParName( cam_nb, cc_nb );
	if( cc_level < Espia::Focla::CCLevel.size() ) {
		pval = Espia::Focla::CCLevel[cc_level];
		setParam( pname, pval );
	} else {
		THROW_HW_ERROR(Error) << "Internal error"; // ???
	}
}


string Espia::Focla::Dev::ccSignalName( int cam_nb, int cc_nb )
{
	DEB_MEMBER_FUNCT();

	ostringstream s;
	s << "CC" << (cc_nb + 1);
	return s.str();
}


int Espia::Focla::Dev::sigNbFromName( const string sname )
{
	DEB_MEMBER_FUNCT();

	map<string, int>::iterator snm = SigName2IdxMap.find(sname);
	if( snm == SigName2IdxMap.end() )
		THROW_HW_ERROR(InvalidValue) << "No such Focla signal name: "
					     << DEB_VAR1(sname);
	return snm->second;
}


/***************************************************************//**
 * @brief       ccPulseStart() function
 *
 * @param[in]   cam_nb    int number of the camera
 * @param[in]   cc_nb     int
 * @param[in]   polarity  int
 * @param[in]   width_us  int
 * @param[in]   delay_us  int
 * @param[in]   nb_pulse  int
 *******************************************************************/
void Espia::Focla::Dev::ccPulseStart( int cam_nb, int cc_nb, int polarity, 
                                      int width_us, int delay_us, int nb_pulse )
{
	DEB_MEMBER_FUNCT();

	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	int width_arr[2] = {width_us, delay_us};
	int nb_stage = sizeof(width_arr) / sizeof(width_arr[0]);
	CHECK_CALL( focla_sig_pulse_start( m_focla, cam_nb, sig_nb, polarity, 
	                                   width_arr, nb_stage, nb_pulse ) );
}


/***************************************************************//**
 * @brief       ccPulseStop() function
 *
 * @param[in]   cam_nb    int number of the camera
 * @param[in]   cc_nb     int
 *******************************************************************/
void Espia::Focla::Dev::ccPulseStop( int cam_nb, int cc_nb )
{
	DEB_MEMBER_FUNCT();

	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	CHECK_CALL( focla_sig_pulse_stop( m_focla, cam_nb, sig_nb ) );
}


/***************************************************************//**
 * @brief       ccPulseStatus() function
 *
 * @param[in]   cam_nb       int number of the camera
 * @param[in]   cc_nb        int
 * @param[out]  pulse_active int reference
 * @param[out]  curr_pulse   int reference
 * @param[out]  curr_stage   int reference
 *******************************************************************/
void Espia::Focla::Dev::ccPulseStatus( int cam_nb, int cc_nb, int &pulse_active, 
                                       int &curr_pulse, int &curr_stage )
{
	DEB_MEMBER_FUNCT();

	string sig_name = ccSignalName( cam_nb, cc_nb );
	int sig_nb = sigNbFromName( sig_name );
	CHECK_CALL( focla_sig_pulse_status( m_focla, cam_nb, sig_nb, 
				   &pulse_active, &curr_pulse, &curr_stage ) );
}


void Espia::Focla::Dev::setCLDev( int cam_nb, void *cl_dev )
{
	DEB_MEMBER_FUNCT();

	m_cl_dev[cam_nb] = cl_dev; // XXX What about the del callback in Python?
}


void Espia::Focla::Dev::getCLDev( int cam_nb, void **cl_dev )
{
	DEB_MEMBER_FUNCT();

	*cl_dev = m_cl_dev[cam_nb];  // XXX
}


void Espia::Focla::Dev::delCLDev( void *cl_dev )  // XXX Should be called back
{
	DEB_MEMBER_FUNCT();

	for( unsigned i=0; i<(sizeof(m_cl_dev)/sizeof(m_cl_dev[0])); i++ ) {
		if( m_cl_dev[i] == cl_dev )
			m_cl_dev[i] = NULL;
	}
}


/***************************************************************//**
 * @brief Espia::Focla::SerialLine class constructor
 *
 * @param[in] focla_dev  refeernce to Focla::Dev object
 *******************************************************************/
Espia::Focla::SerialLine::SerialLine( Focla::Dev &focla_dev ) :
	HwSerialLine(),
	m_fdev( focla_dev )
{
	DEB_CONSTRUCTOR();
}


Espia::Focla::SerialLine::~SerialLine()
{
	DEB_DESTRUCTOR();
}


/***************************************************************//**
 * @return  Espia::Focla::Dev object reference
 *******************************************************************/
Espia::Focla::Dev &Espia::Focla::SerialLine::getFoclaDev()
{
	return m_fdev;
}


/***************************************************************//**
 * @return  Espia::Dev object reference
 *******************************************************************/
Espia::Dev &Espia::Focla::SerialLine::getEspiaDev()
{
	return m_fdev.getEspiaDev();
}


/***************************************************************//**
 * @brief Get Focla serial line timeout in us
 *
 * This is just an auxiliary function
 *******************************************************************/
int Espia::Focla::SerialLine::getTimeoutUSec( double timeout_s, bool avail )
{
	DEB_MEMBER_FUNCT();

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


/***************************************************************//**
 * @brief Flush Focla serial line - read and discard.
 *******************************************************************/
void Espia::Focla::SerialLine::flush()
{
	DEB_MEMBER_FUNCT();

	CHECK_CALL( focla_ser_flush( m_fdev.getFocla() ) );
}


/***************************************************************//**
 * @brief Get the number of bytes available for reading from Focla serial line
 *
 * @param[out]  nb_avail  int reference 
 *******************************************************************/
void Espia::Focla::SerialLine::getNbAvailBytes( int &nb_avail )
{
	DEB_MEMBER_FUNCT();

	unsigned long ret_len=0;
	CHECK_CALL( focla_ser_read( m_fdev.getFocla(), NULL, &ret_len, 0 ) );
	nb_avail = (int) ret_len;
}


/***************************************************************//**
 * @brief Read max_len bytest from Focla serial line
 *******************************************************************/
void Espia::Focla::SerialLine::read( string& buffer, int max_len, 
                                     double timeout )
{
	DEB_MEMBER_FUNCT();

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


/***************************************************************//**
 * @brief Read from Focla serial line until line terminator
 *******************************************************************/
void Espia::Focla::SerialLine::readLine( std::string& buffer, int max_len, 
                                         double timeout )
{
	DEB_MEMBER_FUNCT();

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


/***************************************************************//**
 * @brief Write buffer to Focla serial line
 *******************************************************************/
void Espia::Focla::SerialLine::write( const std::string& buffer, bool no_wait )
{
	DEB_MEMBER_FUNCT();

	unsigned long nb_wr_bytes = buffer.size();

	char *ptr = nb_wr_bytes ? (char *) buffer.data() : NULL;

	int block_size; getBlockSize( block_size );

	double block_delay; getBlockDelay( block_delay );

	CHECK_CALL( focla_ser_write( m_fdev.getFocla(), ptr, &nb_wr_bytes, 
	                             block_size, getTimeoutUSec(block_delay),
	                             !no_wait ) );
}
