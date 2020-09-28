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
#include <cmath>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <numeric>

#ifdef __linux__ 
#include <dirent.h>
#include <sys/statvfs.h>
#else
#include <direct.h>
#endif

#include "lima/CtSaving.h"
#include "lima/CtEvent.h"
#include "CtSaving_Edf.h"
#include "lima/CtAcquisition.h"
#include "lima/CtBuffer.h"

#ifdef WITH_NXS_SAVING
#include "CtSaving_Nxs.h"
#endif

#ifdef WITH_CBF_SAVING
#include "CtSaving_Cbf.h"
#endif

#ifdef WITH_FITS_SAVING
#include "CtSaving_Fits.h"
#endif

#ifdef WITH_TIFF_SAVING
#include "CtSaving_Tiff.h"
#endif

#ifdef WITH_HDF5_SAVING
#include "CtSaving_Hdf5.h"
#endif

#include "processlib/TaskMgr.h"
#include "processlib/SinkTask.h"

using namespace lima;

static const char DIR_SEPARATOR = '/';
static const int COMPRESSION_PRIORITY = 0;
static const int SAVING_PRIORITY = 1;
/** @brief save task class
 */
class CtSaving::Stream::_SaveTask : public SinkTaskBase
{
	DEB_CLASS_NAMESPC(DebModControl, "CtSaving::Stream::_SaveTask", "Control");
public:
	_SaveTask(CtSaving::Stream& stream, long frame_nr)
		: SinkTaskBase(), m_stream(stream)
	{
		m_stream.prepareWrittingFrame(frame_nr);
	}

	virtual void process(Data& aData)
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(aData);

		m_stream.writeFile(aData, m_header);
	}

	CtSaving::HeaderMap	 m_header;
private:
	CtSaving::Stream& m_stream;
};
/** @brief save callback
 */
class CtSaving::Stream::_SaveCBK : public TaskEventCallback
{
	DEB_CLASS_NAMESPC(DebModControl, "CtSaving::Stream::_SaveCBK", "Control");
public:
	_SaveCBK(Stream& stream) : m_stream(stream) {}
	virtual void finished(Data& aData)
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(aData);

		m_stream.saveFinished(aData);
	}
private:
	Stream& m_stream;
};
/** @brief compression callback
 */
class CtSaving::Stream::_CompressionCBK : public TaskEventCallback
{
	DEB_CLASS_NAMESPC(DebModControl, "CtSaving::_CompressionCBK", "Control");
public:
	_CompressionCBK(Stream& stream) : m_stream(stream) {}
	virtual void started(Data& aData)
	{
		m_stream.compressionStart(aData);
	}
	virtual void finished(Data& aData)
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(aData);

		m_stream.compressionFinished(aData);
	}
private:
	Stream& m_stream;
};
/** @brief manual background saving
 */
class CtSaving::_ManualBackgroundSaveTask : public SinkTaskBase
{
public:
	_ManualBackgroundSaveTask(CtSaving& ct_saving,
		HeaderMap& aHeader) :
		m_saving(ct_saving),
		m_header(aHeader)
	{
	}

	~_ManualBackgroundSaveTask()
	{
		AutoMutex lock(m_saving.m_cond.mutex());
		m_saving.m_cond.broadcast();
	}

	virtual void process(Data& aData)
	{
		m_saving._synchronousSaving(aData, m_header);
	}
private:
	CtSaving& m_saving;
	HeaderMap m_header;
};

class CtSaving::_SavingErrorHandler : public TaskMgr::EventCallback
{
public:
	_SavingErrorHandler(CtSaving& saving, CtEvent& event) :
		m_saving(saving),
		m_event(event) {}

	virtual void error(Data&, const char* errmsg)
	{
		Event* anEvent = new Event(Control, Event::Error, Event::Saving,
			Event::Default, errmsg);
		m_event.reportEvent(anEvent);
		m_saving.m_cond.broadcast();
	}
private:
	CtSaving& m_saving;
	CtEvent& m_event;
};
/** @brief Parameters default constructor
 */
CtSaving::Parameters::Parameters()
	: imageType(Bpp8), nextNumber(0), fileFormat(RAW), savingMode(Manual),
	overwritePolicy(Abort),
	indexFormat("%04d"), framesPerFile(1),
	nbframes(0)
{
}

void CtSaving::Parameters::checkValid() const
{
	DEB_MEMBER_FUNCT();
	switch (fileFormat)
	{
#ifdef WITH_CBF_SAVING
	case CBFFormat:
	case CBFMiniHeader:
		if (framesPerFile > 1)
			THROW_CTL_ERROR(InvalidValue) << "CBF file format does not support "
			"multi frame per file";
		break;
#endif
#ifdef WITH_TIFF_SAVING
	case TIFFFormat:
		if (framesPerFile > 1)
			THROW_CTL_ERROR(InvalidValue) << "TIFF file format does not support "
			"multi frame per file";
		break;
#endif
	case EDF:
	default:
		break;
	}
}


//@brief constructor
CtSaving::Stream::Stream(CtSaving& aCtSaving, int idx)
	: m_saving(aCtSaving), m_idx(idx),
	m_save_cnt(NULL),
	m_pars_dirty_flag(false),
	m_active(false),
	m_compression_cbk(NULL)
{
	DEB_CONSTRUCTOR();

	createSaveContainer();
	m_saving_cbk = new _SaveCBK(*this);
	m_compression_cbk = new _CompressionCBK(*this);
}

//@brief destructor
CtSaving::Stream::~Stream()
{
	DEB_DESTRUCTOR();

	delete m_save_cnt;
	m_saving_cbk->unref();
	m_compression_cbk->unref();
}

const
CtSaving::Parameters& CtSaving::Stream::getParameters(ParameterType type) const
{
	bool from_acq = (type == Acq) || ((type == Auto) && !m_pars_dirty_flag);
	return *(from_acq ? &m_acquisition_pars : &m_pars);
}

CtSaving::Parameters& CtSaving::Stream::getParameters(ParameterType type)
{
	bool from_acq = (type == Acq) || ((type == Auto) && !m_pars_dirty_flag);
	return *(from_acq ? &m_acquisition_pars : &m_pars);
}

void CtSaving::Stream::setParameters(const CtSaving::Parameters& pars)
{
	DEB_MEMBER_FUNCT();

	if (pars.nextNumber == m_acquisition_pars.nextNumber)
		m_pars.nextNumber = pars.nextNumber;

	if (pars == m_pars)
		return;

	pars.checkValid();
	m_pars_dirty_flag = true;
	m_pars = pars;

	DEB_TRACE() << "pars changed";
}

void CtSaving::Stream::setActive(bool active)
{
	DEB_MEMBER_FUNCT();

	if (active == m_active)
		return;

	if (!active)
		m_save_cnt->close();

	m_active = active;
}

void CtSaving::Stream::prepare()
{
	DEB_MEMBER_FUNCT();

	if (m_cnt_status == Open)
		m_save_cnt->close();

	m_cnt_status = Init;

	updateParameters();

	if (hasAutoSaveMode())
		_prepare();
}

void CtSaving::Stream::_prepare()
{
	DEB_MEMBER_FUNCT();
	checkWriteAccess();
	m_save_cnt->prepare(m_saving.m_ctrl);
	m_cnt_status = Prepare;
}

void CtSaving::Stream::close()
{
	m_save_cnt->close();
	m_cnt_status = Init;
}

void CtSaving::Stream::clear()
{
	m_save_cnt->clear();
	m_cnt_status = Init;
}

void CtSaving::Stream::prepareWrittingFrame(long frame_nr)
{
	if (m_cnt_status == Init)
		_prepare();
	m_save_cnt->prepareWrittingFrame(frame_nr);
}

void CtSaving::Stream::updateParameters()
{
	DEB_MEMBER_FUNCT();

	if (!m_pars_dirty_flag)
		return;

	if (m_pars.fileFormat != m_acquisition_pars.fileFormat)
		createSaveContainer();

	m_acquisition_pars = m_pars;
	m_pars_dirty_flag = false;
}

void CtSaving::Stream::createSaveContainer()
{
	DEB_MEMBER_FUNCT();

	int statistic_size = -1;
	int nb_writing_thread = -1;
	bool enable_log_stat = false;

	switch (m_pars.fileFormat) {
	case CBFFormat:
	case CBFMiniHeader:
#ifndef WITH_CBF_SAVING
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the cbf "
			"saving option, not managed";
#endif
		goto common;

	case NXS:
#ifndef WITH_NXS_SAVING
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the nxs "
			"saving option, not managed";
#endif        
		goto common;

	case FITS:
#ifndef WITH_FITS_SAVING
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the fits "
			"saving option, not managed";
#endif        
		goto common;
	case EDFGZ:
#ifndef WITH_Z_COMPRESSION
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the edf gzip "
			"saving option, not managed";
#endif
		goto common;
	case EDFLZ4:
#ifndef WITH_LZ4_COMPRESSION
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the edf lz4 "
			"saving option, not managed";
#endif
		goto common;
	case TIFFFormat:
#ifndef WITH_TIFF_SAVING
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the tiff "
			"saving option, not managed";
#endif
		goto common;
	case HDF5:
#ifndef WITH_HDF5_SAVING
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the hdf5 "
			"saving option, not managed";
#endif
		goto common;
	case HDF5GZ:
#if !defined  (WITH_HDF5_SAVING) && !defined (WITH_Z_COMPRESSION)
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the hdf5 gzip"
			"saving option, not managed";
#endif
		goto common;
	case HDF5BS:
#if !defined  (WITH_HDF5_SAVING) && !defined (WITH_BS_COMPRESSION)
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the hdf5 bs"
			"saving option, not managed";
#endif
		goto common;
	case EDFConcat:
#ifndef __unix
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the edf concat "
			"saving option, not managed";
#endif
		goto common;
	case RAW:
	case EDF:


	common:
		if (m_save_cnt) {
			statistic_size = m_save_cnt->getStatisticSize();
			nb_writing_thread = m_save_cnt->getMaxConcurrentWritingTask();
			m_save_cnt->getEnableLogStat(enable_log_stat);
			m_save_cnt->close();
			delete m_save_cnt;
		}
		break;

	default:
		THROW_CTL_ERROR(NotSupported) << "File format not yet managed";
	}

	switch (m_pars.fileFormat)
	{
	case RAW:
	case EDF:
	case EDFGZ:
	case EDFLZ4:
	case EDFConcat:
		m_save_cnt = new SaveContainerEdf(*this, m_pars.fileFormat);
		break;
#ifdef WITH_CBF_SAVING
	case CBFFormat:
	case CBFMiniHeader:
		m_save_cnt = new SaveContainerCbf(*this, m_pars.fileFormat);
		m_pars.framesPerFile = 1;
		break;
#endif
#ifdef WITH_NXS_SAVING
	case NXS:
		m_save_cnt = new SaveContainerNxs(*this);
		break;
#endif
#ifdef WITH_FITS_SAVING
	case FITS:
		m_save_cnt = new SaveContainerFits(*this);
		break;
#endif
#ifdef WITH_TIFF_SAVING
	case TIFFFormat:
		m_save_cnt = new SaveContainerTiff(*this);
		m_pars.framesPerFile = 1;
		break;
#endif
#ifdef WITH_HDF5_SAVING
	case HDF5:
	case HDF5GZ:
	case HDF5BS:
		m_save_cnt = new SaveContainerHdf5(*this, m_pars.fileFormat);
		break;
#endif
	default:
		break;
	}
	if (statistic_size != -1)
		m_save_cnt->setStatisticSize(statistic_size);
	if (nb_writing_thread != -1)
		m_save_cnt->setMaxConcurrentWritingTask(nb_writing_thread);
	m_save_cnt->setEnableLogStat(enable_log_stat);

	m_cnt_status = Init;
}

