#!/usr/bin/python3

import sys
f1, f2 = sys.argv[1:3]
open(f2, "w").write(open(f1).read())
