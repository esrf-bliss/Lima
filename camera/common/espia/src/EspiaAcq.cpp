#include "EspiaAcq.h"
#include "MemUtils.h"

using namespace lima;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

#define ESPIA_MIN_BUFFER_SIZE	(128 * 1024)


EspiaAcq::EspiaAcq(EspiaDev& dev)
	: m_dev(dev)
{
	m_nb_buffers = m_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
	
	m_nb_frames = 0;
	m_started = false;

	resetFrameInfo(m_last_frame_info);
	m_last_frame_cb_nr = Invalid;

	registerLastFrameCb();
}

EspiaAcq::~EspiaAcq()
{
	bufferFree();
	unregisterLastFrameCb();
}

int EspiaAcq::dispatchFrameCb(struct espia_cb_data *cb_data)
{
	EspiaAcq *espia = (EspiaAcq *) cb_data->data;

	void (EspiaAcq::*method)(struct espia_cb_data *cb_data) = NULL;

	int& cb_nr = cb_data->cb_nr;
	if (cb_nr == espia->m_last_frame_cb_nr)
		method = &EspiaAcq::lastFrameCb;
	
	if (method) {
		try {
			(espia->*method)(cb_data);
		} catch (...) {

		}
	}

	return ESPIA_OK;
}

void EspiaAcq::registerLastFrameCb()
{
	if (m_last_frame_cb_nr != Invalid)
		return;

	struct espia_cb_data cb_data;
	cb_data.type    = ESPIA_CB_ACQ;
	cb_data.cb      = dispatchFrameCb;
	cb_data.data    = this;
	cb_data.timeout = BlockForever;

	struct img_frame_info& req_finfo = cb_data.info.acq.req_finfo;
	req_finfo.buffer_nr    = ESPIA_ACQ_ANY;
	req_finfo.frame_nr     = ESPIA_ACQ_ANY;
	req_finfo.round_count  = ESPIA_ACQ_ANY;
	req_finfo.acq_frame_nr = ESPIA_ACQ_EACH;

	m_dev.registerCallback(cb_data, m_last_frame_cb_nr);
}

void EspiaAcq::unregisterLastFrameCb()
{
	if (m_last_frame_cb_nr == Invalid)
		return;

	m_dev.unregisterCallback(m_last_frame_cb_nr);
}

void EspiaAcq::lastFrameCb(struct espia_cb_data *cb_data)
{
	AutoMutex l = acqLock();

	struct img_frame_info& cb_finfo = cb_data->info.acq.cb_finfo;
	if (!finished_espia_frame_info(&cb_finfo, cb_data->ret))
		m_last_frame_info = cb_finfo;
}

void EspiaAcq::bufferAlloc(const FrameDim& frame_dim, int& nb_buffers,
			int buffer_frames)
{
	if (!frame_dim.isValid() || (nb_buffers <= 0) || (buffer_frames <= 0))
		throw LIMA_HW_EXC(InvalidValue, "Invalid frame_dim, "
				  "nb_buffers and/or buffer_frames");

	if ((frame_dim == m_frame_dim) && (nb_buffers == m_nb_buffers) &&
	    (buffer_frames == m_buffer_frames))
		return;

	bufferFree();

	int& virt_buffers   = nb_buffers;
	int& virt_frames    = buffer_frames;

	int real_buffers    = virt_buffers;
	int real_frames     = virt_frames;
	int real_frame_size = frame_dim.getMemSize();
	if (virt_frames == 1) {
		int page_size;
		GetPageSize(page_size);
		real_frame_size += page_size - 1;
		real_frame_size &= ~(page_size - 1);
	}

	int real_buffer_size = real_frame_size * real_frames;
	int frame_factor = 1;
	if (real_buffer_size < ESPIA_MIN_BUFFER_SIZE) {
		frame_factor  = ESPIA_MIN_BUFFER_SIZE / real_buffer_size;
		real_frames  *= frame_factor;
		real_buffers += frame_factor - 1;
		real_buffers /= frame_factor;
		virt_buffers  = real_buffers * frame_factor;
	}

	CHECK_CALL(espia_buffer_alloc(m_dev, real_buffers, real_frames,
				      real_frame_size));

	m_frame_dim         = frame_dim;
	m_nb_buffers        = virt_buffers;
	m_buffer_frames     = virt_frames;
	m_real_frame_factor = frame_factor;
	m_real_frame_size   = real_frame_size;
}

