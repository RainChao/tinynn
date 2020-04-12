#!/usr/bin/env python
#-*- coding:utf-8 -*-
from __future__ import print_function
import sys

for line in sys.stdin:
    items = line.strip('\n').split(' ')
    if len(items) >= 2:
        print(items[0])
