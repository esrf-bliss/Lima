#include "EspiaDev.h"

using namespace lima::Espia;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

const double Dev::ResetLinkTime = 0.5;


const string lima::Espia::OPT_DEBUG_LEVEL   = "DEBUG_LEVEL";
const string lima::Espia::OPT_NO_FIFO_RESET = "NO_FIFO_RESET";

map<string, int> lima::Espia::EspiaDrvOptMap;


Dev::Dev(int dev_nb)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM_VAR1(dev_nb);

	ostringstream os;
	if (dev_nb == MetaDev)
		os << "MetaDev";
	else
		os << "Dev#" << dev_nb;
	DEB_SET_OBJ_NAME(os.str());

	m_dev_nb = Invalid;
	m_dev = ESPIA_DEV_INVAL;

	open(dev_nb);

	initEspiaDrvOptMap();
}

Dev::~Dev()
{
	DEB_DESTRUCTOR();
	close();
}

void Dev::open(int dev_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR1(dev_nb);

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
	DEB_PARAM_VAR1(cb_data.type);

	DEB_TRACE() << "Calling espia_register_callback";
	CHECK_CALL(espia_register_callback(m_dev, &cb_data, &cb_nr));
	DEB_TRACE_VAR1(cb_nr);

	try {
		DEB_TRACE() << "Calling espia_callback_active";
		CHECK_CALL(espia_callback_active(m_dev, cb_nr, true));
	} catch (...) {
		unregisterCallback(cb_nr);
		throw;
	}

	DEB_RETURN_VAR1(cb_nr);
}

void Dev::unregisterCallback(int& cb_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR1(cb_nr);

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
	DEB_RETURN_VAR1(ccd_status);
}


void Dev::initEspiaDrvOptMap()
{
	DEB_MEMBER_FUNCT();

	static bool is_init_EspiaDrvOptMap = false;

	if (is_init_EspiaDrvOptMap)
		return;

	DEB_TRACE() << "Initing EspiaDrvOptMap";

	int nr_option = -1;
	DEB_TRACE() << "Getting number of options";
	CHECK_CALL(espia_get_option_data(m_dev, &nr_option, NULL));
	DEB_TRACE_VAR1(nr_option);
	
	for (int i = 0; i < nr_option; i++) {
		struct espia_option *eoption;
		DEB_TRACE() << "Getting option #" << i;
		CHECK_CALL(espia_get_option_data(m_dev, &i, &eoption));
		
		string option_name = eoption->name;
		DEB_TRACE_VAR1(option_name);
		EspiaDrvOptMap[option_name] = eoption->option;
	}

	is_init_EspiaDrvOptMap = true;
}


void Dev::getDrvOption(const string& opt_name, int& val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR1(opt_name);

	map<string, int>::iterator pop = EspiaDrvOptMap.find(opt_name);
	if (pop == EspiaDrvOptMap.end()) {
		DEB_ERROR() << "Invalid driver option: " << opt_name;
		throw LIMA_HW_EXC(InvalidValue, "No such Espia driver option");
	}

	int action = SCDXIPCI_OPT_RD;
	DEB_TRACE() << "Reading driver option #" << pop->second;
	CHECK_CALL(espia_option(m_dev, pop->second, action, &val));

	DEB_RETURN_VAR1(val);
}


void Dev::setDrvOption(const string& opt_name, int val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM_VAR2(opt_name, val);

	map<string, int>::iterator pop = EspiaDrvOptMap.find(opt_name);
	if (pop == EspiaDrvOptMap.end()) {
		DEB_ERROR() << "Invalid driver option: " << opt_name;
		throw LIMA_HW_EXC(InvalidValue, "No such Espia driver option");
	}

	int action = SCDXIPCI_OPT_RD_WR;
	DEB_TRACE() << "Writing driver option #" << pop->second << "=" << val;
	CHECK_CALL(espia_option(m_dev, pop->second, action, &val));
}
