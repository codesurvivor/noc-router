from subprocess import call
import matplotlib.pyplot as plt
import os

# injection rate
dataRate = [];
for n in range(1, 9):
	dataRate.append(n*0.001)

# XY routing
dataXY = []
routing = "my_xy"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	dataXY.append(allData[0])

# Odd Even routing
dataOddEven = []
routing = "odd_even"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	dataOddEven.append(allData[0])

## DyXY routing
#dataDyXY = []
#routing = "dy_xy"
#print routing
#for n in range(len(dataRate)):
#	print "Inj. Rate:", dataRate[n]
#	with open(os.devnull, "w") as fnull:
#		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
#	allData = map(float, open('dump.txt').readlines())
#	dataDyXY.append(allData[0])

# NoP routing
dataNoP = []
routing = "my_nop"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	dataNoP.append(allData[0])



# print	
print "======XY packet latencies======"
for n in range(len(dataRate)):
	print dataXY[n]


print "======Odd Even packet latencies======"
for n in range(len(dataRate)):
	print dataOddEven[n]

#print "======DyXY packet latencies======"
#for n in range(len(dataRate)):
#	print dataDyXY[n]

print "======NoP packet latencies======"
for n in range(len(dataRate)):
	print dataNoP[n]



# plot figure
fig = plt.figure()

plt.plot(dataRate, dataXY, label='XY')
plt.plot(dataRate, dataOddEven, label='Odd Even')
#plt.plot(dataRate, dataDyXY, label='DyXY')
plt.plot(dataRate, dataNoP, label='NoP')

plt.legend(loc='upper left')
plt.xlabel('Injection Rate (Packets/Cycle)')
plt.ylabel('Average packet latencies (Cycles)')

plt.savefig("latency.eps")



