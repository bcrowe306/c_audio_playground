MidiMsg = {}

MidiMsg.isNoteOn = function(msg)
    if msg[1] & 0xF0 == 0x90 then
        if msg[3] > 0 then
            return true
        else
            return false
        end
    else
        return false
    end
end

MidiMsg.isNoteOff = function(msg)
    return msg[1] == 0x80
end

MidiMsg.isControlChange = function(msg)
    return msg[1] == 0xB0
end

MidiMsg.isProgramChange = function(msg)
    return msg[1] == 0xC0
end

MidiMsg.isPitchBend = function(msg)
    return msg[1] == 0xE0
end

MidiMsg.isChannelPressure = function(msg)
    return msg[1] == 0xD0
end

MidiMsg.isPolyPressure = function(msg)
    return msg[1] == 0xA0
end

MidiMsg.isSystemCommon = function(msg)
    return msg[1] == 0xF0
end

MidiMsg.isSystemRealTime = function(msg)
    return msg[1] == 0xF8
end

MidiMsg.isSystemExclusive = function(msg)
    return msg[1] == 0xF0
end


return MidiMsg

