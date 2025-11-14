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
#include <numeric>
#include <atomic>
#include <algorithm>
#include <iomanip>

#ifdef __linux__
#include <unistd.h>
#include <dirent.h>
#include <sys/statvfs.h>
#else
#include <processlib/win/unistd.h>
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

#include "lima/SidebandData.h"
#include "lima/SoftOpExternalMgr.h"

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
	_SaveTask(CtSaving::Stream& stream, Data& data)
		: SinkTaskBase(), m_stream(stream)
	{
		m_stream.prepareWritingFrame(data);
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

struct CtSaving::_SavingSidebandData : public sideband::Data
{
	Mutex m_lock;
	std::atomic<long> m_nb_cbk{0};
	ZBufferList m_buffers;
	SaveContainer::FrameParameters m_params;
	SaveContainer::Stat m_stat;

	_SavingSidebandData(::Data& data) : m_stat(data)
	{}

	std::string repr() override {
		std::ostringstream os;
		AutoMutex l(m_lock);
		os << "<"
		   << "nb_cbk=" << m_nb_cbk
		   << ", zbuffers=[";
		ZBufferList::const_iterator it, end = m_buffers.end();
		bool first = true;
		for (it = m_buffers.begin(); it != end; ++it, first = false)
			os << (first ? "" : ", ") << it->used_size;
		os << "]"
		   << ", stat=" << m_stat
		   << ">";
		return os.str();
	}
};

const std::string CtSaving::m_saving_data_key = "saving";

inline bool CtSaving::_hasSavingData(Data& data)
{
	DEB_STATIC_FUNCT();
	bool res = data.sideband.contains(m_saving_data_key);
	DEB_RETURN() << DEB_VAR1(res);
	return res;
}

inline CtSaving::_SavingDataPtr CtSaving::_getSavingData(Data& data)
{
	DEB_STATIC_FUNCT();
	auto res = data.sideband.get(m_saving_data_key);
	if (!res)
		THROW_CTL_ERROR(Error) << "Saving SidebandData not found";

	return sideband::DataCast<_SavingSidebandData>(*res);
}

void CtSaving::_createSavingData(Data& data)
{
	DEB_STATIC_FUNCT();
	if (_hasSavingData(data)) {
		DEB_ERROR() << "Saving SidebandData already created";
		return;
	}
	_SavingDataPtr saving = std::make_shared<_SavingSidebandData>(data);
	if (!data.sideband.insert(m_saving_data_key, saving))
		THROW_CTL_ERROR(Error) << "Saving SidebandData of wrong type";

	for (int s = 0; s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.createSavingData(data);
	}
}


/** @brief Parameters default constructor
 */
CtSaving::Parameters::Parameters()
	: imageType(Bpp8), nextNumber(0), fileFormat(RAW), savingMode(Manual),
	overwritePolicy(Abort), useHwComp(false),
	indexFormat("%04d"), framesPerFile(1), everyNFrames(1),
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

	{
		AutoMutex lock(m_cond.mutex());
		if (m_cnt_status == Open) {
			m_cnt_status = Init;
			AutoMutexUnlock u(lock);
			m_save_cnt->close();
		}
	}

	updateParameters();

	if (hasAutoSaveMode())
		_prepare();
}

void CtSaving::Stream::_prepare()
{
	DEB_MEMBER_FUNCT();

	checkWriteAccess();
	m_save_cnt->prepare(m_saving.m_ctrl);
	AutoMutex lock(m_cond.mutex());
	m_cnt_status = Prepared;
}

void CtSaving::Stream::close()
{
	m_save_cnt->close();

	AutoMutex lock(m_cond.mutex());
	m_cnt_status = Init;
}

void CtSaving::Stream::clear()
{
	m_save_cnt->clear();

	AutoMutex lock(m_cond.mutex());
	m_cnt_status = Init;
}

void CtSaving::Stream::prepareWritingFrame(Data& data)
{
	{
		AutoMutex lock(m_cond.mutex());
		if (m_cnt_status == Init) {
			m_cnt_status = Preparing;
			{
				AutoMutexUnlock u(lock);
				_prepare();
			}
			m_cond.broadcast();
		} else while (m_cnt_status == Preparing)
			m_cond.wait();
	}
	m_save_cnt->prepareWritingFrame(data);
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
	BufferHelper::Parameters zbuffer_params;

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
#if !defined  (WITH_HDF5_SAVING) || !defined (WITH_Z_COMPRESSION)
		THROW_CTL_ERROR(NotSupported) << "Lima is not compiled with the hdf5 gzip"
			"saving option, not managed";
#endif
		goto common;
	case HDF5BS:
#if !defined  (WITH_HDF5_SAVING) || !defined (WITH_BS_COMPRESSION)
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
			BufferHelper& buffer_helper = getZBufferHelper();
			buffer_helper.getParameters(zbuffer_params);
			m_save_cnt->close();
			delete m_save_cnt;
			m_save_cnt = NULL;
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
	BufferHelper& buffer_helper = getZBufferHelper();
	buffer_helper.setParameters(zbuffer_params);

	AutoMutex lock(m_cond.mutex());
	m_cnt_status = Init;
}

void CtSaving::Stream::writeFile(Data& data, HeaderMap& header)
{
	DEB_MEMBER_FUNCT();

	m_save_cnt->writeFile(data, header);

	AutoMutex lock(m_cond.mutex());
	m_cnt_status = Open;
}


