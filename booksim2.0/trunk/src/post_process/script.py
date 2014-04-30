from subprocess import call
import matplotlib.pyplot as plt
import os

def start_sim(routing, dataRate):
	result = [0]*len(dataRate)
	print routing
	for n in range(len(dataRate)):
		print "Inj. Rate:", dataRate[n]
		with open(os.devnull, "w") as fnull:
			call(["../booksim", "../examples/chaoconfig", "injection_rate=" + str(dataRate[n]), 
	          "routing_function = " + routing], stdout=fnull)
		allData = map(float, open('dump.txt').readlines())
		os.remove('dump.txt')
		result[n] = allData[0]

	return result

def print_msg(data, routing):
	print  "======", routing, "latencies======"
	for n in range(len(data)):
		print data[n]

# injection rate
dataRate = [];
for n in range(1, 9):
	dataRate.append(n*0.001)

# start simulation
# XY routing
dataXY = start_sim('my_xy', dataRate)

# West first routing
dataWest = start_sim('west_first', dataRate)

# Negative first routing
dataNegative = start_sim('negative_first', dataRate)

## Odd Even routing
dataOddEven = start_sim('odd_even', dataRate)

# NoP routing
dataNoP = start_sim('my_nop', dataRate)



# print	
print_msg(dataXY, "XY")
print_msg(dataWest, "West first")
print_msg(dataNegative, "Negative first")
print_msg(dataOddEven, "Odd Even")
print_msg(dataNoP, "NoP")

#exit();

# plot figure
fig = plt.figure()

plt.plot(dataRate, dataXY, label='XY')
plt.plot(dataRate, dataWest, label='West first')
plt.plot(dataRate, dataNegative, label='Negative first')
plt.plot(dataRate, dataOddEven, label='Odd Even')
plt.plot(dataRate, dataNoP, label='NoP')

plt.legend(loc='upper left')
plt.xlabel('Injection Rate (Packets/Cycle)')
plt.ylabel('Average packet latencies (Cycles)')
plt.grid()

plt.savefig("latency.eps")



