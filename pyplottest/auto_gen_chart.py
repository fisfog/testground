# coding = utf8
#!/bin/python

"""
	created by myl 20180914
	vacc month data statistic chart generate
"""

from pylab import *
import seaborn as sns

p2pid_map = {
		'TKLC0001':u'通卡连城',
		'HZSC0001':u'市民卡惠民金服',
		'SCJF0001':u'首创金服',
		'JTX00001':u'金投行',
		'ZNJF0001':u'浙农金服'
}

test_acc_cnt_data = {
		'TKLC0001':100,
		'HZSC0001':600,
		'SCJF0001':400,
		'JTX00001':400,
		'ZNJF0001':300
}

def gen_acc_count_pie(acc_cnt_data):
	val = list(acc_cnt_data.values())
	lab = [p2pid_map[pid] for pid in acc_cnt_data.keys()]
	print(val)
	print(lab)
	plt = pie(val,labels=lab,autopct=lambda p:'{v:d}'.format(p,v=int(round(p*sum(val)/100.0))))
	return plt


if __name__ == '__main__':
	plt = gen_acc_count_pie(test_acc_cnt_data)
	show()
