#!/usr/bin/env python
#-*- coding:utf-8 -*-
from __future__ import print_function
import sys

pre_qid = None
rg_dict = {'pos_list': [], 'neg_list': []}
rs_dict = {'pos_list': [], 'neg_list': []}
for line in sys.stdin:
    items = line.strip('\n').split('\t')
    if len(items) < 4:
        continue
    train_data = items[0]

    has_pair_feature = False
    slots = train_data.split(' ')[3:]
    for slot in slots:
        sign, slot_id = slot.split(':')
        if slot_id == '3101':
            has_pair_feature = True
            break
    if not has_pair_feature:
        continue
    
    labels = items[2].split(' ')
    label_dict = {}
    for label in labels:
        label_name, label_value = label.split(':')
        label_dict[label_name] = label_value

    dl_click = label_dict['dl_click']
    loss_weight = 1.0 + float(dl_click)
    if loss_weight < 1.0:
        loss_weight = 1.0
    elif loss_weight > 5.0:
        loss_weight = 5.0

    out_line = "%s *%s" % (train_data, loss_weight)

    qid = label_dict['request_id']
    product = label_dict['product']
    click = int(label_dict['click'])

    if qid != pre_qid and pre_qid is not None:
        if len(rg_dict['pos_list']) != 0 and len(rg_dict['neg_list']) != 0:
            for one_out_line in rg_dict['pos_list'] + rg_dict['neg_list']:
                print (one_out_line)
        rg_dict = {'pos_list': [], 'neg_list': []}
        if len(rs_dict['pos_list']) != 0 and len(rs_dict['neg_list']) != 0:
            for one_out_line in rs_dict['pos_list'] + rs_dict['neg_list']:
                print (one_out_line)
        rs_dict = {'pos_list': [], 'neg_list': []}
    pre_qid = qid

    if product == '1':
        if click > 0:
            rs_dict['pos_list'].append(out_line)
        else:
            rs_dict['neg_list'].append(out_line)
    elif product == '2':
        if click > 0:
            rg_dict['pos_list'].append(out_line)
        else:
            rg_dict['neg_list'].append(out_line)
            
if pre_qid is not None:
    if len(rg_dict['pos_list']) != 0 and len(rg_dict['neg_list']) != 0:
        for one_out_line in rg_dict['pos_list'] + rg_dict['neg_list']:
            print (one_out_line)
    rg_dict = {'pos_list': [], 'neg_list': []}
    if len(rs_dict['pos_list']) != 0 and len(rs_dict['neg_list']) != 0:
        for one_out_line in rs_dict['pos_list'] + rs_dict['neg_list']:
            print (one_out_line)
    rs_dict = {'pos_list': [], 'neg_list': []}
