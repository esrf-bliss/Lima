
namespace Espia
{

class Dev
{
%TypeHeaderCode
#include <EspiaDev.h>
using namespace lima;
%End
 public:
	Dev(int dev_nb);
	~Dev();

	bool isMeta();

	void resetLink();
	void getCcdStatus(int& ccd_status);

	void getDrvOption( const std::string &opt_name, int &val /Out/);
	void setDrvOption( const std::string &opt_name, int new_val, 
	                                                        int &old_val /Out/);
};

}; // namespace Espia

