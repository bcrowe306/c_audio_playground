MidiMsg = require("midi")


msg = {0x90, 0x40, 0x7F}

print(MidiMsg.isNoteOn(msg))