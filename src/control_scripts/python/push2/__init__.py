from . import ControlSurface

push2 = ControlSurface.ControlSurface("Push 2")

def onMidiIn(MidiMsg):
    print(MidiMsg)
    # push2.onMidiIn(MidiMsg)
