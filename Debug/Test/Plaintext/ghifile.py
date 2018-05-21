with open('100MB.txt', 'r') as file:
	data = file.read()

with open('1700MB.txt', 'a+') as obj:
	for i in range(17):
		obj.write(data) 
with open('1800MB.txt', 'a+') as obj:
	for i in range(18):
		obj.write(data)

with open('1900MB.txt', 'a+') as obj:
	for i in range(19):
		obj.write(data) 

with open('2000MB.txt', 'a+') as obj:
	for i in range(20):
		obj.write(data)  