SinkTaskBase* CtSaving::Stream::getTask(TaskType type, const HeaderMap& header,
					Data& data, int& priority)
{
	DEB_MEMBER_FUNCT();

	SinkTaskBase* save_task;

	if ((type == Compression) && needCompressionTask(data)) {
		save_task = m_save_cnt->getCompressionTask(header);
		save_task->setEventCallback(m_compression_cbk);
		priority = COMPRESSION_PRIORITY;
	}
	else {
		_SaveTask* real_task = new _SaveTask(*this, data);
		real_task->m_header = header;
		save_task = real_task;
		save_task->setEventCallback(m_saving_cbk);
		priority = SAVING_PRIORITY;
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
		saving_setting.set("useHwComp", pars.useHwComp);
		saving_setting.set("indexFormat", pars.indexFormat);
		saving_setting.set("framesPerFile", pars.framesPerFile);
		saving_setting.set("everyNFrames", pars.everyNFrames);
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

		bool useHwComp;
		if (saving_setting.get("useHwComp", useHwComp))
			pars.useHwComp = useHwComp;

		saving_setting.get("indexFormat", pars.indexFormat);

		int framesPerFile;
		if (saving_setting.get("framesPerFile", framesPerFile))
			pars.framesPerFile = framesPerFile;
		
		int everyNFrames;
		if (saving_setting.get("everyNFrames", everyNFrames))
			pars.everyNFrames = everyNFrames;

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
	m_frames_to_save(-1, -1),
	m_end_cbk(NULL),
	m_managed_mode(Software),
	m_saving_stop(false),
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

/** @brief set zbuffer parameters for a saving stream

	@param pars zbuffer parameters for the saving stream
	@param stream_idx the id of the saving stream
 */
void CtSaving::setZBufferParameters(const BufferHelper::Parameters& pars,
				    int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(pars, stream_idx);
	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	BufferHelper& buffer_helper = stream.getZBufferHelper();
	buffer_helper.setParameters(pars);
}

/** @brief get the zbuffer stream parameters

	@param pars the return zbuffer parameters
	@param stream_idx the stream id
 */
void CtSaving::getZBufferParameters(BufferHelper::Parameters& pars,
				    int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);
	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	BufferHelper& buffer_helper = stream.getZBufferHelper();
	buffer_helper.getParameters(pars);
	DEB_RETURN() << DEB_VAR1(pars);
}

void CtSaving::_ReadImage(Data& image, int frameNumber)
{
	DEB_MEMBER_FUNCT();

	if (m_hwsaving->getCapabilities() & HwSavingCtrlObj::MANUAL_READ)
	{
		HwFrameInfoType frame;
		m_hwsaving->readFrame(frame, frameNumber);
		CtBuffer *buffer = m_ctrl.buffer();
		buffer->getDataFromHwFrameInfo(image, frame);
	}
	else
		THROW_CTL_ERROR(NotSupported) << "Image read is not supported for this hardware";
}

bool CtSaving::_allStreamsReady()
{
	DEB_MEMBER_FUNCT();

	bool ready_flag = true;
	for (int s = 0; ready_flag && s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			ready_flag = stream.isReady();
	}

	DEB_RETURN() << DEB_VAR1(ready_flag);
	return ready_flag;
}

bool CtSaving::_allStreamsReadyFor(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data.frameNumber);

	bool ready_flag = true;
	for (int s = 0; ready_flag && s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			ready_flag = stream.isReadyFor(data);
	}

	DEB_RETURN() << DEB_VAR1(ready_flag);
	return ready_flag;
}

bool CtSaving::_allStreamsFinished()
{
	DEB_MEMBER_FUNCT();

	bool finished_flag = true;
	for (int s = 0; finished_flag && s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			finished_flag = stream.finished();
	}

	DEB_RETURN() << DEB_VAR1(finished_flag);
	return finished_flag;
}

void CtSaving::_waitWritingThreads()
{
	DEB_MEMBER_FUNCT();

	// Waiting all writing thread
	while (!_controlIsFault()) {
		AutoMutex aLock(m_cond.mutex());
		if (_allStreamsReady())
			break;
		m_cond.wait();
	}

	if (_controlIsFault())
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
/** @brief set the useHwComp active flag for a saving stream
 */
void CtSaving::setUseHwComp(bool active, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(active, stream_idx);

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.useHwComp = active;

	stream.setParameters(pars);
}
/** @brief get the useHwComp active flag for a saving stream
 */
void CtSaving::getUseHwComp(bool& active, int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	active = pars.useHwComp;

	DEB_RETURN() << DEB_VAR1(active);
}
/** @brief set the number of frame saved per file for a saving stream
 */
void CtSaving::setFramesPerFile(unsigned long frames_per_file, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frames_per_file, stream_idx);

	if (frames_per_file <= 0)
		THROW_CTL_ERROR(InvalidValue) << DEB_VAR1(frames_per_file) << "Not supported";

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
/** @brief set the saving period of the subsampled frame for a saving stream
 */
void CtSaving::setEveryNFrames(long every_n_frames, int stream_idx)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(every_n_frames, stream_idx);

	if (every_n_frames == 0)
		THROW_CTL_ERROR(InvalidValue) << DEB_VAR1(every_n_frames) << "Not supported";

	AutoMutex aLock(m_cond.mutex());
	Stream& stream = getStream(stream_idx);
	Parameters pars = stream.getParameters(Auto);
	pars.everyNFrames = every_n_frames;
	stream.setParameters(pars);
}
/** @brief get the saving period of the subsampled frame for a saving stream
 */
void CtSaving::getEveryNFrames(long& every_n_frames,
	int stream_idx) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(stream_idx);

	AutoMutex aLock(m_cond.mutex());
	const Stream& stream = getStream(stream_idx);
	const Parameters& pars = stream.getParameters(Auto);
	every_n_frames = pars.everyNFrames;

	DEB_RETURN() << DEB_VAR1(every_n_frames);
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

void CtSaving::_getTaskList(TaskType type, Data& data, const HeaderMap& header,
			    TaskList& task_list, int& priority)
{
	DEB_MEMBER_FUNCT();

	task_list.clear();
	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (stream.isActive()) {
			SinkTaskBase* save_task = stream.getTask(type, header,
								 data, priority);
			if (save_task)
				task_list.push_back(save_task);
		}
	}
	size_t nb_cbk = task_list.size();
	if (nb_cbk == 0)
		THROW_CTL_ERROR(Error) << "No saving task for frame "
				       << data.frameNumber;
	DEB_TRACE() << DEB_VAR1(nb_cbk);
	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex l(saving->m_lock);
	saving->m_nb_cbk = nb_cbk;
}

