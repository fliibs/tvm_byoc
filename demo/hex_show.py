import struct

def read_float_from_hex_file(file_path):
    with open(file_path, 'rb') as file:
        # 读取二进制数据
        hex_data = file.read()

        # 计算数据的字节数
        num_bytes = len(hex_data)

        # 检查数据长度是否为浮点数的字节数（4字节）
        if num_bytes % 4 != 0:
            print("文件数据长度不是浮点数的整数倍")
            return

        # 将二进制数据分割为每个浮点数的字节数组
        float_bytes_list = [hex_data[i:i+4] for i in range(0, num_bytes, 4)]

        # 将字节数组转换为浮点数并打印
        for float_bytes in float_bytes_list:
            float_value = struct.unpack('f', float_bytes)[0]
            print(float_value)

# 示例用法
file_path = 'param/bias_add1_kernel.hex'
read_float_from_hex_file(file_path)