void CtSaving::Stream::writeFile(Data& data, HeaderMap& header)
{
	DEB_MEMBER_FUNCT();

	m_save_cnt->writeFile(data, header);
	m_cnt_status = Open;
}


SinkTaskBase* CtSaving::Stream::getTask(TaskType type, const HeaderMap& header, long frame_nr)
{
	DEB_MEMBER_FUNCT();

	SinkTaskBase* save_task;

	if (type == Compression) {
		if (!needCompression())
			return NULL;
		save_task = m_save_cnt->getCompressionTask(header);
		save_task->setEventCallback(m_compression_cbk);
	}
	else {
		_SaveTask* real_task = new _SaveTask(*this, frame_nr);
		real_task->m_header = header;
		save_task = real_task;
		save_task->setEventCallback(m_saving_cbk);
	}

	return save_task;
}

void CtSaving::Stream::compressionFinished(Data& data)
{
	DEB_MEMBER_FUNCT();
	m_save_cnt->compressionFinished(data);
	m_saving._compressionFinished(data, *this);
}

void CtSaving::Stream::saveFinished(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(data, getIndex());

	m_saving._saveFinished(data, *this);
}

class CtSaving::_NewFrameSaveCBK : public HwSavingCtrlObj::Callback
{
public:
	_NewFrameSaveCBK(CtSaving& ct_saving) :
		m_saving(ct_saving)
	{
	}
	bool newFrameWritten(int frame_id)
	{
		return m_saving._newFrameWrite(frame_id);
	}
private:
	CtSaving& m_saving;
};

#ifdef WITH_CONFIG
// --- Config
class CtSaving::_ConfigHandler : public CtConfig::ModuleTypeCallback
{
public:
	_ConfigHandler(CtSaving& saving) :
		CtConfig::ModuleTypeCallback("Saving"),
		m_saving(saving) {}
	virtual void store(Setting& saving_setting)
	{
		CtSaving::Parameters pars;
		m_saving.getParameters(pars);

		saving_setting.set("directory", pars.directory);
		saving_setting.set("prefix", pars.prefix);
		saving_setting.set("suffix", pars.suffix);
		saving_setting.set("options", pars.options);
		saving_setting.set("imageType", convert_2_string(pars.imageType));
		saving_setting.set("nextNumber", pars.nextNumber);
		saving_setting.set("fileFormat", convert_2_string(pars.fileFormat));
		saving_setting.set("savingMode", convert_2_string(pars.savingMode));
		saving_setting.set("overwritePolicy", convert_2_string(pars.overwritePolicy));
		saving_setting.set("indexFormat", pars.indexFormat);
		saving_setting.set("framesPerFile", pars.framesPerFile);
		saving_setting.set("nbframes", pars.nbframes);

		CtSaving::ManagedMode managedmode;
		m_saving.getManagedMode(managedmode);
		saving_setting.set("managedmode", convert_2_string(managedmode));
	}
	virtual void restore(const Setting& saving_setting)
	{
		CtSaving::Parameters pars;
		m_saving.getParameters(pars);

		saving_setting.get("directory", pars.directory);
		saving_setting.get("prefix", pars.prefix);
		saving_setting.get("suffix", pars.suffix);
		saving_setting.get("options", pars.options);

		std::string strimageType;
		if (saving_setting.get("imageType", strimageType))
			convert_from_string(strimageType, pars.imageType);

		int nextNumber;
		if (saving_setting.get("nextNumber", nextNumber))
			pars.nextNumber = nextNumber;

		std::string strfileFormat;
		if (saving_setting.get("fileFormat", strfileFormat))
			convert_from_string(strfileFormat, pars.fileFormat);

		std::string strsavingMode;
		if (saving_setting.get("savingMode", strsavingMode))
			convert_from_string(strsavingMode, pars.savingMode);

		std::string stroverwritePolicy;
		if (saving_setting.get("overwritePolicy", stroverwritePolicy))
			convert_from_string(stroverwritePolicy, pars.overwritePolicy);

		saving_setting.get("indexFormat", pars.indexFormat);

		int framesPerFile;
		if (saving_setting.get("framesPerFile", framesPerFile))
			pars.framesPerFile = framesPerFile;

		int nbframes;
		if (saving_setting.get("nbframes", nbframes))
			pars.nbframes = nbframes;

		std::string strmanagedmode;
		if (saving_setting.get("managedmode", strmanagedmode))
		{
			CtSaving::ManagedMode managedmode;
			convert_from_string(strmanagedmode, managedmode);
			m_saving.getManagedMode(managedmode);
		}

		m_saving.setParameters(pars);
	}
private:
	CtSaving& m_saving;
};
#endif //WITH_CONFIG

//@brief constructor
CtSaving::CtSaving(CtControl& aCtrl) :
	m_ctrl(aCtrl),
	m_stream(NULL),
	m_need_compression(false),
	m_end_cbk(NULL),
	m_managed_mode(Software),
	m_saving_error_handler(NULL)
{
	DEB_CONSTRUCTOR();

	m_nb_stream = 5;
	m_stream = new Stream * [m_nb_stream];
	for (int s = 0; s < m_nb_stream; ++s)
		m_stream[s] = new Stream(*this, s);

	m_stream[0]->setActive(true);

	HwInterface* hw = aCtrl.hwInterface();
#ifdef __linux__
	m_has_hwsaving = hw->getHwCtrlObj(m_hwsaving);
	if (m_has_hwsaving)
	{
		m_new_frame_save_cbk = new _NewFrameSaveCBK(*this);
		m_hwsaving->registerCallback(m_new_frame_save_cbk);
	}
	else
		m_new_frame_save_cbk = NULL;
#else
	m_has_hwsaving = false;
	m_new_frame_save_cbk = NULL;
#endif

	m_format_list.push_back(CtSaving::RAW);
	m_format_list.push_back(CtSaving::EDF);
	m_format_list.push_back(CtSaving::EDFConcat);
#ifdef WITH_LZ4_COMPRESSION
	m_format_list.push_back(CtSaving::EDFLZ4);
#endif
#ifdef WITH_Z_COMPRESSION
	m_format_list.push_back(CtSaving::EDFGZ);
#endif
#ifdef WITH_CBF_SAVING
	m_format_list.push_back(CtSaving::CBFFormat);
	m_format_list.push_back(CtSaving::CBFMiniHeader);
#endif
#ifdef WITH_NXS_SAVING
	m_format_list.push_back(CtSaving::NXS);
#endif
#ifdef WITH_FITS_SAVING
	m_format_list.push_back(CtSaving::FITS);
#endif
#ifdef WITH_TIFF_SAVING
	m_format_list.push_back(CtSaving::TIFFFormat);
#endif
#ifdef WITH_HDF5_SAVING
	m_format_list.push_back(CtSaving::HDF5);
#ifdef WITH_Z_COMPRESSION
	m_format_list.push_back(CtSaving::HDF5GZ);
#endif
#ifdef WITH_BS_COMPRESSION
	m_format_list.push_back(CtSaving::HDF5BS);
#endif
#endif
}

//@brief destructor
CtSaving::~CtSaving()
{
	DEB_DESTRUCTOR();

	for (int s = 0; s < m_nb_stream; ++s)
		delete m_stream[s];
	delete[] m_stream;

	setEndCallback(NULL);
	if (m_has_hwsaving)
	{
		m_hwsaving->unregisterCallback(m_new_frame_save_cbk);
		delete m_new_frame_save_cbk;
	}
	delete m_saving_error_handler;
}

CtSaving::Stream& CtSaving::getStreamExc(int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	THROW_CTL_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(stream_idx);
}

/** @brief set saving parameter for a saving stream

	@param pars parameters for the saving stream
	@param stream_idx the id of the saving stream
 */