inline void CtSaving::_insertFrameData(FrameMap::value_type frame_data)
{
	DEB_MEMBER_FUNCT();

	const long& frame_nb = frame_data.first;
	std::pair<FrameMap::iterator, bool> insert;
	insert = m_frame_datas.insert(frame_data);
	if (!insert.second)
		THROW_CTL_ERROR(Error) << "Frame already inserted in "
				       << "to-save map: " << frame_nb;

	FrameMap::iterator& it = insert.first;
	if (m_frame_datas.size() == 1)
		m_frames_to_save.first = m_frames_to_save.second = frame_nb;
	else if (it == m_frame_datas.begin())
		m_frames_to_save.first = frame_nb;
	else if (++it ==  m_frame_datas.end())
		m_frames_to_save.second = frame_nb;
}

inline void CtSaving::_eraseFrameData(FrameMap::iterator it)
{
	DEB_MEMBER_FUNCT();

	if (m_frame_datas.size() == 1)
		m_frames_to_save.first = m_frames_to_save.second = -1;
	else if (it == m_frame_datas.begin()) {
		FrameMap::iterator aux = it;
		m_frames_to_save.first = (++aux)->first;
	} else {
		FrameMap::iterator aux1 = it, aux2 = it;
		if (++aux1 ==  m_frame_datas.end())
			m_frames_to_save.second = (--aux2)->first;
	}

	m_frame_datas.erase(it);
}

inline void CtSaving::_clearFrameDatas()
{
	DEB_MEMBER_FUNCT();
	m_frame_datas.clear();
	m_frames_to_save.first = m_frames_to_save.second = -1;
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
	aLock.unlock();

	_validateFrameHeader(frame_nr);
}
/** @brief validate a header for a frame.
	this mean that the header is ready and can now be save.
	If you are in AutoHeader this will trigger the saving if the data frame is available
 */
void CtSaving::validateFrameHeader(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);
	_validateFrameHeader(frame_nr);
}

void CtSaving::_validateFrameHeader(long frame_nr)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_nr);

	SavingMode saving_mode = getAcqSavingMode();
	if (saving_mode != CtSaving::AutoHeader)
		return;

	AutoMutex aLock(m_cond.mutex());

	FrameMap::iterator frame_iter = m_frame_datas.find(frame_nr);
	if (frame_iter == m_frame_datas.end())
		return;

	Data aData = frame_iter->second;

	bool need_compression;
	{
		AutoMutexUnlock u(aLock);
		need_compression = _needCompression(aData);
	}

	bool can_save = _allStreamsReadyFor(aData);
	if (!(need_compression || can_save))
		return;

	HeaderMap task_header;
	FrameHeaderMap::iterator aHeaderIter;
	aHeaderIter = m_frame_headers.find(frame_nr);
	bool keep_header = need_compression;
	_takeHeader(aHeaderIter, task_header, keep_header);

	_eraseFrameData(frame_iter);

	aLock.unlock();

	TaskType task_type = need_compression ? Compression : Save;
	TaskList task_list;
	int priority;
	_getTaskList(task_type, aData, task_header, task_list, priority);

	_postTaskList(aData, task_list, priority);
}
void CtSaving::_resetReadyFlag()
{
	AutoMutex lock(m_cond.mutex());
	for (int s = 0; s < m_nb_stream; ++s)
	{
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.setReady();
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
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_id);
	
	if (m_end_cbk)
	{
		Data aData;
		aData.frameNumber = frame_id;
		m_end_cbk->finished(aData);
	} else
		DEB_WARNING() << "No end callback registered";

	return !!m_end_cbk;
}

void CtSaving::frameReady(Data& aData)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aData);

	if (_controlIsFault()) {
		DEB_WARNING() << "Skip saving data: " << aData;
		return;
	} else if (!m_end_cbk)
		DEB_WARNING() << "No end callback registered";

	_createSavingData(aData);

	bool need_compression = _needCompression(aData);

	AutoMutex aLock(m_cond.mutex());

	SavingMode saving_mode = getAcqSavingMode();
	bool auto_header = (saving_mode == AutoHeader);
	long frame_nr = aData.frameNumber;
	FrameHeaderMap::iterator aHeaderIter;
	aHeaderIter = m_frame_headers.find(frame_nr);
	bool header_available = (aHeaderIter != m_frame_headers.end());
	bool can_save = _allStreamsReadyFor(aData);
	DEB_TRACE() << DEB_VAR5(saving_mode, need_compression, can_save,
		auto_header, header_available);
	if (!(need_compression || can_save) ||
		(auto_header && !header_available) || (saving_mode == Manual)) {
		FrameMap::value_type map_pair(frame_nr, aData);
		_insertFrameData(map_pair);
		return;
	}

	HeaderMap task_header;
	bool keep_header = need_compression;
	_takeHeader(aHeaderIter, task_header, keep_header);

	aLock.unlock();

	TaskType task_type = need_compression ? Compression : Save;
	TaskList task_list;
	int priority;
	_getTaskList(task_type, aData, task_header, task_list, priority);

	_postTaskList(aData, task_list, priority);
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

	// Incoming size is constant, average frame period
	std::vector<double> frame_period_list(incoming_speed_list.size());
	auto unit_time = [](double s) { return std::isinf(s) ? 0. : (1. / s); };
	std::transform(incoming_speed_list.begin(), incoming_speed_list.end(),
		frame_period_list.begin(), unit_time);
	double frame_period = std::accumulate(frame_period_list.begin(),
		frame_period_list.end(), 0.);
	if (!frame_period_list.empty()) frame_period /= frame_period_list.size();
	incoming_speed = frame_period ? (1. / frame_period) : 0.;
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

	if (m_frame_datas.size())
		DEB_WARNING() << DEB_VAR1(m_frame_datas.size());
	_clearFrameDatas();
}

