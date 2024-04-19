import serial
import uinput

ser = serial.Serial('/dev/rfcomm0', 9600) # Mude a porta para rfcomm0 se estiver usando bluetooth no linux
# Caso você esteja usando windows você deveria definir uma porta fixa para seu dispositivo (para facilitar sua vida mesmo)
# Siga esse tutorial https://community.element14.com/technologies/internet-of-things/b/blog/posts/standard-serial-over-bluetooth-on-windows-10 e mude o código acima para algo como: ser = serial.Serial('COMX', 9600) (onde X é o número desejado)

# (Mais códigos aqui https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/include/uapi/linux/input-event-codes.h?h=v4.7)
buttons = [
    uinput.REL_X,
    uinput.REL_Y,
    uinput.BTN_LEFT,
    uinput.BTN_RIGHT,
    uinput.KEY_W,
    uinput.KEY_A,
    uinput.KEY_S,
    uinput.KEY_D,
    uinput.KEY_Q,
    uinput.KEY_SPACE,
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
    uinput.KEY_P,
    uinput.KEY_E,
]
buttons_names = [
    "ANL AXIS X",
    "ANL AXIS Y",
    "LARGE LEFT BUTTON",
    "LARGE RIGHT BUTTON",
    "RED BUTTON",
    "GREEN BUTTON",
    "BLUE BUTTON",
    "YELLOW BUTTON",
    "BLACK BUTTON",
    "ANL BUTTON",
    "KEYPAD 0",
    "KEYPAD 1",
    "KEYPAD 2",
    "KEYPAD 3",
    "KEYPAD 4",
    "KEYPAD 5",
    "KEYPAD 6",
    "KEYPAD 7",
    "KEYPAD 8",
    "KEYPAD 9",
    "KEYPAD *",
    "KEYPAD #",
]
button_quantity = len(buttons)
device = uinput.Device(buttons)


# Função para analisar os dados recebidos do dispositivo externo
def parse_data(data):
    """
    Esta função analisa os dados recebidos do dispositivo externo e retorna o botão e o valor correspondentes.

    Argumentos:
    data (bytes): Os dados recebidos do dispositivo externo.

    Retorna:
    int, int: O número do botão e o valor do botão.
    """
    button = data[0]  # Axis no C, o botão apertado
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)
    print(f"Received data: {data}")
    print(f"button: {buttons_names[button]}, value: {value}") if button < button_quantity else print(f"button invalid")
    return button, value

def emulate_controller(button, value):
    """
    Esta função emula a entrada do controlador no sistema com base no botão e valor recebidos.

    button (int): O número do botão a ser emulado.
    value (int): O valor do botão.

    """
    if button < button_quantity:
        device.emit(buttons[button], value)

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