void EspiaAcq::bufferFree()
{
	if ((m_nb_buffers == 0) || (m_buffer_frames == 0))
		return;

	stopAcq();

	CHECK_CALL(espia_buffer_free(m_dev));

	m_frame_dim = FrameDim();
	m_nb_buffers = m_buffer_frames = 0;
	m_real_frame_factor = m_real_frame_size = 0;
}

void EspiaAcq::getFrameDim(FrameDim& frame_dim)
{
	frame_dim = m_frame_dim;
}

void EspiaAcq::getNbBuffers(int& nb_buffers)
{
	nb_buffers = m_nb_buffers;
}

void EspiaAcq::getBufferFrames(int& buffer_frames)
{
	buffer_frames = m_buffer_frames;
}

void *EspiaAcq::getBufferFramePtr(int buffer_nb, int frame_nb)
{
	int real_buffer = realBufferNb(buffer_nb, frame_nb);
	int real_frame  = realFrameNb(buffer_nb, frame_nb);
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, real_buffer, real_frame, &ptr));
	return ptr;
}

void *EspiaAcq::getAcqFramePtr(int acq_frame_nb)
{
	int buffer_nb = ESPIA_ACQ_ANY;
	void *ptr;
	CHECK_CALL(espia_frame_address(m_dev, buffer_nb, acq_frame_nb, &ptr));
	return ptr;
}

void EspiaAcq::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	struct img_frame_info finfo;
	finfo.buffer_nr    = ESPIA_ACQ_ANY;
	finfo.frame_nr     = ESPIA_ACQ_ANY;
	finfo.round_count  = ESPIA_ACQ_ANY;
	finfo.acq_frame_nr = acq_frame_nb;
	CHECK_CALL(espia_get_frame(m_dev, &finfo, NoBlock));

	real2virtFrameInfo(finfo, info);
}

void EspiaAcq::real2virtFrameInfo(const struct img_frame_info& real_info, 
			       HwFrameInfoType& virt_info)
{
	char *buffer_ptr = (char *) real_info.buffer_ptr;
	int frame_offset = real_info.frame_nr * m_real_frame_size;

	virt_info.acq_frame_nb    = real_info.acq_frame_nr;
	virt_info.frame_ptr       = buffer_ptr + frame_offset;
	virt_info.frame_dim       = &m_frame_dim;
	virt_info.frame_timestamp = usec2sec(real_info.time_us);
	virt_info.valid_pixels    = real_info.pixels;
}

void EspiaAcq::resetFrameInfo(struct img_frame_info& frame_info)
{
	frame_info.buffer_ptr   = NULL;
	frame_info.buffer_nr    = Invalid;
	frame_info.frame_nr     = Invalid;
	frame_info.round_count  = Invalid;
	frame_info.acq_frame_nr = Invalid;
	frame_info.time_us      = 0;
	frame_info.pixels       = 0;
}

void EspiaAcq::setNbFrames(int nb_frames)
{
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void EspiaAcq::getNbFrames(int& nb_frames)
{
	nb_frames = m_nb_frames;
}

void EspiaAcq::startAcq()
{
	AutoMutex l = acqLock();

	if (m_started)
		throw LIMA_HW_EXC(Error, "Acquisition already running");

	resetFrameInfo(m_last_frame_info);

	CHECK_CALL(espia_start_acq(m_dev, 0, m_nb_frames, NoBlock));

	m_start_ts = Timestamp::now();
	m_started = true;
}

void EspiaAcq::stopAcq()
{
	AutoMutex l = acqLock();

	if (!m_started)
		return;

	CHECK_CALL(espia_stop_acq(m_dev));
	
	m_started = false;
}

void EspiaAcq::getAcqStatus(AcqStatusType& acq_status)
{
	AutoMutex l = acqLock();

	unsigned long acq_run_nb;
	int acq_running = espia_acq_active(m_dev, &acq_run_nb);
	CHECK_CALL(acq_running);

	acq_status.acq_started = m_started;
	acq_status.acq_running = acq_running;
	acq_status.acq_run_nb = acq_run_nb;
	acq_status.last_acq_frame_nb = m_last_frame_info.acq_frame_nr;
}

