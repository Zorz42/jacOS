import os

"""
file head structure:

size 512:

first sector:
[16 file type] [490 file name] [2 flags] [4 next sector]

data sector:
[508 file data] [4 next sector]
"""

with open("filesystem.img", mode='wb') as fs_file:
	data = []
	for filename in os.listdir("filesystem"):
		with open("filesystem/" + filename) as file:
			file_head = [0] * 512
			for i, c in enumerate(filename.split(".")[-1]):
				file_head[i] = ord(c)

			for i, c in enumerate(".".join(filename.split(".")[0:-1])):
				file_head[16 + i] = ord(c)

			next_sector = len(data) + 1
			file_head[511] = next_sector & 0xFF
			file_head[510] = (next_sector >> 4) & 0xFF
			file_head[509] = (next_sector >> 8) & 0xFF
			file_head[508] = (next_sector >> 12) & 0xFF

			data.append(file_head)

			file_data = []
			for char in file.read():
				file_data.append(ord(char))
				if len(file_data) == 508:
					next_sector = len(data) + 1
					file_data += [0, 0, 0, 0]
					file_data[511] = next_sector & 0xFF
					file_data[510] = (next_sector >> 4) & 0xFF
					file_data[509] = (next_sector >> 8) & 0xFF
					file_data[508] = (next_sector >> 12) & 0xFF
					data.append(file_data.copy())
					file_data.clear()
			data.append(file_data.copy())

	byte_arr = []
	for sector in data:
		for i in range(512):
			if i < len(sector):
				byte_arr.append(sector[i])
			else:
				byte_arr.append(0)
	fs_file.write(bytes(byte_arr))