void CtSaving::setParameters(const CtSaving::Parameters& pars, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(pars, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	stream.setParameters(pars);
}

/** @brief get the saving stream parameters

	@param pars the return parameters
	@param stream_idx the stream id
 */
void CtSaving::getParameters(CtSaving::Parameters& pars, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	pars = stream.getParameters(Auto);

	DEB_RETURN() << DEB_VAR1(pars);
}
/** @brief set the saving directory for a saving stream
 */
void CtSaving::setDirectory(const std::string& directory, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(directory, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	stream.checkDirectoryAccess(directory);
	pars.directory = directory;
	stream.setParameters(pars);
}
/** @brief get the saving directory for a saving stream
 */
void CtSaving::getDirectory(std::string& directory, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	directory = pars.directory;

	DEB_RETURN() << DEB_VAR1(directory);
}
/** @brief set the filename prefix for a saving stream
 */
void CtSaving::setPrefix(const std::string& prefix, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(prefix, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.prefix = prefix;
	stream.setParameters(pars);
}
/** @brief get the filename prefix for a saving stream
 */
void CtSaving::getPrefix(std::string& prefix, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	prefix = pars.prefix;

	DEB_RETURN() << DEB_VAR1(prefix);
}
/** @brief set the filename suffix for a saving stream
 */
void CtSaving::setSuffix(const std::string& suffix, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(suffix, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.suffix = suffix;
	stream.setParameters(pars);
}
/** @brief get the filename suffix for a saving stream
 */
void CtSaving::getSuffix(std::string& suffix, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	suffix = pars.suffix;

	DEB_RETURN() << DEB_VAR1(suffix);
}

/** @brief set the additional options for a saving stream
 */
void CtSaving::setOptions(const std::string& options, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(options, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.options = options;
	stream.setParameters(pars);
}
/** @brief get the additional options for a saving stream
 */
void CtSaving::getOptions(std::string& options, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	options = pars.options;

	DEB_RETURN() << DEB_VAR1(options);
}

/** @brief set the next number for the filename for a saving stream
 */
void CtSaving::setNextNumber(long number, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(number, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);

	pars.nextNumber = number;
	stream.setParameters(pars);
}
/** @brief get the next number for the filename for a saving stream
 */
void CtSaving::getNextNumber(long& number, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	number = pars.nextNumber;

	DEB_RETURN() << DEB_VAR1(number);
}
/** @brief set the saving format for a saving stream
 */
void CtSaving::setFormat(FileFormat format, int stream_idx)
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.fileFormat = format;
	stream.setParameters(pars);
}
/** @brief get the saving format for a saving stream
 */
void CtSaving::getFormat(FileFormat& format, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	format = pars.fileFormat;

	DEB_RETURN() << DEB_VAR1(format);
}
/** @brief set the saving format as string for a saving stream
 */
void CtSaving::setFormatAsString(const std::string& format, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	FileFormat file_format;
	convert_from_string(format, file_format);
	setFormat(file_format, stream_idx);
}
/** @brief get the saving format as string for a saving stream
 */
void CtSaving::getFormatAsString(std::string& format, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	FileFormat file_format;
	getFormat(file_format, stream_idx);
	format = convert_2_string(file_format);
}
/** @brief get supported format list
 */
void CtSaving::getFormatList(std::list<FileFormat>& format_list) const
{
	DEB_MEMBER_FUNCT();
	format_list = m_format_list;
}

/** @brief get supported format list as string
 */
void CtSaving::getFormatListAsString(std::list<std::string>& format_list) const
{
	DEB_MEMBER_FUNCT();
	for (std::list<FileFormat>::const_iterator i = m_format_list.begin();
		i != m_format_list.end(); ++i) {
		format_list.push_back(convert_2_string(*i));
	}
}

/** @brief force saving suffix to be the default format extension
 */
void CtSaving::setFormatSuffix(int stream_idx)
{
	DEB_MEMBER_FUNCT();
	std::string ext;
	FileFormat format;

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	format = pars.fileFormat;
	pars.fileFormat = format;
	stream.setParameters(pars);
	switch (format)
	{
	case RAW: ext = std::string(".raw"); break;
	case EDF: ext = std::string(".edf"); break;
	case CBFFormat: ext = std::string(".cbf"); break;
	case CBFMiniHeader: ext = std::string(".cbf"); break;
	case NXS: ext = std::string(".nxs"); break;
	case FITS: ext = std::string(".fits"); break;
	case EDFGZ: ext = std::string(".edf.gz"); break;
	case TIFFFormat: ext = std::string(".tiff"); break;
	case EDFConcat: ext = std::string(".edf"); break;
	case EDFLZ4: ext = std::string(".edf.lz4"); break;
	case HDF5: ext = std::string(".h5"); break;
	case HDF5GZ: ext = std::string(".h5"); break;
	case HDF5BS: ext = std::string(".h5"); break;
	default: ext = std::string(".dat");
		break;
	}

	pars.suffix = ext;
	stream.setParameters(pars);
}

/** @brief return a list of hardware possible saving format
 */
void CtSaving::getHardwareFormatList(std::list<std::string>& format_list) const
{
	DEB_MEMBER_FUNCT();

	if (!m_has_hwsaving)
		THROW_CTL_ERROR(NotSupported) << "No hardware saving for this camera";

	m_hwsaving->getPossibleSaveFormat(format_list);
}

void CtSaving::setHardwareFormat(const std::string& format)
{
	DEB_MEMBER_FUNCT();

	if (!m_has_hwsaving)
		THROW_CTL_ERROR(NotSupported) << "No hardware saving for this camera";

	bool found = _checkHwFileFormat(format);

	if (!found)
	{
		THROW_CTL_ERROR(NotSupported) <<
			"Hardware does not support" << DEB_VAR1(format);
	}

	m_specific_hardware_format = format;
}

bool CtSaving::_checkHwFileFormat(const std::string& format) const
{
	std::list<std::string> format_list;
	m_hwsaving->getPossibleSaveFormat(format_list);
	bool found = false;
	for (std::list<std::string>::const_iterator i = format_list.begin();
		!found && i != format_list.end(); ++i)
		found = *i == format;
	return found;
}

void CtSaving::_ReadImage(Data& image, int frameNumber)
{
	DEB_MEMBER_FUNCT();

	if (m_hwsaving->getCapabilities() & HwSavingCtrlObj::MANUAL_READ)
	{
		HwFrameInfoType frame;
		m_hwsaving->readFrame(frame, frameNumber);
		CtBuffer::transformHwFrameInfoToData(image, frame);
	}
	else
		THROW_CTL_ERROR(NotSupported) << "Image read is not supported for this hardware";
}

bool CtSaving::_allStreamReady(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	
	bool ready_flag = true;
	for (int s = 0; ready_flag && s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			ready_flag = stream.isReady(frame_nr);
	}
	
	DEB_RETURN() << DEB_VAR1(ready_flag);	
	return ready_flag;
}
void CtSaving::_waitWritingThreads()
{
	DEB_MEMBER_FUNCT();

	// Waiting all writing thread
	CtControl::Status status;
	m_ctrl.getStatus(status);
	while (status.AcquisitionStatus != AcqFault &&
		!_allStreamReady(-1))
	{
		m_cond.wait();
		m_ctrl.getStatus(status);
	}

	if (status.AcquisitionStatus == AcqFault)
		THROW_CTL_ERROR(Error) << "Acquisition status is in Fault";
}
void CtSaving::getHardwareFormat(std::string& format) const
{
	DEB_MEMBER_FUNCT();

	if (!m_has_hwsaving)
		THROW_CTL_ERROR(NotSupported) << "No hardware saving for this camera";

	format = m_specific_hardware_format;
}

/** @brief set the saving mode for a saving stream
 */
void CtSaving::setSavingMode(SavingMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

	AutoMutex aLock(m_cond.mutex());
	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		Parameters pars = stream.getParameters(Auto);
		pars.savingMode = mode;
		stream.setParameters(pars);
	}
}
/** @brief get the saving mode for a saving stream
 */
void CtSaving::getSavingMode(SavingMode& mode) const
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(0);
	const Parameters& pars = stream.getParameters(Auto);
	mode = pars.savingMode;

	DEB_RETURN() << DEB_VAR1(mode);
}
/** @brief set the overwrite policy for a saving stream
 */
void CtSaving::setOverwritePolicy(OverwritePolicy policy, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(policy, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.overwritePolicy = policy;

	stream.setParameters(pars);
}
/** @brief get the overwrite policy for a saving stream
 */
void CtSaving::getOverwritePolicy(OverwritePolicy& policy,
	int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	policy = pars.overwritePolicy;

	DEB_RETURN() << DEB_VAR1(policy);
}
/** @brief set the number of frame saved per file for a saving stream
 */
void CtSaving::setFramesPerFile(unsigned long frames_per_file, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frames_per_file, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.framesPerFile = frames_per_file;
	stream.setParameters(pars);
}
/** @brief get the number of frame saved per file for a saving stream
 */
void CtSaving::getFramesPerFile(unsigned long& frames_per_file,
	int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	frames_per_file = pars.framesPerFile;

	DEB_RETURN() << DEB_VAR1(frames_per_file);
}

/** @brief set who will manage the saving.
 *
 *  with this methode you can choose who will do the saving
 *   - if mode is set to Software, the saving will be managed by Lima core
 *   - if mode is set to Hardware then it's the sdk or the hardware of the camera that will manage the saving.
 *  @param mode can be either Software or Hardware
*/
void CtSaving::setManagedMode(CtSaving::ManagedMode mode)
{
	DEB_MEMBER_FUNCT();
	if (mode == Hardware && !m_has_hwsaving)
		THROW_CTL_ERROR(InvalidValue) << DEB_VAR1(mode) << "Not supported";

	AutoMutex aLock(m_cond.mutex());
	if (mode == Hardware)
	{
		if (!m_has_hwsaving)
			THROW_CTL_ERROR(NotSupported) << "Hardware saving is not supported";

		int hw_cap = m_hwsaving->getCapabilities();
		if (hw_cap & HwSavingCtrlObj::COMMON_HEADER)
			m_hwsaving->setCommonHeader(m_common_header);
		else if (!m_common_header.empty())
		{
			THROW_CTL_ERROR(Error) << "Hardware saving do not manage common header"
				<< ", clear it first";
		}
	}
	m_managed_mode = mode;
}

void CtSaving::getManagedMode(CtSaving::ManagedMode& mode) const
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	mode = m_managed_mode;
}
void CtSaving::_createStatistic(Data& data)
{
	for (int s = 0; s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.createStatistic(data);
	}
}

void CtSaving::_getTaskList(TaskType type, long frame_nr,
	const HeaderMap& header, TaskList& task_list)
{
	DEB_MEMBER_FUNCT();

	task_list.clear();
	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (stream.isActive()) {
			SinkTaskBase* save_task = stream.getTask(type, header, frame_nr);
			if (save_task)
				task_list.push_back(save_task);
		}
	}
	size_t nb_cbk = task_list.size();
	DEB_TRACE() << DEB_VAR1(nb_cbk);
	FrameCbkCountMap::value_type map_pair(frame_nr, long(nb_cbk));
	AutoMutex aLock(m_cond.mutex());
	m_nb_cbk.insert(map_pair);
}
/** @brief clear the common header
 */
void CtSaving::resetCommonHeader()
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	ManagedMode managed_mode = getManagedMode();
	if (managed_mode == Hardware)
	{
		int hw_cap = m_hwsaving->getCapabilities();
		if (hw_cap & HwSavingCtrlObj::COMMON_HEADER)
			m_hwsaving->resetCommonHeader();
		else
			THROW_CTL_ERROR(NotSupported) << "Common header is not supported";
	}
	m_common_header.clear();
}
/** @brief set the common header.
	This is the header which will be write for all frame for this acquisition
 */