void CtSaving::close()
{
	DEB_MEMBER_FUNCT();
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

	{
		AutoMutex lock(m_cond.mutex());
		if (getAcqSavingMode() != Manual)
			THROW_CTL_ERROR(Error) << "Manual saving is only permitted when "
				"saving mode == Manual";
	}

	// wait until the saving is no more used
	_waitWritingThreads();

	ManagedMode managed_mode;
	{
		AutoMutex lock(m_cond.mutex());
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

	_createSavingData(anImage2Save);

	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (stream.isActive())
			stream.prepareWritingFrame(anImage2Save);
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

bool CtSaving::_needCompression(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data.frameNumber);

	bool need_compression = false;
	for (int s = 0; (s < m_nb_stream) && !need_compression; ++s) {
		Stream& stream = getStream(s);
		if (stream.isActive())
			need_compression = stream.needCompressionTask(data);
	}

	DEB_RETURN() << DEB_VAR1(need_compression);
	return need_compression;
}

void CtSaving::_synchronousSaving(Data& anImage2Save, HeaderMap& header)
{
	for (int s = 0; s < m_nb_stream; ++s) {
		Stream& stream = getStream(s);
		if (!stream.isActive())
			continue;

		if (stream.needCompressionTask(anImage2Save)) {
			SinkTaskBase* aCompressionTaskPt;
			int priority;
			aCompressionTaskPt = stream.getTask(Compression, header,
							    anImage2Save,
							    priority);
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

	if (task_list.empty())
		THROW_CTL_ERROR(Error) << "Scheduling empty task list";

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

	_SavingDataPtr saving = _getSavingData(aData);
	{
		AutoMutex l(saving->m_lock);
		if (--saving->m_nb_cbk > 0)
			return;
	}

	long frame_nr = aData.frameNumber;

	AutoMutex aLock(m_cond.mutex());
	if (!_allStreamsReadyFor(aData)) {
		FrameMap::value_type map_pair(frame_nr, aData);
		_insertFrameData(map_pair);
		return;
	}

	HeaderMap header;
	FrameHeaderMap::iterator header_it = m_frame_headers.find(frame_nr);
	_takeHeader(header_it, header, false);

	aLock.unlock();

	TaskList task_list;
	int priority;
	_getTaskList(Save, aData, header, task_list, priority);

	_postTaskList(aData, task_list, priority);
}

void CtSaving::_saveFinished(Data& aData, Stream& stream)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(aData, stream.getIndex());

	_SavingDataPtr saving = _getSavingData(aData);
	{
		AutoMutex l(saving->m_lock);
		if (--saving->m_nb_cbk > 0)
			return;
	}

	//@todo check if the frame is still available
	if (m_end_cbk)
		m_end_cbk->finished(aData);

	aData.releaseBuffer(); // release finished data

	AutoMutex aLock(m_cond.mutex());

	SavingMode saving_mode = getAcqSavingMode();
	bool auto_saving = (saving_mode == AutoFrame) || (saving_mode == AutoHeader);
	if (!auto_saving)
	{
		if (m_saving_stop)
		{
			AutoMutexUnlock u(aLock);
			_close();
		}
		m_cond.signal();
		return;
	}

	// limit search depth to the number of concurrent writing tasks
	int tasks;
	stream.getMaxConcurrentWritingTask(tasks);
	FrameHeaderMap::iterator header_it;
	FrameMap::iterator data_it, data_end = m_frame_datas.end();
	for (data_it = m_frame_datas.begin(); tasks && (data_it != data_end); ++data_it, --tasks)
	{
		long frame_nr = data_it->first;
		header_it = m_frame_headers.find(frame_nr);
		// sorted frames: if header is not available yet, abort search 
		if ((saving_mode == AutoHeader) && (header_it == m_frame_headers.end()))
			return;
		bool can_save = _allStreamsReadyFor(data_it->second);
		if (can_save)
			break;
	}
	if ((data_it == data_end) || !tasks)
		return;

	Data nextData = data_it->second;
	_eraseFrameData(data_it);

	HeaderMap task_header;
	_takeHeader(header_it, task_header, false);

	aLock.unlock();

	TaskList task_list;
	int priority;
	_getTaskList(Save, nextData, task_header, task_list, priority);

	_postTaskList(nextData, task_list, priority);
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

	{
		AutoMutex aLock(m_ctrl.m_cond.mutex());
		if (m_ctrl.m_status.AcquisitionStatus != AcqFault) {
			m_ctrl.m_status.AcquisitionStatus = AcqFault;
			m_ctrl.m_status.Error = anErrorCode;
			DEB_ERROR() << DEB_VAR2(m_ctrl.m_status, saving_mode);
		}
	}

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
		//prepare all the active streams
		for (int s = 0; s < m_nb_stream; ++s) {
			Stream& stream = getStream(s);
			if (stream.isActive()) {
				AutoMutexUnlock u(aLock);
				stream.prepare();
			}
		}

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
		}
	}

	_close();
}

