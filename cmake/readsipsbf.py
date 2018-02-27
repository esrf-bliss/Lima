from __future__ import print_function
import sys, os


sbf_fname = sys.argv[1]
out_dir = sys.argv[2]

src_prefix = 'sources = '
for l in open(sbf_fname):
    if l.startswith(src_prefix):
        l = l[len(src_prefix):]
        files = l.split()
        s = ';'.join(map(lambda x: '%s/%s' % (out_dir, x), files))
        print(s)

		