void CtSaving::setCommonHeader(const HeaderMap& header)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(header);

	AutoMutex aLock(m_cond.mutex());
	ManagedMode managed_mode = getManagedMode();
	if (managed_mode == Hardware)
	{
		int hw_cap = m_hwsaving->getCapabilities();
		if (hw_cap & HwSavingCtrlObj::COMMON_HEADER)
			m_hwsaving->setCommonHeader(header);
		else
			THROW_CTL_ERROR(NotSupported) << "Common header is not supported";
	}
	m_common_header = header;
}
/** @brief replace/add field in the common header
 */
void CtSaving::updateCommonHeader(const HeaderMap& header)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(header);

	AutoMutex aLock(m_cond.mutex());
	//Update
	for (HeaderMap::const_iterator i = header.begin();
		i != header.end(); ++i)
	{
		std::pair<HeaderMap::iterator, bool> result =
			m_common_header.insert(HeaderMap::value_type(i->first, i->second));
		//if it exist, update
		if (!result.second)
			result.first->second = i->second;
	}
}
/** @brief get the current common header
 */
void CtSaving::getCommonHeader(HeaderMap& header) const
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	header = HeaderMap(m_common_header);

	DEB_RETURN() << DEB_VAR1(header);
}
/** @brief add/replace a header value in the current common header
 */
void CtSaving::addToCommonHeader(const HeaderValue& value)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(value);

	AutoMutex aLock(m_cond.mutex());
	m_common_header.insert(value);
}
/** @brief add/replace a header value in the current frame header
 */
void CtSaving::addToFrameHeader(long frame_nr, const HeaderValue& value)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_nr, value);

	AutoMutex aLock(m_cond.mutex());
	m_frame_headers[frame_nr].insert(value);
}
/** @brief add/replace several value in the current frame header
 */
void CtSaving::updateFrameHeader(long frame_nr, const HeaderMap& header)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_nr, header);

	AutoMutex aLock(m_cond.mutex());
	HeaderMap& frameHeader = m_frame_headers[frame_nr];
	for (HeaderMap::const_iterator i = header.begin();
		i != header.end(); ++i)
	{
		std::pair<HeaderMap::iterator, bool> result =
			frameHeader.insert(HeaderMap::value_type(i->first, i->second));
		//if it exist, update
		if (!result.second)
			result.first->second = i->second;
	}
	_validateFrameHeader(frame_nr, aLock);
}
/** @brief validate a header for a frame.
	this mean that the header is ready and can now be save.
	If you are in AutoHeader this will trigger the saving if the data frame is available
 */
void CtSaving::validateFrameHeader(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex aLock(m_cond.mutex());
	_validateFrameHeader(frame_nr, aLock);
}

void CtSaving::_validateFrameHeader(long frame_nr,
	AutoMutex& aLock)
{
	SavingMode saving_mode = getAcqSavingMode();
	if (saving_mode != CtSaving::AutoHeader)
		return;

	FrameMap::iterator frame_iter = m_frame_datas.find(frame_nr);
	bool data_available = (frame_iter != m_frame_datas.end());
	bool can_save = _allStreamReady(frame_nr);
	if (!data_available || !(m_need_compression || can_save))
		return;
	Data aData = frame_iter->second;

	HeaderMap task_header;
	FrameHeaderMap::iterator aHeaderIter;
	aHeaderIter = m_frame_headers.find(frame_nr);
	bool keep_header = m_need_compression;
	_takeHeader(aHeaderIter, task_header, keep_header);

	if (!m_need_compression)
		m_frame_datas.erase(frame_iter);

	aLock.unlock();

	TaskType task_type = m_need_compression ? Compression : Save;
	TaskList task_list;
	_getTaskList(task_type, frame_nr, task_header, task_list);

	_postTaskList(aData, task_list,
		m_need_compression ? COMPRESSION_PRIORITY : SAVING_PRIORITY);
}
void CtSaving::_resetReadyFlag()
{
	AutoMutex lock(m_cond.mutex());
	for (int s = 0; s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.setReady(-1);
	}
}
/** @brief get the frame header.

	@param frame_nr the frame id
	@param header the current frame header
 */
void CtSaving::getFrameHeader(long frame_nr, HeaderMap& header) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex aLock(m_cond.mutex());
	FrameHeaderMap::const_iterator i = m_frame_headers.find(frame_nr);
	if (i != m_frame_headers.end())
		header.insert(i->second.begin(), i->second.end());

	DEB_RETURN() << DEB_VAR1(header);
}

/** @brief get the frame header and remove it from the container
 */
void CtSaving::takeFrameHeader(long frame_nr, HeaderMap& header)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex aLock(m_cond.mutex());
	FrameHeaderMap::iterator i = m_frame_headers.find(frame_nr);
	if (i != m_frame_headers.end())
	{
		header = i->second;
		m_frame_headers.erase(i);
	}

	DEB_RETURN() << DEB_VAR1(header);
}
/** @brief remove a frame header

	@param frame_nr the frame id
 */
void CtSaving::removeFrameHeader(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex aLock(m_cond.mutex());
	m_frame_headers.erase(frame_nr);
}
/** @brief remove all frame header
 */
void CtSaving::removeAllFrameHeaders()
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	m_frame_headers.clear();
}

// Private methodes

void CtSaving::_getCommonHeader(HeaderMap& header)
{
	header.insert(m_internal_common_header.begin(),
		m_internal_common_header.end());
	header.insert(m_common_header.begin(), m_common_header.end());
}
void CtSaving::_takeHeader(FrameHeaderMap::iterator& headerIter,
	HeaderMap& header, bool keep_in_map)
{
	_getCommonHeader(header);

	if (headerIter == m_frame_headers.end())
		return;

	HeaderMap& aFrameHeaderMap = headerIter->second;
	for (HeaderMap::iterator i = aFrameHeaderMap.begin();
		i != aFrameHeaderMap.end(); ++i)
	{
		std::pair<HeaderMap::iterator, bool> result =
			header.insert(HeaderMap::value_type(i->first, i->second));
		if (!result.second)
			result.first->second = i->second;
	}

	if (!keep_in_map)
		m_frame_headers.erase(headerIter);
}

void CtSaving::setEndCallback(TaskEventCallback* aCbkPt)
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	if (m_end_cbk)
		m_end_cbk->unref();
	m_end_cbk = aCbkPt;
	if (m_end_cbk)
		m_end_cbk->ref();
}

void CtSaving::resetInternalCommonHeader()
{
	AutoMutex aLock(m_cond.mutex());
	m_internal_common_header.clear();
}

void CtSaving::addToInternalCommonHeader(const HeaderValue& value)
{
	AutoMutex aLock(m_cond.mutex());
	m_internal_common_header.insert(value);
}

bool CtSaving::_controlIsFault()
{
	DEB_MEMBER_FUNCT();
	CtControl::Status status;
	m_ctrl.getStatus(status);
	bool fault = (status.AcquisitionStatus == AcqFault);
	DEB_RETURN() << DEB_VAR1(fault);
	return fault;
}

bool CtSaving::_newFrameWrite(int frame_id)
{
	if (m_end_cbk)
	{
		Data aData;
		aData.frameNumber = frame_id;
		m_end_cbk->finished(aData);
	}
	return !!m_end_cbk;
}

void CtSaving::frameReady(Data& aData)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aData);

	if (_controlIsFault()) {
		DEB_WARNING() << "Skip saving data: " << aData;
		return;
	}

	AutoMutex aLock(m_cond.mutex());

	_createStatistic(aData);

	SavingMode saving_mode = getAcqSavingMode();
	bool auto_header = (saving_mode == AutoHeader);
	long frame_nr = aData.frameNumber;
	FrameHeaderMap::iterator aHeaderIter;
	aHeaderIter = m_frame_headers.find(frame_nr);
	bool header_available = (aHeaderIter != m_frame_headers.end());
	bool can_save = _allStreamReady(frame_nr);
	DEB_TRACE() << DEB_VAR5(saving_mode, m_need_compression, can_save,
		auto_header, header_available);
	if (!(m_need_compression || can_save) ||
		(auto_header && !header_available) || (saving_mode == Manual)) {
		FrameMap::value_type map_pair(frame_nr, aData);
		m_frame_datas.insert(map_pair);
		return;
	}

	HeaderMap task_header;
	bool keep_header = m_need_compression;
	_takeHeader(aHeaderIter, task_header, keep_header);

	aLock.unlock();

	TaskType task_type = m_need_compression ? Compression : Save;	
	TaskList task_list;	
	_getTaskList(task_type, frame_nr, task_header, task_list);

	_postTaskList(aData, task_list,
		m_need_compression ? COMPRESSION_PRIORITY : SAVING_PRIORITY);
}
/** @brief get write statistic
 */
void CtSaving::getStatistic(std::list<double>& saving_speed,
	std::list<double>& compression_speed,
	std::list<double>& compression_ratio,
	std::list<double>& incoming_speed,
	int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	const Stream& stream = getStream(stream_idx);
	stream.getStatistic(saving_speed,
		compression_speed, compression_ratio,
		incoming_speed);
}

void CtSaving::getStatisticCounters(double& saving_speed,
	double& compression_speed,
	double& compression_ratio,
	double& incoming_speed,
	int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	const Stream& stream = getStream(stream_idx);
	std::list<double> saving_speed_list;
	std::list<double> compression_speed_list;
	std::list<double> compression_ratio_list;
	std::list<double> incoming_speed_list;

	stream.getStatistic(saving_speed_list,
		compression_speed_list, compression_ratio_list,
		incoming_speed_list);

	saving_speed = std::accumulate(saving_speed_list.begin(),
		saving_speed_list.end(), 0.);
	if (!saving_speed_list.empty()) saving_speed /= saving_speed_list.size();

	compression_speed = std::accumulate(compression_speed_list.begin(),
		compression_speed_list.end(), 0.);
	if (!compression_speed_list.empty()) compression_speed /= compression_speed_list.size();

	compression_ratio = std::accumulate(compression_ratio_list.begin(),
		compression_ratio_list.end(), 0.);
	if (!compression_ratio_list.empty()) compression_ratio /= compression_ratio_list.size();

	incoming_speed = std::accumulate(incoming_speed_list.begin(),
		incoming_speed_list.end(), 0.);
	if (!incoming_speed_list.empty()) incoming_speed /= incoming_speed_list.size();
}

