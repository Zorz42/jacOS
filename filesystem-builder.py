import os
import math

"""
file head structure:

size 512:

first sector:
[12 file type] [490 file name] [2 flags] [4 file size] [4 next sector]

data sector:
[508 file data] [4 next sector]
"""

with open("filesystem.img", mode='wb') as fs_file:
	file_pointers = []
	data = [file_pointers]
	for filename in os.listdir("filesystem"):
		with open("filesystem/" + filename) as file:
			pointer = len(data)
			file_pointers.append(pointer & 0xFF)
			file_pointers.append((pointer >> 8) & 0xFF)
			file_pointers.append((pointer >> 16) & 0xFF)
			file_pointers.append((pointer >> 24) & 0xFF)

			file_head = [0] * 512
			for i, c in enumerate(filename.split(".")[-1]):
				file_head[i] = ord(c)

			for i, c in enumerate(".".join(filename.split(".")[0:-1])):
				file_head[12 + i] = ord(c)

			next_sector = len(data) + 1
			file_head[508] = next_sector & 0xFF
			file_head[509] = (next_sector >> 8) & 0xFF
			file_head[510] = (next_sector >> 16) & 0xFF
			file_head[511] = (next_sector >> 24) & 0xFF

			char_count = os.path.getsize("filesystem/" + filename)

			file_head[504] = char_count & 0xFF
			file_head[505] = (char_count >> 8) & 0xFF
			file_head[506] = (char_count >> 16) & 0xFF
			file_head[507] = (char_count >> 24) & 0xFF

			data.append(file_head)

			file_data = []
			for char in file.read():
				file_data.append(ord(char))
				if len(file_data) == 508:
					next_sector = len(data) + 1
					file_data.append(next_sector & 0xFF)
					file_data.append((next_sector >> 8) & 0xFF)
					file_data.append((next_sector >> 16) & 0xFF)
					file_data.append((next_sector >> 24) & 0xFF)
					data.append(file_data.copy())
					file_data.clear()
			data.append(file_data.copy())

	num_sectors = len(data)
	num_desc_sectors = math.ceil(num_sectors / 8 / 508)

	for i in range(0, len(data[0]), 4):
		index = 0
		index += data[0][i]
		index += data[0][i + 1] << 8
		index += data[0][i + 2] << 16
		index += data[0][i + 3] << 24
		index += num_desc_sectors
		data[0][i] = index & 0xFF
		data[0][i + 1] = (index >> 8) & 0xFF
		data[0][i + 2] = (index >> 16) & 0xFF
		data[0][i + 3] = (index >> 24) & 0xFF

	for sector in data[1:-1]:
		if len(sector) == 512:
			index = 0
			index += sector[508]
			index += sector[509] << 8
			index += sector[510] << 16
			index += sector[511] << 24
			index += num_desc_sectors
			sector[508] = index & 0xFF
			sector[509] = (index >> 8) & 0xFF
			sector[510] = (index >> 16) & 0xFF
			sector[511] = (index >> 24) & 0xFF

	for _ in range(num_desc_sectors):
		data.insert(0, [0] * 512)
		magic_number = 0xAABBCCDD
		data[0][511] = (magic_number >> 24) & 0xFF
		data[0][510] = (magic_number >> 16) & 0xFF
		data[0][509] = (magic_number >> 8) & 0xFF
		data[0][508] = magic_number & 0xFF

	for i in range(num_sectors):
		sector = i // 8 // 508
		byte_index = i // 8 % 508
		offset = i % 8
		data[sector][byte_index] |= 1 << offset

	byte_arr = []
	for sector in data:
		for i in range(512):
			if i < len(sector):
				byte_arr.append(sector[i])
			else:
				byte_arr.append(0)
	fs_file.write(bytes(byte_arr))
