import math
import os

"""
size 512:

first sector:
[4 root_size] [ceil(root_size / 512) * 4 root_sectors] [496 - size(root_sectors) empty] [8 magic number 0xAABBCCDD]

file data in directory:
[name] [type] [4 size] [2 flags (16 bits = 16 flags)] [ceil(size / 512) * 4 sector_indexes]

data sector:
[508 file data] [4 next sector]
"""

disk_size_sectors = 1000
data = []

FLAG_DIRECTORY = 0


class FileDescriptor:
    def __init__(self, name: str, file_type: str, size: int):
        self.name = name
        self.file_type = file_type
        self.size = size
        self.sectors = []
        self.flags = 0

    def createData(self):
        my_data = []
        for c in self.name:
            my_data.append(ord(c))
        my_data.append(0)
        for c in self.file_type:
            my_data.append(ord(c))
        my_data.append(0)

        my_data.append(self.size & 0xFF)
        my_data.append((self.size >> 8) & 0xFF)
        my_data.append((self.size >> 16) & 0xFF)
        my_data.append((self.size >> 24) & 0xFF)

        my_data.append(self.flags & 0xFF)
        my_data.append((self.flags >> 8) & 0xFF)

        for sector in self.sectors:
            my_data.append(sector & 0xFF)
            my_data.append((sector >> 8) & 0xFF)
            my_data.append((sector >> 16) & 0xFF)
            my_data.append((sector >> 24) & 0xFF)

        return my_data

    def setFlag(self, flag: int, value: bool):
        if value:
            self.flags |= 1 << flag
        else:
            self.flags &= ~(1 << flag)

    def addSector(self, sector: int):
        self.sectors.append(sector)


def addDirectory(directory_path: str):
    file_descriptors = []
    for filename in os.listdir(directory_path):
        if filename == ".DS_Store":
            continue

        if os.path.isdir(directory_path + filename):
            dir_sectors, dir_size = addDirectory(directory_path + filename + "/")
            dir_descriptor = FileDescriptor(filename, "", dir_size)
            dir_descriptor.sectors = dir_sectors
            dir_descriptor.setFlag(FLAG_DIRECTORY, True)
            file_descriptors.append(dir_descriptor)
        else:
            with open(directory_path + filename, "rb") as file:

                file_descriptor = FileDescriptor(".".join(filename.split(".")[0:-1]), filename.split(".")[-1],
                                                 os.path.getsize(directory_path + filename))

                file_data = []
                for byte in file.read():
                    file_data.append(byte)
                    if len(file_data) == 512:
                        file_descriptor.addSector(len(data))
                        data.append(file_data.copy())
                        file_data.clear()
                if file_data:
                    file_descriptor.addSector(len(data))
                    data.append(file_data.copy())
                file_descriptors.append(file_descriptor)

    directory_data = []
    for file_descriptor in file_descriptors:
        directory_data += file_descriptor.createData()

    size = len(directory_data)
    sectors = []

    while directory_data:
        if len(directory_data) >= 508:
            sector = directory_data[0:512].copy()
            sectors.append(len(data))
            data.append(sector)
            directory_data = directory_data[512:-1]
        else:
            sectors.append(len(data))
            data.append(directory_data.copy())
            directory_data.clear()
    return sectors, size


def main():
    with open("filesystem.img", mode='wb') as fs_file:
        first_sector = [0] * 508 + [0xDD, 0xCC, 0xBB, 0xAA]

        num_desc_sectors = math.ceil(disk_size_sectors / 8 / 512)

        data.append(first_sector)

        for _ in range(num_desc_sectors):
            data.insert(1, [0] * 512)

        root_sectors, root_size = addDirectory("filesystem/")

        data[0][0] = root_size & 0xFF
        data[0][1] = (root_size >> 8) & 0xFF
        data[0][2] = (root_size >> 16) & 0xFF
        data[0][3] = (root_size >> 24) & 0xFF

        for i, sector in enumerate(root_sectors):
            data[0][i * 4 + 4] = sector & 0xFF
            data[0][i * 4 + 5] = (sector >> 8) & 0xFF
            data[0][i * 4 + 6] = (sector >> 16) & 0xFF
            data[0][i * 4 + 7] = (sector >> 24) & 0xFF

        num_used_sectors = len(data)

        for i in range(disk_size_sectors - len(data)):
            data.append([])

        for i in range(num_used_sectors):
            sector = i // 8 // 512 + 1
            byte_index = i // 8 % 512
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


if __name__ == "__main__":
    main()
