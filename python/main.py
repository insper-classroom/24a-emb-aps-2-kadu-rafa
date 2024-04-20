import serial
import uinput

ser = serial.Serial('/dev/rfcomm0', 9600)

# (Mais códigos aqui https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h?h=v4.7)
buttons = [
    uinput.REL_X,
    uinput.REL_Y,

    uinput.BTN_LEFT,
    uinput.BTN_RIGHT,

    uinput.KEY_W,
    uinput.KEY_S,
    uinput.KEY_A,
    uinput.KEY_D,

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
    "ANL AXIS X [REL_X]",
    "ANL AXIS Y [REL_Y]",

    "ANL BUTTON [BTN_LEFT]",
    "BLACK BUTTON [BTN_RIGHT]",

    "ANL UP [KEY_W]",
    "ANL DOWN [KEY_S]",
    "ANL LEFT [KEY_A]",
    "ANL RIGHT [KEY_D]",

    "RED BUTTON [SPACE]",
    "GREEN BUTTON [KEY_F]",
    "BLUE BUTTON [KEY_ESC]",
    "YELLOW BUTTON [KEY_ENTER]",

    "PAD 0 [KEY_0]",
    "PAD 1 [KEY_1]",
    "PAD 2 [KEY_2]",
    "PAD 3 [KEY_3]",
    "PAD 4 [KEY_4]",
    "PAD 5 [KEY_5]",
    "PAD 6 [KEY_6]",
    "PAD 7 [KEY_7]",
    "PAD 8 [KEY_8]",
    "PAD 9 [KEY_9]",
    "PAD AST [KEY_M]",
    "PAD HASH [KEY_I]",
]
button_quantity = len(buttons)
device = uinput.Device(buttons)

MOUSE_MODE = True

def parse_data(data):
    button = data[0]  # Axis no C, o botão apertado
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)
    print(f"Received data: {data}")
    print(f"button: {buttons_names[button]}, value: {value}") if button < button_quantity else print(f"button invalid")
    return button, value

def emulate_controller(button, value):
    if button < button_quantity:
        if button < 3:
            device.emit(buttons[button], value)
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

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()