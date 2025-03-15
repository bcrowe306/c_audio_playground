from . import ControlSurface

mpc_studio_mk2 = ControlSurface.ControlSurface("MPC Studio Mk2")

def onMidiIn(MidiMsg):
    print(MidiMsg)
