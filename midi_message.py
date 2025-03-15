class MidiMessage:
    class TYPE:
        NOTE_OFF = 0x80
        NOTE_ON = 0x90
        AFTER_TOUCH = 0xA0
        CONTROL_CHANGE = 0xB0
        PROGRAM_CHANGE = 0xC0
        CHANNEL_PRESSURE = 0xD0
        PITCH_BEND = 0xE0
        SYSTEM_MESSAGE = 0xF0

    type_map = {
        0x80: "Note Off",
        0x90: "Note On",
        0xA0: "Aftertouch",
        0xB0: "Control Change",
        0xC0: "Program Change",
        0xD0: "Channel Pressure",
        0xE0: "Pitch Bend",
        0xF0: "System Message",
    }

    def __init__(self, raw_msg: tuple[int, int, int]):
        self.raw_msg = raw_msg
        self.message_type_bits = raw_msg[0] & 0xF0
        self.message_type = self.type_map.get(self.message_type_bits, "Unknown")
        self.channel = raw_msg[0] & 0x0F
        self.identifier = raw_msg[1]
        self.value = raw_msg[2]

    def __repr__(self):
        return f"Msg Type: {self.message_type}, Channel: {self.channel}, Identifier: {self.identifier}, Value: {self.value}"

    def __str__(self):
        return self.__repr__()

    def raw_message(self):
        return self.raw_msg
    
    def set_age(self, age):
        self.age = age
        
    def get_age(self):
        return self.age
    def get_note_number(self):
        return self.identifier
    
    def get_note_velocity(self):
        return self.value
    
    def get_control_number(self):
        return self.identifier
    
    def get_control_value(self):
        return self.value
    
    def is_note_on(self):
        return self.message_type_bits == self.TYPE.NOTE_ON

    def is_note_off(self):
        return self.message_type_bits == self.TYPE.NOTE_OFF or (
            self.message_type_bits == self.TYPE.NOTE_ON and self.value == 0
        )

    def is_control_change(self):
        return self.message_type_bits == self.TYPE.CONTROL_CHANGE

    def is_program_change(self):
        return self.message_type_bits == self.TYPE.PROGRAM_CHANGE

    def is_pitch_bend(self):
        return self.message_type_bits == self.TYPE.PITCH_BEND

    def is_system_message(self):
        return self.message_type_bits == self.TYPE.SYSTEM_MESSAGE

    def is_aftertouch(self):
        return self.message_type_bits == self.TYPE.AFTER_TOUCH

    def is_channel_pressure(self):
        return self.message_type_bits == self.TYPE.CHANNEL_PRESSURE

    def is_note_message(self):
        return self.is_note_on() or self.is_note_off()

    def is_control_message(self):
        return (
            self.is_control_change()
            or self.is_program_change()
            or self.is_pitch_bend()
            or self.is_aftertouch()
            or self.is_channel_pressure()
        )

    def is_channel_message(self):
        return self.is_note_message() or self.is_control_message()
