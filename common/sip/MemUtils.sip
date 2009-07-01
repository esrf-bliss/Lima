%ModuleHeaderCode
#include <MemUtils.h>
using namespace lima;
%End
void GetSystemMem(int& mem_unit, int& system_mem);
	
void GetPageSize(int& page_size);

int GetDefMaxNbBuffers(const FrameDim& frame_dim, double sys_mem_factor = 0);

void ClearBuffer(void *ptr, int nb_concat_frames, const FrameDim& frame_dim);