/** @brief set the size of the write time static list
 */
void CtSaving::setStatisticHistorySize(int aSize, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aSize, stream_idx);

	Stream& stream = getStream(stream_idx);
	stream.setStatisticSize(aSize);
}

/** @brief get the size of the write time static list
 */
int CtSaving::getStatisticHistorySize(int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	const Stream& stream = getStream(stream_idx);
	return stream.getStatisticSize();
}

/** @brief activate/desactivate a stream
 */
void CtSaving::setStreamActive(int stream_idx, bool  active)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(stream_idx, active);

	if ((stream_idx == 0) && !active)
		THROW_CTL_ERROR(InvalidValue) << "Cannot deactivate file stream 0!";

	Stream& stream = getStream(stream_idx);
	stream.setActive(active);
}
/** @brief get if stream is active
 */
void CtSaving::getStreamActive(int stream_idx, bool& active) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);
	const Stream& stream = getStream(stream_idx);
	active = stream.isActive();
	DEB_RETURN() << DEB_VAR1(active);
}

/** @brief get the maximum number of parallel writing tasks
 */
void CtSaving::getMaxConcurrentWritingTask(int& nb_threads, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	const Stream& stream = getStream(stream_idx);
	stream.getMaxConcurrentWritingTask(nb_threads);
}
/** @brief get the maximum number of parallel writing tasks
 */
void CtSaving::setMaxConcurrentWritingTask(int nb_threads, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	Stream& stream = getStream(stream_idx);
	stream.setMaxConcurrentWritingTask(nb_threads);
}

void CtSaving::getEnableLogStat(bool& enable, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	const Stream& stream = getStream(stream_idx);
	stream.getEnableLogStat(enable);
}

void CtSaving::setEnableLogStat(bool enable, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(enable, stream_idx);
	Stream& stream = getStream(stream_idx);
	stream.setEnableLogStat(enable);
}

/** @brief clear everything.
	- all header
	- all waiting data to be saved
	- close all stream
*/
void CtSaving::clear()
{
	DEB_MEMBER_FUNCT();

	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		stream.clear();
	}

	AutoMutex aLock(m_cond.mutex());
	m_frame_headers.clear();
	m_common_header.clear();	// @fix Should we clear common header???
	m_frame_datas.clear();

}

void CtSaving::close()
{
	DEB_MEMBER_FUNCT();
	AutoMutex aLock(m_cond.mutex());
	_close();
}

/** @brief write manually a frame

	@param aFrameNumber the frame id you want to save
	@param aNbFrames the number of frames you want to concatenate
 */
void CtSaving::writeFrame(int aFrameNumber, int aNbFrames, bool synchronous)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aFrameNumber);

	ManagedMode managed_mode;
	{
		AutoMutex lock(m_cond.mutex());

		if (getAcqSavingMode() != Manual)
			THROW_CTL_ERROR(Error) << "Manual saving is only permitted when "
				"saving mode == Manual";

		// wait until the saving is no more used
		_waitWritingThreads();

		if (!m_saving_error_handler)
			m_saving_error_handler = new _SavingErrorHandler(*this, *m_ctrl.event());

		managed_mode = getManagedMode();
	}
	
	if (managed_mode == Hardware) {
		int hw_cap = m_hwsaving->getCapabilities();
		if (hw_cap & HwSavingCtrlObj::MANUAL_WRITE)
			m_hwsaving->writeFrame(aFrameNumber, aNbFrames);
		else
			THROW_CTL_ERROR(NotSupported) << "Manual write is not available";
		return;
	}

	Data anImage2Save;
	m_ctrl.ReadImage(anImage2Save, aFrameNumber, aNbFrames);
	if (aFrameNumber < 0) {
		aFrameNumber = anImage2Save.frameNumber;
		DEB_TRACE() << DEB_VAR1(aFrameNumber);
	}

	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.prepareWrittingFrame(aFrameNumber);
	}

	// Saving
	HeaderMap header;
	{
		AutoMutex lock(m_cond.mutex());
		FrameHeaderMap::iterator aHeaderIter;
		aHeaderIter = m_frame_headers.find(anImage2Save.frameNumber);
		_takeHeader(aHeaderIter, header, false);
	}

	if (synchronous)
		_synchronousSaving(anImage2Save, header);
	else
	{
		TaskMgr* aSavingManualMgrPt = new TaskMgr();
		aSavingManualMgrPt->setEventCallback(m_saving_error_handler);
		Data copyImage = anImage2Save.copy();
		aSavingManualMgrPt->setInputData(copyImage);
		SinkTaskBase* aTaskPt = new CtSaving::_ManualBackgroundSaveTask(*this,
			header);
		aSavingManualMgrPt->addSinkTask(0, aTaskPt);
		aTaskPt->unref();

		PoolThreadMgr::get().addProcess(aSavingManualMgrPt);
	}
}

void CtSaving::_synchronousSaving(Data& anImage2Save, HeaderMap& header)
{
	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (!stream.isActive())
			continue;

		if (stream.needCompression()) {
			SinkTaskBase* aCompressionTaskPt;
			aCompressionTaskPt = stream.getTask(Compression, header, anImage2Save.frameNumber);
			aCompressionTaskPt->setEventCallback(NULL);
			aCompressionTaskPt->process(anImage2Save);
			aCompressionTaskPt->unref();
		}

		stream.writeFile(anImage2Save, header);
	}

	_newFrameWrite(anImage2Save.frameNumber);
}

void CtSaving::_postTaskList(Data& aData,
	const TaskList& task_list, int priority)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aData, task_list.size());

	TaskMgr* aSavingMgrPt = new TaskMgr(priority);
	aSavingMgrPt->setEventCallback(m_saving_error_handler);

	TaskList::const_iterator it, end = task_list.end();
	for (it = task_list.begin(); it != end; ++it) {
		SinkTaskBase* save_task = *it;
		aSavingMgrPt->addSinkTask(0, save_task);
		save_task->unref();
	}
	aSavingMgrPt->setInputData(aData);

	PoolThreadMgr::get().addProcess(aSavingMgrPt);
}

void CtSaving::_compressionFinished(Data& aData, Stream& stream)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aData, stream.getIndex());

	AutoMutex aLock(m_cond.mutex());

	long frame_nr = aData.frameNumber;
	FrameCbkCountMap::iterator count_it = m_nb_cbk.find(frame_nr);
	if (--count_it->second > 0)
		return;
	m_nb_cbk.erase(count_it);

	bool ready_flag = _allStreamReady(frame_nr);

	if (!ready_flag) {
		FrameMap::value_type map_pair(frame_nr, aData);
		m_frame_datas.insert(map_pair);
		return;
	}

	HeaderMap header;
	FrameHeaderMap::iterator header_it = m_frame_headers.find(frame_nr);
	_takeHeader(header_it, header, false);

	aLock.unlock();
	
	TaskList task_list;
	_getTaskList(Save, frame_nr, header, task_list);

	_postTaskList(aData, task_list, SAVING_PRIORITY);
}

void CtSaving::_saveFinished(Data& aData, Stream& stream)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aData, stream.getIndex());

	AutoMutex aLock(m_cond.mutex());

	long frame_nr = aData.frameNumber;
	FrameCbkCountMap::iterator count_it = m_nb_cbk.find(frame_nr);
	if (--count_it->second > 0)
		return;
	m_nb_cbk.erase(count_it);

	//@todo check if the frame is still available
	if (m_end_cbk) {
		aLock.unlock();
		m_end_cbk->finished(aData);
		aLock.lock();
	}

	SavingMode saving_mode = getAcqSavingMode();
	bool auto_saving = (saving_mode == AutoFrame) || (saving_mode == AutoHeader);
	if (!auto_saving)
	{
		if (m_saving_stop) _close();
		m_cond.signal();
		return;
	}

	for (FrameMap::iterator nextDataIter = m_frame_datas.begin();
		nextDataIter != m_frame_datas.end(); ++nextDataIter)
	{
		frame_nr = nextDataIter->first;
		FrameHeaderMap::iterator aHeaderIter = m_frame_headers.find(frame_nr);
		bool header_available = (aHeaderIter != m_frame_headers.end());
		bool can_save = _allStreamReady(frame_nr);
		if (!can_save ||
			((saving_mode == AutoHeader) && !header_available))
			continue;

		Data aNewData = nextDataIter->second;
		m_frame_datas.erase(nextDataIter);

		HeaderMap task_header;
		_takeHeader(aHeaderIter, task_header, false);

		aLock.unlock();

		TaskList task_list;
		_getTaskList(Save, frame_nr, task_header, task_list);

		_postTaskList(aNewData, task_list, SAVING_PRIORITY);
		break;
	}
}

/** @brief this methode set the error saving status in CtControl
 */
void CtSaving::_setSavingError(CtControl::ErrorCode anErrorCode)
{
	DEB_MEMBER_FUNCT();
	// We do not stop the acquisition if we are in Manual Saving
	SavingMode saving_mode = getAcqSavingMode();
	if (saving_mode == Manual)
		return;

	AutoMutex aLock(m_ctrl.m_cond.mutex());
	if (m_ctrl.m_status.AcquisitionStatus != AcqFault)
	{
		m_ctrl.m_status.AcquisitionStatus = AcqFault;
		m_ctrl.m_status.Error = anErrorCode;

		DEB_ERROR() << DEB_VAR1(m_ctrl.m_status);
	}

	aLock.unlock();

	m_ctrl.stopAcq();
}
/** @brief preparing new acquisition
	this methode will resetLastFrameNb if mode is AutoSave
	and validate the parameter for this new acquisition
 */
