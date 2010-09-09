#include <iostream>
#include "SoftOpExternalMgr.h"
#include "SizeUtils.h"

using namespace std;
using namespace lima;

int main(int argc, char *argv[])
{
	cout << "Hello!" << endl;

	SoftOpExternalMgr op_ext_mgr;
	cout << "SoftOpExternalMgr created!" << endl;

	Roi roi = Roi(0, 0, 100, 100);
	cout << "roi=" << roi << endl;

	SoftOpInstance roi_cnt_op_inst;
	op_ext_mgr.addOp(ROICOUNTERS, "RoiCounters", 0, roi_cnt_op_inst);
	cout << "ROICOUNTERS op added!" << endl;

	SoftOpBaseClass *p = roi_cnt_op_inst.m_opt;
	SoftOpRoiCounter *roi_cnt_op = static_cast<SoftOpRoiCounter *>(p);

	list<Roi> roi_list;
	roi_list.push_back(roi);
	roi_cnt_op->set(roi_list);
	cout << "roi list added!" << endl;

	list<Roi> roi_list_check;
	roi_cnt_op->get(roi_list_check);
	list<Roi>::const_iterator i, end = roi_list_check.end();
	for (i = roi_list_check.begin(); i != end; ++i) {
		const Roi& roi_check = *i;
		cout << "roi_check=" << roi_check << endl;
	}

	return 0;

}
