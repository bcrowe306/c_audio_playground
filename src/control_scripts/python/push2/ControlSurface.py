class ControlSurface:
    def __init__(self, name):
        self.name = name

    def onMidiIn(self, MidiMsg):
        print(MidiMsg.data())
