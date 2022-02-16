# Arduino 作为I2C主设备， OpenMV作为I2C从设备。
#
# 请把OpenMV和Arduino按照下面连线：
#
# OpenMV Cam Master I2C Data  (P5) - Arduino Uno Data  (A4)
# OpenMV Cam Master I2C Clock (P4) - Arduino Uno Clock (A5)
# OpenMV Cam Ground                - Arduino Ground


import sensor, image, time, math, utime,ustruct,pyb



sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking



red_threshold = [(46, 79, 122, -2, 33, 92)]#识别阈值
ROIS = [ (0, 0, 320, 240),]#识别区域

bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)  #看不懂的I2C通信
bus.deinit() # 完全关闭设备
bus = pyb.I2C(2, pyb.I2C.SLAVE, addr=0x12)
print("Waiting for Arduino...")


while(True):
    img = sensor.snapshot()
    center_pos_red = 0

    #利用颜色识别分别寻找三个矩形区域内的线段
    for r in ROIS:
        blobs_red = img.find_blobs(red_threshold, roi=r[0:4],merge=True)
        if blobs_red:
            most_pixels = 0
            largest_blob = 0
            for i in range(len(blobs_red)):#遍历像素块找到最大的一个
                if blobs_red[i].pixels() > most_pixels:
                    most_pixels = blobs_red[i].pixels()
                    largest_blob = i    #最大像素块下标
            img.draw_rectangle(blobs_red[largest_blob].rect())
            img.draw_cross(blobs_red[largest_blob].cx(),
                           blobs_red[largest_blob].cy())

            center_pos_red = blobs_red[largest_blob].cx()


    print(center_pos_red )

    text = str(center_pos_red) #以下都是发送坐标
    data = ustruct.pack("<%ds" % len(text), text)
    try:
        bus.send(ustruct.pack("<h", len(data)), timeout=10000) # 首先发送长度 (16-bits).
        try:
            bus.send(data, timeout=10000) # 然后发送数据
            print("Sent Data!") # 没有遇到错误时，会显示
        except OSError as err:
            pass # 不用担心遇到错误，会跳过
            # 请注意，有3个可能的错误。 超时错误（timeout error），
            # 通用错误（general purpose error）或繁忙错误
            #（busy error）。 “err.arg[0]”的错误代码分别
            # 为116,5,16。
    except OSError as err:
        pass # 不用担心遇到错误，会跳过
        # 请注意，有3个可能的错误。 超时错误（timeout error），
        # 通用错误（general purpose error）或繁忙错误
        #（busy error）。 “err.arg[0]”的错误代码分别
        # 为116,5,16。
