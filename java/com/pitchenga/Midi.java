package com.pitchenga;

import com.pitchenga.domain.Tone;

import javax.sound.midi.*;
import java.util.List;

public class Midi implements Receiver {

    public static final int NOTE_ON = 0x90;
    public static final int NOTE_OFF = 0x80;

    private final MidiChannel channel;

    public Midi(MidiChannel keyboardInstrumentChannel) {
        channel = keyboardInstrumentChannel;
        MidiDevice device;
        MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();
        for (MidiDevice.Info info : infos) {
            try {
                if (!"GarageKey mini".equals(info.toString())) {
                    continue;
                }
                device = MidiSystem.getMidiDevice(info);
                Pitchenga.debug("midi=" + info);
                List<Transmitter> transmitters = device.getTransmitters();
                for (Transmitter transmitter : transmitters) {
                    transmitter.setReceiver(this);
                }
                Transmitter transmitter = device.getTransmitter();
                transmitter.setReceiver(this);
                device.open();
                Pitchenga.debug(device.getDeviceInfo() + " opened");
            } catch (MidiUnavailableException e) {
                e.printStackTrace();
            }
        }


    }

    @Override
    public void send(MidiMessage message, long timeStamp) {
        if (message instanceof ShortMessage) {
            ShortMessage sMessage = (ShortMessage) message;
            int command = sMessage.getCommand();
            int key = sMessage.getData1();
            int octave = (key / 12) - 1;
            int note = key % 12;
            String noteName = Tone.values()[note].name();
            int velocity = sMessage.getData2();
            switch (command) {
                case NOTE_ON:
                    Pitchenga.debug("midi on=" + noteName + octave + ", key=" + key + ", velocity=" + velocity);
                    channel.noteOn(key, velocity);
                    break;
                case NOTE_OFF:
                    Pitchenga.debug("midi off=" + noteName + octave + ", key=" + key + ", velocity=" + velocity);
                    channel.noteOff(key, velocity);
                    break;
            }
        }
    }

    @Override
    public void close() {

    }

}