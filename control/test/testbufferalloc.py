import os
import threading
import time
import numpy as np
import gc

from lima import core, simulator

class TestBufferAlloc:

  @staticmethod
  def get_simu_hw_inter():
    cam = simulator.Camera()
    hw_inter = simulator.Interface(cam)
    return hw_inter, cam

  @staticmethod
  def get_default_config(frame_rate=1e3, readout_time=0):
    exp_time = 1.0 / frame_rate - readout_time
    return dict(use_control=True,
                use_thread=True,
                alloc_numpy=False,
                start_gdb=False,
                do_abort=False,
                exp_time=exp_time,
                readout_time=readout_time,
                max_nb_warnings=2,
                acq_time=3)

  def __init__(self, hw_inter, deps, config):
    self.hw_inter = hw_inter
    self.deps = deps
    self.config = config

    if self.use_control():
      self.ct = core.CtControl(self.hw_inter)
      self.acq = self.ct.acquisition()
      self.acq.setLatencyTime(self.config['readout_time'])
      self.acq.setAcqExpoTime(self.config['exp_time'])

      image = self.ct.image()
      self.frame_dim = image.getImageDim()
    else:
      det_info = self.hw_inter.getHwCtrlObj(core.HwCap.DetInfo)
      self.frame_dim = core.FrameDim(det_info.getDetectorImageSize(), 
                                     det_info.getCurrImageType())
  
      self.buffer = self.hw_inter.getHwCtrlObj(core.HwCap.Buffer)
      self.buffer.setFrameDim(self.frame_dim)

      self.sync = self.hw_inter.getHwCtrlObj(core.HwCap.Sync)
      self.sync.setLatTime(self.config['readout_time'])
      self.sync.setExpTime(self.config['exp_time'])

    self.max_nb_buffers = int(core.GetDefMaxNbBuffers(self.frame_dim) * 0.7) / 2
    self.max_nb_buffers += 1000 - self.max_nb_buffers % 1000
    min_nb_buffers = min(6000, self.max_nb_buffers / 2)
    self.nb_frames_list = range(min_nb_buffers, self.max_nb_buffers + 1, 500)

    self.end = False
    self.nb_frames = None
    self.base_mem = self.memory_usage_gb()
    self.nb_warnings = 0

    mega_bytes = self.frame_dim.getMemSize() / 1024.0**2
    print("Frame dim: %s (%s Mbytes)" % (self.frame_dim, mega_bytes))

  def use_control(self):
    return self.config['use_control']

  def __del__(self):
    if self.use_control():
      del self.acq
      del self.ct
    else:
      del self.sync
      del self.buffer
    del self.hw_inter
    del self.deps

  def acq_status(self):
    if self.use_control():
      return self.ct.getStatus().AcquisitionStatus
    else:
      return self.hw_inter.getStatus().acq

  @staticmethod
  def memory_usage_gb():
    with open('/proc/self/status') as f:
      for l in f.readlines():
        token = l.split()
        if token[0].strip(':') == 'VmSize':
          return float(token[1]) / 1024**2

  def allocate_numpy(self):
    fsize = self.frame_dim.getSize()
    shape = (fsize.getHeight(), fsize.getWidth())
    dtype = 'uint%s' % (self.frame_dim.getDepth() * 8)
    while not self.end:
      z = np.zeros(shape, dtype) + 1
      del z
      gc.collect()
      gc.collect()
      time.sleep(0.5)

  def check_mem(self, only_warn):
    if not self.nb_frames:
      return

    nb_buffers = min(self.nb_frames, self.max_nb_buffers)
    buffer_mem_gb = self.frame_dim.getMemSize() * nb_buffers / 1024.0**3
    max_mem_gb = 1.2 * buffer_mem_gb
    used_mem = self.memory_usage_gb() - self.base_mem
    over_mem = used_mem > max_mem_gb
    sign = ("!" if over_mem else "*") * 10
    if over_mem or not only_warn:
      print("%s Used mem %.1f, expected %.1f %s" % \
          (sign, used_mem, buffer_mem_gb, sign))
    if over_mem:
      self.nb_warnings += 1
      if self.nb_warnings == self.config['max_nb_warnings']:
        if self.config['start_gdb']:
          pid = os.getpid()
          xterm_opts = '-sb -sl 10000 -geom 120x80'
          os.system('xterm %s -e gdb $(which python) %d &' % (xterm_opts, pid))
        elif self.config['do_abort']:
          os.abort()

  def test_buffers(self, nb_frames_list):
    for self.nb_frames in nb_frames_list:
      sync_nb_frames = self.nb_frames
      print("Starting %s" % self.nb_frames)
      if self.use_control():
        self.acq.setAcqNbFrames(self.nb_frames)
        self.ct.prepareAcq()
        self.ct.startAcq()
      else:
        self.buffer.setNbBuffers(self.nb_frames)
        self.sync.setNbFrames(sync_nb_frames)
        self.hw_inter.prepareAcq()
        self.hw_inter.startAcq()
      self.check_mem(False)
      t0 = time.time()
      while self.acq_status() == core.AcqStatus.AcqRunning:
        time.sleep(0.1)
        if time.time() - t0 > self.config['acq_time']:
          if self.use_control():
            self.ct.stopAcq()
          else:
            self.hw_inter.stopAcq()  
      print("Finished!")
    self.end = True

  def test_threads(self, nb_frames_list):
    tlist = []
    th = threading.Thread(target=self.test_buffers, args=(nb_frames_list,))
    tlist.append(th)

    if self.config['alloc_numpy']:
      th = threading.Thread(target=self.allocate_numpy)
      tlist.append(th)

    for th in tlist:
      th.start()

    while not self.end:
      self.check_mem(True)
      time.sleep(2)

    for th in tlist:
      th.join()

  def run(self):
    if self.config['use_thread']:
      self.test_threads(self.nb_frames_list)
    else:
      self.test_buffers(self.nb_frames_list)


def main():
  config = TestBufferAlloc.get_default_config()
  hw_inter, deps = TestBufferAlloc.get_simu_hw_inter()
  test = TestBufferAlloc(hw_inter, deps, config)
  test.run()


if __name__ == '__main__':
  main()
