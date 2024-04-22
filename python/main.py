import serial
import uinput
import time

# ser = serial.Serial('/dev/ttyACM0', 115200)
ser = serial.Serial('/dev/rfcomm0', 9600)

# (Mais códigos aqui https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h?h=v4.7)
buttons = [
    uinput.REL_X,
    uinput.REL_Y,

    uinput.KEY_A,
    uinput.KEY_D,
    uinput.KEY_W,
    uinput.KEY_S,

    uinput.BTN_LEFT,
    uinput.BTN_RIGHT,

    uinput.KEY_SPACE,
    uinput.KEY_F,
    uinput.KEY_ESC,
    uinput.KEY_ENTER,

    uinput.KEY_0,
    uinput.KEY_1,
    uinput.KEY_2,
    uinput.KEY_3,
    uinput.KEY_4,
    uinput.KEY_5,
    uinput.KEY_6,
    uinput.KEY_7,
    uinput.KEY_8,
    uinput.KEY_9,
    uinput.KEY_M,
    uinput.KEY_I,
]
buttons_names = [
    "ANL AXIS X [REL_X]", #0
    "ANL AXIS Y [REL_Y]", #1

    "ANL LEFT [KEY_A]", #2
    "ANL RIGHT [KEY_D]", #3
    "ANL UP [KEY_W]", #4
    "ANL DOWN [KEY_S]", #5

    "ANL BUTTON [BTN_LEFT]", #6
    "BLACK BUTTON [BTN_RIGHT]", #7

    "RED BUTTON [KEY_SPACE]", #8
    "GREEN BUTTON [KEY_F]", #9
    "BLUE BUTTON [KEY_ESC]", #10
    "YELLOW BUTTON [KEY_ENTER]", #11

    "PAD 0 [KEY_0]", #12
    "PAD 1 [KEY_1]", #13
    "PAD 2 [KEY_2]", #14
    "PAD 3 [KEY_3]", #15
    "PAD 4 [KEY_4]", #16
    "PAD 5 [KEY_5]", #17
    "PAD 6 [KEY_6]", #18
    "PAD 7 [KEY_7]", #19
    "PAD 8 [KEY_8]", #20
    "PAD 9 [KEY_9]", #21
    "PAD AST [KEY_M]", #22
    "PAD HASH [KEY_I]", #23
]
button_quantity = len(buttons)
device = uinput.Device(buttons)

def parse_data(data):
    button = data[0]
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)
    print(f"Received data: {data}")
    print(f"button: {buttons_names[button]}, value: {value}") if button < button_quantity else print(f"button invalid")
    return button, value

def emulate_controller(button, value):
    if button < button_quantity:
        if button < 7:
            device.emit(buttons[button], value)
        elif button == 7:
            device.emit(buttons[button], 1)
            device.emit(buttons[button], 0)
        else:
            device.emit_click(buttons[button])

try:
    # Pacote de sync
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            if data == b'\xff':
                break

        # Lendo 4 bytes da uart
        data = ser.read(3)
        button, value = parse_data(data)
        emulate_controller(button, value)
        print()

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    for button in buttons:
        device.emit(button, 0)
    ser.close()