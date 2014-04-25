from subprocess import call
import matplotlib.pyplot as plt
import os

# injection rate
dataRate = [];
for n in range(1, 5):
	dataRate.append(n*0.001)

# XY routing
dataXY = []
routing = "my_xy"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), 
          "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	os.remove('dump.txt')
	dataXY.append(allData[0])

## Odd Even routing
#dataOddEven = []
#routing = "odd_even"
#print routing
#for n in range(len(dataRate)):
#	print "Inj. Rate:", dataRate[n]
#	with open(os.devnull, "w") as fnull:
#		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
#	allData = map(float, open('dump.txt').readlines())
#	os.remove('dump.txt')
#	dataOddEven.append(allData[0])

# DyXY routing
dataDyXY = []
routing = "dy_xy"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	os.remove('dump.txt')
	dataDyXY.append(allData[0])

## NoP routing
#dataNoP = []
#routing = "my_nop"
#print routing
#for n in range(len(dataRate)):
#	print "Inj. Rate:", dataRate[n]
#	with open(os.devnull, "w") as fnull:
#		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
#	allData = map(float, open('dump.txt').readlines())
#	os.remove('dump.txt')
#	dataNoP.append(allData[0])

# West first routing
dataWest = []
routing = "west_first"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	os.remove('dump.txt')
	dataWest.append(allData[0])


# North last routing
dataNorth = []
routing = "north_last"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	os.remove('dump.txt')
	dataNorth.append(allData[0])


# Negative first routing
dataNegative = []
routing = "negative_first"
print routing
for n in range(len(dataRate)):
	print "Inj. Rate:", dataRate[n]
	with open(os.devnull, "w") as fnull:
		call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing], stdout=fnull)
	allData = map(float, open('dump.txt').readlines())
	os.remove('dump.txt')
	dataNegative.append(allData[0])



# print	
print "======XY packet latencies======"
for n in range(len(dataRate)):
	print dataXY[n]


#print "======Odd Even packet latencies======"
#for n in range(len(dataRate)):
#	print dataOddEven[n]

print "======DyXY packet latencies======"
for n in range(len(dataRate)):
	print dataDyXY[n]

#print "======NoP packet latencies======"
#for n in range(len(dataRate)):
#	print dataNoP[n]

print "======West first packet latencies======"
for n in range(len(dataRate)):
	print dataWest[n]

print "======North last packet latencies======"
for n in range(len(dataRate)):
	print dataNorth[n]

print "======Negative first packet latencies======"
for n in range(len(dataRate)):
	print dataNegative[n]



exit();

# plot figure
fig = plt.figure()

plt.plot(dataRate, dataXY, label='XY')
#plt.plot(dataRate, dataOddEven, label='Odd Even')
plt.plot(dataRate, dataDyXY, label='DyXY')
#plt.plot(dataRate, dataNoP, label='NoP')
plt.plot(dataRate, dataWest, label='West first')

plt.legend(loc='upper left')
plt.xlabel('Injection Rate (Packets/Cycle)')
plt.ylabel('Average packet latencies (Cycles)')

plt.savefig("latency.eps")