void CtSaving::_prepare()
{
	DEB_MEMBER_FUNCT();

	if (!hasAutoSaveMode())
		DEB_TRACE() << "No auto save activated";

	AutoMutex aLock(m_cond.mutex());

	if (!m_saving_error_handler)
		m_saving_error_handler = new _SavingErrorHandler(*this, *m_ctrl.event());

	if (m_managed_mode == Software)
	{
		m_need_compression = false;

		//prepare all the active streams
		for (int s = 0; s < m_nb_stream; ++s) {
			Stream& stream = getStream(s);
			if (stream.isActive()) {
				AutoMutexUnlock u(aLock);
				stream.prepare();
				if (stream.needCompression())
					m_need_compression = true;
			}
		}

		m_nb_cbk.clear();

		if (m_has_hwsaving)
		{
			m_hwsaving->stop();
			m_hwsaving->setActive(false);
		}
	}
	else
	{
		for (int s = 0; s < m_nb_stream; ++s) {
			const Stream& stream = getStream(s);
			if (stream.isActive()) {
				Parameters params = stream.getParameters(Auto);

				m_hwsaving->setDirectory(params.directory, s);
				m_hwsaving->setPrefix(params.prefix, s);
				m_hwsaving->setSuffix(params.suffix, s);
				m_hwsaving->setOptions(params.options, s);
				m_hwsaving->setNextNumber(params.nextNumber, s);
				m_hwsaving->setIndexFormat(params.indexFormat, s);
				m_hwsaving->setOverwritePolicy(convert_2_string(params.overwritePolicy), s);
				m_hwsaving->setFramesPerFile(params.framesPerFile, s);
				std::string fileFormat;
				switch (params.fileFormat) {
				case RAW: fileFormat = HwSavingCtrlObj::RAW_FORMAT_STR; break;
				case EDF: fileFormat = HwSavingCtrlObj::EDF_FORMAT_STR; break;
				case CBFFormat: fileFormat = HwSavingCtrlObj::CBF_FORMAT_STR; break;
				case HARDWARE_SPECIFIC: fileFormat = m_specific_hardware_format; break;
				case TIFFFormat: fileFormat = HwSavingCtrlObj::TIFF_FORMAT_STR; break;
				case HDF5: fileFormat = HwSavingCtrlObj::HDF5_FORMAT_STR; break;
				default:
					THROW_CTL_ERROR(NotSupported) << "Not supported yet"; break;
				}

				if (!_checkHwFileFormat(fileFormat))
					THROW_CTL_ERROR(NotSupported) << "Hardware doesn't support " << DEB_VAR1(fileFormat);

				m_hwsaving->setSaveFormat(fileFormat, s);
				m_hwsaving->setActive(true, s);
				m_hwsaving->prepare(s);
				m_hwsaving->start(s);
			}
		}
	}
	m_saving_stop = false;
}

// CtSaving::_stop is only called from CtControl::_stopAcq()
void CtSaving::_stop()
{
	DEB_MEMBER_FUNCT();
	
	// Get the last image acquired counter
	CtControl::ImageStatus img_status;
	m_ctrl.getImageStatus(img_status);

	for (int s = 0; s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
		{
			// Update the number of frames so that SaveContainer::writeFile() will properly
			// call SaveContainer::close()
			stream.updateNbFrames(img_status.LastImageAcquired + 1);
			
			// Clean up the frame parameters so that _allStreamReady() return true
			stream.cleanRemainingFrames(img_status.LastImageAcquired + 1);
		}
	}

	_close();
}

void CtSaving::_close()
{
	DEB_MEMBER_FUNCT();
	
	if (_allStreamReady(-1))
	{
		for (int s = 0; s < m_nb_stream; ++s)
		{
			Stream& stream = getStream(s);
			if (stream.isActive())
				stream.close();
		}
	}
	else
		m_saving_stop = true;
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtSaving::_getConfigHandler()
{
	return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG

CtSaving::SaveContainer::SaveContainer(Stream& stream)
	: m_stream(stream), m_statistic_size(16),
	  m_log_stat_enable(false), m_log_stat_file(NULL),
	  m_max_writing_task(1), m_running_writing_task(0)
{
	DEB_CONSTRUCTOR();
}

CtSaving::SaveContainer::~SaveContainer()
{
	DEB_DESTRUCTOR();
}

void CtSaving::SaveContainer::writeFile(Data& aData, HeaderMap& aHeader)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aData, aHeader);

	Timestamp start_write = Timestamp::now();

	long frameId = aData.frameNumber;

	AutoMutex lock(m_cond.mutex());
	Frame2Params::iterator fpars = m_frame_params.find(frameId);
	if (fpars == m_frame_params.end())
		THROW_CTL_ERROR(Error) << "Can't find saving parameters for frame"
		<< DEB_VAR1(frameId);

	FrameParameters& frame_par = fpars->second;
	const CtSaving::Parameters pars = frame_par.m_pars;
	lock.unlock();

	long write_size;
	Params2Handler::value_type par_handler = open(frame_par);
	try
	{
		write_size = _writeFile(par_handler.second.m_handler, aData, aHeader, pars.fileFormat);
	}
	catch (std::ios_base::failure & error)
	{
		DEB_ERROR() << "Write failed :" << error.what();
#ifdef __linux__ 
		/**       struct statvfs {
		  unsigned long  f_bsize;    // file system block size
		  unsigned long  f_frsize;   //fragment size
		  fsblkcnt_t     f_blocks;   // size of fs in f_frsize units
		  fsblkcnt_t     f_bfree;    // # free blocks
		  fsblkcnt_t     f_bavail;   // # free blocks for non-root
		  fsfilcnt_t     f_files;    // # inodes
		  fsfilcnt_t     f_ffree;    // # free inodes
		  fsfilcnt_t     f_favail;   // # free inodes for non-root
		  unsigned long  f_fsid;     // file system ID
		  unsigned long  f_flag;     //mount flags
		  unsigned long  f_namemax;  // maximum filename length
				  }
		*/
		//Check if disk full
		struct statvfs vfs;
		if (!statvfs(pars.directory.c_str(), &vfs))
		{
			if (vfs.f_favail < 1024 || vfs.f_bavail < 1024)
			{
				m_stream.setSavingError(CtControl::SaveDiskFull);
				try {
					close(&pars);
				}
				catch (...) {
				}
				THROW_CTL_ERROR(Error) << "Disk full!!!";
			}
		};


#endif
		m_stream.setSavingError(CtControl::SaveUnknownError);
		try {
			close(&pars);
		}
		catch (...) {
		}
		THROW_CTL_ERROR(Error) << "Save unknown error";
	}
	catch (...)
	{
		m_stream.setSavingError(CtControl::SaveUnknownError);
		try {
			close(&pars);
		}
		catch (...) {
		}
		THROW_CTL_ERROR(Error) << "Save unknown error";
	}

	lock.lock();
	++m_written_frames;
	bool acq_end = (m_written_frames == m_nb_frames_to_write);

	m_frame_params.erase(frameId);
	--m_running_writing_task;
	lock.unlock();

	if (pars.overwritePolicy != MultiSet || acq_end) // Close at the end
	{
		try {
			close(&pars, acq_end);
		}
		catch (...) {
			m_stream.setSavingError(CtControl::SaveCloseError);
			THROW_CTL_ERROR(Error) << "Save file close error";
		}
	}

	Timestamp end_write = Timestamp::now();

	Timestamp diff = end_write - start_write;
	DEB_TRACE() << "Write took : " << diff << "s";

	lock.lock();
	writeFileStat(aData, start_write, end_write, write_size);
}

void CtSaving::SaveContainer::writeFileStat(Data& aData, Timestamp start, Timestamp end, long wsize)
{
	long frameId = aData.frameNumber;

	if (long(m_statistic.size()) >= m_statistic_size)
		m_statistic.erase(m_statistic.begin());

	StatisticsType::iterator i = m_statistic.find(frameId);
	if (i != m_statistic.end())
	{
		i->second.writing_start = start;
		i->second.writing_end = end;
		i->second.write_size = wsize;
		if (m_log_stat_enable && m_log_stat_file) {
			double comp_time = 0., comp_rate = 0., comp_ratio = 0.;
			double write_time, write_rate, total_time;

			if (i->second.compression_end.isSet() && i->second.compression_start.isSet()) {
				comp_time = i->second.compression_end - i->second.compression_start;
				comp_rate = i->second.incoming_size / comp_time / 1024. / 1024.;
				comp_ratio = i->second.incoming_size / i->second.write_size;
			}

			write_time = i->second.writing_end - i->second.writing_start;
			write_rate = i->second.write_size / write_time / 1024. / 1024.;
			total_time = i->second.writing_end - i->second.received_time;

			fprintf(m_log_stat_file, "%ld %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", \
				frameId, (double)i->second.received_time, \
				comp_time * 1000.0, comp_rate, comp_ratio, \
				write_time * 1000.0, write_rate, total_time * 1000.0);
		}
	}
}

void CtSaving::SaveContainer::setEnableLogStat(bool enable)
{
	// TODO: check that no current saving is active
	AutoMutex aLock = AutoMutex(m_cond.mutex());
	if (m_log_stat_enable && !enable) {
		fclose(m_log_stat_file);
		m_log_stat_file = NULL;
	}
	m_log_stat_enable = enable;
}

void CtSaving::SaveContainer::getEnableLogStat(bool& enable) const
{
	enable = m_log_stat_enable;
}

void CtSaving::SaveContainer::prepareLogStat(const CtSaving::Parameters& pars)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << DEB_VAR1(m_log_stat_enable);

	int stream_idx = m_stream.getIndex();

	if (m_log_stat_enable) {
		if (m_log_stat_directory.empty()) {
			m_log_stat_directory = pars.directory;
		}
		else {
			// check if directory changed, then update
			if (m_log_stat_directory != pars.directory)
				m_log_stat_directory = pars.directory;
			else
				m_stream.checkDirectoryAccess(m_log_stat_directory);
		}

		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%Y%m%d", timeinfo);
		std::string log_name(buffer);
		std::ostringstream stream_str;
		std::string new_path;
		stream_str << stream_idx;
		log_name = log_name + "_" + stream_str.str() + ".log";
		new_path = m_log_stat_directory + DIR_SEPARATOR + log_name;

		if (new_path != m_log_stat_filename) {
			if (m_log_stat_file)
				fclose(m_log_stat_file);
			DEB_TRACE() << "Open Stat Log File : " << new_path;
			m_log_stat_file = fopen(new_path.c_str(), "aw");
			if (!m_log_stat_file) {
				m_log_stat_filename = "";
				DEB_ERROR() << "Cannot create log stat file";
			}
			else {
				m_log_stat_filename = new_path;
			}
		}
		if (m_log_stat_file) {
			std::ostringstream head_str;
			head_str << pars;
			strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", timeinfo);
			fprintf(m_log_stat_file, "\n# %s\n# %s\n", buffer, head_str.str().c_str());
			fprintf(m_log_stat_file, "# frame frame_time(sec) comp_time(msec) comp_rate(MB/s) comp_ratio write_time(msec) write_rate(MB/s) total_time(msec)\n");
		}
	}
}