void CtSaving::_close()
{
	DEB_MEMBER_FUNCT();

	if (_allStreamsFinished())
	{
		for (int s = 0; s < m_nb_stream; ++s)
		{
			Stream& stream = getStream(s);
			if (stream.isActive())
				stream.close();
		}
	}
	else
	{
		AutoMutex aLock(m_cond.mutex());
		m_saving_stop = true;
	}
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtSaving::_getConfigHandler()
{
	return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG

CtSaving::SaveContainer::SaveContainer(Stream& stream)
	: m_lock(m_cond.mutex()), m_stream(stream), m_statistic_size(16),
	  m_log_stat_enable(false), m_log_stat_file(NULL),
	  m_max_writing_task(1), m_last_task_closes_all(false)
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

	const long frameId = aData.frameNumber;

	class RunningCleanup {
		DEB_CLASS_NAMESPC(DebModControl,
				  "CtSaving::SaveContainer::writeFile::RunningCleanup",
				  "Control");
	public:
		RunningCleanup(SaveContainer& c, long f) :
			m_c(c), m_frameId(f)
		{}

		~RunningCleanup()
		{
			try {
				if (!m_done)
					exec();
			} catch (...) {
			}
		}

		void exec()
		{
			DEB_MEMBER_FUNCT();
			bool close_all = false;
			{
				AutoMutex lock(m_c.m_lock);
				WritingTasks& running_tasks = m_c.m_running_tasks;
				WritingTasks::iterator it = running_tasks.find(m_frameId);
				if (it == running_tasks.end())
					THROW_CTL_ERROR(Error) << "Could not find running task";
				running_tasks.erase(it);
				if (running_tasks.empty() && m_c.m_last_task_closes_all)
					close_all = true;
			}
			if (close_all)
				m_c.close();
			m_done = true;
		}

	private:
		SaveContainer& m_c;
		long m_frameId;
		bool m_done{false};
	} running_cleanup(*this, frameId);

	_SavingDataPtr saving = _getSavingData(aData);
	Stat& stat = saving->m_stat;

	{
		AutoMutex l(saving->m_lock);
		stat.writing_start = Timestamp::now();
	}

	FrameParameters& frame_par = saving->m_params;
	if (!frame_par.isValid())
		THROW_CTL_ERROR(Error) << "Can't find saving parameters for frame"
				       << DEB_VAR1(frameId);
	const CtSaving::Parameters& pars = frame_par.m_pars;

	long write_size = 0;
	Params2Handler::value_type par_handler = open(frame_par);
	bool inverted = pars.everyNFrames < 0;
	unsigned long every_n_frames = abs(pars.everyNFrames);
	try
	{
	  if ((!inverted && !(frameId % pars.everyNFrames)) ||
	      (inverted && (frameId % pars.everyNFrames)) )
		write_size = _writeFile(par_handler.second.m_handler, aData, aHeader, pars.fileFormat);
	  else if (_hasBuffers(aData))
		_takeBuffers(aData);
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
#endif //__linux__

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

	bool acq_end = false;
	{
		AutoMutex lock(m_lock);
		++m_written_frames;
		acq_end = _allFramesWritten();
		DEB_TRACE() << DEB_VAR3(acq_end, m_written_frames, m_frames_to_write);
	}

	// close before marking that we have finished the frame
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

	double diff;
	{
		AutoMutex l(saving->m_lock);
		stat.write_size = write_size;
		stat.writing_end = Timestamp::now();
		diff = stat.writing_end - stat.writing_start;
	}

	DEB_TRACE() << "Write took : " << diff << "s";

	running_cleanup.exec();

	writeFileStat(aData);
}

void CtSaving::SaveContainer::writeFileStat(Data& aData)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aData);

	_SavingDataPtr saving = _getSavingData(aData);

	long frameId = aData.frameNumber;

	StatisticsType::value_type stat_pair(frameId, Stat());
	{
		AutoMutex l(saving->m_lock);
		stat_pair.second = saving->m_stat;
	}

	{
		AutoMutex lock(m_lock);

		if (long(m_statistic.size()) >= m_statistic_size)
			if (!m_statistic.empty())
				m_statistic.erase(m_statistic.begin());
		m_statistic.insert(stat_pair);

		if (!m_log_stat_enable || !m_log_stat_file)
			return;
	}

	Stat& stat = stat_pair.second;

	double comp_time = 0., comp_rate = 0., comp_ratio = 0.;
	double write_time, write_rate, total_time;

	if (stat.compression_end.isSet() && stat.compression_start.isSet()) {
		comp_time = stat.compression_end - stat.compression_start;
		comp_rate = stat.incoming_size / comp_time / 1024. / 1024.;
		comp_ratio = stat.incoming_size / stat.write_size;
	}

	write_time = stat.writing_end - stat.writing_start;
	write_rate = stat.write_size / write_time / 1024. / 1024.;
	total_time = stat.writing_end - stat.received_time;

	{
		AutoMutex lock(m_lock);

		if (m_log_stat_enable && m_log_stat_file) {
			fprintf(m_log_stat_file, "%ld %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n", \
				frameId, (double)stat.received_time, \
				comp_time * 1000.0, comp_rate, comp_ratio, \
				write_time * 1000.0, write_rate, total_time * 1000.0);
		}
	}
}

