#include "EspiaDev.h"

using namespace lima::Espia;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

const double Dev::ResetLinkTime = 0.5;


const string lima::Espia::OPT_DEBUG_LEVEL   = "DEBUG_LEVEL";
const string lima::Espia::OPT_NO_FIFO_RESET = "NO_FIFO_RESET";

map<string, int> lima::Espia::EspiaDrvOptMap;

const string lima::Espia::PARAM_CHAN_UP_LED = "CHAN_UP_LED";

map<string, int> lima::Espia::EspiaParamMap;


Dev::Dev(int dev_nb)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR1(dev_nb);

	ostringstream os;
	if (dev_nb == MetaDev)
		os << "MetaDev";
	else
		os << "Dev#" << dev_nb;
	DEB_SET_OBJ_NAME(os.str());

	m_dev_nb = Invalid;
	m_dev = ESPIA_DEV_INVAL;

	open(dev_nb);

	initDrvOptMap();
	initParamMap();
}

Dev::~Dev()
{
	DEB_DESTRUCTOR();
	close();
}

void Dev::open(int dev_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(dev_nb);

	if (dev_nb == m_dev_nb) {
		DEB_TRACE() << "Device already opened";
		return;
	}

	close();

	DEB_TRACE() << "Calling espia_open";
	CHECK_CALL(espia_open(dev_nb, &m_dev));
	m_dev_nb = dev_nb;
}

void Dev::close()
{
	DEB_MEMBER_FUNCT();

	if (m_dev_nb == Invalid)
		return;

	DEB_TRACE() << "Calling espia_close";
	CHECK_CALL(espia_close(m_dev));

	m_dev = ESPIA_DEV_INVAL;
	m_dev_nb = Invalid;
}

void Dev::registerCallback(struct espia_cb_data& cb_data, int& cb_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_data.type);

	DEB_TRACE() << "Calling espia_register_callback";
	CHECK_CALL(espia_register_callback(m_dev, &cb_data, &cb_nr));
	DEB_TRACE() << "After espia_register_callback: " << DEB_VAR1(cb_nr);

	try {
		DEB_TRACE() << "Calling espia_callback_active";
		CHECK_CALL(espia_callback_active(m_dev, cb_nr, true));
	} catch (...) {
		unregisterCallback(cb_nr);
		throw;
	}

	DEB_RETURN() << DEB_VAR1(cb_nr);
}

void Dev::unregisterCallback(int& cb_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(cb_nr);

	DEB_TRACE() << "Calling espia_unregister_callback";
	CHECK_CALL(espia_unregister_callback(m_dev, cb_nr));

	cb_nr = Invalid;
}

void Dev::resetLink()
{
	DEB_MEMBER_FUNCT();
	
	DEB_TRACE() << "Calling espia_reset_link";
	CHECK_CALL(espia_reset_link(m_dev));

	DEB_TRACE() << "Sleeping ResetLinkTime=" << ResetLinkTime;
	Sleep(ResetLinkTime);
}

void Dev::getCcdStatus(int& ccd_status)
{
	DEB_MEMBER_FUNCT();

	unsigned char status;

	DEB_TRACE() << "Calling espia_ccd_status";
	CHECK_CALL(espia_ccd_status(m_dev, &status, SCDXIPCI_NO_BLOCK));
	ccd_status = status;
	DEB_RETURN() << DEB_VAR1(DEB_HEX(ccd_status));
}


void Dev::initDrvOptMap()
{
	DEB_MEMBER_FUNCT();

	static bool inited = false;
	if (inited)
		return;

	DEB_TRACE() << "Initing EspiaDrvOptMap";

	int nr_option = -1;
	DEB_TRACE() << "Getting number of options";
	CHECK_CALL(espia_get_option_data(m_dev, &nr_option, NULL));
	DEB_TRACE() << DEB_VAR1(nr_option);
	
	for (int i = 0; i < nr_option; i++) {
		struct espia_option *eoption;
		DEB_TRACE() << "Getting option #" << i;
		CHECK_CALL(espia_get_option_data(m_dev, &i, &eoption));
		
		string option_name = eoption->name;
		DEB_TRACE() << DEB_VAR1(option_name);
		EspiaDrvOptMap[option_name] = eoption->option;
	}

	inited = true;
}


void Dev::getDrvOption(const string& opt_name, int& val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(opt_name);

	map<string, int>::iterator pop = EspiaDrvOptMap.find(opt_name);
	if (pop == EspiaDrvOptMap.end())
		THROW_HW_ERROR(InvalidValue) << "Invalid driver option: " 
					     << DEB_VAR1(opt_name);

	int action = SCDXIPCI_OPT_RD;
	DEB_TRACE() << "Reading driver option #" << pop->second;
	CHECK_CALL(espia_option(m_dev, pop->second, action, &val));

	DEB_RETURN() << DEB_VAR1(val);
}


void Dev::setDrvOption(const string& opt_name, int val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(opt_name, val);

	map<string, int>::iterator pop = EspiaDrvOptMap.find(opt_name);
	if (pop == EspiaDrvOptMap.end())
		THROW_HW_ERROR(InvalidValue) << "Invalid driver option: " 
					     << DEB_VAR1(opt_name);

	int action = SCDXIPCI_OPT_RD_WR;
	DEB_TRACE() << "Writing driver option #" << pop->second << "=" << val;
	CHECK_CALL(espia_option(m_dev, pop->second, action, &val));
}


void Dev::initParamMap()
{
	DEB_MEMBER_FUNCT();

	static bool inited = false;
	if (inited)
		return;

	DEB_TRACE() << "Initing EspiaParamMap";

	int nr_param = -1;
	DEB_TRACE() << "Getting number of params";
	CHECK_CALL(espia_get_param_data(m_dev, &nr_param, NULL));
	DEB_TRACE() << DEB_VAR1(nr_param);
	
	for (int i = 0; i < nr_param; i++) {
		struct espia_param *eparam;
		DEB_TRACE() << "Getting param #" << i;
		CHECK_CALL(espia_get_param_data(m_dev, &i, &eparam));
		
		string param_name = eparam->name;
		DEB_TRACE() << DEB_VAR1(param_name);
		EspiaParamMap[param_name] = i;
	}

	inited = true;
}


void Dev::getParam(const string& param_name, int& val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(param_name);

	map<string, int>::iterator pop = EspiaParamMap.find(param_name);
	if (pop == EspiaParamMap.end())
		THROW_HW_ERROR(InvalidValue) << "Invalid driver param: " 
					     << DEB_VAR1(param_name);

	DEB_TRACE() << "Reading espia param #" << pop->second;
	unsigned int aux_val = val;
	CHECK_CALL(espia_get_param(m_dev, &pop->second, &aux_val, 1));
	val = aux_val;
	DEB_RETURN() << DEB_VAR1(val);
}


void Dev::setParam(const string& param_name, int val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(param_name, val);

	map<string, int>::iterator pop = EspiaParamMap.find(param_name);
	if (pop == EspiaParamMap.end())
		THROW_HW_ERROR(InvalidValue) << "Invalid driver param: " 
					     << DEB_VAR1(param_name);

	DEB_TRACE() << "Writing espia param #" << pop->second << "=" << val;
	unsigned int aux_val = val;
	CHECK_CALL(espia_set_param(m_dev, &pop->second, &aux_val, 1));
}


void Dev::getChanUpLed(int& chan_up_led)
{
	DEB_MEMBER_FUNCT();
	getParam(PARAM_CHAN_UP_LED, chan_up_led);
	DEB_RETURN() << DEB_VAR1(chan_up_led);
}