void CtSaving::SaveContainer::setStatisticSize(int aSize)
{
	AutoMutex aLock = AutoMutex(m_cond.mutex());
	if (long(m_statistic.size()) > aSize)
	{
		size_t aDiffSize = m_statistic.size() - aSize;
		for (StatisticsType::iterator i = m_statistic.begin();
			aDiffSize; --aDiffSize)
			m_statistic.erase(i++);
	}
	m_statistic_size = aSize;
}

int CtSaving::SaveContainer::getStatisticSize() const
{
	AutoMutex aLock(m_cond.mutex());
	return m_statistic_size;
}

inline bool _sort_stat_timing(const std::pair<Timestamp, CtSaving::SaveContainer::Stat*>& s1,
	const std::pair<Timestamp, CtSaving::SaveContainer::Stat*>& s2)
{
	return s1.first < s2.first;
}



void CtSaving::SaveContainer::getStatistic(std::list<double>& writing_speed,
	std::list<double>& compression_speed,
	std::list<double>& compression_ratio,
	std::list<double>& incoming_speed) const
{



	AutoMutex aLock = AutoMutex(m_cond.mutex());
	StatisticsType copy = m_statistic;
	aLock.unlock();

	StatisticsType::const_iterator next = copy.begin();
	if (next != copy.end())
		for (StatisticsType::const_iterator prev = next++;
			next != copy.end(); prev = next++)
			incoming_speed.push_back(prev->second.incoming_size /
			(next->second.received_time -
				prev->second.received_time));
	//Writing/compression statistic
	typedef std::list<std::pair<Timestamp, Stat*> > Index;
	Index writing_index;
	Index compression_index;

	for (StatisticsType::iterator i = copy.begin();
		i != copy.end(); ++i)
	{
		if (i->second.writing_start.isSet() &&
			i->second.writing_end.isSet())
		{
			Index::value_type s(i->second.writing_start,
				&i->second);
			writing_index.push_back(s);
			Index::value_type e(i->second.writing_end,
				&i->second);
			writing_index.push_back(e);
			compression_ratio.push_back(double(i->second.incoming_size) /
				i->second.write_size);
		}
		if (i->second.compression_start.isSet() &&
			i->second.compression_end.isSet())
		{
			Index::value_type s(i->second.compression_start,
				&i->second);
			compression_index.push_back(s);
			Index::value_type e(i->second.compression_end,
				&i->second);
			compression_index.push_back(e);
		}
	}
	writing_index.sort(_sort_stat_timing);
	compression_index.sort(_sort_stat_timing);

	//Writing mean
	std::set<Stat*> current;
	for (Index::iterator i = writing_index.begin();
		i != writing_index.end(); ++i)
	{
		if (i->first - i->second->writing_start < 1e-9) // start
			current.insert(i->second);
		else			// end
			current.erase(i->second);

		if (current.empty()) continue;

		double mean = 0.;
		for (std::set<Stat*>::iterator k = current.begin();
			k != current.end(); ++k)
			mean += (*k)->write_size / ((*k)->writing_end - (*k)->writing_start);
		writing_speed.push_back(mean);
	}
	current.clear();

	//Compression mean
	for (Index::iterator i = compression_index.begin();
		i != compression_index.end(); ++i)
	{
		if (i->first - i->second->compression_start < 1e-9) // start
			current.insert(i->second);
		else			// end
			current.erase(i->second);

		if (current.empty()) continue;

		double mean = 0.;
		for (std::set<Stat*>::iterator k = current.begin();
			k != current.end(); ++k)
			mean += (*k)->incoming_size / ((*k)->compression_end - (*k)->compression_start);
		compression_speed.push_back(mean);
	}
}

void CtSaving::SaveContainer::getParameters(CtSaving::Parameters& pars) const
{
	pars = m_stream.getParameters(Acq);
}


void CtSaving::SaveContainer::clear()
{
	DEB_MEMBER_FUNCT();

	this->close();

	AutoMutex aLock(m_cond.mutex());
	m_statistic.clear();
	_clear();			// call inheritance if needed
}

void CtSaving::SaveContainer::prepare(CtControl& ct)
{
	DEB_MEMBER_FUNCT();
	int nb_frames;
	ct.acquisition()->getAcqNbFrames(nb_frames);
	CtSaving::Parameters pars = m_stream.getParameters(Auto);
	AutoMutex lock(m_cond.mutex());
	m_statistic.clear();
	m_nb_frames_to_write = nb_frames;
	m_written_frames = 0;
	if (m_nb_frames_to_write && 	// if not live
		pars.savingMode != CtSaving::Manual)
	{
		long nextNumber = pars.nextNumber - 1;
		for (long i = 0; i < nb_frames; ++i)
		{
			FrameParameters frame_par(pars);

			if (pars.overwritePolicy == MultiSet)
				frame_par.m_pars.framesPerFile = 1; // force to 1
			else
			{
				bool new_file = !(i % pars.framesPerFile);
				if (new_file) ++nextNumber;
				frame_par.m_pars.nextNumber = nextNumber;
				frame_par.m_threadable = new_file;
			}
			std::pair<Frame2Params::iterator, bool> result =
				m_frame_params.insert(Frame2Params::value_type(i, frame_par));
			if (!result.second)
				result.first->second = frame_par;
		}
	}
	m_running_writing_task = 0;
	prepareLogStat(pars);
	lock.unlock();
	_prepare(ct);			// call inheritance if needed
}

void CtSaving::SaveContainer::updateNbFrames(long last_acquired_frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << DEB_VAR1(last_acquired_frame_nr);
	
	AutoMutex lock(m_cond.mutex());
	m_nb_frames_to_write = last_acquired_frame_nr;
}

bool CtSaving::SaveContainer::isReady(long frame_nr) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex lock(m_cond.mutex());
	bool ready;

	// mean all writing tasks
	if (frame_nr < 0)
	{
		ready = m_frame_params.empty();
		for (Frame2Params::const_iterator i = m_frame_params.begin();
			ready && i != m_frame_params.end(); ++i)
			ready = !i->second.m_running;
	}
	else
	{
		Frame2Params::const_iterator it = m_frame_params.find(frame_nr);
		if (it == m_frame_params.end())
		{
			// if no task is running then ready
			ready =  m_frame_params.empty();
		}
		else if (it->second.m_threadable)
		{
			DEB_TRACE() << DEB_VAR2(m_running_writing_task, m_max_writing_task);
			
			ready = (m_running_writing_task + 1 <= m_max_writing_task);
		}
		else
			// ready if we are the next (the first)
			ready = (it == m_frame_params.begin());
	}

	DEB_RETURN() << DEB_VAR1(ready);
	return ready;
}

void CtSaving::SaveContainer::cleanRemainingFrames(long last_acquired_frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(last_acquired_frame_nr);

	AutoMutex lock(m_cond.mutex());
	m_frame_params.erase(
		m_frame_params.find(last_acquired_frame_nr),
		m_frame_params.end());
}

void CtSaving::SaveContainer::setReady(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	AutoMutex lock(m_cond.mutex());
	// mean all frames
	if (frame_nr < 0)
		m_frame_params.clear();
	else
	{
		Frame2Params::iterator i = m_frame_params.find(frame_nr);
		if (i != m_frame_params.end())
			m_frame_params.erase(i);
	}
}

void CtSaving::SaveContainer::prepareWrittingFrame(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	
	AutoMutex lock(m_cond.mutex());
	Frame2Params::iterator i = m_frame_params.find(frame_nr);
	if (i == m_frame_params.end())
	{
		CtSaving::Parameters pars = m_stream.getParameters(Acq);
		FrameParameters frame_par(pars);
		frame_par.m_running = true;
		m_frame_params.insert(Frame2Params::value_type(frame_nr, frame_par));
	}
	else
		i->second.m_running = true;
	++m_running_writing_task;
}

void CtSaving::SaveContainer::createStatistic(Data& data)
{
	AutoMutex lock(m_cond.mutex());
	//Insert statistic
	StatisticsType::value_type stat_pair(data.frameNumber, Stat());
	stat_pair.second.incoming_size = data.size();
	m_statistic.insert(stat_pair);
}

void CtSaving::SaveContainer::compressionStart(Data& data)
{
	Timestamp start = Timestamp::now();
	AutoMutex lock(m_cond.mutex());
	StatisticsType::iterator i = m_statistic.find(data.frameNumber);
	if (i != m_statistic.end())
		i->second.compression_start = start;
}

void CtSaving::SaveContainer::compressionFinished(Data& data)
{
	Timestamp end = Timestamp::now();
	AutoMutex lock(m_cond.mutex());
	StatisticsType::iterator i = m_statistic.find(data.frameNumber);
	if (i != m_statistic.end())
		i->second.compression_end = end;
}

int CtSaving::SaveContainer::getMaxConcurrentWritingTask() const
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_max_writing_task);

	return m_max_writing_task;
}