void CtSaving::SaveContainer::setEnableLogStat(bool enable)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(enable);
	
	// TODO: check that no current saving is active
	AutoMutex aLock = AutoMutex(m_lock);
	if (m_log_stat_enable && !enable && m_log_stat_file != NULL) {
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

	int stream_idx = m_stream.getIndex();

	DEB_TRACE() << DEB_VAR1(m_log_stat_enable);
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
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(aSize);
	
	AutoMutex aLock = AutoMutex(m_lock);
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
	DEB_MEMBER_FUNCT();
	
	AutoMutex aLock(m_lock);
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
	DEB_MEMBER_FUNCT();

	StatisticsType copy;
	{
		AutoMutex aLock = AutoMutex(m_lock);
		copy = m_statistic;
	}

	if (copy.empty())
		return;

	//Incoming statistic
	StatisticsType::const_iterator next = copy.begin();
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
		Stat& stat = i->second;
		if (stat.writing_start.isSet() && stat.writing_end.isSet())
		{
			Index::value_type s(stat.writing_start, &stat);
			writing_index.push_back(s);
			Index::value_type e(stat.writing_end, &stat);
			writing_index.push_back(e);
			compression_ratio.push_back(double(stat.incoming_size) /
						    stat.write_size);
		}
		if (stat.compression_start.isSet() && stat.compression_end.isSet())
		{
			Index::value_type s(stat.compression_start, &stat);
			compression_index.push_back(s);
			Index::value_type e(stat.compression_end, &stat);
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
		Stat *stat = i->second;
		if (i->first - stat->writing_start < 1e-9) // start
			current.insert(stat);
		else			// end
			current.erase(stat);

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
		Stat *stat = i->second;
		if (i->first - stat->compression_start < 1e-9) // start
			current.insert(stat);
		else			// end
			current.erase(stat);

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
	DEB_MEMBER_FUNCT();
	
	pars = m_stream.getParameters(Acq);
}

void CtSaving::SaveContainer::clear()
{
	DEB_MEMBER_FUNCT();

	this->close();

	AutoMutex aLock(m_lock);
	m_statistic.clear();
	_clear();			// call inheritance if needed
}

void CtSaving::SaveContainer::prepare(CtControl& ct)
{
	DEB_MEMBER_FUNCT();

	int nb_frames;
	ct.acquisition()->getAcqNbFrames(nb_frames);
	CtSaving::Parameters pars = m_stream.getParameters(Auto);
	AutoMutex lock(m_lock);
	m_statistic.clear();
	m_frames_to_write = nb_frames;
	m_files_to_write = 0;
	m_written_frames = 0;
	if (m_frames_to_write && 	// if not live
		pars.savingMode != CtSaving::Manual)
	{
		long nextNumber = pars.nextNumber - 1;
		bool multi_set = (pars.overwritePolicy == MultiSet);
		for (long i = 0; i < m_frames_to_write; ++i)
		{
			FrameParameters frame_par(pars);
			CtSaving::Parameters& file_pars = frame_par.m_pars;

			if (multi_set)
				file_pars.framesPerFile = 1; // force to 1
			else
			{
				int idx = i % file_pars.framesPerFile;
				bool new_file = (idx == 0);
				if (new_file) ++nextNumber;
				file_pars.nextNumber = nextNumber;
				frame_par.m_threadable = new_file;
				long first_frame = i - idx;
				if (first_frame + file_pars.framesPerFile > m_frames_to_write)
					file_pars.framesPerFile = m_frames_to_write - first_frame;
			}
			std::pair<Frame2Params::iterator, bool> result =
				m_frame_params.insert(Frame2Params::value_type(i, frame_par));
			if (!result.second)
				result.first->second = frame_par;
			if (!result.first->second.isValid())
				THROW_CTL_ERROR(Error) << "Inserted invalid params";
		}
		m_files_to_write = multi_set ? 1 : (nextNumber - pars.nextNumber + 1);
	}
	m_last_task_closes_all = false;
	prepareLogStat(pars);
	lock.unlock();

	// check if ZBuffer pool needs to be allocated
	prepareCompressionBuffers(ct);

	_prepare(ct);			// call inheritance if needed
}

void CtSaving::SaveContainer::prepareCompressionBuffers(CtControl& ct)
{
	DEB_MEMBER_FUNCT();

	FrameDim fdim;
	ct.image()->getImageDim(fdim);
	int buffer_size = getCompressedBufferSize(fdim.getMemSize(),
						  fdim.getDepth());
	DEB_TRACE() << DEB_VAR2(fdim, buffer_size);
	if (buffer_size <= 0) {
		m_zbuffer_helper.releaseBuffers();
		return;
	}

	int nb_frames;
	ct.acquisition()->getAcqNbFrames(nb_frames);
	m_zbuffer_helper.prepareBuffers(nb_frames, buffer_size);

	auto size_2_buffers = m_zbuffer_helper.getSize2NbAllocBuffersMap();

	const char *error_header = "Saving compression buffers: unexpected ";

	if (size_2_buffers.empty())
		return;
	else if (size_2_buffers.size() != 1)
		THROW_CTL_ERROR(Error) << error_header
				       << DEB_VAR1(size_2_buffers.size());

	auto it = size_2_buffers.begin();
	int alloc_size = it->first, alloc_nb = it->second;
	if (alloc_size != buffer_size)
		THROW_CTL_ERROR(Error) << error_header
				       << DEB_VAR1(alloc_size);

	// If (not in-place) soft ext op link task is active, frame buffers are
	// allocated on-the-fly, outside CtBuffer, without checking for overrun
	SoftOpExternalMgr *ext_op = ct.externalOperation();
	bool ext_link_task, ext_sink_task;
	ext_op->isTaskActive(ext_link_task, ext_sink_task);
	if (ext_link_task)
		return;

	long required;
	ct.buffer()->getMaxNumber(required);
	if (nb_frames < required)
		required = nb_frames;
	DEB_TRACE() << DEB_VAR3(nb_frames, required, alloc_nb);
	if (alloc_nb >= required)
		return;

	BufferHelper::Parameters params;
	m_zbuffer_helper.getParameters(params);
	double min_req_percent = params.reqMemSizePercent * required / alloc_nb;
	DEB_WARNING() << error_header << DEB_VAR2(alloc_nb, required);
	DEB_WARNING() << "Should set "
		      << "SavingZBufferParameters.reqMemSizePercent="
		      << std::setprecision(2) << min_req_percent << " or higer";
}

void CtSaving::SaveContainer::updateNbFrames(long nb_acquired_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_acquired_frames);
	
	AutoMutex lock(m_lock);
	m_frames_to_write = nb_acquired_frames;

	// Remove waiting tasks that will never run
	m_waiting_tasks.erase(
		m_waiting_tasks.find(nb_acquired_frames),
		m_waiting_tasks.end());

	// If running tasks are the last and all the frames were written
	// ensure to close at end
	if (m_waiting_tasks.empty() && !m_running_tasks.empty() &&
	    _allFramesWritten())
	    m_last_task_closes_all = true;

	// Remove all future frame params
	if (m_frame_params.empty())
		return;
	long first = std::max(m_frame_params.begin()->first, nb_acquired_frames);
	m_frame_params.erase(m_frame_params.find(first), m_frame_params.end());
}

bool CtSaving::SaveContainer::_isReady() const
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << DEB_VAR3(m_running_tasks.size(), m_waiting_tasks.size(),
				m_frame_params.size());
	bool ready = (m_running_tasks.empty() && m_waiting_tasks.empty() &&
		      m_frame_params.empty());
	DEB_RETURN() << DEB_VAR1(ready);
	return ready;
}

bool CtSaving::SaveContainer::isReady() const
{
	DEB_MEMBER_FUNCT();
	AutoMutex lock(m_lock);
	return _isReady();
}

bool CtSaving::SaveContainer::isReadyFor(Data& data) const
{
	DEB_MEMBER_FUNCT();
	const long frameId = data.frameNumber;
	DEB_PARAM() << DEB_VAR1(frameId);

	bool ready;

	_SavingDataPtr saving = _getSavingData(data);
	FrameParameters& frame_par = saving->m_params;

	AutoMutex lock(m_lock);
	int running_tasks = _getNbRunningTasks();
	if (running_tasks == 0) {
		long next = -1;
		// check the first in the list of frames not yet injected
		if (!m_frame_params.empty())
			next = m_frame_params.begin()->first;
		// and also the list of frames waiting to be saved
		if (!m_waiting_tasks.empty()) {
			long first = m_waiting_tasks.begin()->first;
			if ((next == -1) || (first < next))
				next = first;
		}
		// allow only the first to be saved: force sequential ordering
		ready = ((next == -1) || (frameId == next));
	} else if (frame_par.m_threadable) {
		DEB_TRACE() << DEB_VAR2(running_tasks, m_max_writing_task);
		ready = (running_tasks + 1 <= m_max_writing_task);
	} else
		ready = false;

	DEB_RETURN() << DEB_VAR1(ready);
	return ready;
}

bool CtSaving::SaveContainer::finished() const
{
	DEB_MEMBER_FUNCT();

	AutoMutex lock(m_lock);
	DEB_TRACE() << DEB_VAR2(m_written_frames, m_frames_to_write);
	bool finished = _isReady() && _allFramesWritten();
	DEB_RETURN() << DEB_VAR1(finished);
	return finished;
}

