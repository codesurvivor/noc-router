from subprocess import call
import matplotlib.pyplot as plt



# injection rate
dataRate = [];
for n in range(1, 7):
	dataRate.append(n*0.001)

# XY routing
dataXY = []
routing = "my_xy"
for n in range(len(dataRate)):
	call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing])
	allData = map(float, open('dump.txt').readlines())
	dataXY.append(allData[0])


# Odd Even routing
dataOddEven = []
routing = "odd_even"
for n in range(len(dataRate)):
	call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing])
	allData = map(float, open('dump.txt').readlines())
	dataOddEven.append(allData[0])


# DyXY routing
dataDyXY = []
routing = "dy_xy"
for n in range(len(dataRate)):
	call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), "routing_function = " + routing])
	allData = map(float, open('dump.txt').readlines())
	dataDyXY.append(allData[0])


# print	
print "======XY packet latencies======"
for n in range(len(dataRate)):
	print dataXY[n]


print "======Odd Even packet latencies======"
for n in range(len(dataRate)):
	print dataOddEven[n]

print "======DyXY packet latencies======"
for n in range(len(dataRate)):
	print dataDyXY[n]


# plot figure
fig = plt.figure()

plt.plot(dataRate, dataXY, label='XY')
plt.plot(dataRate, dataOddEven, label='Odd Even')
plt.plot(dataRate, dataDyXY, label='DyXY')

plt.legend(loc='upper left')
plt.xlabel('Injection Rate')
plt.ylabel('Average packet latencies')

plt.savefig("latency.eps")



