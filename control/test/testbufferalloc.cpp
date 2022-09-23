#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <iomanip>
#include <cmath>

#include <memory.h>
#ifdef __unix
#include <unistd.h>
#else
#include <processlib/win/unistd.h>
#endif
#include <stdlib.h>
#include <sys/sysinfo.h>
#ifdef __SSE2__
#include <emmintrin.h>
#endif

#include <sys/time.h>

using namespace std;

typedef void *Ptr;
typedef vector<Ptr> PtrList;

struct thread_data {
	volatile bool end;
};

int alignment = 16;
int block_size = 1024 * 1024 * 4;
int min_nb_frames = 3000;
int max_nb_frames = 6000;
int frames_step = 500;
int page_size = 4096;
double acq_time = 2;

double base_mem;

double mem_usage_gb()
{
	ifstream f("/proc/self/status");
	while (f) {
		string l;
		if ((f >> l) && (l == "VmSize:")) {
			long s;
			f >> s;
			return float(s) / pow(1024, 2);
		}
	}

	throw exception();
}

double get_elapsed_time(struct timeval *t0, struct timeval *t)
{
	return (t->tv_sec - t0->tv_sec) + (t->tv_usec - t0->tv_usec) * 1e-6;
}

void post_alloc(void *p, int size)
{
	char *ptr = static_cast<char *>(p);
	long page_size = sysconf(_SC_PAGESIZE);
#ifdef __SSE2__
	if(!((long)ptr & 15))	// aligned to 128 bits
	  {
              __m128i zero = _mm_setzero_si128();
	      for(long i = 0;i < size;i += page_size,ptr+=page_size)
		{
		  if(size_t(size - i) >= sizeof(__m128i))
		    _mm_store_si128((__m128i*)ptr,zero);
		  else
		    *ptr = 0;
		}
          }
	else
	  {
#endif
	      for(long i = 0;i < size;i += page_size,ptr+=page_size)
		*ptr = 0;
#ifdef __SSE2__
          }
#endif
}

PtrList alloc_buffers(int nb_frames)
{
	PtrList list;
	for (int i = 0; i < nb_frames; ++i) {
		Ptr p;
		int ret = posix_memalign(&p, alignment, block_size);
		if (ret != 0) {
			cerr << "Error!" << endl;
			abort();
		}
		list.push_back(p);

		post_alloc(p, block_size);
	}
	return list;
}

void release_buffers(PtrList& list)
{
	PtrList::iterator it, end = list.end();
	for (it = list.begin(); it != end; ++it)
		free(*it);
	list.clear();
}

void check_mem(int i)
{
	double mem = mem_usage_gb() - base_mem;
	double expected = float(block_size) * i / pow(1024, 3);
	string prefix = (mem > expected * 1.2) ? "!!!!!!!" : "*******";
	cout << fixed << setprecision(1) 
	     << prefix << " Used mem: " << mem << ", expected: " << expected 
	     << endl;
}

bool elapsed(struct timeval *t0, double acq_time)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return get_elapsed_time(t0, &t) >= acq_time;
}

void use_buffer_func(const PtrList& list)
{
	struct timeval t0;
	gettimeofday(&t0, NULL);
	PtrList::const_iterator it, end = list.end();
	for (it = list.begin(); (it != end) && !elapsed(&t0, acq_time); ++it)
		memset(*it, 0, block_size);
}

void *use_buffer_thread_func(void *data)
{
	const PtrList *list = static_cast<const PtrList *>(data);
	use_buffer_func(*list);
	return NULL;
}

void *alloc_thread_func(void *data)
{
	struct thread_data *tdata = static_cast<struct thread_data *>(data);
	for (int i = min_nb_frames; i <= max_nb_frames; i += frames_step) {
		cout << "Starting " << i << endl;
		PtrList list = alloc_buffers(i);
		check_mem(i);
		pthread_t t;
		int ret = pthread_create(&t, NULL, use_buffer_thread_func, 
					 &list);
		if (ret != 0)
			cerr << "Error creating use thread" << endl;
		else
			pthread_join(t, NULL);
		release_buffers(list);
		cout << "Finished!" << endl;
	}
	tdata->end = true;
	return NULL;
}

void do_calc()
{
	double a = M_PI / 4;
	double b = sin(a);
	double c = asin(b);
	if (fabs(c - a) > 1e-3 * c)
		cout << "Warning: a=" << a << ", c=" << c << endl;
}

void *calc_thread_func(void *data)
{
	struct thread_data *tdata = static_cast<struct thread_data *>(data);
	while (!tdata->end)
		do_calc();
	return NULL;
}

int main(int argc, char *argv[])
{
	base_mem = mem_usage_gb();

	typedef vector<pthread_t> ThreadList;
	ThreadList thread_list;

	struct thread_data tdata;
	tdata.end = false;

	pthread_t t;
	int ret;
	ret = pthread_create(&t, NULL, alloc_thread_func, &tdata);
	if (ret != 0) {
		cerr << "Error creating alloc thread" << endl;
		throw exception();
	}
	thread_list.push_back(t);

	ret = pthread_create(&t, NULL, calc_thread_func, &tdata);
	if (ret != 0) {
		cerr << "Error creating calc thread" << endl;
		throw exception();
	}
	thread_list.push_back(t);

	ThreadList::iterator it, end = thread_list.end();
	for (it = thread_list.begin(); it != end; ++it)
		pthread_join(*it, NULL);

	return 0;
}	