void CtSaving::SaveContainer::setReady()
{
	DEB_MEMBER_FUNCT();

	AutoMutex lock(m_lock);
	m_waiting_tasks.clear();
	m_frame_params.clear();
}

void CtSaving::SaveContainer::prepareWritingFrame(Data& data)
{
	DEB_MEMBER_FUNCT();
	const long frameId = data.frameNumber;
	DEB_PARAM() << DEB_VAR1(frameId);

	_SavingDataPtr saving = _getSavingData(data);
	FrameParameters& frame_par = saving->m_params;
	if (!frame_par.isValid()) {
		CtSaving::Parameters pars = m_stream.getParameters(Acq);
		frame_par.setParameters(pars);
	}

	AutoMutex lock(m_lock);
	WritingTasks::iterator it = m_waiting_tasks.find(frameId);
	if (it == m_waiting_tasks.end())
		THROW_CTL_ERROR(Error) << "Frame not in waiting WritingTasks";
	m_waiting_tasks.erase(it);
	m_running_tasks.emplace(frameId, saving);
}

void CtSaving::SaveContainer::createSavingData(Data& data)
{
	DEB_MEMBER_FUNCT();
	const long frameId = data.frameNumber;
	DEB_PARAM() << DEB_VAR1(frameId);

	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex lock(m_lock);
	Frame2Params::iterator fpars = m_frame_params.find(frameId);
	if (fpars != m_frame_params.end()) {
		saving->m_params = fpars->second;
		m_frame_params.erase(fpars);
	}
	m_waiting_tasks.emplace(frameId, saving);
}

void CtSaving::SaveContainer::compressionStart(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data);

	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex l(saving->m_lock);
	saving->m_stat.compression_start = Timestamp::now();
}

void CtSaving::SaveContainer::compressionFinished(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data);
	
	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex l(saving->m_lock);
	saving->m_stat.compression_end = Timestamp::now();
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

	CtSaving::Parameters& pars = fpars.m_pars;

	class OpeningMgr {
		DEB_CLASS_NAMESPC(DebModControl, "OpeningMgr",
				  "SaveContainer::open");
	public:
		OpeningMgr(Cond& c, OpeningPars& o, CtSaving::Parameters& p)
			: m_cond(c), m_opening(o), m_pars(p), m_in(false)
		{
			DEB_CONSTRUCTOR();
			DEB_PARAM() << DEB_VAR1(m_pars);
		}

		~OpeningMgr()
		{
			DEB_DESTRUCTOR();
			if (!m_in)
				return;
			AutoMutex lock(m_cond.mutex());
			remove(lock);
		}

		void wait(AutoMutex& l)
		{
			DEB_MEMBER_FUNCT();
			auto must_wait = [&]() {
				typedef OpeningPars::const_iterator Iterator;
				Iterator it, end = m_opening.end();
				for (it = m_opening.begin(); it != end; ++it)
					if (**it == m_pars)
						return true;
				return false;
			};
			while (must_wait()) {
				DEB_TRACE() << "Waiting";
				m_cond.wait();
			}				
		}

		void add(AutoMutex& l)
		{
			DEB_MEMBER_FUNCT();
			m_opening.insert(&m_pars);
			m_in = true;
		}

		void remove(AutoMutex& l)
		{
			DEB_MEMBER_FUNCT();
			m_opening.erase(&m_pars);
			m_cond.broadcast();
			m_in = false;
		}
		
	private:
		Cond& m_cond;
		OpeningPars& m_opening;
		CtSaving::Parameters m_pars;
		bool m_in;
	} opening(m_cond, m_opening_handlers, pars);

	{
		AutoMutex lock(m_lock);
		opening.wait(lock);

		Params2Handler::iterator handler = m_params_handler.find(pars);
		if (handler != m_params_handler.end())
			return *handler;

		opening.add(lock);
	}

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
	for (int nbTry = 0; !handler.m_handler && (nbTry < 5); ++nbTry)
	{
		try {
			handler.m_handler = _open(aFileName, openFlags, pars);
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

		if (!handler.m_handler && access(pars.directory.c_str(), W_OK))
		{
			m_stream.setSavingError(CtControl::SaveAccessError);
			std::string output = "Can not write in directory: " + pars.directory;
			THROW_CTL_ERROR(Error) << output;
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

	DEB_TRACE() << "Open file: " << aFileName;
	handler.m_nb_frames = pars.framesPerFile;
	Params2Handler::value_type map_pair(pars, handler);
	bool ok;
	{
		AutoMutex lock(m_lock);
		ok = m_params_handler.insert(map_pair).second;
		opening.remove(lock);
	}
	if (!ok) {
		_close(handler.m_handler);
		THROW_CTL_ERROR(Error) << "Error inserting handle";
	}
	return map_pair;
}

inline void CtSaving::SaveContainer::close(const Params2Handler::iterator& it, AutoMutex& l)
{
	DEB_MEMBER_FUNCT();
	
	if (!l.locked())
		THROW_CTL_ERROR(Error) << "Internal error: mutex not locked";

	void* raw_handler = it->second.m_handler;
	if (raw_handler == NULL)
		return;

	it->second.m_handler = NULL;

	{
		AutoMutexUnlock u(l);
		_close(raw_handler);
	}

	Parameters& pars = m_stream.getParameters(Acq);
	if ((pars.overwritePolicy != MultiSet) &&
	    (pars.overwritePolicy != Append)) {
		int nextNumber = it->first.nextNumber + 1;
		if (pars.nextNumber < nextNumber)
			pars.nextNumber = nextNumber;
	}
}

void CtSaving::SaveContainer::close(const CtSaving::Parameters* params,
	bool force_close)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(force_close);

	AutoMutex aLock(m_lock);
	if (!params)			// close all
	{
		for (Params2Handler::iterator it = m_params_handler.begin();
			it != m_params_handler.end(); ++it)
			close(it, aLock);
		m_params_handler.clear();
	}
	else
	{
		Params2Handler::iterator it = m_params_handler.find(*params);
		if (it == m_params_handler.end())
			THROW_CTL_ERROR(Error) << "Could not find handle for "
					       << DEB_VAR1(params);
		if (force_close || !--it->second.m_nb_frames) {
			close(it, aLock);
			m_params_handler.erase(it);
		}
	}

	// flush log file each time a frame file is closed
	aLock.unlock();
	if (m_log_stat_enable)
		fflush(m_log_stat_file);
}

bool CtSaving::SaveContainer::needCompressionTask(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data.frameNumber);

	Parameters& params = m_stream.getParameters(Acq);

	static const std::string gzip_key = "comp_gzip";
	static const std::string lz4_key = "comp_lz4";
	static const std::string bslz4_key = "comp_bshuffle_lz4";

#define RETURN_WITH_DEB(x)			\
	do {					\
		bool ok = (x);			\
		DEB_RETURN() << DEB_VAR1(ok);	\
		return ok;			\
	} while (0)

	if (_hasBuffers(data))
		RETURN_WITH_DEB(false);
	else if (!params.useHwComp)
		RETURN_WITH_DEB(needParallelCompression());

	sideband::BlobList blob_list;

	switch (params.fileFormat) {
#ifdef WITH_Z_COMPRESSION
	case CtSaving::EDFGZ:
		blob_list = checkCompressedSidebandData(gzip_key, data);
		if (!blob_list.empty()) {
			typedef SaveContainerEdf Edf;
			// this will not work with AutoHeader
			FileZCompression comp(dynamic_cast<Edf&>(*this), {});
			// EDF header is small, can afford compression here
			ZBufferList zheader = comp.compress_header(data);
			useCompressedSidebandData(data, blob_list,
						  std::move(zheader));
			RETURN_WITH_DEB(false);
		}
		RETURN_WITH_DEB(true);
#endif
#ifdef WITH_LZ4_COMPRESSION
	case CtSaving::EDFLZ4:
		blob_list = checkCompressedSidebandData(lz4_key, data);
		if (!blob_list.empty()) {
			typedef SaveContainerEdf Edf;
			// this will not work with AutoHeader
			FileLz4Compression comp(dynamic_cast<Edf&>(*this), {});
			// EDF header is small, can afford compression here
			ZBufferList zheader = comp.compress_header(data);
			useCompressedSidebandData(data, blob_list,
						  std::move(zheader));
			RETURN_WITH_DEB(false);
		}
		RETURN_WITH_DEB(true);
#endif
#ifdef WITH_HDF5_SAVING
#ifdef WITH_Z_COMPRESSION
	case CtSaving::HDF5GZ:
		blob_list = checkCompressedSidebandData(gzip_key, data);
		if (!blob_list.empty()) {
			useCompressedSidebandData(data, blob_list);
			RETURN_WITH_DEB(false);
		}
		RETURN_WITH_DEB(true);
#endif
#ifdef WITH_BS_COMPRESSION
	case CtSaving::HDF5BS:
		blob_list = checkCompressedSidebandData(bslz4_key, data);
		if (!blob_list.empty()) {
			useCompressedSidebandData(data, blob_list);
			RETURN_WITH_DEB(false);
		}
		RETURN_WITH_DEB(true);
#endif
#endif // WITH_HDF5_SAVING
	}

	return needParallelCompression();
}

