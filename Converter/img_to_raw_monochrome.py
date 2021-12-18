import tkinter as tk
from tkinter import filedialog
import numpy as np
from PIL import Image

root = tk.Tk()
root.withdraw()

img_path = filedialog.askopenfilename(filetypes=(("Image files", "*.png;*.jpg;*.jpeg;*.bmp"), ("All files", "*.*")))
img = Image.open(img_path)
x, y = img.size
obj = img.load()

raw_monochrome_uint8 = np.zeros(32, dtype=('uint8'))

adr = np.array([[7, 0], [0, 4], [15, 0], [8, 4]], dtype=int)

for yi in range(y):
    for xi in range(x):
        r, g, b = obj[xi, yi]
        r_new = 0
        g_new = 0
        b_new = 0
        if (r + g + b) > 250:
            r_new = 255
            g_new = 255
            b_new = 255
        obj[xi, yi] = (r_new, g_new, b_new)


img.show()

def WriteBit(x_s: int, y_s: int):
    n_byte: int = 0

    if y_s >= 8:
        y_s -= 8
        n_byte = 16

    if y_s < 4:
        if x_s < 8:
            n_byte += (y_s % 4) * 4
        else:
            n_byte += 2 + (y_s % 4) * 4
        raw_monochrome_uint8[n_byte] |= 128 >> (x_s % 8)
    else:
        if x_s < 8:
            n_byte += 1 + (y_s % 4) * 4
        else:
            n_byte += 3 + (y_s % 4) * 4
        raw_monochrome_uint8[n_byte] |= 1 << (x_s % 8)


for yi in range(y):
    for xi in range(x):
        r, g, b = obj[xi, yi]
        if r > 0: WriteBit(xi, yi)

raw_path = img_path + '.to_MC'
print("Массив для MC:")
for j in range(8):
    St_bin = ""
    for i in range(4):
        # if (j % 8) == 0: St_bin += '\n'
        St_bin += str(format(raw_monochrome_uint8[i + (j * 4)], 'b')).rjust(8, "0") + " "
    print(St_bin)

file = open(raw_path, 'w')
for j in range(8):
    for i in range(4):
        if (j == 0) and (i == 0):
            file.write(str(raw_monochrome_uint8[i + (j * 4)]))
        else:
            file.write(',' + str(raw_monochrome_uint8[i + (j * 4)]))
file.close()
print("Сохранено в файл: ", raw_path)
input()