void CtSaving::SaveContainer::setMaxConcurrentWritingTask(int nb_thread)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_thread);

	if (nb_thread < 1)
		THROW_CTL_ERROR(Error) << "Can't set number of writing threads below 1"
		<< DEB_VAR1(nb_thread);
	m_max_writing_task = nb_thread;
}
CtSaving::SaveContainer::Params2Handler::value_type
CtSaving::SaveContainer::open(FrameParameters& fpars)
{
	DEB_MEMBER_FUNCT();

	AutoMutex lock(m_cond.mutex());

	CtSaving::Parameters& pars = fpars.m_pars;
	Params2Handler::iterator handler = m_params_handler.find(pars);

	if (handler != m_params_handler.end())
		return Params2Handler::value_type(handler->first, handler->second);
	else
	{
		lock.unlock();

		std::string aFileName = pars.directory + DIR_SEPARATOR + pars.prefix;
		long index = pars.nextNumber;
		char idx[64];
		if (index < 0) index = 0;
		snprintf(idx, sizeof(idx), pars.indexFormat.c_str(), index);
		aFileName += idx;
		aFileName += pars.suffix;

		DEB_TRACE() << DEB_VAR1(aFileName);

		if (pars.overwritePolicy == Abort &&
			!access(aFileName.c_str(), R_OK))
		{
			m_stream.setSavingError(CtControl::SaveOverwriteError);
			std::string output;
			output = "Try to over write file: " + aFileName;
			THROW_CTL_ERROR(Error) << output;
		}
		std::ios_base::openmode openFlags = std::ios_base::out | std::ios_base::binary;
		if (pars.overwritePolicy == Append ||
			pars.overwritePolicy == MultiSet)
			openFlags |= std::ios_base::app;
		else if (pars.overwritePolicy == Overwrite)
			openFlags |= std::ios_base::trunc;

		std::string error_desc;
		Handler handler;
		for (int nbTry = 0; nbTry < 5; ++nbTry)
		{
			try {
				handler.m_handler = _open(aFileName, openFlags);
			}
			catch (std::ios_base::failure & error) {
				error_desc = error.what();
				DEB_WARNING() << "Could not open " << aFileName << ": "
					<< error_desc;
			}
			catch (...) {
				error_desc = "Unknown error";
				DEB_WARNING() << "Could not open " << aFileName << ": "
					<< error_desc;
			}

			if (!handler.m_handler)
			{
				std::string output;

				if (access(pars.directory.c_str(), W_OK))
				{
					m_stream.setSavingError(CtControl::SaveAccessError);
					output = "Can not write in directory: " + pars.directory;
					THROW_CTL_ERROR(Error) << output;
				}
			}
			else
			{
				DEB_TRACE() << "Open file: " << aFileName;
				handler.m_nb_frames = pars.framesPerFile;
				lock.lock();
				Params2Handler::value_type map_pair(pars, handler);
				std::pair<Params2Handler::iterator, bool> result =
					m_params_handler.insert(map_pair);
				return Params2Handler::value_type(result.first->first,
					result.first->second);
			}
		}

		if (!handler.m_handler)
		{
			m_stream.setSavingError(CtControl::SaveOpenError);
			std::string output;
			output = "Failure opening " + aFileName;
			if (!error_desc.empty())
				output += ": " + error_desc;
			THROW_CTL_ERROR(Error) << output;
		}
	}
	// we can't reach this line (normally) just for compiler
	return Params2Handler::value_type(CtSaving::Parameters(), Handler());
}

void CtSaving::SaveContainer::close(const CtSaving::Parameters* params,
	bool force_close)
{
	DEB_MEMBER_FUNCT();

	AutoMutex aLock(m_cond.mutex());
	if (!params)			// close all
	{
		for (Params2Handler::iterator i = m_params_handler.begin();
			i != m_params_handler.end(); ++i)
		{
			if (i->second.m_handler)
				_close(i->second.m_handler);
		}
		m_params_handler.clear();
	}
	else
	{
		Params2Handler::iterator handler = m_params_handler.find(*params);
		if (force_close || !--handler->second.m_nb_frames)
		{
			void* raw_handler = handler->second.m_handler;
			const Parameters frame_pars = handler->first;
			m_params_handler.erase(handler);
			aLock.unlock();
			_close(raw_handler);

			Parameters& pars = m_stream.getParameters(Acq);
			if (pars.overwritePolicy != MultiSet &&
				pars.overwritePolicy != Append)
			{
				int nextNumber = frame_pars.nextNumber + 1;
				aLock.lock();
				if (pars.nextNumber < nextNumber)
					pars.nextNumber = nextNumber;
			}
		}
	}

	// flush log file each time a frame file is closed
	aLock.unlock();
	if (m_log_stat_enable)
		fflush(m_log_stat_file);
}

void CtSaving::SaveContainer::_setBuffer(int frameNumber, ZBufferList&& buffers)
{
	AutoMutex aLock(m_lock);
	std::pair<dataId2ZBufferList::iterator, bool> result;
	result = m_buffers.emplace(std::move(frameNumber), std::move(buffers));
	if (!result.second)
		result.first->second = std::move(buffers);
}

ZBufferList CtSaving::SaveContainer::_takeBuffers(int dataId)
{
	AutoMutex aLock(m_lock);
	dataId2ZBufferList::iterator i = m_buffers.find(dataId);
	ZBufferList aReturnBufferPt(std::move(i->second));
	m_buffers.erase(i);
	return aReturnBufferPt;
}

void CtSaving::SaveContainer::_clear()
{
	AutoMutex aLock(m_lock);
	m_buffers.clear();
}

/** @brief check if all file can be written
 */
#ifdef WIN32
#define CTSAVING_MKDIR(a,b) _mkdir(a)
#else
#define CTSAVING_MKDIR(a,b) mkdir(a,b)
#endif
void CtSaving::Stream::checkWriteAccess()
{
	DEB_MEMBER_FUNCT();
	std::string output;
	// check if directory exist
	DEB_TRACE() << "Check if directory exist";
	if (!access(m_pars.directory.c_str(), F_OK))
	{
		// check if it's a directory
		struct stat aDirectoryStat;
		if (stat(m_pars.directory.c_str(), &aDirectoryStat))
		{
			output = "Can stat directory : " + m_pars.directory;
			THROW_CTL_ERROR(Error) << output;
		}
		DEB_TRACE() << "Check if it's really a directory";
		if (!S_ISDIR(aDirectoryStat.st_mode))
		{
			output = "Path : " + m_pars.directory + " is not a directory";
			THROW_CTL_ERROR(Error) << output;
		}

		// check if it's writable
		DEB_TRACE() << "Check if directory is writable";
		if (access(m_pars.directory.c_str(), W_OK))
		{
			output = "Directory : " + m_pars.directory + " is not writable";
			THROW_CTL_ERROR(Error) << output;
		}
	}
	else if (CTSAVING_MKDIR(m_pars.directory.c_str(), 0777))
	{
		output = "Directory : " + m_pars.directory + " can't be created";
		THROW_CTL_ERROR(Error) << output;
	}
	else				// Creation was successful don't need to test other thing
		return;

	// test all file is mode == Abort
	if (m_pars.overwritePolicy == Abort)
	{

		CtAcquisition* anAcq = m_saving.m_ctrl.acquisition();
		int nbAcqFrames;
		anAcq->getAcqNbFrames(nbAcqFrames);
		int framesPerFile = m_pars.framesPerFile;
		int nbFiles = (nbAcqFrames + framesPerFile - 1) / framesPerFile;
		int firstFileNumber = m_acquisition_pars.nextNumber;
		int lastFileNumber = m_acquisition_pars.nextNumber + nbFiles - 1;

#ifdef WIN32
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;
		const int maxNameLen = FILENAME_MAX;
		char filesToSearch[maxNameLen];

		sprintf_s(filesToSearch, FILENAME_MAX, "%s/*.*", m_pars.directory.c_str());
		if ((hFind = FindFirstFile(filesToSearch, &FindFileData)) == INVALID_HANDLE_VALUE)
#else
		struct dirent buffer;
		struct dirent* result;
		const int maxNameLen = 256;

		DIR* aDirPt = opendir(m_pars.directory.c_str());
		if (!aDirPt)
#endif
		{
			output = "Can't open directory : " + m_pars.directory;
			THROW_CTL_ERROR(Error) << output;
		}


		bool errorFlag = false;
		char testString[maxNameLen];
		snprintf(testString, sizeof(testString),
			"%s%s%s",
			m_pars.prefix.c_str(),
			m_pars.indexFormat.c_str(),
			m_pars.suffix.c_str());

		char firstFileName[maxNameLen], lastFileName[maxNameLen];
		snprintf(firstFileName, maxNameLen, testString, firstFileNumber);
		snprintf(lastFileName, maxNameLen, testString, lastFileNumber);
		DEB_TRACE() << "Test if file between: " DEB_VAR2(firstFileName, lastFileName);

		char* fname;

		int fileIndex;

#ifdef WIN32
		BOOL doIt = true;
		while (!errorFlag && doIt) {
			fname = FindFileData.cFileName;
			doIt = FindNextFile(hFind, &FindFileData);

			if (sscanf_s(fname, testString, &fileIndex) == 1)
#else
		int returnFlag;    // not used???
		while (!errorFlag &&
			!(returnFlag = readdir_r(aDirPt, &buffer, &result)) && result) {
			fname = result->d_name;
			if (sscanf(result->d_name, testString, &fileIndex) == 1)
#endif
			{
				char auxFileName[maxNameLen];
				snprintf(auxFileName, maxNameLen, testString, fileIndex);
				if ((strncmp(fname, auxFileName, maxNameLen) == 0) &&
					(fileIndex >= firstFileNumber) && (fileIndex <= lastFileNumber))
				{
					output = "File : ";
					output += fname;
					output += " already exist";
					errorFlag = true;
				}
			} // if sscanf
		} // while


#ifdef WIN32
		FindClose(hFind);
#else
		closedir(aDirPt);
#endif


		if (errorFlag)
			THROW_CTL_ERROR(Error) << output;
	} // if(m_pars.overwritePolicy == Abort)
}

void CtSaving::Stream::checkDirectoryAccess(const std::string & directory)
{
	DEB_MEMBER_FUNCT();
	std::string local_directory = directory;
	std::string output;
	// check if directory exist
	DEB_TRACE() << "Check if directory exist";
	if (access(local_directory.c_str(), F_OK))
	{
		bool continue_flag;
		do
		{
#ifdef WIN32
			size_t pos = local_directory.find_last_of("\\/");
#else
			size_t pos = local_directory.find_last_of("/");
#endif
			size_t string_length = local_directory.size() - 1;
			if (pos != std::string::npos)
			{
				local_directory = local_directory.substr(0, pos);
				continue_flag = pos == string_length;
			}
			else
				continue_flag = false;
		} while (continue_flag);

		if (access(local_directory.c_str(), F_OK))
		{
			output = "Directory :" + local_directory + " doesn't exist";
			THROW_CTL_ERROR(Error) << output;
		}
	}

	// check if it's a directory
	struct stat aDirectoryStat;
	if (stat(local_directory.c_str(), &aDirectoryStat))
	{
		output = "Can stat directory : " + local_directory;
		THROW_CTL_ERROR(Error) << output;
	}
	DEB_TRACE() << "Check if it's really a directory";
	if (!S_ISDIR(aDirectoryStat.st_mode))
	{
		output = "Path : " + local_directory + " is not a directory";
		THROW_CTL_ERROR(Error) << output;
	}

	// check if it's writable
	DEB_TRACE() << "Check if directory is writable";
	if (access(local_directory.c_str(), W_OK))
	{
		output = "Directory : " + local_directory + " is not writable";
		THROW_CTL_ERROR(Error) << output;
	}
}