sideband::BlobList
CtSaving::SaveContainer::checkCompressedSidebandData(const std::string& key, Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(key, data);

	Data::SidebandContainer::Optional res = data.sideband.get(key);
	if (!res) {
		DEB_WARNING() << "Missing '" << key << "' in " << data;
		return {};
	}

	typedef sideband::CompressedData CompressedData;
	std::shared_ptr<CompressedData> comp_data = sideband::DataCast<CompressedData>(*res);

	DEB_TRACE() << DEB_VAR3(comp_data->decomp_dims[0],
				comp_data->decomp_dims[1],
				comp_data->pixel_depth);
	// check size & depth
	bool ok = ((data.dimensions == comp_data->decomp_dims) &&
	      (data.depth() == comp_data->pixel_depth));
	if (!ok) {
		DEB_WARNING() << "Uncompressed image size/depth mismatch with "
				      << "'" << key << "' in " << data;
		return {};
	}

	// all ok, return blobs
	DEB_RETURN() << DEB_VAR1(comp_data->comp_blobs.size());
	return comp_data->comp_blobs;
}

void CtSaving::SaveContainer::useCompressedSidebandData(Data& data,
							sideband::BlobList& blob_list,
							ZBufferList&& zheader)
{
	DEB_MEMBER_FUNCT();
	ZBufferList buffers(std::move(zheader));
	sideband::BlobList::iterator bit, bend = blob_list.end();
	for (bit = blob_list.begin(); bit != bend; ++bit)
		buffers.emplace_back(bit->first, bit->second);
	DEB_TRACE() << DEB_VAR2(data.frameNumber, buffers.size());
	_setBuffers(data, std::move(buffers));
}

bool CtSaving::SaveContainer::_hasBuffers(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data.frameNumber);
	bool ok = _hasSavingData(data);
	if (ok) {
		_SavingDataPtr saving = _getSavingData(data);
		AutoMutex l(saving->m_lock);
		ok = !saving->m_buffers.empty();
	}
	DEB_RETURN() << DEB_VAR1(ok);
	return ok;
}

void CtSaving::SaveContainer::_setBuffers(Data& data, ZBufferList&& buffers)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(data.frameNumber, buffers.size());
	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex l(saving->m_lock);
	saving->m_buffers = std::move(buffers);
}

ZBufferList CtSaving::SaveContainer::_takeBuffers(Data& data)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(data.frameNumber);
	_SavingDataPtr saving = _getSavingData(data);
	AutoMutex l(saving->m_lock);
	DEB_RETURN() << DEB_VAR1(saving->m_buffers.size());
	return std::move(saving->m_buffers);
}

void CtSaving::SaveContainer::_clear()
{
	DEB_MEMBER_FUNCT();
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
		int framesToWrite;
		anAcq->getAcqNbFrames(framesToWrite);
		if (framesToWrite == 0)
			framesToWrite = 1;
		int framesPerFile = m_pars.framesPerFile;
		int nbFiles = (framesToWrite + framesPerFile - 1) / framesPerFile;
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
