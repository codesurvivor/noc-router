from subprocess import call
import matplotlib.pyplot as plt
import os

def start_sim(routing, dataRate):
	result = []
	dataRate2 = []
	print routing
	for n in range(len(dataRate)):
		print "Inj. Rate:", dataRate[n]
		with open(os.devnull, "w") as fnull:
			call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), 
	          "routing_function = " + routing], stdout=fnull)
		try:
			allData = map(float, open('dump.txt').readlines())
		except:
			break
		os.remove('dump.txt')
		result.append(allData[0])
		dataRate2.append(dataRate[n])

	return result, dataRate2

def print_msg(data, routing):
	print  "======", routing, "latencies======"
	for n in range(len(data)):
		print data[n]

# injection rate
dataRate = [];
for n in range(1, 20):
	dataRate.append(n*0.001)

# start simulation
dataXY, dataRateXY = start_sim('my_xy', dataRate)
dataWest, dataRateWest = start_sim('west_first', dataRate)
dataNegative, dataRateNegative = start_sim('negative_first', dataRate)
dataOddEven, dataRateOddEven = start_sim('odd_even', dataRate)
dataNoP, dataRateNoP = start_sim('my_nop', dataRate)

# print	
print_msg(dataXY, "XY")
print_msg(dataWest, "West first")
print_msg(dataNegative, "Negative first")
print_msg(dataOddEven, "Odd Even")
print_msg(dataNoP, "NoP")

#exit();

# plot figure
fig = plt.figure()

plt.plot(dataRateXY, dataXY, label='XY')
plt.plot(dataRateWest, dataWest, label='West first')
plt.plot(dataRateNegative, dataNegative, label='Negative first')
plt.plot(dataRateOddEven, dataOddEven, label='Odd Even')
plt.plot(dataRateNoP, dataNoP, label='NoP')

plt.legend(loc='upper left')
plt.xlabel('Injection Rate (Packets/Cycle)')
plt.ylabel('Average packet latencies (Cycles)')
plt.grid()

plt.savefig("latency.eps")



