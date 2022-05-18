from time import sleep
from machine import Pin, PWM

led = Pin(25, Pin.OUT)
motorD1 = PWM(Pin(6, Pin.OUT))
motorD2 = PWM(Pin(7, Pin.OUT))
motorE1 = PWM(Pin(8, Pin.OUT))
motorE2 = PWM(Pin(9, Pin.OUT))

button = Pin(22, Pin.IN, Pin.PULL_DOWN)
FD = Pin(12, Pin.IN, Pin.PULL_DOWN)
D = Pin(13, Pin.IN, Pin.PULL_DOWN)
E = Pin(14, Pin.IN, Pin.PULL_DOWN)
FE = Pin(15, Pin.IN, Pin.PULL_DOWN)

motorD1.freq(100)
motorD2.freq(100)
motorE1.freq(100)
motorE2.freq(100)

last_direction = ''
duty_cicle = 43690  # 65536


def set_motors(v1, v2, v3, v4):
    print(v1, v2, v3, v4)
    motorD1.duty_u16(v1 * duty_cicle)
    motorD2.duty_u16(v2 * duty_cicle)
    motorE1.duty_u16(v3 * duty_cicle)
    motorE2.duty_u16(v4 * duty_cicle)


def move(direction):
    global last_direction

    if direction == last_direction:
        return

    if direction == 'park':
        set_motors(0, 0, 0, 0)
    elif direction == 'forward':
        set_motors(0, 1, 0, 1)
    elif direction == 'backward':
        set_motors(1, 0, 1, 0)
    elif direction == 'forward left':
        set_motors(0, 1, 0, 0)
    elif direction == 'forward right':
        set_motors(0, 0, 0, 1)
    elif direction == 'backward left':
        set_motors(1, 0, 0, 0)
    elif direction == 'backward right':
        set_motors(0, 0, 1, 0)
    elif direction == 'left':
        set_motors(0, 1, 1, 0)
    elif direction == 'right':
        set_motors(1, 0, 0, 1)

    last_direction = direction


def follow_line():
    led.value(1)
    while FD.value() == 0 and FE.value() == 0:
        if D.value():
            move('right')
        elif E.value():
            move('left')
        else:
            move('forward')
    led.value(0)


def turn_right():
    move('forward')
    sleep(0.1)
    while D.value() or E.value():
        move('forward')
    sleep(0.1)
    move('right')
    while not D.value():
        move('right')
    move('left')
    sleep(0.05)
    move('park')


def turn_left():
    move('forward')
    sleep(0.1)
    while E.value():
        move('forward')
    sleep(0.1)
    move('left')
    while not E.value():
        move('left')
    move('right')
    sleep(0.05)
    move('park')


def Uturn():
    while E.value():
        move('forward')
    sleep(0.2)
    move('left')
    while not E.value():
        move('left')
    move('right')
    sleep(0.06)
    move('park')


def test():
    follow_line()
    turn_right()


def sequence():
    follow_line()
    turn_right()
    follow_line()
    turn_left()
    follow_line()
    turn_right()
    follow_line()
    Uturn()
    follow_line()
    turn_left()
    follow_line()
    turn_right()
    follow_line()

    move('forward')
    sleep(0.05)
    while E.value():
        move('forward')

    follow_line()
    Uturn()
    follow_line()
    turn_right()
    follow_line()
    Uturn()


while True:
    print('FE: {} E: {} D: {} FD: {}'.format(FE.value(), E.value(), D.value(), FD.value()))

    if button.value():
        sequence()
        # test()
    else:
        move('park')

    sleep(0.1)
